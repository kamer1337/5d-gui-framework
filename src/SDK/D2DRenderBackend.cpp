#include "../../include/SDK/D2DRenderBackend.h"
#include <d2d1_1.h>
#include <d2d1effects.h>

namespace SDK {

D2DRenderBackend::D2DRenderBackend()
    : m_hwnd(nullptr)
    , m_pD2DFactory(nullptr)
    , m_pRenderTarget(nullptr)
    , m_pDWriteFactory(nullptr)
    , m_pCachedBrush(nullptr)
    , m_cachedBrushColor{0, 0, 0, 255}
    , m_pBlurEffect(nullptr)
    , m_pBloomEffect(nullptr)
{
}

D2DRenderBackend::~D2DRenderBackend() {
    Shutdown();
}

bool D2DRenderBackend::Initialize(HWND hwnd) {
    m_hwnd = hwnd;
    
    // Create D2D factory
    HRESULT hr = D2D1CreateFactory(
        D2D1_FACTORY_TYPE_SINGLE_THREADED,
        &m_pD2DFactory
    );
    
    if (FAILED(hr)) {
        return false;
    }
    
    // Get window size
    RECT rc;
    GetClientRect(hwnd, &rc);
    
    D2D1_SIZE_U size = D2D1::SizeU(
        rc.right - rc.left,
        rc.bottom - rc.top
    );
    
    // Create render target
    hr = m_pD2DFactory->CreateHwndRenderTarget(
        D2D1::RenderTargetProperties(),
        D2D1::HwndRenderTargetProperties(hwnd, size),
        &m_pRenderTarget
    );
    
    if (FAILED(hr)) {
        m_pD2DFactory->Release();
        m_pD2DFactory = nullptr;
        return false;
    }
    
    // Enable antialiasing
    m_pRenderTarget->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
    m_pRenderTarget->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_CLEARTYPE);
    
    // Create DWrite factory for text rendering
    hr = DWriteCreateFactory(
        DWRITE_FACTORY_TYPE_SHARED,
        __uuidof(IDWriteFactory),
        reinterpret_cast<IUnknown**>(&m_pDWriteFactory)
    );
    
    if (FAILED(hr)) {
        m_pRenderTarget->Release();
        m_pD2DFactory->Release();
        m_pRenderTarget = nullptr;
        m_pD2DFactory = nullptr;
        return false;
    }
    
    // Try to create effects (requires D2D1.1+, may fail on Windows 7)
    ID2D1DeviceContext* pDeviceContext = nullptr;
    hr = m_pRenderTarget->QueryInterface(&pDeviceContext);
    if (SUCCEEDED(hr) && pDeviceContext) {
        // Create blur effect
        pDeviceContext->CreateEffect(CLSID_D2D1GaussianBlur, &m_pBlurEffect);
        
        // Note: Bloom is not a built-in effect, would need custom implementation
        // For now, we'll implement it in software when needed
        
        pDeviceContext->Release();
    }
    
    return true;
}

void D2DRenderBackend::Shutdown() {
    if (m_pBloomEffect) {
        m_pBloomEffect->Release();
        m_pBloomEffect = nullptr;
    }
    
    if (m_pBlurEffect) {
        m_pBlurEffect->Release();
        m_pBlurEffect = nullptr;
    }
    
    if (m_pCachedBrush) {
        m_pCachedBrush->Release();
        m_pCachedBrush = nullptr;
    }
    
    if (m_pDWriteFactory) {
        m_pDWriteFactory->Release();
        m_pDWriteFactory = nullptr;
    }
    
    if (m_pRenderTarget) {
        m_pRenderTarget->Release();
        m_pRenderTarget = nullptr;
    }
    
    if (m_pD2DFactory) {
        m_pD2DFactory->Release();
        m_pD2DFactory = nullptr;
    }
    
    m_hwnd = nullptr;
}

bool D2DRenderBackend::BeginDraw() {
    if (!m_pRenderTarget) {
        return false;
    }
    
    m_pRenderTarget->BeginDraw();
    return true;
}

void D2DRenderBackend::EndDraw() {
    if (m_pRenderTarget) {
        HRESULT hr = m_pRenderTarget->EndDraw();
        
        // Handle device loss
        if (hr == D2DERR_RECREATE_TARGET) {
            // Re-create the render target
            if (m_pCachedBrush) {
                m_pCachedBrush->Release();
                m_pCachedBrush = nullptr;
            }
            
            m_pRenderTarget->Release();
            m_pRenderTarget = nullptr;
            
            // Re-initialize
            Initialize(m_hwnd);
        }
    }
}

void D2DRenderBackend::Clear(Color color) {
    if (m_pRenderTarget) {
        m_pRenderTarget->Clear(ToD2DColor(color));
    }
}

HDC D2DRenderBackend::GetDC() const {
    // D2D doesn't use HDC, but we can get one if needed for interop
    if (m_pRenderTarget) {
        HDC hdc = nullptr;
        m_pRenderTarget->QueryInterface(__uuidof(ID2D1GdiInteropRenderTarget), (void**)&hdc);
        return hdc;
    }
    return nullptr;
}

void* D2DRenderBackend::GetNativeContext() const {
    return (void*)m_pRenderTarget;
}

D2D1_COLOR_F D2DRenderBackend::ToD2DColor(Color color) const {
    return D2D1::ColorF(
        color.r / 255.0f,
        color.g / 255.0f,
        color.b / 255.0f,
        color.a / 255.0f
    );
}

D2D1_RECT_F D2DRenderBackend::ToD2DRect(const RECT& rect) const {
    return D2D1::RectF(
        (float)rect.left,
        (float)rect.top,
        (float)rect.right,
        (float)rect.bottom
    );
}

ID2D1SolidColorBrush* D2DRenderBackend::GetBrush(Color color) {
    // Reuse cached brush if color matches
    if (m_pCachedBrush && 
        m_cachedBrushColor.r == color.r &&
        m_cachedBrushColor.g == color.g &&
        m_cachedBrushColor.b == color.b &&
        m_cachedBrushColor.a == color.a) {
        return m_pCachedBrush;
    }
    
    // Release old brush
    if (m_pCachedBrush) {
        m_pCachedBrush->Release();
        m_pCachedBrush = nullptr;
    }
    
    // Create new brush
    if (m_pRenderTarget) {
        m_pRenderTarget->CreateSolidColorBrush(ToD2DColor(color), &m_pCachedBrush);
        m_cachedBrushColor = color;
    }
    
    return m_pCachedBrush;
}

void D2DRenderBackend::DrawRectangle(const RECT& rect, Color fillColor, Color borderColor, float borderWidth) {
    if (!m_pRenderTarget) return;
    
    D2D1_RECT_F d2dRect = ToD2DRect(rect);
    
    // Fill
    if (fillColor.a > 0) {
        ID2D1SolidColorBrush* brush = GetBrush(fillColor);
        if (brush) {
            m_pRenderTarget->FillRectangle(d2dRect, brush);
        }
    }
    
    // Border
    if (borderWidth > 0 && borderColor.a > 0) {
        ID2D1SolidColorBrush* brush = GetBrush(borderColor);
        if (brush) {
            m_pRenderTarget->DrawRectangle(d2dRect, brush, borderWidth);
        }
    }
}

void D2DRenderBackend::DrawRoundedRectangle(const RECT& rect, float radius, Color fillColor, Color borderColor, float borderWidth) {
    if (!m_pRenderTarget) return;
    
    D2D1_ROUNDED_RECT roundedRect = D2D1::RoundedRect(
        ToD2DRect(rect),
        radius,
        radius
    );
    
    // Fill
    if (fillColor.a > 0) {
        ID2D1SolidColorBrush* brush = GetBrush(fillColor);
        if (brush) {
            m_pRenderTarget->FillRoundedRectangle(roundedRect, brush);
        }
    }
    
    // Border
    if (borderWidth > 0 && borderColor.a > 0) {
        ID2D1SolidColorBrush* brush = GetBrush(borderColor);
        if (brush) {
            m_pRenderTarget->DrawRoundedRectangle(roundedRect, brush, borderWidth);
        }
    }
}

void D2DRenderBackend::DrawLine(int x1, int y1, int x2, int y2, Color color, float width) {
    if (!m_pRenderTarget) return;
    
    ID2D1SolidColorBrush* brush = GetBrush(color);
    if (brush) {
        m_pRenderTarget->DrawLine(
            D2D1::Point2F((float)x1, (float)y1),
            D2D1::Point2F((float)x2, (float)y2),
            brush,
            width
        );
    }
}

void D2DRenderBackend::DrawEllipse(int cx, int cy, int rx, int ry, Color fillColor, Color borderColor, float borderWidth) {
    if (!m_pRenderTarget) return;
    
    D2D1_ELLIPSE ellipse = D2D1::Ellipse(
        D2D1::Point2F((float)cx, (float)cy),
        (float)rx,
        (float)ry
    );
    
    // Fill
    if (fillColor.a > 0) {
        ID2D1SolidColorBrush* brush = GetBrush(fillColor);
        if (brush) {
            m_pRenderTarget->FillEllipse(ellipse, brush);
        }
    }
    
    // Border
    if (borderWidth > 0 && borderColor.a > 0) {
        ID2D1SolidColorBrush* brush = GetBrush(borderColor);
        if (brush) {
            m_pRenderTarget->DrawEllipse(ellipse, brush, borderWidth);
        }
    }
}

void D2DRenderBackend::DrawText(const std::wstring& text, const RECT& rect, Color color, const std::wstring& fontFamily, float fontSize, int fontWeight) {
    if (!m_pRenderTarget || !m_pDWriteFactory) return;
    
    // Create text format
    IDWriteTextFormat* pTextFormat = nullptr;
    HRESULT hr = m_pDWriteFactory->CreateTextFormat(
        fontFamily.c_str(),
        nullptr,
        (DWRITE_FONT_WEIGHT)fontWeight,
        DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL,
        fontSize,
        L"en-us",
        &pTextFormat
    );
    
    if (SUCCEEDED(hr) && pTextFormat) {
        ID2D1SolidColorBrush* brush = GetBrush(color);
        if (brush) {
            m_pRenderTarget->DrawText(
                text.c_str(),
                (UINT32)text.length(),
                pTextFormat,
                ToD2DRect(rect),
                brush
            );
        }
        
        pTextFormat->Release();
    }
}

void D2DRenderBackend::DrawLinearGradient(const RECT& rect, Color startColor, Color endColor, bool horizontal) {
    if (!m_pRenderTarget) return;
    
    D2D1_GRADIENT_STOP gradientStops[2];
    gradientStops[0].color = ToD2DColor(startColor);
    gradientStops[0].position = 0.0f;
    gradientStops[1].color = ToD2DColor(endColor);
    gradientStops[1].position = 1.0f;
    
    ID2D1GradientStopCollection* pGradientStops = nullptr;
    HRESULT hr = m_pRenderTarget->CreateGradientStopCollection(
        gradientStops,
        2,
        D2D1_GAMMA_2_2,
        D2D1_EXTEND_MODE_CLAMP,
        &pGradientStops
    );
    
    if (SUCCEEDED(hr) && pGradientStops) {
        ID2D1LinearGradientBrush* pBrush = nullptr;
        
        D2D1_POINT_2F start, end;
        if (horizontal) {
            start = D2D1::Point2F((float)rect.left, (float)rect.top);
            end = D2D1::Point2F((float)rect.right, (float)rect.top);
        } else {
            start = D2D1::Point2F((float)rect.left, (float)rect.top);
            end = D2D1::Point2F((float)rect.left, (float)rect.bottom);
        }
        
        hr = m_pRenderTarget->CreateLinearGradientBrush(
            D2D1::LinearGradientBrushProperties(start, end),
            pGradientStops,
            &pBrush
        );
        
        if (SUCCEEDED(hr) && pBrush) {
            m_pRenderTarget->FillRectangle(ToD2DRect(rect), pBrush);
            pBrush->Release();
        }
        
        pGradientStops->Release();
    }
}

void D2DRenderBackend::DrawRadialGradient(const RECT& rect, Color centerColor, Color edgeColor, int cx, int cy) {
    if (!m_pRenderTarget) return;
    
    D2D1_GRADIENT_STOP gradientStops[2];
    gradientStops[0].color = ToD2DColor(centerColor);
    gradientStops[0].position = 0.0f;
    gradientStops[1].color = ToD2DColor(edgeColor);
    gradientStops[1].position = 1.0f;
    
    ID2D1GradientStopCollection* pGradientStops = nullptr;
    HRESULT hr = m_pRenderTarget->CreateGradientStopCollection(
        gradientStops,
        2,
        D2D1_GAMMA_2_2,
        D2D1_EXTEND_MODE_CLAMP,
        &pGradientStops
    );
    
    if (SUCCEEDED(hr) && pGradientStops) {
        ID2D1RadialGradientBrush* pBrush = nullptr;
        
        float radiusX = (float)(rect.right - rect.left) / 2.0f;
        float radiusY = (float)(rect.bottom - rect.top) / 2.0f;
        
        hr = m_pRenderTarget->CreateRadialGradientBrush(
            D2D1::RadialGradientBrushProperties(
                D2D1::Point2F((float)cx, (float)cy),
                D2D1::Point2F(0, 0),
                radiusX,
                radiusY
            ),
            pGradientStops,
            &pBrush
        );
        
        if (SUCCEEDED(hr) && pBrush) {
            m_pRenderTarget->FillRectangle(ToD2DRect(rect), pBrush);
            pBrush->Release();
        }
        
        pGradientStops->Release();
    }
}

void D2DRenderBackend::DrawShadow(const RECT& rect, int offsetX, int offsetY, int blur, Color shadowColor) {
    if (!m_pRenderTarget) return;
    
    // Create shadow effect using D2D shadow effect if available
    ID2D1DeviceContext* pDeviceContext = nullptr;
    HRESULT hr = m_pRenderTarget->QueryInterface(&pDeviceContext);
    
    if (SUCCEEDED(hr) && pDeviceContext) {
        ID2D1Effect* pShadowEffect = nullptr;
        hr = pDeviceContext->CreateEffect(CLSID_D2D1Shadow, &pShadowEffect);
        
        if (SUCCEEDED(hr) && pShadowEffect) {
            // Set shadow properties
            pShadowEffect->SetValue(D2D1_SHADOW_PROP_BLUR_STANDARD_DEVIATION, (float)blur);
            pShadowEffect->SetValue(D2D1_SHADOW_PROP_COLOR, ToD2DColor(shadowColor));
            
            // Apply shadow (would need bitmap input)
            // For now, fallback to simple rectangle
            RECT shadowRect = rect;
            OffsetRect(&shadowRect, offsetX, offsetY);
            
            ID2D1SolidColorBrush* brush = GetBrush(shadowColor);
            if (brush) {
                m_pRenderTarget->FillRectangle(ToD2DRect(shadowRect), brush);
            }
            
            pShadowEffect->Release();
        }
        
        pDeviceContext->Release();
    } else {
        // Fallback to simple shadow
        RECT shadowRect = rect;
        OffsetRect(&shadowRect, offsetX, offsetY);
        
        ID2D1SolidColorBrush* brush = GetBrush(shadowColor);
        if (brush) {
            m_pRenderTarget->FillRectangle(ToD2DRect(shadowRect), brush);
        }
    }
}

void D2DRenderBackend::DrawGlow(const RECT& rect, int radius, Color glowColor) {
    if (!m_pRenderTarget) return;
    
    // Draw multiple layers with decreasing opacity for glow effect
    for (int i = radius; i > 0; i--) {
        RECT glowRect = rect;
        InflateRect(&glowRect, i, i);
        
        Color layerColor = glowColor;
        layerColor.a = (uint8_t)(glowColor.a * (1.0f - (float)i / radius));
        
        ID2D1SolidColorBrush* brush = GetBrush(layerColor);
        if (brush) {
            m_pRenderTarget->FillRectangle(ToD2DRect(glowRect), brush);
        }
    }
}

void D2DRenderBackend::ApplyBlur(const RECT& rect, int blurRadius) {
    if (!m_pRenderTarget || !m_pBlurEffect) return;
    
    // Set blur parameters
    m_pBlurEffect->SetValue(D2D1_GAUSSIANBLUR_PROP_STANDARD_DEVIATION, (float)blurRadius);
    
    // Apply blur (would need bitmap as input)
    // This is a simplified version - full implementation would capture rect as bitmap first
}

void D2DRenderBackend::ApplyBloom(const RECT& rect, float threshold, float intensity) {
    // Bloom requires custom implementation
    // Would involve: extract bright pixels -> blur -> composite
    // For now, this is a placeholder that could be implemented later
}

RenderBackend::Capabilities D2DRenderBackend::GetCapabilities() const {
    Capabilities caps;
    caps.supportsGPUAcceleration = true;
    caps.supportsAdvancedEffects = (m_pBlurEffect != nullptr);
    caps.supportsAntialiasing = true;
    caps.supportsTransparency = true;
    caps.maxTextureSize = 16384;  // D2D typical limit
    return caps;
}

} // namespace SDK
