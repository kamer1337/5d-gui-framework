#include "../../include/SDK/GDIRenderBackend.h"
#include "../../include/SDK/Renderer.h"

namespace SDK {

GDIRenderBackend::GDIRenderBackend()
    : m_hwnd(nullptr)
    , m_hdc(nullptr)
    , m_memDC(nullptr)
    , m_memBitmap(nullptr)
    , m_oldBitmap(nullptr)
    , m_width(0)
    , m_height(0)
    , m_isDrawing(false)
{
}

GDIRenderBackend::~GDIRenderBackend() {
    Shutdown();
}

bool GDIRenderBackend::Initialize(HWND hwnd) {
    m_hwnd = hwnd;
    
    // Get window dimensions
    RECT rc;
    GetClientRect(hwnd, &rc);
    m_width = rc.right - rc.left;
    m_height = rc.bottom - rc.top;
    
    // Get window DC
    m_hdc = GetDC(hwnd);
    if (!m_hdc) {
        return false;
    }
    
    // Create memory DC for double buffering
    m_memDC = CreateCompatibleDC(m_hdc);
    if (!m_memDC) {
        ReleaseDC(hwnd, m_hdc);
        m_hdc = nullptr;
        return false;
    }
    
    // Create memory bitmap
    m_memBitmap = CreateCompatibleBitmap(m_hdc, m_width, m_height);
    if (!m_memBitmap) {
        DeleteDC(m_memDC);
        ReleaseDC(hwnd, m_hdc);
        m_memDC = nullptr;
        m_hdc = nullptr;
        return false;
    }
    
    m_oldBitmap = (HBITMAP)SelectObject(m_memDC, m_memBitmap);
    
    return true;
}

void GDIRenderBackend::Shutdown() {
    if (m_memDC) {
        if (m_oldBitmap) {
            SelectObject(m_memDC, m_oldBitmap);
            m_oldBitmap = nullptr;
        }
        DeleteDC(m_memDC);
        m_memDC = nullptr;
    }
    
    if (m_memBitmap) {
        DeleteObject(m_memBitmap);
        m_memBitmap = nullptr;
    }
    
    if (m_hdc && m_hwnd) {
        ReleaseDC(m_hwnd, m_hdc);
        m_hdc = nullptr;
    }
    
    m_hwnd = nullptr;
}

bool GDIRenderBackend::BeginDraw() {
    m_isDrawing = true;
    return true;
}

void GDIRenderBackend::EndDraw() {
    if (m_isDrawing && m_memDC && m_hdc) {
        // Blit from memory DC to window DC
        BitBlt(m_hdc, 0, 0, m_width, m_height, m_memDC, 0, 0, SRCCOPY);
    }
    m_isDrawing = false;
}

void GDIRenderBackend::Clear(Color color) {
    if (!m_memDC) return;
    
    RECT rc = { 0, 0, m_width, m_height };
    HBRUSH brush = CreateSolidBrush(RGB(color.r, color.g, color.b));
    FillRect(m_memDC, &rc, brush);
    DeleteObject(brush);
}

HDC GDIRenderBackend::GetDC() const {
    return m_memDC;
}

void* GDIRenderBackend::GetNativeContext() const {
    return (void*)m_memDC;
}

void GDIRenderBackend::DrawRectangle(const RECT& rect, Color fillColor, Color borderColor, float borderWidth) {
    if (!m_memDC) return;
    
    HBRUSH brush = CreateSolidBrush(RGB(fillColor.r, fillColor.g, fillColor.b));
    HPEN pen = CreatePen(PS_SOLID, (int)borderWidth, RGB(borderColor.r, borderColor.g, borderColor.b));
    
    HBRUSH oldBrush = (HBRUSH)SelectObject(m_memDC, brush);
    HPEN oldPen = (HPEN)SelectObject(m_memDC, pen);
    
    Rectangle(m_memDC, rect.left, rect.top, rect.right, rect.bottom);
    
    SelectObject(m_memDC, oldPen);
    SelectObject(m_memDC, oldBrush);
    DeleteObject(pen);
    DeleteObject(brush);
}

void GDIRenderBackend::DrawRoundedRectangle(const RECT& rect, float radius, Color fillColor, Color borderColor, float borderWidth) {
    if (!m_memDC) return;
    
    // Use existing Renderer implementation for rounded rectangles
    Renderer::DrawRoundedRect(m_memDC, rect, (int)radius, fillColor, borderColor, (int)borderWidth);
}

void GDIRenderBackend::DrawLine(int x1, int y1, int x2, int y2, Color color, float width) {
    if (!m_memDC) return;
    
    HPEN pen = CreatePen(PS_SOLID, (int)width, RGB(color.r, color.g, color.b));
    HPEN oldPen = (HPEN)SelectObject(m_memDC, pen);
    
    MoveToEx(m_memDC, x1, y1, nullptr);
    LineTo(m_memDC, x2, y2);
    
    SelectObject(m_memDC, oldPen);
    DeleteObject(pen);
}

void GDIRenderBackend::DrawEllipse(int cx, int cy, int rx, int ry, Color fillColor, Color borderColor, float borderWidth) {
    if (!m_memDC) return;
    
    HBRUSH brush = CreateSolidBrush(RGB(fillColor.r, fillColor.g, fillColor.b));
    HPEN pen = CreatePen(PS_SOLID, (int)borderWidth, RGB(borderColor.r, borderColor.g, borderColor.b));
    
    HBRUSH oldBrush = (HBRUSH)SelectObject(m_memDC, brush);
    HPEN oldPen = (HPEN)SelectObject(m_memDC, pen);
    
    Ellipse(m_memDC, cx - rx, cy - ry, cx + rx, cy + ry);
    
    SelectObject(m_memDC, oldPen);
    SelectObject(m_memDC, oldBrush);
    DeleteObject(pen);
    DeleteObject(brush);
}

void GDIRenderBackend::DrawText(const std::wstring& text, const RECT& rect, Color color, const std::wstring& fontFamily, float fontSize, int fontWeight) {
    if (!m_memDC) return;
    
    HFONT font = CreateFontW(
        (int)fontSize, 0, 0, 0, fontWeight,
        FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
        fontFamily.c_str()
    );
    
    HFONT oldFont = (HFONT)SelectObject(m_memDC, font);
    SetTextColor(m_memDC, RGB(color.r, color.g, color.b));
    SetBkMode(m_memDC, TRANSPARENT);
    
    DrawTextW(m_memDC, text.c_str(), -1, (LPRECT)&rect, DT_LEFT | DT_TOP | DT_WORDBREAK);
    
    SelectObject(m_memDC, oldFont);
    DeleteObject(font);
}

void GDIRenderBackend::DrawLinearGradient(const RECT& rect, Color startColor, Color endColor, bool horizontal) {
    if (!m_memDC) return;
    
    if (horizontal) {
        Renderer::DrawHorizontalGradient(m_memDC, rect, startColor, endColor);
    } else {
        Renderer::DrawVerticalGradient(m_memDC, rect, startColor, endColor);
    }
}

void GDIRenderBackend::DrawRadialGradient(const RECT& rect, Color centerColor, Color edgeColor, int cx, int cy) {
    if (!m_memDC) return;
    
    Renderer::DrawRadialGradient(m_memDC, rect, centerColor, edgeColor, cx, cy);
}

void GDIRenderBackend::DrawShadow(const RECT& rect, int offsetX, int offsetY, int blur, Color shadowColor) {
    if (!m_memDC) return;
    
    Renderer::DrawShadow(m_memDC, rect, offsetX, offsetY, blur, shadowColor);
}

void GDIRenderBackend::DrawGlow(const RECT& rect, int radius, Color glowColor) {
    if (!m_memDC) return;
    
    Renderer::DrawGlow(m_memDC, rect, radius, glowColor);
}

void GDIRenderBackend::ApplyBlur(const RECT& rect, int blurRadius) {
    if (!m_memDC) return;
    
    // Software blur using existing Renderer implementation
    Renderer::ApplyBlur(m_memDC, rect, blurRadius);
}

void GDIRenderBackend::ApplyBloom(const RECT& rect, float threshold, float intensity) {
    if (!m_memDC) return;
    
    // Software bloom using existing Renderer implementation
    Renderer::ApplyBloom(m_memDC, rect, threshold, intensity);
}

RenderBackend::Capabilities GDIRenderBackend::GetCapabilities() const {
    Capabilities caps;
    caps.supportsGPUAcceleration = false;
    caps.supportsAdvancedEffects = true;  // Software effects
    caps.supportsAntialiasing = true;     // Software antialiasing
    caps.supportsTransparency = true;
    caps.maxTextureSize = 16384;          // Typical GDI limit
    return caps;
}

} // namespace SDK
