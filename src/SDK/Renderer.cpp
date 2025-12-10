#include "../../include/SDK/Renderer.h"
#include <cmath>
#include <algorithm>

namespace SDK {

void Renderer::DrawGradient(HDC hdc, const RECT& rect, const Gradient& gradient) {
    switch (gradient.type) {
        case GradientType::VERTICAL:
            DrawVerticalGradient(hdc, rect, gradient.startColor, gradient.endColor);
            break;
        case GradientType::HORIZONTAL:
            DrawHorizontalGradient(hdc, rect, gradient.startColor, gradient.endColor);
            break;
        case GradientType::RADIAL:
            DrawRadialGradient(hdc, rect, gradient.startColor, gradient.endColor,
                             gradient.centerX, gradient.centerY);
            break;
    }
}

void Renderer::DrawVerticalGradient(HDC hdc, const RECT& rect, Color startColor, Color endColor) {
    int height = rect.bottom - rect.top;
    if (height <= 0) return;
    
    for (int y = 0; y < height; y++) {
        float t = (float)y / (float)height;
        Color color = InterpolateColor(startColor, endColor, t);
        
        HPEN pen = CreatePen(PS_SOLID, 1, color.ToCOLORREF());
        HPEN oldPen = (HPEN)SelectObject(hdc, pen);
        
        MoveToEx(hdc, rect.left, rect.top + y, nullptr);
        LineTo(hdc, rect.right, rect.top + y);
        
        SelectObject(hdc, oldPen);
        DeleteObject(pen);
    }
}

void Renderer::DrawHorizontalGradient(HDC hdc, const RECT& rect, Color startColor, Color endColor) {
    int width = rect.right - rect.left;
    if (width <= 0) return;
    
    for (int x = 0; x < width; x++) {
        float t = (float)x / (float)width;
        Color color = InterpolateColor(startColor, endColor, t);
        
        HPEN pen = CreatePen(PS_SOLID, 1, color.ToCOLORREF());
        HPEN oldPen = (HPEN)SelectObject(hdc, pen);
        
        MoveToEx(hdc, rect.left + x, rect.top, nullptr);
        LineTo(hdc, rect.left + x, rect.bottom);
        
        SelectObject(hdc, oldPen);
        DeleteObject(pen);
    }
}

void Renderer::DrawRadialGradient(HDC hdc, const RECT& rect, Color centerColor, Color edgeColor, int cx, int cy) {
    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;
    
    // Default center if not specified
    if (cx == 0 && cy == 0) {
        cx = width / 2;
        cy = height / 2;
    }
    
    // Maximum radius
    float maxRadius = sqrt((float)(width * width + height * height)) / 2.0f;
    
    // Draw pixel by pixel
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            float dx = (float)(x - cx);
            float dy = (float)(y - cy);
            float distance = sqrt(dx * dx + dy * dy);
            float t = std::min(distance / maxRadius, 1.0f);
            
            Color color = InterpolateColor(centerColor, edgeColor, t);
            SetPixel(hdc, rect.left + x, rect.top + y, color.ToCOLORREF());
        }
    }
}

void Renderer::DrawRoundedRect(HDC hdc, const RECT& rect, int radius, Color fillColor, Color borderColor, int borderWidth) {
    // Create rounded rectangle region
    HRGN region = CreateRoundRectRgn(
        rect.left, rect.top,
        rect.right + 1, rect.bottom + 1,
        radius * 2, radius * 2
    );
    
    // Fill with color
    HBRUSH brush = CreateSolidBrush(fillColor.ToCOLORREF());
    FillRgn(hdc, region, brush);
    DeleteObject(brush);
    
    // Draw border if needed
    if (borderWidth > 0) {
        HPEN pen = CreatePen(PS_SOLID, borderWidth, borderColor.ToCOLORREF());
        HPEN oldPen = (HPEN)SelectObject(hdc, pen);
        HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));
        
        RoundRect(hdc, rect.left, rect.top, rect.right, rect.bottom, radius * 2, radius * 2);
        
        SelectObject(hdc, oldPen);
        SelectObject(hdc, oldBrush);
        DeleteObject(pen);
    }
    
    DeleteObject(region);
}

void Renderer::DrawShadow(HDC hdc, const RECT& rect, int offsetX, int offsetY, int blur, Color shadowColor) {
    // Simple shadow using alpha-blended rectangles
    RECT shadowRect = rect;
    shadowRect.left += offsetX;
    shadowRect.top += offsetY;
    shadowRect.right += offsetX;
    shadowRect.bottom += offsetY;
    
    // Draw multiple layers for blur effect
    for (int i = blur; i > 0; i--) {
        RECT blurRect = shadowRect;
        blurRect.left -= i;
        blurRect.top -= i;
        blurRect.right += i;
        blurRect.bottom += i;
        
        float alpha = (float)shadowColor.a / (float)blur * (float)i;
        Color blurColor = shadowColor;
        blurColor.a = (BYTE)alpha;
        
        // Draw shadow layer
        HBRUSH brush = CreateSolidBrush(RGB(blurColor.r, blurColor.g, blurColor.b));
        
        // Use alpha blending
        BLENDFUNCTION blend;
        blend.BlendOp = AC_SRC_OVER;
        blend.BlendFlags = 0;
        blend.SourceConstantAlpha = blurColor.a;
        blend.AlphaFormat = 0;
        
        FillRect(hdc, &blurRect, brush);
        DeleteObject(brush);
    }
}

void Renderer::DrawGlow(HDC hdc, const RECT& rect, int radius, Color glowColor) {
    // Draw expanding glow layers
    for (int i = 0; i < radius; i++) {
        RECT glowRect = rect;
        glowRect.left -= i;
        glowRect.top -= i;
        glowRect.right += i;
        glowRect.bottom += i;
        
        float alpha = (float)glowColor.a * (1.0f - (float)i / (float)radius);
        Color layerColor = glowColor;
        layerColor.a = (BYTE)alpha;
        
        HPEN pen = CreatePen(PS_SOLID, 1, RGB(layerColor.r, layerColor.g, layerColor.b));
        HPEN oldPen = (HPEN)SelectObject(hdc, pen);
        HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));
        
        Rectangle(hdc, glowRect.left, glowRect.top, glowRect.right, glowRect.bottom);
        
        SelectObject(hdc, oldPen);
        SelectObject(hdc, oldBrush);
        DeleteObject(pen);
    }
}

void Renderer::DrawParticles(HDC hdc, const std::vector<Particle>& particles) {
    for (const auto& particle : particles) {
        if (particle.life <= 0.0f) continue;
        
        // Draw particle as a small circle
        int x = (int)particle.x;
        int y = (int)particle.y;
        int size = 2;
        
        HBRUSH brush = CreateSolidBrush(particle.color.ToCOLORREF());
        HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, brush);
        
        Ellipse(hdc, x - size, y - size, x + size, y + size);
        
        SelectObject(hdc, oldBrush);
        DeleteObject(brush);
    }
}

void Renderer::UpdateParticles(std::vector<Particle>& particles, float deltaTime) {
    for (auto& particle : particles) {
        particle.x += particle.vx * deltaTime;
        particle.y += particle.vy * deltaTime;
        particle.life -= deltaTime;
        
        // Apply gravity
        particle.vy += 50.0f * deltaTime;
    }
    
    // Remove dead particles
    particles.erase(
        std::remove_if(particles.begin(), particles.end(),
            [](const Particle& p) { return p.life <= 0.0f; }),
        particles.end()
    );
}

std::vector<Renderer::Particle> Renderer::CreateParticleEmission(int x, int y, int count, Color color) {
    std::vector<Particle> particles;
    particles.reserve(count);
    
    for (int i = 0; i < count; i++) {
        Particle p;
        p.x = (float)x;
        p.y = (float)y;
        
        // Random velocity
        float angle = (float)(rand() % 360) * 3.14159f / 180.0f;
        float speed = 50.0f + (float)(rand() % 100);
        p.vx = cos(angle) * speed;
        p.vy = sin(angle) * speed - 50.0f;  // Bias upward
        
        p.life = 0.5f + (float)(rand() % 100) / 100.0f;
        p.color = color;
        
        particles.push_back(p);
    }
    
    return particles;
}

void Renderer::DrawIcon(HDC hdc, IconType type, int x, int y, int size, Color color, float alpha) {
    Color iconColor = color;
    iconColor.a = (BYTE)(iconColor.a * alpha);
    
    HBRUSH brush = CreateSolidBrush(iconColor.ToCOLORREF());
    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, brush);
    HPEN pen = CreatePen(PS_SOLID, 2, iconColor.ToCOLORREF());
    HPEN oldPen = (HPEN)SelectObject(hdc, pen);
    
    switch (type) {
        case IconType::CIRCLE:
            Ellipse(hdc, x - size/2, y - size/2, x + size/2, y + size/2);
            break;
            
        case IconType::SQUARE:
            Rectangle(hdc, x - size/2, y - size/2, x + size/2, y + size/2);
            break;
            
        case IconType::DIAMOND: {
            POINT points[4] = {
                {x, y - size/2},
                {x + size/2, y},
                {x, y + size/2},
                {x - size/2, y}
            };
            Polygon(hdc, points, 4);
            break;
        }
            
        case IconType::FOLDER:
            Rectangle(hdc, x - size/2, y - size/4, x + size/2, y + size/2);
            Rectangle(hdc, x - size/2, y - size/2, x, y - size/4);
            break;
            
        case IconType::STAR: {
            // Simple 5-point star
            const int numPoints = 10;
            POINT points[numPoints];
            for (int i = 0; i < numPoints; i++) {
                float angle = (float)i * 3.14159f / 5.0f - 3.14159f / 2.0f;
                float radius = (i % 2 == 0) ? (float)size/2 : (float)size/4;
                points[i].x = x + (int)(cos(angle) * radius);
                points[i].y = y + (int)(sin(angle) * radius);
            }
            Polygon(hdc, points, numPoints);
            break;
        }
    }
    
    SelectObject(hdc, oldBrush);
    SelectObject(hdc, oldPen);
    DeleteObject(brush);
    DeleteObject(pen);
}

Color Renderer::InterpolateColor(Color c1, Color c2, float t) {
    t = std::max(0.0f, std::min(1.0f, t));
    
    return Color(
        (BYTE)(c1.r + (c2.r - c1.r) * t),
        (BYTE)(c1.g + (c2.g - c1.g) * t),
        (BYTE)(c1.b + (c2.b - c1.b) * t),
        (BYTE)(c1.a + (c2.a - c1.a) * t)
    );
}

BYTE Renderer::BlendAlpha(BYTE src, BYTE dst, BYTE alpha) {
    return (BYTE)(((int)src * alpha + (int)dst * (255 - alpha)) / 255);
}

HDC Renderer::CreateMemoryDC(int width, int height, HBITMAP* outBitmap) {
    HDC screenDC = GetDC(nullptr);
    HDC memDC = CreateCompatibleDC(screenDC);
    
    *outBitmap = CreateCompatibleBitmap(screenDC, width, height);
    SelectObject(memDC, *outBitmap);
    
    ReleaseDC(nullptr, screenDC);
    
    return memDC;
}

void Renderer::DeleteMemoryDC(HDC hdc, HBITMAP bitmap) {
    DeleteObject(bitmap);
    DeleteDC(hdc);
}

} // namespace SDK
