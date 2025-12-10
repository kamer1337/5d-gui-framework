#include "../../include/SDK/CameraController.h"
#include <cmath>
#include <algorithm>

namespace SDK {

namespace {
    constexpr float PI = 3.14159265358979323846f;
    constexpr float DEG_TO_RAD = PI / 180.0f;
    constexpr float RAD_TO_DEG = 180.0f / PI;
    
    constexpr float DEFAULT_DISTANCE = 300.0f;
    constexpr float DEFAULT_FOV = 500.0f;
    constexpr float DEFAULT_NEAR_PLANE = 1.0f;
    constexpr float DEFAULT_FAR_PLANE = 1000.0f;
    
    constexpr float DEFAULT_MOVE_SPEED = 100.0f;
    constexpr float DEFAULT_ROTATE_SPEED = 0.5f;
    constexpr float DEFAULT_ZOOM_SPEED = 50.0f;
    constexpr float DEFAULT_SMOOTHING = 0.15f;
    
    constexpr float MIN_DISTANCE = 50.0f;
    constexpr float MAX_DISTANCE = 1000.0f;
    constexpr float MIN_PITCH = -89.0f;
    constexpr float MAX_PITCH = 89.0f;
}

CameraController::CameraController()
    : m_position(0.0f, 0.0f, 0.0f)
    , m_target(0.0f, 0.0f, 0.0f)
    , m_up(0.0f, 1.0f, 0.0f)
    , m_pitch(0.0f)
    , m_yaw(0.0f)
    , m_roll(0.0f)
    , m_distance(DEFAULT_DISTANCE)
    , m_fov(DEFAULT_FOV)
    , m_nearPlane(DEFAULT_NEAR_PLANE)
    , m_farPlane(DEFAULT_FAR_PLANE)
    , m_cameraMode(CameraMode::MODE_3D)
    , m_mouseMode(MouseMode::NONE)
    , m_lastMouseX(0)
    , m_lastMouseY(0)
    , m_moveSpeed(DEFAULT_MOVE_SPEED)
    , m_rotateSpeed(DEFAULT_ROTATE_SPEED)
    , m_zoomSpeed(DEFAULT_ZOOM_SPEED)
    , m_smoothingEnabled(true)
    , m_smoothingFactor(DEFAULT_SMOOTHING)
    , m_targetPosition(0.0f, 0.0f, 0.0f)
    , m_targetRotation(0.0f, 0.0f, 0.0f)
    , m_forward(0.0f, 0.0f, -1.0f)
    , m_right(1.0f, 0.0f, 0.0f)
    , m_upVector(0.0f, 1.0f, 0.0f)
{
    for (int i = 0; i < 3; i++) {
        m_mouseButtonDown[i] = false;
    }
    for (int i = 0; i < 256; i++) {
        m_keysDown[i] = false;
    }
    
    // Initialize camera position behind origin
    m_position.z = -m_distance;
    UpdateCameraVectors();
}

CameraController::~CameraController() {
}

void CameraController::SetPosition(float x, float y, float z) {
    m_position.x = x;
    m_position.y = y;
    m_position.z = z;
    UpdateCameraVectors();
}

void CameraController::GetPosition(float& x, float& y, float& z) const {
    x = m_position.x;
    y = m_position.y;
    z = m_position.z;
}

void CameraController::SetTarget(float x, float y, float z) {
    m_target.x = x;
    m_target.y = y;
    m_target.z = z;
    UpdateCameraVectors();
}

void CameraController::GetTarget(float& x, float& y, float& z) const {
    x = m_target.x;
    y = m_target.y;
    z = m_target.z;
}

void CameraController::SetRotation(float pitch, float yaw, float roll) {
    m_pitch = std::max(MIN_PITCH, std::min(MAX_PITCH, pitch));
    m_yaw = yaw;
    m_roll = roll;
    UpdateCameraVectors();
}

void CameraController::GetRotation(float& pitch, float& yaw, float& roll) const {
    pitch = m_pitch;
    yaw = m_yaw;
    roll = m_roll;
}

void CameraController::SetDistance(float distance) {
    m_distance = std::max(MIN_DISTANCE, std::min(MAX_DISTANCE, distance));
}

void CameraController::SetFieldOfView(float fov) {
    m_fov = fov;
}

void CameraController::SetNearPlane(float near) {
    m_nearPlane = near;
}

void CameraController::SetFarPlane(float far) {
    m_farPlane = far;
}

void CameraController::HandleMouseDown(int x, int y, int button) {
    if (button >= 0 && button < 3) {
        m_mouseButtonDown[button] = true;
        m_lastMouseX = x;
        m_lastMouseY = y;
        
        // Determine mouse mode based on button
        if (button == 0) { // Left button - orbit
            m_mouseMode = MouseMode::ORBIT;
        } else if (button == 1) { // Middle button - pan
            m_mouseMode = MouseMode::PAN;
        } else if (button == 2) { // Right button - zoom
            m_mouseMode = MouseMode::ZOOM;
        }
    }
}

void CameraController::HandleMouseUp(int x, int y, int button) {
    if (button >= 0 && button < 3) {
        m_mouseButtonDown[button] = false;
        
        // Clear mouse mode if no buttons are pressed
        bool anyButtonDown = false;
        for (int i = 0; i < 3; i++) {
            if (m_mouseButtonDown[i]) {
                anyButtonDown = true;
                break;
            }
        }
        if (!anyButtonDown) {
            m_mouseMode = MouseMode::NONE;
        }
    }
}

void CameraController::HandleMouseMove(int x, int y) {
    int deltaX = x - m_lastMouseX;
    int deltaY = y - m_lastMouseY;
    
    switch (m_mouseMode) {
        case MouseMode::ORBIT:
            ProcessOrbitInput(deltaX, deltaY);
            break;
        case MouseMode::PAN:
            ProcessPanInput(deltaX, deltaY);
            break;
        case MouseMode::ZOOM:
            ProcessZoomInput(deltaY);
            break;
        default:
            break;
    }
    
    m_lastMouseX = x;
    m_lastMouseY = y;
}

void CameraController::HandleMouseWheel(int delta) {
    // Zoom with mouse wheel
    float zoomAmount = (delta / 120.0f) * m_zoomSpeed;
    Zoom(zoomAmount);
}

void CameraController::HandleKeyDown(int keyCode) {
    if (keyCode >= 0 && keyCode < 256) {
        m_keysDown[keyCode] = true;
    }
}

void CameraController::HandleKeyUp(int keyCode) {
    if (keyCode >= 0 && keyCode < 256) {
        m_keysDown[keyCode] = false;
    }
}

void CameraController::Update(float deltaTime) {
    UpdateFromKeyboard(deltaTime);
    
    // Apply smoothing if enabled
    if (m_smoothingEnabled) {
        // Smooth position
        m_position.x += (m_targetPosition.x - m_position.x) * m_smoothingFactor;
        m_position.y += (m_targetPosition.y - m_position.y) * m_smoothingFactor;
        m_position.z += (m_targetPosition.z - m_position.z) * m_smoothingFactor;
        
        // Smooth rotation
        m_pitch += (m_targetRotation.x - m_pitch) * m_smoothingFactor;
        m_yaw += (m_targetRotation.y - m_yaw) * m_smoothingFactor;
        m_roll += (m_targetRotation.z - m_roll) * m_smoothingFactor;
    }
    
    UpdateCameraVectors();
}

void CameraController::UpdateCameraVectors() {
    // Calculate forward vector from pitch and yaw
    float pitchRad = m_pitch * DEG_TO_RAD;
    float yawRad = m_yaw * DEG_TO_RAD;
    
    m_forward.x = std::cos(pitchRad) * std::sin(yawRad);
    m_forward.y = std::sin(pitchRad);
    m_forward.z = std::cos(pitchRad) * std::cos(yawRad);
    
    // Normalize forward vector
    float forwardLen = std::sqrt(m_forward.x * m_forward.x + 
                                  m_forward.y * m_forward.y + 
                                  m_forward.z * m_forward.z);
    if (forwardLen > 0.001f) {
        m_forward.x /= forwardLen;
        m_forward.y /= forwardLen;
        m_forward.z /= forwardLen;
    }
    
    // Calculate right vector (cross product of forward and world up)
    Renderer::Vector3D worldUp(0.0f, 1.0f, 0.0f);
    m_right.x = m_forward.y * worldUp.z - m_forward.z * worldUp.y;
    m_right.y = m_forward.z * worldUp.x - m_forward.x * worldUp.z;
    m_right.z = m_forward.x * worldUp.y - m_forward.y * worldUp.x;
    
    // Normalize right vector
    float rightLen = std::sqrt(m_right.x * m_right.x + 
                                m_right.y * m_right.y + 
                                m_right.z * m_right.z);
    if (rightLen > 0.001f) {
        m_right.x /= rightLen;
        m_right.y /= rightLen;
        m_right.z /= rightLen;
    }
    
    // Calculate up vector (cross product of right and forward)
    m_upVector.x = m_right.y * m_forward.z - m_right.z * m_forward.y;
    m_upVector.y = m_right.z * m_forward.x - m_right.x * m_forward.z;
    m_upVector.z = m_right.x * m_forward.y - m_right.y * m_forward.x;
    
    // Normalize up vector
    float upLen = std::sqrt(m_upVector.x * m_upVector.x + 
                             m_upVector.y * m_upVector.y + 
                             m_upVector.z * m_upVector.z);
    if (upLen > 0.001f) {
        m_upVector.x /= upLen;
        m_upVector.y /= upLen;
        m_upVector.z /= upLen;
    }
    
    m_up = m_upVector;
}

void CameraController::ProcessOrbitInput(int deltaX, int deltaY) {
    m_yaw += deltaX * m_rotateSpeed;
    m_pitch -= deltaY * m_rotateSpeed;
    
    // Clamp pitch
    m_pitch = std::max(MIN_PITCH, std::min(MAX_PITCH, m_pitch));
    
    // Update camera position based on orbit
    float pitchRad = m_pitch * DEG_TO_RAD;
    float yawRad = m_yaw * DEG_TO_RAD;
    
    m_position.x = m_target.x - m_distance * std::cos(pitchRad) * std::sin(yawRad);
    m_position.y = m_target.y - m_distance * std::sin(pitchRad);
    m_position.z = m_target.z - m_distance * std::cos(pitchRad) * std::cos(yawRad);
}

void CameraController::ProcessPanInput(int deltaX, int deltaY) {
    float panSpeed = m_distance * 0.002f; // Scale pan speed with distance
    
    // Pan along right and up vectors (use all components for consistency)
    m_position.x -= m_right.x * deltaX * panSpeed + m_upVector.x * deltaY * panSpeed;
    m_position.y -= m_right.y * deltaX * panSpeed + m_upVector.y * deltaY * panSpeed;
    m_position.z -= m_right.z * deltaX * panSpeed + m_upVector.z * deltaY * panSpeed;
    
    m_target.x -= m_right.x * deltaX * panSpeed + m_upVector.x * deltaY * panSpeed;
    m_target.y -= m_right.y * deltaX * panSpeed + m_upVector.y * deltaY * panSpeed;
    m_target.z -= m_right.z * deltaX * panSpeed + m_upVector.z * deltaY * panSpeed;
}

void CameraController::ProcessZoomInput(int delta) {
    float zoomAmount = delta * 2.0f;
    Zoom(-zoomAmount);
}

void CameraController::UpdateFromKeyboard(float deltaTime) {
    float moveAmount = m_moveSpeed * deltaTime;
    
    // WASD movement
    if (m_keysDown['W'] || m_keysDown[VK_UP]) {
        MoveForward(moveAmount);
    }
    if (m_keysDown['S'] || m_keysDown[VK_DOWN]) {
        MoveForward(-moveAmount);
    }
    if (m_keysDown['A'] || m_keysDown[VK_LEFT]) {
        MoveRight(-moveAmount);
    }
    if (m_keysDown['D'] || m_keysDown[VK_RIGHT]) {
        MoveRight(moveAmount);
    }
    
    // Q/E for up/down
    if (m_keysDown['Q']) {
        MoveUp(-moveAmount);
    }
    if (m_keysDown['E']) {
        MoveUp(moveAmount);
    }
    
    // R to reset
    if (m_keysDown['R']) {
        Reset();
    }
}

void CameraController::MoveForward(float amount) {
    m_position.x += m_forward.x * amount;
    m_position.y += m_forward.y * amount;
    m_position.z += m_forward.z * amount;
    
    m_target.x += m_forward.x * amount;
    m_target.y += m_forward.y * amount;
    m_target.z += m_forward.z * amount;
}

void CameraController::MoveRight(float amount) {
    m_position.x += m_right.x * amount;
    m_position.y += m_right.y * amount;
    m_position.z += m_right.z * amount;
    
    m_target.x += m_right.x * amount;
    m_target.y += m_right.y * amount;
    m_target.z += m_right.z * amount;
}

void CameraController::MoveUp(float amount) {
    m_position.x += m_upVector.x * amount;
    m_position.y += m_upVector.y * amount;
    m_position.z += m_upVector.z * amount;
    
    m_target.x += m_upVector.x * amount;
    m_target.y += m_upVector.y * amount;
    m_target.z += m_upVector.z * amount;
}

void CameraController::RotateYaw(float amount) {
    m_yaw += amount;
    UpdateCameraVectors();
}

void CameraController::RotatePitch(float amount) {
    m_pitch += amount;
    m_pitch = std::max(MIN_PITCH, std::min(MAX_PITCH, m_pitch));
    UpdateCameraVectors();
}

void CameraController::Zoom(float amount) {
    m_distance -= amount;
    m_distance = std::max(MIN_DISTANCE, std::min(MAX_DISTANCE, m_distance));
    
    // Update position to maintain target
    m_position.x = m_target.x - m_distance * m_forward.x;
    m_position.y = m_target.y - m_distance * m_forward.y;
    m_position.z = m_target.z - m_distance * m_forward.z;
}

void CameraController::Reset() {
    m_position = Renderer::Vector3D(0.0f, 0.0f, -m_distance);
    m_target = Renderer::Vector3D(0.0f, 0.0f, 0.0f);
    m_pitch = 0.0f;
    m_yaw = 0.0f;
    m_roll = 0.0f;
    UpdateCameraVectors();
}

Renderer::Vector3D CameraController::GetForwardVector() const {
    return m_forward;
}

Renderer::Vector3D CameraController::GetRightVector() const {
    return m_right;
}

Renderer::Vector3D CameraController::GetUpVector() const {
    return m_upVector;
}

void CameraController::TransformPoint3D(const Renderer::Vector3D& worldPoint, Renderer::Vector3D& viewPoint) const {
    // Transform from world space to camera view space
    Renderer::Vector3D relative;
    relative.x = worldPoint.x - m_position.x;
    relative.y = worldPoint.y - m_position.y;
    relative.z = worldPoint.z - m_position.z;
    
    // Apply camera rotation
    viewPoint.x = relative.x * m_right.x + relative.y * m_right.y + relative.z * m_right.z;
    viewPoint.y = relative.x * m_upVector.x + relative.y * m_upVector.y + relative.z * m_upVector.z;
    viewPoint.z = relative.x * m_forward.x + relative.y * m_forward.y + relative.z * m_forward.z;
}

void CameraController::TransformPoint4D(const Renderer::Vector4D& worldPoint, Renderer::Vector4D& viewPoint) const {
    Renderer::Vector3D world3D(worldPoint.x, worldPoint.y, worldPoint.z);
    Renderer::Vector3D view3D;
    TransformPoint3D(world3D, view3D);
    
    viewPoint.x = view3D.x;
    viewPoint.y = view3D.y;
    viewPoint.z = view3D.z;
    viewPoint.w = worldPoint.w;
}

void CameraController::TransformPoint5D(const Renderer::Vector5D& worldPoint, Renderer::Vector5D& viewPoint) const {
    Renderer::Vector3D world3D(worldPoint.x, worldPoint.y, worldPoint.z);
    Renderer::Vector3D view3D;
    TransformPoint3D(world3D, view3D);
    
    viewPoint.x = view3D.x;
    viewPoint.y = view3D.y;
    viewPoint.z = view3D.z;
    viewPoint.w = worldPoint.w;
    viewPoint.d = worldPoint.d;
}

void CameraController::ApplyToProjection(const Renderer::Vector3D& point3D, int& x2D, int& y2D, int originX, int originY, float scale) const {
    // Transform point to view space
    Renderer::Vector3D viewPoint;
    TransformPoint3D(point3D, viewPoint);
    
    // Apply perspective projection
    float distance = viewPoint.z + m_distance;
    distance = std::max(distance, m_nearPlane); // Clamp to near plane
    
    float perspectiveScale = m_fov / distance;
    
    x2D = originX + (int)(viewPoint.x * perspectiveScale * scale);
    y2D = originY + (int)(viewPoint.y * perspectiveScale * scale);
}

} // namespace SDK
