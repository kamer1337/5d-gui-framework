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
    
private:
    Renderer() = delete;
};

} // namespace SDK
