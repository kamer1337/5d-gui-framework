#pragma once

#include "Widget.h"
#include "Renderer.h"
#include <string>

namespace SDK {

/**
 * ProgressBar - Visual widget for displaying progress
 * Supports horizontal/vertical orientation, gradient fills, and text labels
 */
class ProgressBar : public Widget {
public:
    enum class Orientation {
        HORIZONTAL,
        VERTICAL
    };
    
    ProgressBar();
    virtual ~ProgressBar() = default;
    
    // Progress value (0.0 to max)
    void SetValue(float value);
    float GetValue() const { return m_value; }
    
    void SetMaxValue(float maxValue);
    float GetMaxValue() const { return m_maxValue; }
    
    // Set progress as percentage (0-100)
    void SetPercentage(float percentage);
    float GetPercentage() const;
    
    // Orientation
    void SetOrientation(Orientation orientation);
    Orientation GetOrientation() const { return m_orientation; }
    
    // Colors
    void SetBackgroundColor(Color color);
    Color GetBackgroundColor() const { return m_backgroundColor; }
    
    void SetForegroundColor(Color color);
    Color GetForegroundColor() const { return m_foregroundColor; }
    
    void SetBorderColor(Color color);
    Color GetBorderColor() const { return m_borderColor; }
    
    // Gradient support
    void SetGradient(bool enabled);
    bool HasGradient() const { return m_useGradient; }
    
    void SetGradientColors(Color startColor, Color endColor);
    
    // Text display
    void SetShowText(bool show);
    bool ShowsText() const { return m_showText; }
    
    void SetText(const std::wstring& text);
    std::wstring GetText() const { return m_text; }
    
    // Animation
    void SetAnimated(bool animated);
    bool IsAnimated() const { return m_animated; }
    
    void SetAnimationSpeed(float speed);
    
    // Corner radius
    void SetCornerRadius(int radius);
    int GetCornerRadius() const { return m_cornerRadius; }
    
    // Rendering
    virtual void Render(HDC hdc) override;
    virtual void Update(float deltaTime) override;
    
private:
    float m_value;
    float m_maxValue;
    float m_displayValue;  // For smooth animation
    
    Orientation m_orientation;
    
    Color m_backgroundColor;
    Color m_foregroundColor;
    Color m_borderColor;
    
    bool m_useGradient;
    Color m_gradientStart;
    Color m_gradientEnd;
    
    bool m_showText;
    std::wstring m_text;
    
    bool m_animated;
    float m_animationSpeed;
    
    int m_cornerRadius;
};

} // namespace SDK
