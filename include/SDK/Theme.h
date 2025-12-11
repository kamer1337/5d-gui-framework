#pragma once


#include "Platform.h"
#include <string>
#include <vector>

namespace SDK {

// Gradient types
enum class GradientType {
    VERTICAL,
    HORIZONTAL,
    RADIAL
};

// Color structure with alpha
struct Color {
    BYTE r, g, b, a;
    
    Color() : r(0), g(0), b(0), a(255) {}
    Color(BYTE red, BYTE green, BYTE blue, BYTE alpha = 255)
        : r(red), g(green), b(blue), a(alpha) {}
    
    COLORREF ToCOLORREF() const {
        return RGB(r, g, b);
    }
    
    static Color FromRGB(BYTE r, BYTE g, BYTE b) {
        return Color(r, g, b, 255);
    }
};

// Gradient definition
struct Gradient {
    GradientType type;
    Color startColor;
    Color endColor;
    int centerX, centerY; // For radial gradients
    
    Gradient() : type(GradientType::VERTICAL), centerX(0), centerY(0) {}
    Gradient(GradientType t, Color start, Color end)
        : type(t), startColor(start), endColor(end), centerX(0), centerY(0) {}
};

/**
 * Theme - Visual theme for windows
 * Supports gradients, colors, and visual effects as described in README
 */
class Theme {
public:
    Theme();
    Theme(const std::string& name);
    
    // Theme name
    void SetName(const std::string& name) { m_name = name; }
    std::string GetName() const { return m_name; }
    
    // Title bar settings
    void SetTitleBarGradient(const Gradient& gradient);
    Gradient GetTitleBarGradient() const { return m_titleBarGradient; }
    
    void SetTitleBarHeight(int height) { m_titleBarHeight = height; }
    int GetTitleBarHeight() const { return m_titleBarHeight; }
    
    // Window colors
    void SetBackgroundColor(const Color& color) { m_backgroundColor = color; }
    Color GetBackgroundColor() const { return m_backgroundColor; }
    
    void SetBorderColor(const Color& color) { m_borderColor = color; }
    Color GetBorderColor() const { return m_borderColor; }
    
    void SetBorderWidth(int width) { m_borderWidth = width; }
    int GetBorderWidth() const { return m_borderWidth; }
    
    // Corner settings
    void SetCornerRadius(int radius) { m_cornerRadius = radius; }
    int GetCornerRadius() const { return m_cornerRadius; }
    
    // Shadow settings
    void SetShadowColor(const Color& color) { m_shadowColor = color; }
    Color GetShadowColor() const { return m_shadowColor; }
    
    void SetShadowOffset(int x, int y) { m_shadowOffsetX = x; m_shadowOffsetY = y; }
    void GetShadowOffset(int& x, int& y) const { x = m_shadowOffsetX; y = m_shadowOffsetY; }
    
    void SetShadowBlur(int blur) { m_shadowBlur = blur; }
    int GetShadowBlur() const { return m_shadowBlur; }
    
    // Glow effect
    void SetGlowEnabled(bool enabled) { m_glowEnabled = enabled; }
    bool IsGlowEnabled() const { return m_glowEnabled; }
    
    void SetGlowColor(const Color& color) { m_glowColor = color; }
    Color GetGlowColor() const { return m_glowColor; }
    
    // Particle effects
    void SetParticlesEnabled(bool enabled) { m_particlesEnabled = enabled; }
    bool AreParticlesEnabled() const { return m_particlesEnabled; }
    
    // Glass effect
    void SetGlassEffect(bool enabled) { m_glassEffect = enabled; }
    bool IsGlassEffectEnabled() const { return m_glassEffect; }
    
    // Predefined themes
    static Theme CreateDefaultTheme();
    static Theme CreateModernTheme();
    static Theme CreateAuroraTheme();
    static Theme CreateDarkTheme();
    static Theme CreateLightTheme();
    
private:
    std::string m_name;
    
    // Title bar
    Gradient m_titleBarGradient;
    int m_titleBarHeight;
    
    // Colors
    Color m_backgroundColor;
    Color m_borderColor;
    int m_borderWidth;
    
    // Corners
    int m_cornerRadius;
    
    // Shadow
    Color m_shadowColor;
    int m_shadowOffsetX;
    int m_shadowOffsetY;
    int m_shadowBlur;
    
    // Effects
    bool m_glowEnabled;
    Color m_glowColor;
    bool m_particlesEnabled;
    bool m_glassEffect;
};

} // namespace SDK
