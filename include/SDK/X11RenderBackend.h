#pragma once

#include "Platform.h"

#if SDK_PLATFORM_LINUX && SDK_HAS_X11

#include "RenderBackend.h"
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <memory>
#include <map>

namespace SDK {

/**
 * X11RenderBackend - X11-based rendering backend for Linux
 * Implements the RenderBackend interface using X11/Xlib
 */
class X11RenderBackend : public RenderBackend {
public:
    X11RenderBackend();
    virtual ~X11RenderBackend();
    
    // RenderBackend interface implementation
    bool Initialize(HWND hwnd) override;
    void Shutdown() override;
    
    bool BeginDraw() override;
    void EndDraw() override;
    void Clear(Color color) override;
    
    HDC GetDC() const override;
    void* GetNativeContext() const override;
    
    void DrawRectangle(const RECT& rect, Color fillColor, Color borderColor, float borderWidth) override;
    void DrawRoundedRectangle(const RECT& rect, float radius, Color fillColor, Color borderColor, float borderWidth) override;
    void DrawLine(int x1, int y1, int x2, int y2, Color color, float width) override;
    void DrawEllipse(int cx, int cy, int rx, int ry, Color fillColor, Color borderColor, float borderWidth) override;
    
    void DrawText(const std::wstring& text, const RECT& rect, Color color, const std::wstring& fontFamily, float fontSize, int fontWeight) override;
    
    void DrawLinearGradient(const RECT& rect, Color startColor, Color endColor, bool horizontal) override;
    void DrawRadialGradient(const RECT& rect, Color centerColor, Color edgeColor, int cx, int cy) override;
    
    void DrawShadow(const RECT& rect, int offsetX, int offsetY, int blur, Color shadowColor) override;
    void DrawGlow(const RECT& rect, int radius, Color glowColor) override;
    
    bool SupportsGPUEffects() const override { return false; }
    void ApplyBlur(const RECT& rect, int blurRadius) override;
    void ApplyBloom(const RECT& rect, float threshold, float intensity) override;
    void ApplyDepthOfField(const RECT& rect, int focalDepth, int blurAmount, float focalRange) override;
    void ApplyMotionBlur(const RECT& rect, int directionX, int directionY, float intensity) override;
    void ApplyChromaticAberration(const RECT& rect, float strength, int offsetX, int offsetY) override;
    
    BackendType GetType() const override { return BackendType::GDI; }
    bool IsHardwareAccelerated() const override { return false; }
    Capabilities GetCapabilities() const override;
    
    // X11-specific methods
    Display* GetDisplay() const { return m_display; }
    Window GetWindow() const { return m_window; }
    GC GetGC() const { return m_gc; }
    
private:
    unsigned long ColorToPixel(const Color& color);
    void SetGCColor(const Color& color);
    XFontStruct* GetOrCreateFont(int fontSize);
    
    Display* m_display;
    Window m_window;
    HWND m_hwnd;
    GC m_gc;
    Pixmap m_backBuffer;
    int m_width;
    int m_height;
    
    // Font cache
    std::map<int, XFontStruct*> m_fontCache;
    
    bool m_initialized;
};

} // namespace SDK

#endif // SDK_PLATFORM_LINUX && SDK_HAS_X11
