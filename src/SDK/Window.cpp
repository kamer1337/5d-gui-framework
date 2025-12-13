#include "../../include/SDK/Window.h"
#include "../../include/SDK/Widget.h"
#include "../../include/SDK/Renderer.h"
#include "../../include/SDK/DPIManager.h"
#include "../../include/SDK/MonitorManager.h"
#include <dwmapi.h>
#include <algorithm>

#pragma comment(lib, "dwmapi.lib")

namespace SDK {

Window::Window(HWND hwnd)
    : m_hwnd(hwnd)
    , m_depth(WindowDepth::FOREGROUND)
    , m_alpha(255)
    , m_scale(1.0f)
    , m_offsetX(0)
    , m_offsetY(0)
    , m_roundedCorners(true)
    , m_cornerRadius(12)
    , m_shadowEnabled(true)
    , m_shadowIntensity(1.0f)
    , m_theme(nullptr)
    , m_renderCallback(nullptr)
    , m_currentMonitor(nullptr)
{
    // Initialize DPI info
    m_currentDPI = DPIManager::GetInstance().GetDPIForWindow(hwnd);
    
    // Initialize monitor tracking
    m_currentMonitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
}

Window::~Window() {
    // Cleanup
}

void Window::SetDepth(WindowDepth depth) {
    m_depth = depth;
    ApplyDepthSettings();
}

void Window::SetAlpha(BYTE alpha) {
    m_alpha = alpha;
    UpdateLayeredWindow();
}

void Window::SetScale(float scale) {
    m_scale = scale;
    UpdateAppearance();
}

void Window::SetDepthOffset(int x, int y) {
    m_offsetX = x;
    m_offsetY = y;
    UpdateAppearance();
}

void Window::GetDepthOffset(int& x, int& y) const {
    x = m_offsetX;
    y = m_offsetY;
}

void Window::SetTheme(std::shared_ptr<Theme> theme) {
    m_theme = theme;
    UpdateAppearance();
}

void Window::SetRoundedCorners(bool enabled, int radius) {
    m_roundedCorners = enabled;
    m_cornerRadius = radius;
    UpdateAppearance();
}

void Window::SetShadowEnabled(bool enabled) {
    m_shadowEnabled = enabled;
    UpdateAppearance();
}

void Window::SetShadowIntensity(float intensity) {
    m_shadowIntensity = intensity;
    UpdateAppearance();
}

void Window::UpdateAppearance() {
    if (!IsValid()) return;
    
    // Force window redraw
    InvalidateRect(m_hwnd, nullptr, TRUE);
    UpdateWindow(m_hwnd);
}

void Window::EnableLayeredMode() {
    if (!IsValid()) return;
    
    // Enable WS_EX_LAYERED style
    LONG exStyle = GetWindowLong(m_hwnd, GWL_EXSTYLE);
    exStyle |= WS_EX_LAYERED;
    SetWindowLong(m_hwnd, GWL_EXSTYLE, exStyle);
    
    UpdateLayeredWindow();
}

void Window::SetRenderCallback(std::function<void(HDC)> callback) {
    m_renderCallback = callback;
}

void Window::Render(HDC hdc) {
    if (!IsValid()) return;
    
    RECT rect;
    GetClientRect(m_hwnd, &rect);
    
    // Render shadow if enabled
    if (m_shadowEnabled && m_theme) {
        int shadowOffsetX, shadowOffsetY;
        m_theme->GetShadowOffset(shadowOffsetX, shadowOffsetY);
        int shadowBlur = m_theme->GetShadowBlur();
        
        // Scale shadow based on depth
        float depthScale = 1.0f;
        switch (m_depth) {
            case WindowDepth::FAR_BACKGROUND: depthScale = 0.5f; break;
            case WindowDepth::BACKGROUND: depthScale = 0.7f; break;
            case WindowDepth::MIDDLE: depthScale = 0.85f; break;
            case WindowDepth::MID_FRONT: depthScale = 0.95f; break;
            case WindowDepth::FOREGROUND: depthScale = 1.0f; break;
        }
        
        shadowOffsetX = (int)(shadowOffsetX * depthScale * m_shadowIntensity);
        shadowOffsetY = (int)(shadowOffsetY * depthScale * m_shadowIntensity);
        shadowBlur = (int)(shadowBlur * depthScale * m_shadowIntensity);
        
        Color shadowColor = m_theme->GetShadowColor();
        Renderer::DrawShadow(hdc, rect, shadowOffsetX, shadowOffsetY, shadowBlur, shadowColor);
    }
    
    // Render window background
    if (m_theme) {
        Color bgColor = m_theme->GetBackgroundColor();
        if (m_roundedCorners) {
            Color borderColor = m_theme->GetBorderColor();
            int borderWidth = m_theme->GetBorderWidth();
            Renderer::DrawRoundedRect(hdc, rect, m_cornerRadius, bgColor, borderColor, borderWidth);
        } else {
            HBRUSH brush = CreateSolidBrush(bgColor.ToCOLORREF());
            FillRect(hdc, &rect, brush);
            DeleteObject(brush);
        }
        
        // Render title bar with gradient
        RECT titleRect = rect;
        titleRect.bottom = titleRect.top + m_theme->GetTitleBarHeight();
        Gradient titleGradient = m_theme->GetTitleBarGradient();
        Renderer::DrawGradient(hdc, titleRect, titleGradient);
    }
    
    // Custom rendering callback
    if (m_renderCallback) {
        m_renderCallback(hdc);
    }
    
    // Render widgets
    for (auto& widget : m_widgets) {
        widget->Render(hdc);
    }
}

void Window::ApplyDepthSettings() {
    // Apply depth-based defaults
    switch (m_depth) {
        case WindowDepth::FAR_BACKGROUND:
            SetAlpha(170);
            SetScale(0.7f);
            SetDepthOffset(15, 9);
            SetShadowIntensity(0.5f);
            break;
            
        case WindowDepth::BACKGROUND:
            SetAlpha(200);
            SetScale(0.8f);
            SetDepthOffset(10, 6);
            SetShadowIntensity(0.7f);
            break;
            
        case WindowDepth::MIDDLE:
            SetAlpha(230);
            SetScale(0.9f);
            SetDepthOffset(5, 3);
            SetShadowIntensity(0.85f);
            break;
            
        case WindowDepth::MID_FRONT:
            SetAlpha(245);
            SetScale(0.95f);
            SetDepthOffset(2, 1);
            SetShadowIntensity(0.95f);
            break;
            
        case WindowDepth::FOREGROUND:
            SetAlpha(255);
            SetScale(1.0f);
            SetDepthOffset(0, 0);
            SetShadowIntensity(1.0f);
            break;
    }
}

void Window::UpdateLayeredWindow() {
    if (!IsValid()) return;
    
    // Set layered window attributes
    SetLayeredWindowAttributes(m_hwnd, 0, m_alpha, LWA_ALPHA);
}

// Widget management
void Window::AddWidget(std::shared_ptr<Widget> widget) {
    m_widgets.push_back(widget);
}

void Window::RemoveWidget(std::shared_ptr<Widget> widget) {
    auto it = std::find(m_widgets.begin(), m_widgets.end(), widget);
    if (it != m_widgets.end()) {
        m_widgets.erase(it);
    }
}

void Window::ClearWidgets() {
    m_widgets.clear();
}

// Widget input handling
bool Window::HandleWidgetMouseMove(int x, int y) {
    bool handled = false;
    for (auto& widget : m_widgets) {
        if (widget->HandleMouseMove(x, y)) {
            handled = true;
        }
    }
    return handled;
}

bool Window::HandleWidgetMouseDown(int x, int y, int button) {
    for (auto& widget : m_widgets) {
        if (widget->HandleMouseDown(x, y, button)) {
            return true;
        }
    }
    return false;
}

bool Window::HandleWidgetMouseUp(int x, int y, int button) {
    for (auto& widget : m_widgets) {
        if (widget->HandleMouseUp(x, y, button)) {
            return true;
        }
    }
    return false;
}

bool Window::HandleWidgetKeyDown(int keyCode) {
    for (auto& widget : m_widgets) {
        if (widget->HandleKeyDown(keyCode)) {
            return true;
        }
    }
    return false;
}

bool Window::HandleWidgetKeyUp(int keyCode) {
    for (auto& widget : m_widgets) {
        if (widget->HandleKeyUp(keyCode)) {
            return true;
        }
    }
    return false;
}

bool Window::HandleWidgetChar(wchar_t ch) {
    for (auto& widget : m_widgets) {
        if (widget->HandleChar(ch)) {
            return true;
        }
    }
    return false;
}

void Window::UpdateWidgets(float deltaTime) {
    for (auto& widget : m_widgets) {
        widget->Update(deltaTime);
    }
}

// DPI Support (v2.0)
DPIScaleInfo Window::GetDPIScale() const {
    return m_currentDPI;
}

void Window::HandleDPIChange(const DPIScaleInfo& oldDPI, const DPIScaleInfo& newDPI) {
    m_currentDPI = newDPI;
    
    // Scale all widgets
    float scaleFactorX = newDPI.scaleX / oldDPI.scaleX;
    float scaleFactorY = newDPI.scaleY / oldDPI.scaleY;
    
    for (auto& widget : m_widgets) {
        int x, y, width, height;
        widget->GetBounds(x, y, width, height);
        
        // Scale position and size with proper rounding
        x = static_cast<int>(x * scaleFactorX + 0.5f);
        y = static_cast<int>(y * scaleFactorY + 0.5f);
        width = static_cast<int>(width * scaleFactorX + 0.5f);
        height = static_cast<int>(height * scaleFactorY + 0.5f);
        
        widget->SetBounds(x, y, width, height);
    }
    
    // Update corner radius with DPI scaling
    if (m_roundedCorners) {
        m_cornerRadius = DPIManager::GetInstance().ScaleValueX(12, newDPI);
    }
    
    UpdateAppearance();
}

void Window::UpdateForDPI() {
    DPIScaleInfo newDPI = DPIManager::GetInstance().GetDPIForWindow(m_hwnd);
    if (newDPI.dpi != m_currentDPI.dpi) {
        HandleDPIChange(m_currentDPI, newDPI);
    }
}

// Monitor Support (v2.0)
HMONITOR Window::GetMonitor() const {
    return m_currentMonitor;
}

void Window::HandleMonitorChange(HMONITOR oldMonitor, HMONITOR newMonitor) {
    m_currentMonitor = newMonitor;
    
    // Update DPI for new monitor
    DPIScaleInfo oldDPI = m_currentDPI;
    m_currentDPI = DPIManager::GetInstance().GetDPIForMonitor(newMonitor);
    
    // Apply DPI changes if different
    if (oldDPI.dpi != m_currentDPI.dpi) {
        HandleDPIChange(oldDPI, m_currentDPI);
    }
    
    // Check for monitor-specific theme
    auto monitorTheme = MonitorManager::GetInstance().GetMonitorTheme(newMonitor);
    if (monitorTheme) {
        SetTheme(monitorTheme);
    }
    
    UpdateAppearance();
}

} // namespace SDK
