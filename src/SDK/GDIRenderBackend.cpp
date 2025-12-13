#include "../../include/SDK/GDIRenderBackend.h"
#include "../../include/SDK/Renderer.h"
#include <vector>
#include <algorithm>

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

void GDIRenderBackend::ApplyDepthOfField(const RECT& rect, int focalDepth, int blurAmount, float focalRange) {
    if (!m_memDC) return;
    
    // Software depth of field implementation
    // Create a depth-based blur effect where areas outside focal range are blurred
    int centerY = (rect.top + rect.bottom) / 2;
    int height = rect.bottom - rect.top;
    
    // Calculate blur based on distance from focal depth
    for (int y = rect.top; y < rect.bottom; y++) {
        int distanceFromFocus = abs(y - (rect.top + focalDepth));
        float blurFactor = (float)distanceFromFocus / focalRange;
        blurFactor = min(blurFactor, 1.0f);
        
        int currentBlur = (int)(blurAmount * blurFactor);
        if (currentBlur > 0) {
            RECT lineRect = { rect.left, y, rect.right, y + 1 };
            Renderer::ApplyBlur(m_memDC, lineRect, currentBlur);
        }
    }
}

void GDIRenderBackend::ApplyMotionBlur(const RECT& rect, int directionX, int directionY, float intensity) {
    if (!m_memDC) return;
    
    // Software motion blur implementation
    // Create directional blur by sampling pixels along motion vector
    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;
    
    // Create temporary bitmap for motion blur
    HDC tempDC = CreateCompatibleDC(m_memDC);
    HBITMAP tempBitmap = CreateCompatibleBitmap(m_memDC, width, height);
    HBITMAP oldBitmap = (HBITMAP)SelectObject(tempDC, tempBitmap);
    
    // Copy original image
    BitBlt(tempDC, 0, 0, width, height, m_memDC, rect.left, rect.top, SRCCOPY);
    
    // Apply motion blur by blending multiple shifted copies
    int samples = 5;
    BLENDFUNCTION blend = { AC_SRC_OVER, 0, (BYTE)(255 * intensity / samples), 0 };
    
    for (int i = 0; i < samples; i++) {
        int offsetX = (directionX * i) / samples;
        int offsetY = (directionY * i) / samples;
        
        AlphaBlend(m_memDC, rect.left + offsetX, rect.top + offsetY, width, height,
                   tempDC, 0, 0, width, height, blend);
    }
    
    // Cleanup
    SelectObject(tempDC, oldBitmap);
    DeleteObject(tempBitmap);
    DeleteDC(tempDC);
}

void GDIRenderBackend::ApplyChromaticAberration(const RECT& rect, float strength, int offsetX, int offsetY) {
    if (!m_memDC) return;
    
    // Software chromatic aberration implementation
    // Shift color channels to simulate lens distortion
    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;
    
    // Get bitmap bits for direct manipulation
    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = -height; // Top-down
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    
    std::vector<BYTE> pixels(width * height * 4);
    GetDIBits(m_memDC, m_memBitmap, rect.top, height, pixels.data(), &bmi, DIB_RGB_COLORS);
    
    // Apply chromatic aberration by offsetting RGB channels
    std::vector<BYTE> result = pixels;
    
    int scaledOffsetX = (int)(offsetX * strength);
    int scaledOffsetY = (int)(offsetY * strength);
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int idx = (y * width + x) * 4;
            
            // Red channel - shift in one direction
            int redX = x + scaledOffsetX;
            int redY = y;
            if (redX >= 0 && redX < width && redY >= 0 && redY < height) {
                int redIdx = (redY * width + redX) * 4;
                result[idx + 2] = pixels[redIdx + 2]; // R
            }
            
            // Blue channel - shift in opposite direction
            int blueX = x - scaledOffsetX;
            int blueY = y;
            if (blueX >= 0 && blueX < width && blueY >= 0 && blueY < height) {
                int blueIdx = (blueY * width + blueX) * 4;
                result[idx + 0] = pixels[blueIdx + 0]; // B
            }
            
            // Green channel - no shift (center)
            result[idx + 1] = pixels[idx + 1]; // G
        }
    }
    
    // Set modified pixels back
    SetDIBits(m_memDC, m_memBitmap, rect.top, height, result.data(), &bmi, DIB_RGB_COLORS);
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
