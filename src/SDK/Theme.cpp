#include "../../include/SDK/Theme.h"

namespace SDK {

Theme::Theme()
    : m_name("Default")
    , m_titleBarHeight(30)
    , m_backgroundColor(Color(255, 255, 255, 255))
    , m_borderColor(Color(100, 100, 100, 255))
    , m_borderWidth(1)
    , m_cornerRadius(12)
    , m_shadowColor(Color(0, 0, 0, 100))
    , m_shadowOffsetX(5)
    , m_shadowOffsetY(5)
    , m_shadowBlur(10)
    , m_glowEnabled(false)
    , m_glowColor(Color(255, 255, 255, 100))
    , m_particlesEnabled(false)
    , m_glassEffect(false)
{
    // Default vertical gradient for title bar (blue)
    m_titleBarGradient = Gradient(
        GradientType::VERTICAL,
        Color(100, 149, 237, 255),  // Cornflower blue
        Color(65, 105, 225, 255)     // Royal blue
    );
}

Theme::Theme(const std::string& name)
    : Theme()
{
    m_name = name;
}

void Theme::SetTitleBarGradient(const Gradient& gradient) {
    m_titleBarGradient = gradient;
}

Theme Theme::CreateDefaultTheme() {
    Theme theme("Default");
    
    // Blue gradient title bar
    theme.SetTitleBarGradient(Gradient(
        GradientType::VERTICAL,
        Color(100, 149, 237, 255),
        Color(65, 105, 225, 255)
    ));
    
    theme.SetBackgroundColor(Color(245, 245, 245, 255));
    theme.SetBorderColor(Color(100, 100, 100, 255));
    theme.SetBorderWidth(1);
    theme.SetCornerRadius(12);
    
    theme.SetShadowColor(Color(0, 0, 0, 100));
    theme.SetShadowOffset(5, 5);
    theme.SetShadowBlur(10);
    
    return theme;
}

Theme Theme::CreateModernTheme() {
    Theme theme("Modern");
    
    // Horizontal gradient title bar (purple to pink)
    theme.SetTitleBarGradient(Gradient(
        GradientType::HORIZONTAL,
        Color(138, 43, 226, 255),   // Blue violet
        Color(219, 112, 147, 255)   // Pale violet red
    ));
    
    theme.SetBackgroundColor(Color(250, 250, 250, 255));
    theme.SetBorderColor(Color(200, 200, 200, 255));
    theme.SetBorderWidth(0);
    theme.SetCornerRadius(16);
    
    theme.SetShadowColor(Color(0, 0, 0, 80));
    theme.SetShadowOffset(3, 3);
    theme.SetShadowBlur(15);
    
    theme.SetGlowEnabled(true);
    theme.SetGlowColor(Color(138, 43, 226, 50));
    
    theme.SetGlassEffect(true);
    
    return theme;
}

Theme Theme::CreateAuroraTheme() {
    Theme theme("Aurora");
    
    // Radial gradient title bar (aurora colors)
    Gradient gradient(
        GradientType::RADIAL,
        Color(255, 215, 0, 255),    // Gold center
        Color(220, 20, 60, 255)     // Crimson edges
    );
    theme.SetTitleBarGradient(gradient);
    
    theme.SetBackgroundColor(Color(25, 25, 35, 240));  // Dark with slight transparency
    theme.SetBorderColor(Color(100, 150, 255, 255));
    theme.SetBorderWidth(2);
    theme.SetCornerRadius(20);
    
    theme.SetShadowColor(Color(0, 0, 0, 150));
    theme.SetShadowOffset(8, 8);
    theme.SetShadowBlur(20);
    
    theme.SetGlowEnabled(true);
    theme.SetGlowColor(Color(100, 150, 255, 100));
    
    theme.SetParticlesEnabled(true);
    
    return theme;
}

Theme Theme::CreateDarkTheme() {
    Theme theme("Dark");
    
    // Dark gradient title bar
    Gradient gradient(
        GradientType::VERTICAL,
        Color(45, 45, 48, 255),     // Dark gray
        Color(30, 30, 32, 255)      // Darker gray
    );
    theme.SetTitleBarGradient(gradient);
    
    theme.SetBackgroundColor(Color(37, 37, 38, 255));  // VS Code dark background
    theme.SetBorderColor(Color(60, 60, 60, 255));
    theme.SetBorderWidth(1);
    theme.SetCornerRadius(8);
    
    theme.SetShadowColor(Color(0, 0, 0, 180));
    theme.SetShadowOffset(4, 4);
    theme.SetShadowBlur(12);
    
    theme.SetGlowEnabled(false);
    theme.SetGlassEffect(false);
    
    return theme;
}

Theme Theme::CreateLightTheme() {
    Theme theme("Light");
    
    // Light gradient title bar
    Gradient gradient(
        GradientType::VERTICAL,
        Color(255, 255, 255, 255),  // White
        Color(240, 240, 240, 255)   // Light gray
    );
    theme.SetTitleBarGradient(gradient);
    
    theme.SetBackgroundColor(Color(250, 250, 250, 255));  // Almost white
    theme.SetBorderColor(Color(200, 200, 200, 255));
    theme.SetBorderWidth(1);
    theme.SetCornerRadius(8);
    
    theme.SetShadowColor(Color(0, 0, 0, 40));
    theme.SetShadowOffset(2, 2);
    theme.SetShadowBlur(8);
    
    theme.SetGlowEnabled(false);
    theme.SetGlassEffect(false);
    
    return theme;
}

} // namespace SDK
