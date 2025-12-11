#pragma once


#include "Platform.h"
#include "Renderer.h"

namespace SDK {

/**
 * CameraController - Camera control system for 3D-5D rendering
 * Provides mouse and keyboard controls for navigating 3D scenes
 */
class CameraController {
public:
    // Camera mode types
    enum class CameraMode {
        MODE_3D,  // Standard 3D camera
        MODE_4D,  // 4D visualization with time
        MODE_5D,  // 5D rendering with depth layers
        MODE_6D   // 6D multi-timeline
    };

    // Mouse interaction modes
    enum class MouseMode {
        NONE,
        ORBIT,     // Rotate camera around target
        PAN,       // Move camera parallel to view plane
        ZOOM       // Zoom in/out
    };

    CameraController();
    ~CameraController();

    // Camera positioning
    void SetPosition(float x, float y, float z);
    void GetPosition(float& x, float& y, float& z) const;
    Renderer::Vector3D GetPosition() const { return m_position; }

    void SetTarget(float x, float y, float z);
    void GetTarget(float& x, float& y, float& z) const;
    Renderer::Vector3D GetTarget() const { return m_target; }

    void SetRotation(float pitch, float yaw, float roll);
    void GetRotation(float& pitch, float& yaw, float& roll) const;

    // Camera parameters
    void SetDistance(float distance); // Distance from target
    float GetDistance() const { return m_distance; }

    void SetFieldOfView(float fov);
    float GetFieldOfView() const { return m_fov; }

    void SetNearPlane(float near);
    float GetNearPlane() const { return m_nearPlane; }

    void SetFarPlane(float far);
    float GetFarPlane() const { return m_farPlane; }

    // Camera mode
    void SetCameraMode(CameraMode mode) { m_cameraMode = mode; }
    CameraMode GetCameraMode() const { return m_cameraMode; }

    // Mouse input handling
    void HandleMouseDown(int x, int y, int button);
    void HandleMouseUp(int x, int y, int button);
    void HandleMouseMove(int x, int y);
    void HandleMouseWheel(int delta);

    // Keyboard input handling
    void HandleKeyDown(int keyCode);
    void HandleKeyUp(int keyCode);

    // Update camera state (for smooth animations)
    void Update(float deltaTime);

    // Transform points using camera view
    void TransformPoint3D(const Renderer::Vector3D& worldPoint, Renderer::Vector3D& viewPoint) const;
    void TransformPoint4D(const Renderer::Vector4D& worldPoint, Renderer::Vector4D& viewPoint) const;
    void TransformPoint5D(const Renderer::Vector5D& worldPoint, Renderer::Vector5D& viewPoint) const;

    // Apply camera transformations to projection
    void ApplyToProjection(const Renderer::Vector3D& point3D, int& x2D, int& y2D, int originX, int originY, float scale) const;

    // Camera movement methods
    void MoveForward(float amount);
    void MoveRight(float amount);
    void MoveUp(float amount);
    void RotateYaw(float amount);
    void RotatePitch(float amount);
    void Zoom(float amount);

    // Reset camera to default position
    void Reset();

    // Smooth camera transitions
    void SetSmoothingEnabled(bool enabled) { m_smoothingEnabled = enabled; }
    bool IsSmoothingEnabled() const { return m_smoothingEnabled; }

    void SetSmoothingFactor(float factor) { m_smoothingFactor = factor; }
    float GetSmoothingFactor() const { return m_smoothingFactor; }

    // Get view direction
    Renderer::Vector3D GetForwardVector() const;
    Renderer::Vector3D GetRightVector() const;
    Renderer::Vector3D GetUpVector() const;

private:
    void UpdateCameraVectors();
    void ProcessOrbitInput(int deltaX, int deltaY);
    void ProcessPanInput(int deltaX, int deltaY);
    void ProcessZoomInput(int delta);
    void UpdateFromKeyboard(float deltaTime);

    // Camera state
    Renderer::Vector3D m_position;
    Renderer::Vector3D m_target;
    Renderer::Vector3D m_up;
    
    float m_pitch;  // Rotation around X axis (up/down)
    float m_yaw;    // Rotation around Y axis (left/right)
    float m_roll;   // Rotation around Z axis (tilt)
    
    float m_distance;  // Distance from target
    float m_fov;       // Field of view
    float m_nearPlane;
    float m_farPlane;

    // Camera mode
    CameraMode m_cameraMode;

    // Mouse interaction state
    MouseMode m_mouseMode;
    int m_lastMouseX;
    int m_lastMouseY;
    bool m_mouseButtonDown[3]; // Left, Middle, Right

    // Keyboard state
    bool m_keysDown[256];
    float m_moveSpeed;
    float m_rotateSpeed;
    float m_zoomSpeed;

    // Smoothing
    bool m_smoothingEnabled;
    float m_smoothingFactor;
    Renderer::Vector3D m_targetPosition;
    Renderer::Vector3D m_targetRotation;

    // Cached vectors
    Renderer::Vector3D m_forward;
    Renderer::Vector3D m_right;
    Renderer::Vector3D m_upVector;
};

} // namespace SDK
