#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include "Widget.h"
#include "Renderer.h"
#include "CameraController.h"

namespace SDK {

/**
 * Widget3D - Base class for widgets placed in 3D space
 * Extends the standard widget system to support 3D positioning and interaction
 */
class Widget3D : public Widget {
public:
    Widget3D();
    virtual ~Widget3D();
    
    // 3D positioning
    void SetPosition3D(float x, float y, float z);
    void GetPosition3D(float& x, float& y, float& z) const;
    Renderer::Vector3D GetPosition3D() const { return m_position3D; }
    
    // 3D rotation (in degrees)
    void SetRotation3D(float pitch, float yaw, float roll);
    void GetRotation3D(float& pitch, float& yaw, float& roll) const;
    
    // 3D scale
    void SetScale3D(float scale) { m_scale3D = scale; }
    float GetScale3D() const { return m_scale3D; }
    
    // Billboard mode - always face camera
    void SetBillboard(bool billboard) { m_billboard = billboard; }
    bool IsBillboard() const { return m_billboard; }
    
    // Depth testing
    void SetDepthTest(bool enabled) { m_depthTest = enabled; }
    bool IsDepthTestEnabled() const { return m_depthTest; }
    
    // Update 2D position from 3D position using camera
    void UpdateScreenPosition(CameraController* camera, int originX, int originY);
    
    // Rendering (overridden to handle 3D to 2D transformation)
    void Render(HDC hdc) override;
    void Render3D(HDC hdc, CameraController* camera, int originX, int originY);
    
    // Hit testing in 3D space
    bool HitTest3D(const Renderer::Vector3D& rayOrigin, const Renderer::Vector3D& rayDirection, float& distance) const;
    
    // Get distance from camera (for sorting)
    float GetDistanceFromCamera(const CameraController* camera) const;
    
    // Calculate ray from screen coordinates
    static void ScreenToRay(int screenX, int screenY, int screenWidth, int screenHeight, 
                           const CameraController* camera, 
                           Renderer::Vector3D& rayOrigin, Renderer::Vector3D& rayDirection);
    
protected:
    // 3D transform
    Renderer::Vector3D m_position3D;
    float m_pitch3D;
    float m_yaw3D;
    float m_roll3D;
    float m_scale3D;
    
    // Rendering options
    bool m_billboard;
    bool m_depthTest;
    
    // Cached screen position
    bool m_screenPositionValid;
    float m_depthValue; // Z-depth for sorting
};

/**
 * Button3D - Button widget in 3D space
 */
class Button3D : public Widget3D {
public:
    Button3D(const std::wstring& text = L"");
    virtual ~Button3D();
    
    void SetText(const std::wstring& text) { m_text = text; }
    std::wstring GetText() const { return m_text; }
    
    void SetBackgroundColor(const Color& color) { m_backgroundColor = color; }
    void SetTextColor(const Color& color) { m_textColor = color; }
    void SetHoverColor(const Color& color) { m_hoverColor = color; }
    
    void Render(HDC hdc) override;
    bool HandleMouseMove(int x, int y) override;
    bool HandleMouseDown(int x, int y, int button) override;
    bool HandleMouseUp(int x, int y, int button) override;
    
private:
    std::wstring m_text;
    Color m_backgroundColor;
    Color m_textColor;
    Color m_hoverColor;
    bool m_pressed;
};

/**
 * Label3D - Label widget in 3D space
 */
class Label3D : public Widget3D {
public:
    Label3D(const std::wstring& text = L"");
    virtual ~Label3D();
    
    void SetText(const std::wstring& text) { m_text = text; }
    std::wstring GetText() const { return m_text; }
    
    void SetTextColor(const Color& color) { m_textColor = color; }
    void SetBackgroundColor(const Color& color) { m_backgroundColor = color; }
    void SetTransparent(bool transparent) { m_transparent = transparent; }
    
    void Render(HDC hdc) override;
    
private:
    std::wstring m_text;
    Color m_textColor;
    Color m_backgroundColor;
    bool m_transparent;
};

/**
 * Panel3D - Container panel in 3D space
 */
class Panel3D : public Widget3D {
public:
    Panel3D();
    virtual ~Panel3D();
    
    void SetBackgroundColor(const Color& color) { m_backgroundColor = color; }
    void SetBorderColor(const Color& color) { m_borderColor = color; }
    void SetBorderWidth(int width) { m_borderWidth = width; }
    void SetRounded(bool rounded) { m_rounded = rounded; }
    void SetCornerRadius(int radius) { m_cornerRadius = radius; }
    
    void Render(HDC hdc) override;
    
private:
    Color m_backgroundColor;
    Color m_borderColor;
    int m_borderWidth;
    bool m_rounded;
    int m_cornerRadius;
};

/**
 * Widget3DManager - Manages 3D widgets and handles interaction
 */
class Widget3DManager {
public:
    Widget3DManager();
    ~Widget3DManager();
    
    // Widget management
    void AddWidget(std::shared_ptr<Widget3D> widget);
    void RemoveWidget(std::shared_ptr<Widget3D> widget);
    void ClearWidgets();
    
    // Get all widgets
    const std::vector<std::shared_ptr<Widget3D>>& GetWidgets() const { return m_widgets; }
    
    // Rendering
    void RenderAll(HDC hdc, CameraController* camera, int originX, int originY);
    
    // Input handling with ray casting
    bool HandleMouseMove(int screenX, int screenY, int screenWidth, int screenHeight, CameraController* camera);
    bool HandleMouseDown(int screenX, int screenY, int screenWidth, int screenHeight, int button, CameraController* camera);
    bool HandleMouseUp(int screenX, int screenY, int screenWidth, int screenHeight, int button, CameraController* camera);
    
    // Update all widgets
    void UpdateAll(float deltaTime, CameraController* camera, int originX, int originY);
    
    // Find widget at screen position
    std::shared_ptr<Widget3D> FindWidgetAtPosition(int screenX, int screenY, int screenWidth, int screenHeight, CameraController* camera);
    
    // Sort widgets by depth (for proper rendering order)
    void SortByDepth(const CameraController* camera);
    
private:
    std::vector<std::shared_ptr<Widget3D>> m_widgets;
    std::shared_ptr<Widget3D> m_hoveredWidget;
    std::shared_ptr<Widget3D> m_focusedWidget;
};

} // namespace SDK
