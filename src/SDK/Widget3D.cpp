#include "../../include/SDK/Widget3D.h"
#include "../../include/SDK/Renderer.h"
#include <cmath>
#include <algorithm>
#include <limits>

namespace SDK {

namespace {
    constexpr float PI = 3.14159265358979323846f;
    constexpr float DEG_TO_RAD = PI / 180.0f;
}

// Widget3D implementation

Widget3D::Widget3D()
    : Widget()
    , m_position3D(0.0f, 0.0f, 0.0f)
    , m_pitch3D(0.0f)
    , m_yaw3D(0.0f)
    , m_roll3D(0.0f)
    , m_scale3D(1.0f)
    , m_billboard(true) // Default to billboard mode for UI elements
    , m_depthTest(true)
    , m_screenPositionValid(false)
    , m_depthValue(0.0f)
{
}

Widget3D::~Widget3D() {
}

void Widget3D::SetPosition3D(float x, float y, float z) {
    m_position3D.x = x;
    m_position3D.y = y;
    m_position3D.z = z;
    m_screenPositionValid = false;
}

void Widget3D::GetPosition3D(float& x, float& y, float& z) const {
    x = m_position3D.x;
    y = m_position3D.y;
    z = m_position3D.z;
}

void Widget3D::SetRotation3D(float pitch, float yaw, float roll) {
    m_pitch3D = pitch;
    m_yaw3D = yaw;
    m_roll3D = roll;
}

void Widget3D::GetRotation3D(float& pitch, float& yaw, float& roll) const {
    pitch = m_pitch3D;
    yaw = m_yaw3D;
    roll = m_roll3D;
}

void Widget3D::UpdateScreenPosition(CameraController* camera, int originX, int originY) {
    if (!camera) return;
    
    // Transform 3D position to screen coordinates
    int screenX, screenY;
    camera->ApplyToProjection(m_position3D, screenX, screenY, originX, originY, m_scale3D);
    
    // Update 2D position for base widget class
    m_x = screenX - m_width / 2;
    m_y = screenY - m_height / 2;
    
    // Calculate depth for sorting
    m_depthValue = GetDistanceFromCamera(camera);
    m_screenPositionValid = true;
}

void Widget3D::Render(HDC hdc) {
    // Default rendering - just call base class
    Widget::Render(hdc);
}

void Widget3D::Render3D(HDC hdc, CameraController* camera, int originX, int originY) {
    if (!m_visible || !camera) return;
    
    // Update screen position before rendering
    UpdateScreenPosition(camera, originX, originY);
    
    // Render using 2D coordinates
    Render(hdc);
}

bool Widget3D::HitTest3D(const Renderer::Vector3D& rayOrigin, const Renderer::Vector3D& rayDirection, float& distance) const {
    // Simple sphere hit test (can be overridden for more complex shapes)
    float radius = std::max(m_width, m_height) * m_scale3D * 0.5f;
    
    // Vector from ray origin to widget center
    Renderer::Vector3D oc;
    oc.x = rayOrigin.x - m_position3D.x;
    oc.y = rayOrigin.y - m_position3D.y;
    oc.z = rayOrigin.z - m_position3D.z;
    
    // Quadratic equation coefficients for ray-sphere intersection
    float a = rayDirection.x * rayDirection.x + 
              rayDirection.y * rayDirection.y + 
              rayDirection.z * rayDirection.z;
    float b = 2.0f * (oc.x * rayDirection.x + 
                      oc.y * rayDirection.y + 
                      oc.z * rayDirection.z);
    float c = oc.x * oc.x + oc.y * oc.y + oc.z * oc.z - radius * radius;
    
    float discriminant = b * b - 4 * a * c;
    
    if (discriminant < 0) {
        return false; // No intersection
    }
    
    // Calculate intersection distance
    distance = (-b - std::sqrt(discriminant)) / (2.0f * a);
    return distance > 0;
}

float Widget3D::GetDistanceFromCamera(const CameraController* camera) const {
    if (!camera) return 0.0f;
    
    Renderer::Vector3D cameraPos = camera->GetPosition();
    float dx = m_position3D.x - cameraPos.x;
    float dy = m_position3D.y - cameraPos.y;
    float dz = m_position3D.z - cameraPos.z;
    
    return std::sqrt(dx * dx + dy * dy + dz * dz);
}

void Widget3D::ScreenToRay(int screenX, int screenY, int screenWidth, int screenHeight,
                          const CameraController* camera,
                          Renderer::Vector3D& rayOrigin, Renderer::Vector3D& rayDirection) {
    if (!camera) return;
    
    // Normalize screen coordinates to [-1, 1]
    float ndcX = (2.0f * screenX) / screenWidth - 1.0f;
    float ndcY = 1.0f - (2.0f * screenY) / screenHeight;
    
    // Get camera vectors
    rayOrigin = camera->GetPosition();
    Renderer::Vector3D forward = camera->GetForwardVector();
    Renderer::Vector3D right = camera->GetRightVector();
    Renderer::Vector3D up = camera->GetUpVector();
    
    // Calculate ray direction with FOV
    float fov = camera->GetFieldOfView();
    float aspectRatio = (float)screenWidth / (float)screenHeight;
    
    // Scale factors based on FOV (convert to proper perspective)
    // The scaling factor maps FOV values (typically 400-600) to normalized view space
    constexpr float FOV_TO_VIEW_SCALE = 0.002f;
    float fovScale = 1.0f / (fov * FOV_TO_VIEW_SCALE);
    
    rayDirection.x = forward.x + ndcX * right.x * aspectRatio * fovScale + ndcY * up.x * fovScale;
    rayDirection.y = forward.y + ndcX * right.y * aspectRatio * fovScale + ndcY * up.y * fovScale;
    rayDirection.z = forward.z + ndcX * right.z * aspectRatio * fovScale + ndcY * up.z * fovScale;
    
    // Normalize ray direction
    float len = std::sqrt(rayDirection.x * rayDirection.x + 
                          rayDirection.y * rayDirection.y + 
                          rayDirection.z * rayDirection.z);
    if (len > 0.001f) {
        rayDirection.x /= len;
        rayDirection.y /= len;
        rayDirection.z /= len;
    }
}

// Button3D implementation

Button3D::Button3D(const std::wstring& text)
    : Widget3D()
    , m_text(text)
    , m_backgroundColor(70, 130, 180, 200)
    , m_textColor(255, 255, 255, 255)
    , m_hoverColor(100, 160, 210, 200)
    , m_pressed(false)
{
    SetSize(120, 40);
}

Button3D::~Button3D() {
}

void Button3D::Render(HDC hdc) {
    if (!m_visible) return;
    
    RECT rect; GetBounds(rect);
    
    // Choose color based on state
    Color bgColor = m_pressed ? m_hoverColor : (m_hovered ? m_hoverColor : m_backgroundColor);
    
    // Draw rounded rectangle
    Renderer::DrawRoundedRect(hdc, rect, 8, bgColor, Color(255, 255, 255, 100), 1);
    
    // Draw text
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, m_textColor.ToCOLORREF());
    
    HFONT font = CreateFontW(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                            DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Arial");
    HFONT oldFont = (HFONT)SelectObject(hdc, font);
    
    DrawTextW(hdc, m_text.c_str(), -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    
    SelectObject(hdc, oldFont);
    DeleteObject(font);
}

bool Button3D::HandleMouseMove(int x, int y) {
    bool wasHovered = m_hovered;
    m_hovered = HitTest(x, y);
    
    if (m_hovered != wasHovered) {
        TriggerEvent(m_hovered ? WidgetEvent::MOUSE_ENTER : WidgetEvent::MOUSE_LEAVE);
    }
    
    return m_hovered;
}

bool Button3D::HandleMouseDown(int x, int y, int button) {
    if (button == 0 && HitTest(x, y)) {
        m_pressed = true;
        return true;
    }
    return false;
}

bool Button3D::HandleMouseUp(int x, int y, int button) {
    if (button == 0 && m_pressed) {
        m_pressed = false;
        if (HitTest(x, y)) {
            TriggerEvent(WidgetEvent::CLICK);
        }
        return true;
    }
    return false;
}

// Label3D implementation

Label3D::Label3D(const std::wstring& text)
    : Widget3D()
    , m_text(text)
    , m_textColor(255, 255, 255, 255)
    , m_backgroundColor(0, 0, 0, 128)
    , m_transparent(false)
{
    SetSize(150, 30);
}

Label3D::~Label3D() {
}

void Label3D::Render(HDC hdc) {
    if (!m_visible) return;
    
    RECT rect; GetBounds(rect);
    
    // Draw background if not transparent
    if (!m_transparent) {
        Renderer::DrawRoundedRect(hdc, rect, 4, m_backgroundColor, Color(0, 0, 0, 0), 0);
    }
    
    // Draw text
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, m_textColor.ToCOLORREF());
    
    HFONT font = CreateFontW(14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                            DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Arial");
    HFONT oldFont = (HFONT)SelectObject(hdc, font);
    
    DrawTextW(hdc, m_text.c_str(), -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    
    SelectObject(hdc, oldFont);
    DeleteObject(font);
}

// Panel3D implementation

Panel3D::Panel3D()
    : Widget3D()
    , m_backgroundColor(50, 50, 50, 200)
    , m_borderColor(100, 100, 100, 255)
    , m_borderWidth(1)
    , m_rounded(true)
    , m_cornerRadius(8)
{
    SetSize(200, 150);
}

Panel3D::~Panel3D() {
}

void Panel3D::Render(HDC hdc) {
    if (!m_visible) return;
    
    RECT rect; GetBounds(rect);
    
    if (m_rounded) {
        Renderer::DrawRoundedRect(hdc, rect, m_cornerRadius, m_backgroundColor, m_borderColor, m_borderWidth);
    } else {
        // Draw regular rectangle
        HBRUSH brush = CreateSolidBrush(m_backgroundColor.ToCOLORREF());
        FillRect(hdc, &rect, brush);
        DeleteObject(brush);
        
        if (m_borderWidth > 0) {
            HPEN pen = CreatePen(PS_SOLID, m_borderWidth, m_borderColor.ToCOLORREF());
            HPEN oldPen = (HPEN)SelectObject(hdc, pen);
            HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));
            
            Rectangle(hdc, rect.left, rect.top, rect.right, rect.bottom);
            
            SelectObject(hdc, oldPen);
            SelectObject(hdc, oldBrush);
            DeleteObject(pen);
        }
    }
}

// Widget3DManager implementation

Widget3DManager::Widget3DManager()
    : m_hoveredWidget(nullptr)
    , m_focusedWidget(nullptr)
{
}

Widget3DManager::~Widget3DManager() {
}

void Widget3DManager::AddWidget(std::shared_ptr<Widget3D> widget) {
    if (widget) {
        m_widgets.push_back(widget);
    }
}

void Widget3DManager::RemoveWidget(std::shared_ptr<Widget3D> widget) {
    auto it = std::find(m_widgets.begin(), m_widgets.end(), widget);
    if (it != m_widgets.end()) {
        m_widgets.erase(it);
    }
}

void Widget3DManager::ClearWidgets() {
    m_widgets.clear();
    m_hoveredWidget = nullptr;
    m_focusedWidget = nullptr;
}

void Widget3DManager::RenderAll(HDC hdc, CameraController* camera, int originX, int originY) {
    if (!camera) return;
    
    // Sort widgets by depth (back to front)
    SortByDepth(camera);
    
    // Render all widgets
    for (auto& widget : m_widgets) {
        if (widget && widget->IsVisible()) {
            widget->Render3D(hdc, camera, originX, originY);
        }
    }
}

bool Widget3DManager::HandleMouseMove(int screenX, int screenY, int screenWidth, int screenHeight, CameraController* camera) {
    if (!camera) return false;
    
    // Find widget at this position
    auto widget = FindWidgetAtPosition(screenX, screenY, screenWidth, screenHeight, camera);
    
    // Update hover state
    if (widget != m_hoveredWidget) {
        if (m_hoveredWidget) {
            m_hoveredWidget->HandleMouseMove(-1, -1); // Move out
        }
        m_hoveredWidget = widget;
    }
    
    // Forward to widget
    if (widget) {
        return widget->HandleMouseMove(screenX, screenY);
    }
    
    return false;
}

bool Widget3DManager::HandleMouseDown(int screenX, int screenY, int screenWidth, int screenHeight, int button, CameraController* camera) {
    if (!camera) return false;
    
    // Find widget at this position
    auto widget = FindWidgetAtPosition(screenX, screenY, screenWidth, screenHeight, camera);
    
    if (widget) {
        m_focusedWidget = widget;
        return widget->HandleMouseDown(screenX, screenY, button);
    }
    
    return false;
}

bool Widget3DManager::HandleMouseUp(int screenX, int screenY, int screenWidth, int screenHeight, int button, CameraController* camera) {
    if (m_focusedWidget) {
        return m_focusedWidget->HandleMouseUp(screenX, screenY, button);
    }
    
    return false;
}

void Widget3DManager::UpdateAll(float deltaTime, CameraController* camera, int originX, int originY) {
    for (auto& widget : m_widgets) {
        if (widget) {
            widget->Update(deltaTime);
            widget->UpdateScreenPosition(camera, originX, originY);
        }
    }
}

std::shared_ptr<Widget3D> Widget3DManager::FindWidgetAtPosition(int screenX, int screenY, int screenWidth, int screenHeight, CameraController* camera) {
    if (!camera) return nullptr;
    
    // Get ray from screen position
    Renderer::Vector3D rayOrigin, rayDirection;
    Widget3D::ScreenToRay(screenX, screenY, screenWidth, screenHeight, camera, rayOrigin, rayDirection);
    
    // Find closest widget hit by ray
    std::shared_ptr<Widget3D> closestWidget = nullptr;
    float closestDistance = std::numeric_limits<float>::max();
    
    for (auto& widget : m_widgets) {
        if (!widget || !widget->IsVisible() || !widget->IsEnabled()) continue;
        
        float distance;
        if (widget->HitTest3D(rayOrigin, rayDirection, distance)) {
            if (distance < closestDistance) {
                closestDistance = distance;
                closestWidget = widget;
            }
        }
    }
    
    return closestWidget;
}

void Widget3DManager::SortByDepth(const CameraController* camera) {
    if (!camera) return;
    
    // Sort widgets by distance from camera (far to near for proper rendering)
    std::sort(m_widgets.begin(), m_widgets.end(),
        [camera](const std::shared_ptr<Widget3D>& a, const std::shared_ptr<Widget3D>& b) {
            return a->GetDistanceFromCamera(camera) > b->GetDistanceFromCamera(camera);
        });
}

} // namespace SDK
