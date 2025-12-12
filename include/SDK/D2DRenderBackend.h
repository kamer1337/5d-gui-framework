#pragma once

#include "RenderBackend.h"
#include <d2d1.h>
#include <dwrite.h>

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")

namespace SDK {

/**
 * D2DRenderBackend - Hardware accelerated rendering using Direct2D
 * Requires Windows 7 or later with Platform Update
 */
class D2DRenderBackend : public RenderBackend {
public:
    D2DRenderBackend();
    ~D2DRenderBackend() override;
    
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
    
    bool SupportsGPUEffects() const override { return true; }
    void ApplyBlur(const RECT& rect, int blurRadius) override;
    void ApplyBloom(const RECT& rect, float threshold, float intensity) override;
    
    BackendType GetType() const override { return BackendType::DIRECT2D; }
    bool IsHardwareAccelerated() const override { return true; }
    Capabilities GetCapabilities() const override;
    
private:
    // Helper to convert SDK Color to D2D1_COLOR_F
    D2D1_COLOR_F ToD2DColor(Color color) const;
    
    // Helper to convert RECT to D2D1_RECT_F
    D2D1_RECT_F ToD2DRect(const RECT& rect) const;
    
    // Create brush from color
    ID2D1SolidColorBrush* GetBrush(Color color);
    
    HWND m_hwnd;
    ID2D1Factory* m_pD2DFactory;
    ID2D1HwndRenderTarget* m_pRenderTarget;
    IDWriteFactory* m_pDWriteFactory;
    
    // Cached brushes for performance
    ID2D1SolidColorBrush* m_pCachedBrush;
    Color m_cachedBrushColor;
    
    // Effects
    ID2D1Effect* m_pBlurEffect;
    ID2D1Effect* m_pBloomEffect;
};

} // namespace SDK
