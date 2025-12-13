#include "SDK/X11RenderBackend.h"

#if SDK_PLATFORM_LINUX && SDK_HAS_X11

#include <cmath>
#include <cstring>
#include <algorithm>
#include <locale>
#include <cwchar>

namespace SDK {

// Helper function to convert wstring to UTF-8 (replaces deprecated std::wstring_convert)
static std::string WStringToUTF8(const std::wstring& wstr) {
    if (wstr.empty()) {
        return std::string();
    }
    
    std::string result;
    result.reserve(wstr.size() * 4); // Reserve worst case (4 bytes per UTF-8 char)
    
    for (wchar_t wc : wstr) {
        if (wc < 0x80) {
            // ASCII range (0x00-0x7F)
            result.push_back(static_cast<char>(wc));
        } else if (wc < 0x800) {
            // 2-byte UTF-8 (0x80-0x7FF)
            result.push_back(static_cast<char>(0xC0 | ((wc >> 6) & 0x1F)));
            result.push_back(static_cast<char>(0x80 | (wc & 0x3F)));
        } else if (wc < 0x10000) {
            // 3-byte UTF-8 (0x800-0xFFFF)
            result.push_back(static_cast<char>(0xE0 | ((wc >> 12) & 0x0F)));
            result.push_back(static_cast<char>(0x80 | ((wc >> 6) & 0x3F)));
            result.push_back(static_cast<char>(0x80 | (wc & 0x3F)));
        } else {
            // 4-byte UTF-8 (0x10000-0x10FFFF)
            result.push_back(static_cast<char>(0xF0 | ((wc >> 18) & 0x07)));
            result.push_back(static_cast<char>(0x80 | ((wc >> 12) & 0x3F)));
            result.push_back(static_cast<char>(0x80 | ((wc >> 6) & 0x3F)));
            result.push_back(static_cast<char>(0x80 | (wc & 0x3F)));
        }
    }
    
    return result;
}

X11RenderBackend::X11RenderBackend()
    : m_display(nullptr)
    , m_window(0)
    , m_hwnd(nullptr)
    , m_gc(nullptr)
    , m_backBuffer(0)
    , m_width(0)
    , m_height(0)
    , m_initialized(false)
{
}

X11RenderBackend::~X11RenderBackend()
{
    Shutdown();
}

bool X11RenderBackend::Initialize(HWND hwnd)
{
    if (m_initialized) {
        return true;
    }
    
    m_hwnd = hwnd;
    m_window = (Window)hwnd;
    
    // Connect to X server
    m_display = XOpenDisplay(nullptr);
    if (!m_display) {
        return false;
    }
    
    // Create GC
    m_gc = XCreateGC(m_display, m_window, 0, nullptr);
    if (!m_gc) {
        XCloseDisplay(m_display);
        m_display = nullptr;
        return false;
    }
    
    // Get window dimensions
    XWindowAttributes attrs;
    if (XGetWindowAttributes(m_display, m_window, &attrs) != 0) {
        m_width = attrs.width;
        m_height = attrs.height;
        
        // Create back buffer
        m_backBuffer = XCreatePixmap(m_display, m_window, m_width, m_height, attrs.depth);
    }
    
    m_initialized = true;
    return true;
}

void X11RenderBackend::Shutdown()
{
    if (!m_initialized) {
        return;
    }
    
    // Free fonts
    for (auto& pair : m_fontCache) {
        if (pair.second) {
            XFreeFont(m_display, pair.second);
        }
    }
    m_fontCache.clear();
    
    // Free back buffer
    if (m_backBuffer && m_display) {
        XFreePixmap(m_display, m_backBuffer);
        m_backBuffer = 0;
    }
    
    // Free GC
    if (m_gc && m_display) {
        XFreeGC(m_display, m_gc);
        m_gc = nullptr;
    }
    
    // Close display
    if (m_display) {
        XCloseDisplay(m_display);
        m_display = nullptr;
    }
    
    m_initialized = false;
}

bool X11RenderBackend::BeginDraw()
{
    if (!m_initialized || !m_display) {
        return false;
    }
    
    // Get current window dimensions
    XWindowAttributes attrs;
    if (XGetWindowAttributes(m_display, m_window, &attrs) == 0) {
        return false;
    }
    
    // Recreate back buffer if dimensions changed
    if (attrs.width != m_width || attrs.height != m_height) {
        m_width = attrs.width;
        m_height = attrs.height;
        
        if (m_backBuffer) {
            XFreePixmap(m_display, m_backBuffer);
        }
        m_backBuffer = XCreatePixmap(m_display, m_window, m_width, m_height, attrs.depth);
    }
    
    return true;
}

void X11RenderBackend::EndDraw()
{
    if (!m_initialized || !m_display || !m_window || !m_backBuffer) {
        return;
    }
    
    // Copy back buffer to window
    XCopyArea(m_display, m_backBuffer, m_window, m_gc, 0, 0, m_width, m_height, 0, 0);
    XFlush(m_display);
}

void X11RenderBackend::Clear(Color color)
{
    if (!m_initialized || !m_display || !m_backBuffer || !m_gc) {
        return;
    }
    
    SetGCColor(color);
    XFillRectangle(m_display, m_backBuffer, m_gc, 0, 0, m_width, m_height);
}

HDC X11RenderBackend::GetDC() const
{
    // Return a pseudo-DC (the window handle) for compatibility
    return reinterpret_cast<HDC>(m_window);
}

void* X11RenderBackend::GetNativeContext() const
{
    return m_gc;
}

void X11RenderBackend::DrawRectangle(const RECT& rect, Color fillColor, Color borderColor, float borderWidth)
{
    if (!m_initialized || !m_display || !m_backBuffer || !m_gc) {
        return;
    }
    
    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;
    
    // Fill if needed
    if (fillColor.a > 0) {
        SetGCColor(fillColor);
        XFillRectangle(m_display, m_backBuffer, m_gc, rect.left, rect.top, width, height);
    }
    
    // Border if needed
    if (borderColor.a > 0 && borderWidth > 0) {
        SetGCColor(borderColor);
        XSetLineAttributes(m_display, m_gc, borderWidth, LineSolid, CapRound, JoinRound);
        XDrawRectangle(m_display, m_backBuffer, m_gc, rect.left, rect.top, width, height);
    }
}

void X11RenderBackend::DrawRoundedRectangle(const RECT& rect, float radius, Color fillColor, Color borderColor, float borderWidth)
{
    if (!m_initialized || !m_display || !m_backBuffer || !m_gc) {
        return;
    }
    
    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;
    int x = rect.left;
    int y = rect.top;
    int r = static_cast<int>(radius);
    
    // Clamp radius
    int maxRadius = std::min(width, height) / 2;
    r = std::min(r, maxRadius);
    
    // Fill if needed
    if (fillColor.a > 0) {
        SetGCColor(fillColor);
        
        // Fill center rectangles
        XFillRectangle(m_display, m_backBuffer, m_gc, x + r, y, width - r * 2, height);
        XFillRectangle(m_display, m_backBuffer, m_gc, x, y + r, r, height - r * 2);
        XFillRectangle(m_display, m_backBuffer, m_gc, x + width - r, y + r, r, height - r * 2);
        
        // Fill corners
        XFillArc(m_display, m_backBuffer, m_gc, x, y, r * 2, r * 2, 90 * 64, 90 * 64);
        XFillArc(m_display, m_backBuffer, m_gc, x + width - r * 2, y, r * 2, r * 2, 0, 90 * 64);
        XFillArc(m_display, m_backBuffer, m_gc, x, y + height - r * 2, r * 2, r * 2, 180 * 64, 90 * 64);
        XFillArc(m_display, m_backBuffer, m_gc, x + width - r * 2, y + height - r * 2, r * 2, r * 2, 270 * 64, 90 * 64);
    }
    
    // Border if needed
    if (borderColor.a > 0 && borderWidth > 0) {
        SetGCColor(borderColor);
        XSetLineAttributes(m_display, m_gc, borderWidth, LineSolid, CapRound, JoinRound);
        
        // Draw straight edges
        XDrawLine(m_display, m_backBuffer, m_gc, x + r, y, x + width - r, y);
        XDrawLine(m_display, m_backBuffer, m_gc, x + r, y + height, x + width - r, y + height);
        XDrawLine(m_display, m_backBuffer, m_gc, x, y + r, x, y + height - r);
        XDrawLine(m_display, m_backBuffer, m_gc, x + width, y + r, x + width, y + height - r);
        
        // Draw corners
        XDrawArc(m_display, m_backBuffer, m_gc, x, y, r * 2, r * 2, 90 * 64, 90 * 64);
        XDrawArc(m_display, m_backBuffer, m_gc, x + width - r * 2, y, r * 2, r * 2, 0, 90 * 64);
        XDrawArc(m_display, m_backBuffer, m_gc, x, y + height - r * 2, r * 2, r * 2, 180 * 64, 90 * 64);
        XDrawArc(m_display, m_backBuffer, m_gc, x + width - r * 2, y + height - r * 2, r * 2, r * 2, 270 * 64, 90 * 64);
    }
}

void X11RenderBackend::DrawLine(int x1, int y1, int x2, int y2, Color color, float width)
{
    if (!m_initialized || !m_display || !m_backBuffer || !m_gc) {
        return;
    }
    
    SetGCColor(color);
    XSetLineAttributes(m_display, m_gc, width, LineSolid, CapRound, JoinRound);
    XDrawLine(m_display, m_backBuffer, m_gc, x1, y1, x2, y2);
}

void X11RenderBackend::DrawEllipse(int cx, int cy, int rx, int ry, Color fillColor, Color borderColor, float borderWidth)
{
    if (!m_initialized || !m_display || !m_backBuffer || !m_gc) {
        return;
    }
    
    // Fill if needed
    if (fillColor.a > 0) {
        SetGCColor(fillColor);
        XFillArc(m_display, m_backBuffer, m_gc,
            cx - rx, cy - ry,
            rx * 2, ry * 2,
            0, 360 * 64);
    }
    
    // Border if needed
    if (borderColor.a > 0 && borderWidth > 0) {
        SetGCColor(borderColor);
        XSetLineAttributes(m_display, m_gc, borderWidth, LineSolid, CapRound, JoinRound);
        XDrawArc(m_display, m_backBuffer, m_gc,
            cx - rx, cy - ry,
            rx * 2, ry * 2,
            0, 360 * 64);
    }
}

void X11RenderBackend::DrawText(const std::wstring& text, const RECT& rect, Color color, 
                                  const std::wstring& fontFamily, float fontSize, int fontWeight)
{
    if (!m_initialized || !m_display || !m_backBuffer || !m_gc || text.empty()) {
        return;
    }
    
    SetGCColor(color);
    
    // Get or create font
    XFontStruct* font = GetOrCreateFont(static_cast<int>(fontSize));
    if (font) {
        XSetFont(m_display, m_gc, font->fid);
    }
    
    // Convert wstring to UTF-8
    std::string utf8Text = WStringToUTF8(text);
    
    // Draw text
    int x = rect.left + 5; // Small padding
    int y = rect.top + static_cast<int>(fontSize) + 5; // Baseline position
    XDrawString(m_display, m_backBuffer, m_gc, x, y, utf8Text.c_str(), utf8Text.length());
}

void X11RenderBackend::DrawLinearGradient(const RECT& rect, Color startColor, Color endColor, bool horizontal)
{
    if (!m_initialized || !m_display || !m_backBuffer || !m_gc) {
        return;
    }
    
    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;
    int steps = horizontal ? width : height;
    
    if (steps <= 0) {
        return;
    }
    
    // Draw gradient as series of lines
    for (int i = 0; i < steps; i++) {
        float t = static_cast<float>(i) / static_cast<float>(steps);
        
        // Interpolate colors
        BYTE r = static_cast<BYTE>(startColor.r + t * (endColor.r - startColor.r));
        BYTE g = static_cast<BYTE>(startColor.g + t * (endColor.g - startColor.g));
        BYTE b = static_cast<BYTE>(startColor.b + t * (endColor.b - startColor.b));
        BYTE a = static_cast<BYTE>(startColor.a + t * (endColor.a - startColor.a));
        
        Color color(r, g, b, a);
        SetGCColor(color);
        
        if (horizontal) {
            XDrawLine(m_display, m_backBuffer, m_gc,
                rect.left + i, rect.top,
                rect.left + i, rect.bottom);
        } else {
            XDrawLine(m_display, m_backBuffer, m_gc,
                rect.left, rect.top + i,
                rect.right, rect.top + i);
        }
    }
}

void X11RenderBackend::DrawRadialGradient(const RECT& rect, Color centerColor, Color edgeColor, int cx, int cy)
{
    if (!m_initialized || !m_display || !m_backBuffer || !m_gc) {
        return;
    }
    
    // Calculate gradient center
    int rectWidth = rect.right - rect.left;
    int rectHeight = rect.bottom - rect.top;
    int gradientCx = (cx >= 0 && cx < rectWidth) ? rect.left + cx : (rect.left + rect.right) / 2;
    int gradientCy = (cy >= 0 && cy < rectHeight) ? rect.top + cy : (rect.top + rect.bottom) / 2;
    
    // Calculate maximum radius from center to corners
    int dx1 = gradientCx - rect.left;
    int dy1 = gradientCy - rect.top;
    int dx2 = rect.right - gradientCx;
    int dy2 = rect.bottom - gradientCy;
    
    int maxRadius = static_cast<int>(std::sqrt(
        std::max({dx1*dx1 + dy1*dy1, dx2*dx2 + dy1*dy1, 
                  dx1*dx1 + dy2*dy2, dx2*dx2 + dy2*dy2})));
    
    if (maxRadius <= 0) {
        return;
    }
    
    // Draw concentric circles from outside to inside for proper layering
    // Use adaptive step count based on radius for better quality/performance balance
    const int steps = std::max(10, std::min(maxRadius / 2, 50));
    for (int i = steps; i >= 0; --i) {
        float t = static_cast<float>(i) / static_cast<float>(steps);
        int radius = static_cast<int>(maxRadius * t);
        
        // Interpolate color
        Color color;
        color.r = static_cast<uint8_t>(centerColor.r + t * (edgeColor.r - centerColor.r));
        color.g = static_cast<uint8_t>(centerColor.g + t * (edgeColor.g - centerColor.g));
        color.b = static_cast<uint8_t>(centerColor.b + t * (edgeColor.b - centerColor.b));
        color.a = static_cast<uint8_t>(centerColor.a + t * (edgeColor.a - centerColor.a));
        
        SetGCColor(color);
        
        // Draw filled circle
        if (radius > 0) {
            XFillArc(m_display, m_backBuffer, m_gc,
                gradientCx - radius, gradientCy - radius,
                radius * 2, radius * 2,
                0, 360 * 64); // X11 uses 1/64th degree units
        } else {
            // Draw center point
            XDrawPoint(m_display, m_backBuffer, m_gc, gradientCx, gradientCy);
        }
    }
}

void X11RenderBackend::DrawShadow(const RECT& rect, int offsetX, int offsetY, int blur, Color shadowColor)
{
    // Simplified implementation - just draw a slightly offset rectangle
    if (!m_initialized || !m_display || !m_backBuffer || !m_gc) {
        return;
    }
    
    RECT shadowRect = {
        rect.left + offsetX,
        rect.top + offsetY,
        rect.right + offsetX,
        rect.bottom + offsetY
    };
    
    SetGCColor(shadowColor);
    XFillRectangle(m_display, m_backBuffer, m_gc,
        shadowRect.left, shadowRect.top,
        shadowRect.right - shadowRect.left,
        shadowRect.bottom - shadowRect.top);
}

void X11RenderBackend::DrawGlow(const RECT& rect, int radius, Color glowColor)
{
    // Simplified implementation - not implemented for basic X11
}

void X11RenderBackend::ApplyBlur(const RECT& rect, int blurRadius)
{
    // Not supported in basic X11
}

void X11RenderBackend::ApplyBloom(const RECT& rect, float threshold, float intensity)
{
    // Not supported in basic X11
}

void X11RenderBackend::ApplyDepthOfField(const RECT& rect, int focalDepth, int blurAmount, float focalRange)
{
    // Not supported in basic X11
}

void X11RenderBackend::ApplyMotionBlur(const RECT& rect, int directionX, int directionY, float intensity)
{
    // Not supported in basic X11
}

void X11RenderBackend::ApplyChromaticAberration(const RECT& rect, float strength, int offsetX, int offsetY)
{
    // Not supported in basic X11
}

RenderBackend::Capabilities X11RenderBackend::GetCapabilities() const
{
    Capabilities caps;
    caps.supportsGPUAcceleration = false;
    caps.supportsAdvancedEffects = false;
    caps.supportsAntialiasing = false;
    caps.supportsTransparency = true;
    caps.maxTextureSize = 4096;
    return caps;
}

unsigned long X11RenderBackend::ColorToPixel(const Color& color)
{
    if (!m_display) {
        return 0;
    }
    
    // Apply alpha blending with white background
    // Note: X11 does not natively support alpha transparency without compositing.
    // This implementation pre-multiplies alpha with a white (255, 255, 255) background.
    // For true transparency, X Composite extension or ARGB visuals would be required.
    float alpha = color.a / 255.0f;
    BYTE r = static_cast<BYTE>(color.r * alpha + 255 * (1 - alpha));
    BYTE g = static_cast<BYTE>(color.g * alpha + 255 * (1 - alpha));
    BYTE b = static_cast<BYTE>(color.b * alpha + 255 * (1 - alpha));
    
    // Convert to X11 pixel value (assuming 24-bit color)
    unsigned long pixel = (r << 16) | (g << 8) | b;
    
    return pixel;
}

void X11RenderBackend::SetGCColor(const Color& color)
{
    if (!m_display || !m_gc) {
        return;
    }
    
    unsigned long pixel = ColorToPixel(color);
    XSetForeground(m_display, m_gc, pixel);
}

XFontStruct* X11RenderBackend::GetOrCreateFont(int fontSize)
{
    if (!m_display) {
        return nullptr;
    }
    
    // Check cache
    auto it = m_fontCache.find(fontSize);
    if (it != m_fontCache.end()) {
        return it->second;
    }
    
    // X11 font specification format: -foundry-family-weight-slant-width-style-pixelsize-...
    // Try to load a scalable font (helvetica is widely available)
    const char* fontFamily = "helvetica";
    const char* fontWeight = "medium";
    const char* fontSlant = "r"; // regular (not italic)
    std::string fontName = "-*-" + std::string(fontFamily) + "-" + std::string(fontWeight) + 
                          "-" + std::string(fontSlant) + "-*-*-" + std::to_string(fontSize) + 
                          "-*-*-*-*-*-*-*";
    XFontStruct* font = XLoadQueryFont(m_display, fontName.c_str());
    
    if (!font) {
        // Fallback to fixed font (always available on X11)
        font = XLoadQueryFont(m_display, "fixed");
    }
    
    m_fontCache[fontSize] = font;
    return font;
}

} // namespace SDK

#endif // SDK_PLATFORM_LINUX && SDK_HAS_X11
