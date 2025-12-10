#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <vector>
#include "Theme.h"

namespace SDK {

/**
 * Renderer - Advanced rendering utilities for 5D GUI
 * Implements gradients, shadows, rounded corners, and effects
 */
class Renderer {
public:
    // Gradient rendering
    static void DrawGradient(HDC hdc, const RECT& rect, const Gradient& gradient);
    static void DrawVerticalGradient(HDC hdc, const RECT& rect, Color startColor, Color endColor);
    static void DrawHorizontalGradient(HDC hdc, const RECT& rect, Color startColor, Color endColor);
    static void DrawRadialGradient(HDC hdc, const RECT& rect, Color centerColor, Color edgeColor, int cx, int cy);
    
    // Rounded rectangle with alpha
    static void DrawRoundedRect(HDC hdc, const RECT& rect, int radius, Color fillColor, Color borderColor, int borderWidth);
    
    // Shadow rendering
    static void DrawShadow(HDC hdc, const RECT& rect, int offsetX, int offsetY, int blur, Color shadowColor);
    
    // Glow effect
    static void DrawGlow(HDC hdc, const RECT& rect, int radius, Color glowColor);
    
    // Particle system
    struct Particle {
        float x, y;
        float vx, vy;
        float life;
        Color color;
    };
    
    static void DrawParticles(HDC hdc, const std::vector<Particle>& particles);
    static void UpdateParticles(std::vector<Particle>& particles, float deltaTime);
    static std::vector<Particle> CreateParticleEmission(int x, int y, int count, Color color);
    
    // Icon rendering for 5D icon system
    enum class IconType {
        CIRCLE,
        SQUARE,
        DIAMOND,
        FOLDER,
        STAR
    };
    
    static void DrawIcon(HDC hdc, IconType type, int x, int y, int size, Color color, float alpha = 1.0f);
    
    // Utility functions
    static Color InterpolateColor(Color c1, Color c2, float t);
    static BYTE BlendAlpha(BYTE src, BYTE dst, BYTE alpha);
    
    // Create memory DC for offscreen rendering
    static HDC CreateMemoryDC(int width, int height, HBITMAP* outBitmap);
    static void DeleteMemoryDC(HDC hdc, HBITMAP bitmap);
    
    // Multi-dimensional rendering (3D/4D/5D/6D)
    struct Vector3D {
        float x, y, z;
        Vector3D(float _x = 0, float _y = 0, float _z = 0) : x(_x), y(_y), z(_z) {}
    };
    
    struct Vector4D {
        float x, y, z, w;
        Vector4D(float _x = 0, float _y = 0, float _z = 0, float _w = 0) 
            : x(_x), y(_y), z(_z), w(_w) {}
    };
    
    struct Vector5D {
        float x, y, z, w, d; // d = depth layer
        Vector5D(float _x = 0, float _y = 0, float _z = 0, float _w = 0, float _d = 0) 
            : x(_x), y(_y), z(_z), w(_w), d(_d) {}
    };
    
    struct Vector6D {
        float x, y, z, w, d, t; // t = time dimension
        Vector6D(float _x = 0, float _y = 0, float _z = 0, float _w = 0, float _d = 0, float _t = 0) 
            : x(_x), y(_y), z(_z), w(_w), d(_d), t(_t) {}
    };
    
    // 3D Rendering (basic perspective projection)
    static void Render3DPoint(HDC hdc, const Vector3D& point, int originX, int originY, Color color, float scale = 1.0f);
    static void Render3DLine(HDC hdc, const Vector3D& start, const Vector3D& end, int originX, int originY, Color color, float scale = 1.0f);
    static void Render3DCube(HDC hdc, const Vector3D& center, float size, int originX, int originY, Color color, float rotationX = 0, float rotationY = 0, float rotationZ = 0);
    
    // 4D Rendering (time-animated 3D)
    static void Render4DPoint(HDC hdc, const Vector4D& point, float time, int originX, int originY, Color color, float scale = 1.0f);
    static void Render4DHypercube(HDC hdc, const Vector4D& center, float size, float time, int originX, int originY, Color color);
    
    // 5D Rendering (depth layers + 3D)
    static void Render5DPoint(HDC hdc, const Vector5D& point, float time, int originX, int originY, Color color, float scale = 1.0f);
    static void Render5DScene(HDC hdc, const std::vector<Vector5D>& points, float time, int originX, int originY, const std::vector<Color>& colors);
    
    // 6D Rendering (multi-timeline rendering)
    static void Render6DPoint(HDC hdc, const Vector6D& point, int originX, int originY, Color color, float scale = 1.0f);
    static void Render6DPath(HDC hdc, const std::vector<Vector6D>& path, int originX, int originY, Color color);
    
    // Projection helpers
    static void Project3Dto2D(const Vector3D& point3D, int& x2D, int& y2D, int originX, int originY, float scale);
    static void Project4Dto3D(const Vector4D& point4D, Vector3D& point3D, float time);
    static void Project5Dto4D(const Vector5D& point5D, Vector4D& point4D, float depthScale);
    static void Project6Dto5D(const Vector6D& point6D, Vector5D& point5D);
    
private:
    Renderer() = delete;
};

} // namespace SDK
