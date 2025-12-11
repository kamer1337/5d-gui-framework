#include "../../include/SDK/Renderer.h"
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <string>
#include <unordered_map>

namespace SDK {

// Rendering constants
namespace {
    constexpr float CAMERA_DISTANCE = 300.0f;
    constexpr float MIN_PROJECTION_DISTANCE = 1.0f;
    constexpr float DEPTH_SCALE_MIN = 0.7f;
    constexpr float DEPTH_SCALE_FACTOR = 0.06f;
}

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

// Multi-dimensional rendering implementations

// 3D Rendering
void Renderer::Render3DPoint(HDC hdc, const Vector3D& point, int originX, int originY, Color color, float scale) {
    int x2D, y2D;
    Project3Dto2D(point, x2D, y2D, originX, originY, scale);
    
    HBRUSH brush = CreateSolidBrush(color.ToCOLORREF());
    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, brush);
    Ellipse(hdc, x2D - 3, y2D - 3, x2D + 3, y2D + 3);
    SelectObject(hdc, oldBrush);
    DeleteObject(brush);
}

void Renderer::Render3DLine(HDC hdc, const Vector3D& start, const Vector3D& end, int originX, int originY, Color color, float scale) {
    int x1, y1, x2, y2;
    Project3Dto2D(start, x1, y1, originX, originY, scale);
    Project3Dto2D(end, x2, y2, originX, originY, scale);
    
    HPEN pen = CreatePen(PS_SOLID, 2, color.ToCOLORREF());
    HPEN oldPen = (HPEN)SelectObject(hdc, pen);
    
    MoveToEx(hdc, x1, y1, nullptr);
    LineTo(hdc, x2, y2);
    
    SelectObject(hdc, oldPen);
    DeleteObject(pen);
}

void Renderer::Render3DCube(HDC hdc, const Vector3D& center, float size, int originX, int originY, Color color, float rotX, float rotY, float rotZ) {
    float halfSize = size / 2.0f;
    
    // Define cube vertices relative to origin
    Vector3D vertices[8] = {
        Vector3D(-halfSize, -halfSize, -halfSize),
        Vector3D(halfSize, -halfSize, -halfSize),
        Vector3D(halfSize, halfSize, -halfSize),
        Vector3D(-halfSize, halfSize, -halfSize),
        Vector3D(-halfSize, -halfSize, halfSize),
        Vector3D(halfSize, -halfSize, halfSize),
        Vector3D(halfSize, halfSize, halfSize),
        Vector3D(-halfSize, halfSize, halfSize)
    };
    
    // Apply rotation matrices
    for (int i = 0; i < 8; i++) {
        Vector3D& v = vertices[i];
        
        // Rotation around X axis (use epsilon comparison to avoid unnecessary calculations)
        if (std::abs(rotX) > 1e-6f) {
            float cosX = cos(rotX);
            float sinX = sin(rotX);
            float y = v.y * cosX - v.z * sinX;
            float z = v.y * sinX + v.z * cosX;
            v.y = y;
            v.z = z;
        }
        
        // Rotation around Y axis
        if (std::abs(rotY) > 1e-6f) {
            float cosY = cos(rotY);
            float sinY = sin(rotY);
            float x = v.x * cosY + v.z * sinY;
            float z = -v.x * sinY + v.z * cosY;
            v.x = x;
            v.z = z;
        }
        
        // Rotation around Z axis
        if (std::abs(rotZ) > 1e-6f) {
            float cosZ = cos(rotZ);
            float sinZ = sin(rotZ);
            float x = v.x * cosZ - v.y * sinZ;
            float y = v.x * sinZ + v.y * cosZ;
            v.x = x;
            v.y = y;
        }
        
        // Translate to center position
        v.x += center.x;
        v.y += center.y;
        v.z += center.z;
    }
    
    // Draw cube edges
    int edges[12][2] = {
        {0,1}, {1,2}, {2,3}, {3,0},  // Back face
        {4,5}, {5,6}, {6,7}, {7,4},  // Front face
        {0,4}, {1,5}, {2,6}, {3,7}   // Connecting edges
    };
    
    for (int i = 0; i < 12; i++) {
        Render3DLine(hdc, vertices[edges[i][0]], vertices[edges[i][1]], originX, originY, color);
    }
}

// 4D Rendering
void Renderer::Render4DPoint(HDC hdc, const Vector4D& point, float time, int originX, int originY, Color color, float scale) {
    Vector3D point3D;
    Project4Dto3D(point, point3D, time);
    Render3DPoint(hdc, point3D, originX, originY, color, scale);
}

void Renderer::Render4DHypercube(HDC hdc, const Vector4D& center, float size, float time, int originX, int originY, Color color) {
    float halfSize = size / 2.0f;
    
    // Define hypercube vertices in 4D (16 vertices)
    Vector4D vertices[16];
    int idx = 0;
    for (int w = -1; w <= 1; w += 2) {
        for (int z = -1; z <= 1; z += 2) {
            for (int y = -1; y <= 1; y += 2) {
                for (int x = -1; x <= 1; x += 2) {
                    vertices[idx++] = Vector4D(
                        center.x + x * halfSize,
                        center.y + y * halfSize,
                        center.z + z * halfSize,
                        center.w + w * halfSize
                    );
                }
            }
        }
    }
    
    // Project to 3D and render edges (simplified)
    for (int i = 0; i < 16; i++) {
        Vector3D v1;
        Project4Dto3D(vertices[i], v1, time);
        
        for (int j = i + 1; j < 16; j++) {
            // Check if vertices are connected (differ in only one dimension)
            int diffCount = 0;
            if (std::abs(vertices[i].x - vertices[j].x) > 0.1f) diffCount++;
            if (std::abs(vertices[i].y - vertices[j].y) > 0.1f) diffCount++;
            if (std::abs(vertices[i].z - vertices[j].z) > 0.1f) diffCount++;
            if (std::abs(vertices[i].w - vertices[j].w) > 0.1f) diffCount++;
            
            if (diffCount == 1) {
                Vector3D v2;
                Project4Dto3D(vertices[j], v2, time);
                Render3DLine(hdc, v1, v2, originX, originY, color);
            }
        }
    }
}

// 5D Rendering
void Renderer::Render5DPoint(HDC hdc, const Vector5D& point, float time, int originX, int originY, Color color, float scale) {
    Vector4D point4D;
    Project5Dto4D(point, point4D, point.d);
    Render4DPoint(hdc, point4D, time, originX, originY, color, scale);
}

void Renderer::Render5DScene(HDC hdc, const std::vector<Vector5D>& points, float time, int originX, int originY, const std::vector<Color>& colors) {
    for (size_t i = 0; i < points.size(); i++) {
        Color color = (i < colors.size()) ? colors[i] : Color(255, 255, 255, 255);
        Render5DPoint(hdc, points[i], time, originX, originY, color);
    }
}

// 6D Rendering
void Renderer::Render6DPoint(HDC hdc, const Vector6D& point, int originX, int originY, Color color, float scale) {
    Vector5D point5D;
    Project6Dto5D(point, point5D);
    Render5DPoint(hdc, point5D, point.t, originX, originY, color, scale);
}

void Renderer::Render6DPath(HDC hdc, const std::vector<Vector6D>& path, int originX, int originY, Color color) {
    if (path.size() < 2) return;
    
    for (size_t i = 0; i < path.size() - 1; i++) {
        Vector5D p1, p2;
        Project6Dto5D(path[i], p1);
        Project6Dto5D(path[i + 1], p2);
        
        Vector4D p4d1, p4d2;
        Project5Dto4D(p1, p4d1, p1.d);
        Project5Dto4D(p2, p4d2, p2.d);
        
        Vector3D p3d1, p3d2;
        Project4Dto3D(p4d1, p3d1, path[i].t);
        Project4Dto3D(p4d2, p3d2, path[i + 1].t);
        
        Render3DLine(hdc, p3d1, p3d2, originX, originY, color);
    }
}

// Projection helpers
void Renderer::Project3Dto2D(const Vector3D& point3D, int& x2D, int& y2D, int originX, int originY, float scale) {
    // Simple perspective projection
    float fov = 500.0f; // Field of view
    float distance = point3D.z + CAMERA_DISTANCE;
    
    if (distance < MIN_PROJECTION_DISTANCE) distance = MIN_PROJECTION_DISTANCE;
    
    float perspectiveScale = fov / distance;
    
    x2D = originX + (int)(point3D.x * perspectiveScale * scale);
    y2D = originY + (int)(point3D.y * perspectiveScale * scale);
}

void Renderer::Project4Dto3D(const Vector4D& point4D, Vector3D& point3D, float time) {
    // Project 4D to 3D using time/w-dimension
    float wScale = std::cos(time + point4D.w * 0.1f) * 0.5f + 0.5f;
    
    point3D.x = point4D.x * wScale;
    point3D.y = point4D.y * wScale;
    point3D.z = point4D.z * wScale;
}

void Renderer::Project5Dto4D(const Vector5D& point5D, Vector4D& point4D, float depthScale) {
    // Project 5D to 4D using depth layer
    float dScale = DEPTH_SCALE_MIN + depthScale * DEPTH_SCALE_FACTOR; // Scale based on depth (70% to 100%)
    
    point4D.x = point5D.x * dScale;
    point4D.y = point5D.y * dScale;
    point4D.z = point5D.z * dScale;
    point4D.w = point5D.w;
}

void Renderer::Project6Dto5D(const Vector6D& point6D, Vector5D& point5D) {
    // Project 6D to 5D using time dimension
    float tScale = std::sin(point6D.t * 0.5f) * 0.2f + 1.0f;
    
    point5D.x = point6D.x * tScale;
    point5D.y = point6D.y * tScale;
    point5D.z = point6D.z * tScale;
    point5D.w = point6D.w;
    point5D.d = point6D.d;
}

// ==================== ADVANCED GRADIENT IMPLEMENTATIONS ====================

void Renderer::DrawMultiStopGradient(HDC hdc, const RECT& rect, const std::vector<GradientStop>& stops, bool horizontal) {
    if (stops.size() < 2) return;
    
    int dimension = horizontal ? (rect.right - rect.left) : (rect.bottom - rect.top);
    if (dimension <= 0) return;
    
    for (int i = 0; i < dimension; i++) {
        float t = (float)i / (float)dimension;
        
        // Find the two stops to interpolate between
        Color color;
        if (t <= stops[0].position) {
            color = stops[0].color;
        } else if (t >= stops[stops.size() - 1].position) {
            color = stops[stops.size() - 1].color;
        } else {
            // Find the surrounding stops
            size_t idx1 = 0, idx2 = 0;
            for (size_t j = 0; j < stops.size() - 1; j++) {
                if (t >= stops[j].position && t <= stops[j + 1].position) {
                    idx1 = j;
                    idx2 = j + 1;
                    break;
                }
            }
            
            // Interpolate between the two stops
            float localT = (t - stops[idx1].position) / (stops[idx2].position - stops[idx1].position);
            color = InterpolateColor(stops[idx1].color, stops[idx2].color, localT);
        }
        
        HPEN pen = CreatePen(PS_SOLID, 1, color.ToCOLORREF());
        HPEN oldPen = (HPEN)SelectObject(hdc, pen);
        
        if (horizontal) {
            MoveToEx(hdc, rect.left + i, rect.top, nullptr);
            LineTo(hdc, rect.left + i, rect.bottom);
        } else {
            MoveToEx(hdc, rect.left, rect.top + i, nullptr);
            LineTo(hdc, rect.right, rect.top + i);
        }
        
        SelectObject(hdc, oldPen);
        DeleteObject(pen);
    }
}

void Renderer::DrawConicalGradient(HDC hdc, const RECT& rect, const std::vector<GradientStop>& stops, int cx, int cy, float startAngle) {
    if (stops.size() < 2) return;
    
    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;
    
    if (cx == 0 && cy == 0) {
        cx = width / 2;
        cy = height / 2;
    }
    
    const float PI = 3.14159265358979323846f;
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            float dx = (float)(x - cx);
            float dy = (float)(y - cy);
            float angle = std::atan2(dy, dx) + PI + startAngle;  // 0 to 2*PI
            
            // Normalize angle to 0-1
            float t = std::fmod(angle / (2.0f * PI), 1.0f);
            if (t < 0) t += 1.0f;
            
            // Find color at this angle
            Color color;
            if (t <= stops[0].position) {
                color = stops[0].color;
            } else if (t >= stops[stops.size() - 1].position) {
                color = stops[stops.size() - 1].color;
            } else {
                size_t idx1 = 0, idx2 = 0;
                for (size_t j = 0; j < stops.size() - 1; j++) {
                    if (t >= stops[j].position && t <= stops[j + 1].position) {
                        idx1 = j;
                        idx2 = j + 1;
                        break;
                    }
                }
                
                float localT = (t - stops[idx1].position) / (stops[idx2].position - stops[idx1].position);
                color = InterpolateColor(stops[idx1].color, stops[idx2].color, localT);
            }
            
            SetPixel(hdc, rect.left + x, rect.top + y, color.ToCOLORREF());
        }
    }
}

// ==================== ADVANCED VISUAL EFFECTS ====================

void Renderer::ApplyBlur(HDC hdc, const RECT& rect, int blurRadius) {
    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;
    
    if (width <= 0 || height <= 0 || blurRadius <= 0) return;
    
    // Simple box blur implementation
    std::vector<COLORREF> pixels(width * height);
    std::vector<COLORREF> tempPixels(width * height);
    
    // Read pixels
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            pixels[y * width + x] = GetPixel(hdc, rect.left + x, rect.top + y);
        }
    }
    
    // Horizontal blur pass
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int r = 0, g = 0, b = 0, count = 0;
            
            for (int dx = -blurRadius; dx <= blurRadius; dx++) {
                int xx = x + dx;
                if (xx >= 0 && xx < width) {
                    COLORREF pixel = pixels[y * width + xx];
                    r += GetRValue(pixel);
                    g += GetGValue(pixel);
                    b += GetBValue(pixel);
                    count++;
                }
            }
            
            tempPixels[y * width + x] = RGB(r / count, g / count, b / count);
        }
    }
    
    // Vertical blur pass
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int r = 0, g = 0, b = 0, count = 0;
            
            for (int dy = -blurRadius; dy <= blurRadius; dy++) {
                int yy = y + dy;
                if (yy >= 0 && yy < height) {
                    COLORREF pixel = tempPixels[yy * width + x];
                    r += GetRValue(pixel);
                    g += GetGValue(pixel);
                    b += GetBValue(pixel);
                    count++;
                }
            }
            
            SetPixel(hdc, rect.left + x, rect.top + y, RGB(r / count, g / count, b / count));
        }
    }
}

void Renderer::ApplyBloom(HDC hdc, const RECT& rect, float threshold, float intensity) {
    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;
    
    if (width <= 0 || height <= 0) return;
    
    std::vector<COLORREF> pixels(width * height);
    
    // Read pixels and extract bright areas
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            COLORREF pixel = GetPixel(hdc, rect.left + x, rect.top + y);
            int r = GetRValue(pixel);
            int g = GetGValue(pixel);
            int b = GetBValue(pixel);
            
            // Calculate brightness
            float brightness = (r + g + b) / (3.0f * 255.0f);
            
            if (brightness > threshold) {
                // Amplify bright pixels
                r = std::min(255, (int)(r * intensity));
                g = std::min(255, (int)(g * intensity));
                b = std::min(255, (int)(b * intensity));
                pixels[y * width + x] = RGB(r, g, b);
            } else {
                pixels[y * width + x] = RGB(0, 0, 0);
            }
        }
    }
    
    // Apply blur to bright areas
    const int blurRadius = 5;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int r = 0, g = 0, b = 0, count = 0;
            
            for (int dy = -blurRadius; dy <= blurRadius; dy++) {
                for (int dx = -blurRadius; dx <= blurRadius; dx++) {
                    int xx = x + dx;
                    int yy = y + dy;
                    if (xx >= 0 && xx < width && yy >= 0 && yy < height) {
                        COLORREF pixel = pixels[yy * width + xx];
                        r += GetRValue(pixel);
                        g += GetGValue(pixel);
                        b += GetBValue(pixel);
                        count++;
                    }
                }
            }
            
            // Blend with original
            COLORREF original = GetPixel(hdc, rect.left + x, rect.top + y);
            int finalR = std::min(255, GetRValue(original) + r / count);
            int finalG = std::min(255, GetGValue(original) + g / count);
            int finalB = std::min(255, GetBValue(original) + b / count);
            
            SetPixel(hdc, rect.left + x, rect.top + y, RGB(finalR, finalG, finalB));
        }
    }
}

void Renderer::ApplyColorCorrection(HDC hdc, const RECT& rect, float brightness, float contrast, float saturation) {
    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;
    
    if (width <= 0 || height <= 0) return;
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            COLORREF pixel = GetPixel(hdc, rect.left + x, rect.top + y);
            float r = GetRValue(pixel) / 255.0f;
            float g = GetGValue(pixel) / 255.0f;
            float b = GetBValue(pixel) / 255.0f;
            
            // Apply brightness
            r += brightness;
            g += brightness;
            b += brightness;
            
            // Apply contrast
            r = (r - 0.5f) * contrast + 0.5f;
            g = (g - 0.5f) * contrast + 0.5f;
            b = (b - 0.5f) * contrast + 0.5f;
            
            // Apply saturation
            float gray = 0.299f * r + 0.587f * g + 0.114f * b;
            r = gray + (r - gray) * saturation;
            g = gray + (g - gray) * saturation;
            b = gray + (b - gray) * saturation;
            
            // Clamp values
            r = std::max(0.0f, std::min(1.0f, r));
            g = std::max(0.0f, std::min(1.0f, g));
            b = std::max(0.0f, std::min(1.0f, b));
            
            SetPixel(hdc, rect.left + x, rect.top + y, RGB((int)(r * 255), (int)(g * 255), (int)(b * 255)));
        }
    }
}

void Renderer::ApplyNoiseOverlay(HDC hdc, const RECT& rect, float intensity, int seed) {
    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;
    
    if (width <= 0 || height <= 0) return;
    
    srand(seed);
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            COLORREF pixel = GetPixel(hdc, rect.left + x, rect.top + y);
            
            // Generate noise
            float noise = ((rand() % 256) / 255.0f - 0.5f) * intensity;
            
            int r = std::max(0, std::min(255, GetRValue(pixel) + (int)(noise * 255)));
            int g = std::max(0, std::min(255, GetGValue(pixel) + (int)(noise * 255)));
            int b = std::max(0, std::min(255, GetBValue(pixel) + (int)(noise * 255)));
            
            SetPixel(hdc, rect.left + x, rect.top + y, RGB(r, g, b));
        }
    }
}

// ==================== PARTICLE POOL IMPLEMENTATION ====================

Renderer::ParticlePool::ParticlePool(size_t initialSize) : activeCount_(0) {
    particles_.resize(initialSize);
    available_.reserve(initialSize);
    
    for (size_t i = 0; i < initialSize; i++) {
        particles_[i].active = false;
        available_.push_back(&particles_[i]);
    }
}

Renderer::Particle* Renderer::ParticlePool::Acquire() {
    if (available_.empty()) {
        // Expand pool if needed
        size_t oldSize = particles_.size();
        size_t newSize = oldSize * 2;
        particles_.resize(newSize);
        
        for (size_t i = oldSize; i < newSize; i++) {
            particles_[i].active = false;
            available_.push_back(&particles_[i]);
        }
    }
    
    Particle* p = available_.back();
    available_.pop_back();
    p->active = true;
    activeCount_++;
    return p;
}

void Renderer::ParticlePool::Release(Particle* particle) {
    if (particle && particle->active) {
        particle->active = false;
        available_.push_back(particle);
        activeCount_--;
    }
}

void Renderer::ParticlePool::ReleaseAll() {
    available_.clear();
    available_.reserve(particles_.size());
    
    for (auto& p : particles_) {
        p.active = false;
        available_.push_back(&p);
    }
    
    activeCount_ = 0;
}

size_t Renderer::ParticlePool::GetActiveCount() const {
    return activeCount_;
}

size_t Renderer::ParticlePool::GetTotalCount() const {
    return particles_.size();
}

void Renderer::DrawParticlesFromPool(HDC hdc, ParticlePool& pool) {
    for (auto& particle : pool.particles_) {
        if (!particle.active || particle.life <= 0.0f) continue;
        
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

void Renderer::UpdateParticlesInPool(ParticlePool& pool, float deltaTime) {
    for (auto& particle : pool.particles_) {
        if (!particle.active) continue;
        
        particle.x += particle.vx * deltaTime;
        particle.y += particle.vy * deltaTime;
        particle.life -= deltaTime;
        particle.vy += 50.0f * deltaTime;  // Gravity
        
        if (particle.life <= 0.0f) {
            pool.Release(&particle);
        }
    }
}

void Renderer::UpdateParticlesInPoolMultiThreaded(ParticlePool& pool, float deltaTime, int numThreads) {
    // Determine number of threads to use
    if (numThreads <= 0) {
        numThreads = std::thread::hardware_concurrency();
        if (numThreads <= 0) numThreads = 4;  // Default fallback
    }
    
    size_t totalParticles = pool.particles_.size();
    if (totalParticles == 0) return;
    
    // Calculate particles per thread
    size_t particlesPerThread = totalParticles / numThreads;
    if (particlesPerThread == 0) {
        // Too few particles, just use single-threaded version
        UpdateParticlesInPool(pool, deltaTime);
        return;
    }
    
    std::vector<std::thread> threads;
    std::mutex releaseMutex;  // Protect Release() calls
    
    // Lambda function to update a range of particles
    auto updateRange = [&](size_t start, size_t end) {
        for (size_t i = start; i < end && i < totalParticles; ++i) {
            Particle& particle = pool.particles_[i];
            if (!particle.active) continue;
            
            // Update particle physics
            particle.x += particle.vx * deltaTime;
            particle.y += particle.vy * deltaTime;
            particle.life -= deltaTime;
            particle.vy += 50.0f * deltaTime;  // Gravity
            
            // Check if particle should be released
            if (particle.life <= 0.0f) {
                // Lock when calling Release to avoid race conditions
                std::lock_guard<std::mutex> lock(releaseMutex);
                pool.Release(&particle);
            }
        }
    };
    
    // Create threads
    for (int i = 0; i < numThreads; ++i) {
        size_t start = i * particlesPerThread;
        size_t end = (i == numThreads - 1) ? totalParticles : (i + 1) * particlesPerThread;
        threads.emplace_back(updateRange, start, end);
    }
    
    // Wait for all threads to complete
    for (auto& thread : threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
}


// ==================== TEXTURE ATLAS IMPLEMENTATION ====================

Renderer::TextureAtlas::TextureAtlas(int width, int height) 
    : atlasWidth_(width), atlasHeight_(height), currentX_(0), currentY_(0), rowHeight_(0) {
}

Renderer::TextureAtlas::~TextureAtlas() {
    Clear();
}

bool Renderer::TextureAtlas::AddTexture(const std::string& name, HBITMAP bitmap, int width, int height) {
    // Check if we need to move to next row
    if (currentX_ + width > atlasWidth_) {
        currentX_ = 0;
        currentY_ += rowHeight_;
        rowHeight_ = 0;
    }
    
    // Check if we have space
    if (currentY_ + height > atlasHeight_) {
        return false;  // Atlas is full
    }
    
    AtlasEntry entry;
    entry.x = currentX_;
    entry.y = currentY_;
    entry.width = width;
    entry.height = height;
    entry.bitmap = bitmap;
    
    textures_[name] = entry;
    
    currentX_ += width;
    rowHeight_ = std::max(rowHeight_, height);
    
    return true;
}

const Renderer::TextureAtlas::AtlasEntry* Renderer::TextureAtlas::GetTexture(const std::string& name) const {
    auto it = textures_.find(name);
    if (it != textures_.end()) {
        return &it->second;
    }
    return nullptr;
}

void Renderer::TextureAtlas::Clear() {
    textures_.clear();
    currentX_ = 0;
    currentY_ = 0;
    rowHeight_ = 0;
}

// ==================== ANIMATION SYSTEM ====================

void Renderer::Animation::AddKeyframe(float time, float value, EasingType easing) {
    Keyframe kf;
    kf.time = time;
    kf.value = value;
    kf.easing = easing;
    keyframes_.push_back(kf);
    
    // Keep keyframes sorted by time
    std::sort(keyframes_.begin(), keyframes_.end(), 
        [](const Keyframe& a, const Keyframe& b) { return a.time < b.time; });
}

void Renderer::Animation::Update(float deltaTime) {
    if (!playing_) return;
    
    currentTime_ += deltaTime;
    
    if (currentTime_ >= duration_) {
        if (looping_) {
            currentTime_ = std::fmod(currentTime_, duration_);
        } else {
            currentTime_ = duration_;
            playing_ = false;
        }
    }
}

float Renderer::Animation::GetValue() const {
    if (keyframes_.empty()) return 0.0f;
    if (keyframes_.size() == 1) return keyframes_[0].value;
    
    // Find the two keyframes to interpolate between
    if (currentTime_ <= keyframes_[0].time) {
        return keyframes_[0].value;
    }
    
    if (currentTime_ >= keyframes_[keyframes_.size() - 1].time) {
        return keyframes_[keyframes_.size() - 1].value;
    }
    
    for (size_t i = 0; i < keyframes_.size() - 1; i++) {
        if (currentTime_ >= keyframes_[i].time && currentTime_ <= keyframes_[i + 1].time) {
            float t = (currentTime_ - keyframes_[i].time) / (keyframes_[i + 1].time - keyframes_[i].time);
            t = ApplyEasing(t, keyframes_[i].easing);
            return keyframes_[i].value + (keyframes_[i + 1].value - keyframes_[i].value) * t;
        }
    }
    
    return keyframes_[keyframes_.size() - 1].value;
}

float Renderer::ApplyEasing(float t, EasingType type) {
    const float PI = 3.14159265358979323846f;
    
    switch (type) {
        case EasingType::LINEAR:
            return t;
            
        case EasingType::EASE_IN_QUAD:
            return t * t;
            
        case EasingType::EASE_OUT_QUAD:
            return t * (2.0f - t);
            
        case EasingType::EASE_IN_OUT_QUAD:
            return t < 0.5f ? 2.0f * t * t : -1.0f + (4.0f - 2.0f * t) * t;
            
        case EasingType::EASE_IN_CUBIC:
            return t * t * t;
            
        case EasingType::EASE_OUT_CUBIC:
            return (--t) * t * t + 1.0f;
            
        case EasingType::EASE_IN_OUT_CUBIC:
            return t < 0.5f ? 4.0f * t * t * t : (t - 1.0f) * (2.0f * t - 2.0f) * (2.0f * t - 2.0f) + 1.0f;
            
        case EasingType::EASE_IN_QUART:
            return t * t * t * t;
            
        case EasingType::EASE_OUT_QUART:
            return 1.0f - (--t) * t * t * t;
            
        case EasingType::EASE_IN_OUT_QUART:
            return t < 0.5f ? 8.0f * t * t * t * t : 1.0f - 8.0f * (--t) * t * t * t;
            
        case EasingType::EASE_IN_ELASTIC: {
            if (t == 0.0f || t == 1.0f) return t;
            float p = 0.3f;
            return -std::pow(2.0f, 10.0f * (t - 1.0f)) * std::sin((t - 1.1f) * 2.0f * PI / p);
        }
            
        case EasingType::EASE_OUT_ELASTIC: {
            if (t == 0.0f || t == 1.0f) return t;
            float p = 0.3f;
            return std::pow(2.0f, -10.0f * t) * std::sin((t - 0.1f) * 2.0f * PI / p) + 1.0f;
        }
            
        case EasingType::EASE_IN_OUT_ELASTIC: {
            if (t == 0.0f || t == 1.0f) return t;
            float p = 0.45f;
            t *= 2.0f;
            if (t < 1.0f) {
                return -0.5f * std::pow(2.0f, 10.0f * (t - 1.0f)) * std::sin((t - 1.1f) * 2.0f * PI / p);
            }
            return std::pow(2.0f, -10.0f * (t - 1.0f)) * std::sin((t - 1.1f) * 2.0f * PI / p) * 0.5f + 1.0f;
        }
            
        case EasingType::EASE_IN_BOUNCE:
            return 1.0f - ApplyEasing(1.0f - t, EasingType::EASE_OUT_BOUNCE);
            
        case EasingType::EASE_OUT_BOUNCE: {
            if (t < (1.0f / 2.75f)) {
                return 7.5625f * t * t;
            } else if (t < (2.0f / 2.75f)) {
                t -= (1.5f / 2.75f);
                return 7.5625f * t * t + 0.75f;
            } else if (t < (2.5f / 2.75f)) {
                t -= (2.25f / 2.75f);
                return 7.5625f * t * t + 0.9375f;
            } else {
                t -= (2.625f / 2.75f);
                return 7.5625f * t * t + 0.984375f;
            }
        }
            
        case EasingType::EASE_IN_OUT_BOUNCE:
            return t < 0.5f 
                ? ApplyEasing(t * 2.0f, EasingType::EASE_IN_BOUNCE) * 0.5f
                : ApplyEasing(t * 2.0f - 1.0f, EasingType::EASE_OUT_BOUNCE) * 0.5f + 0.5f;
            
        default:
            return t;
    }
}

// ==================== RENDER CACHE IMPLEMENTATION ====================

Renderer::RenderCache::RenderCache(int width, int height) 
    : width_(width), height_(height) {
    cacheDC_ = CreateMemoryDC(width, height, &cacheBitmap_);
}

Renderer::RenderCache::~RenderCache() {
    DeleteMemoryDC(cacheDC_, cacheBitmap_);
}

void Renderer::RenderCache::MarkDirty(const RECT& rect) {
    DirtyRect dr;
    dr.rect = rect;
    dr.dirty = true;
    dirtyRegions_.push_back(dr);
}

void Renderer::RenderCache::MarkClean() {
    dirtyRegions_.clear();
}

bool Renderer::RenderCache::IsDirty(const RECT& rect) const {
    for (const auto& dr : dirtyRegions_) {
        if (dr.dirty && RectsIntersect(dr.rect, rect)) {
            return true;
        }
    }
    return false;
}

void Renderer::RenderCache::CopyToTarget(HDC targetDC, const RECT& srcRect, int destX, int destY) {
    int width = srcRect.right - srcRect.left;
    int height = srcRect.bottom - srcRect.top;
    
    BitBlt(targetDC, destX, destY, width, height, 
           cacheDC_, srcRect.left, srcRect.top, SRCCOPY);
}

// ==================== OCCLUSION CULLING ====================

bool Renderer::IsRectOccluded(const RECT& rect, const std::vector<RECT>& occluders) {
    // Simple check: if rect is completely covered by any occluder
    for (const auto& occluder : occluders) {
        if (rect.left >= occluder.left && rect.right <= occluder.right &&
            rect.top >= occluder.top && rect.bottom <= occluder.bottom) {
            return true;
        }
    }
    return false;
}

bool Renderer::RectsIntersect(const RECT& a, const RECT& b) {
    return !(a.right < b.left || a.left > b.right || 
             a.bottom < b.top || a.top > b.bottom);
}

} // namespace SDK
