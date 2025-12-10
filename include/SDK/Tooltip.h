#pragma once

#include "Widget.h"
#include "Renderer.h"
#include <string>
#include <vector>

namespace SDK {

/**
 * Tooltip - Popup text widget that appears on hover
 * Supports auto-positioning, fade animations, and multi-line text
 */
class Tooltip : public Widget {
public:
    enum class Position {
        AUTO,           // Automatically position near cursor
        ABOVE,          // Above the target
        BELOW,          // Below the target
        LEFT,           // To the left of target
        RIGHT,          // To the right of target
        CURSOR          // Follow cursor position
    };
    
    Tooltip();
    virtual ~Tooltip() = default;
    
    // Text content
    void SetText(const std::wstring& text);
    std::wstring GetText() const { return m_text; }
    
    // Multi-line text support
    void SetLines(const std::vector<std::wstring>& lines);
    const std::vector<std::wstring>& GetLines() const { return m_lines; }
    
    // Position mode
    void SetPositionMode(Position mode);
    Position GetPositionMode() const { return m_positionMode; }
    
    // Show tooltip at specific location
    void Show(int x, int y);
    void ShowNearWidget(const Widget* widget);
    void ShowAtCursor();
    void Hide();
    
    // Fade animation
    void SetFadeEnabled(bool enabled);
    bool IsFadeEnabled() const { return m_fadeEnabled; }
    
    void SetFadeSpeed(float speed);
    float GetFadeSpeed() const { return m_fadeSpeed; }
    
    // Delay before showing
    void SetShowDelay(float seconds);
    float GetShowDelay() const { return m_showDelay; }
    
    // Colors
    void SetBackgroundColor(Color color);
    Color GetBackgroundColor() const { return m_backgroundColor; }
    
    void SetTextColor(Color color);
    Color GetTextColor() const { return m_textColor; }
    
    void SetBorderColor(Color color);
    Color GetBorderColor() const { return m_borderColor; }
    
    // Shadow
    void SetShadowEnabled(bool enabled);
    bool IsShadowEnabled() const { return m_shadowEnabled; }
    
    // Corner radius
    void SetCornerRadius(int radius);
    int GetCornerRadius() const { return m_cornerRadius; }
    
    // Padding
    void SetPadding(int padding);
    int GetPadding() const { return m_padding; }
    
    // Auto-size to fit text
    void AutoSize(HDC hdc);
    
    // Rendering
    virtual void Render(HDC hdc) override;
    virtual void Update(float deltaTime) override;
    
    // Check if currently showing
    bool IsShowing() const { return m_isShowing; }
    
    // Opacity for fade effect
    float GetOpacity() const { return m_opacity; }
    
private:
    void UpdatePosition();
    void ParseMultilineText();
    
    std::wstring m_text;
    std::vector<std::wstring> m_lines;
    
    Position m_positionMode;
    int m_targetX;
    int m_targetY;
    
    bool m_isShowing;
    float m_opacity;
    
    bool m_fadeEnabled;
    float m_fadeSpeed;
    float m_showDelay;
    float m_delayTimer;
    
    Color m_backgroundColor;
    Color m_textColor;
    Color m_borderColor;
    
    bool m_shadowEnabled;
    int m_cornerRadius;
    int m_padding;
};

} // namespace SDK
