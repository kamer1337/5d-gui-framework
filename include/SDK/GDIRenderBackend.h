#pragma once

#include "RenderBackend.h"

namespace SDK {

/**
 * GDIRenderBackend - Software rendering using GDI
 * Fallback backend that works on all Windows versions
 */
class GDIRenderBackend : public RenderBackend {
public:
    GDIRenderBackend();
    ~GDIRenderBackend() override;
    
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
    
    BackendType GetType() const override { return BackendType::GDI; }
    bool IsHardwareAccelerated() const override { return false; }
    Capabilities GetCapabilities() const override;
    
private:
    HWND m_hwnd;
    HDC m_hdc;
    HDC m_memDC;
    HBITMAP m_memBitmap;
    HBITMAP m_oldBitmap;
    int m_width;
    int m_height;
    bool m_isDrawing;
};

} // namespace SDK
