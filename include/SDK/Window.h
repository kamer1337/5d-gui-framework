#pragma once


#include "Platform.h"
#include <string>
#include <memory>
#include <functional>
#include <vector>
#include "Theme.h"

namespace SDK {

// Forward declarations
class Widget;

// Window depth levels for 5D rendering
enum class WindowDepth {
    FAR_BACKGROUND = 0,  // 70% scale, alpha 170
    BACKGROUND = 1,       // 80% scale, alpha 200
    MIDDLE = 2,           // 90% scale, alpha 230
    MID_FRONT = 3,        // 95% scale, alpha 245
    FOREGROUND = 4        // 100% scale, alpha 255
};

/**
 * Window - Enhanced window with 5D rendering support
 * Supports layered rendering, theming, and multimodal display
 */
class Window {
public:
    Window(HWND hwnd);
    ~Window();
    
    // Window properties
    HWND GetHandle() const { return m_hwnd; }
    bool IsValid() const { return m_hwnd != nullptr && ::IsWindow(m_hwnd); }
    
    // Depth control
    void SetDepth(WindowDepth depth);
    WindowDepth GetDepth() const { return m_depth; }
    
    // Alpha and scaling
    void SetAlpha(BYTE alpha);
    BYTE GetAlpha() const { return m_alpha; }
    
    void SetScale(float scale);
    float GetScale() const { return m_scale; }
    
    // Position offset for perspective
    void SetDepthOffset(int x, int y);
    void GetDepthOffset(int& x, int& y) const;
    
    // Theming
    void SetTheme(std::shared_ptr<Theme> theme);
    std::shared_ptr<Theme> GetTheme() const { return m_theme; }
    
    // Rounded corners
    void SetRoundedCorners(bool enabled, int radius = 12);
    bool HasRoundedCorners() const { return m_roundedCorners; }
    int GetCornerRadius() const { return m_cornerRadius; }
    
    // Shadow settings
    void SetShadowEnabled(bool enabled);
    bool IsShadowEnabled() const { return m_shadowEnabled; }
    
    void SetShadowIntensity(float intensity);
    float GetShadowIntensity() const { return m_shadowIntensity; }
    
    // Update window appearance based on settings
    void UpdateAppearance();
    
    // Enable layered window mode for alpha blending
    void EnableLayeredMode();
    
    // Custom rendering callback
    void SetRenderCallback(std::function<void(HDC)> callback);
    void Render(HDC hdc);
    
    // Widget management
    void AddWidget(std::shared_ptr<Widget> widget);
    void RemoveWidget(std::shared_ptr<Widget> widget);
    void ClearWidgets();
    const std::vector<std::shared_ptr<Widget>>& GetWidgets() const { return m_widgets; }
    
    // Widget input handling
    bool HandleWidgetMouseMove(int x, int y);
    bool HandleWidgetMouseDown(int x, int y, int button);
    bool HandleWidgetMouseUp(int x, int y, int button);
    bool HandleWidgetKeyDown(int keyCode);
    bool HandleWidgetKeyUp(int keyCode);
    bool HandleWidgetChar(wchar_t ch);
    
    // Update widgets
    void UpdateWidgets(float deltaTime);
    
private:
    void ApplyDepthSettings();
    void UpdateLayeredWindow();
    
    HWND m_hwnd;
    WindowDepth m_depth;
    BYTE m_alpha;
    float m_scale;
    int m_offsetX;
    int m_offsetY;
    
    bool m_roundedCorners;
    int m_cornerRadius;
    
    bool m_shadowEnabled;
    float m_shadowIntensity;
    
    std::shared_ptr<Theme> m_theme;
    std::function<void(HDC)> m_renderCallback;
    std::vector<std::shared_ptr<Widget>> m_widgets;
};

} // namespace SDK
