#include "../../include/SDK/ProgressBar.h"
#include "../../include/SDK/Renderer.h"
#include <algorithm>

namespace SDK {

ProgressBar::ProgressBar()
    : Widget()
    , m_value(0.0f)
    , m_maxValue(100.0f)
    , m_displayValue(0.0f)
    , m_orientation(Orientation::HORIZONTAL)
    , m_backgroundColor(Color(200, 200, 200, 255))
    , m_foregroundColor(Color(100, 149, 237, 255))  // Cornflower blue
    , m_borderColor(Color(100, 100, 100, 255))
    , m_useGradient(true)
    , m_gradientStart(Color(100, 149, 237, 255))
    , m_gradientEnd(Color(65, 105, 225, 255))
    , m_showText(true)
    , m_text(L"")
    , m_animated(true)
    , m_animationSpeed(5.0f)
    , m_cornerRadius(4)
{
    m_width = 200;
    m_height = 30;
}

void ProgressBar::SetValue(float value) {
    m_value = std::max(0.0f, std::min(value, m_maxValue));
}

void ProgressBar::SetMaxValue(float maxValue) {
    m_maxValue = std::max(1.0f, maxValue);
    m_value = std::min(m_value, m_maxValue);
}

void ProgressBar::SetPercentage(float percentage) {
    SetValue((percentage / 100.0f) * m_maxValue);
}

float ProgressBar::GetPercentage() const {
    if (m_maxValue <= 0.0f) return 0.0f;
    return (m_value / m_maxValue) * 100.0f;
}

void ProgressBar::SetOrientation(Orientation orientation) {
    m_orientation = orientation;
}

void ProgressBar::SetBackgroundColor(Color color) {
    m_backgroundColor = color;
}

void ProgressBar::SetForegroundColor(Color color) {
    m_foregroundColor = color;
    if (!m_useGradient) {
        m_gradientStart = color;
        m_gradientEnd = color;
    }
}

void ProgressBar::SetBorderColor(Color color) {
    m_borderColor = color;
}

void ProgressBar::SetGradient(bool enabled) {
    m_useGradient = enabled;
}

void ProgressBar::SetGradientColors(Color startColor, Color endColor) {
    m_gradientStart = startColor;
    m_gradientEnd = endColor;
    m_useGradient = true;
}

void ProgressBar::SetShowText(bool show) {
    m_showText = show;
}

void ProgressBar::SetText(const std::wstring& text) {
    m_text = text;
}

void ProgressBar::SetAnimated(bool animated) {
    m_animated = animated;
    if (!animated) {
        m_displayValue = m_value;
    }
}

void ProgressBar::SetAnimationSpeed(float speed) {
    m_animationSpeed = std::max(0.1f, speed);
}

void ProgressBar::SetCornerRadius(int radius) {
    m_cornerRadius = std::max(0, radius);
}

void ProgressBar::Update(float deltaTime) {
    if (m_animated) {
        // Smooth animation towards target value
        float diff = m_value - m_displayValue;
        if (std::abs(diff) > 0.01f) {
            m_displayValue += diff * m_animationSpeed * deltaTime;
        } else {
            m_displayValue = m_value;
        }
    } else {
        m_displayValue = m_value;
    }
}

void ProgressBar::Render(HDC hdc) {
    if (!m_visible) return;
    
    RECT bounds;
    GetBounds(bounds);
    
    // Draw background
    if (m_cornerRadius > 0) {
        Renderer::DrawRoundedRect(hdc, bounds, m_cornerRadius, 
            m_backgroundColor, m_borderColor, 1);
    } else {
        // Simple rect
        HBRUSH bgBrush = CreateSolidBrush(RGB(m_backgroundColor.r, m_backgroundColor.g, m_backgroundColor.b));
        FillRect(hdc, &bounds, bgBrush);
        DeleteObject(bgBrush);
        
        // Border
        HPEN borderPen = CreatePen(PS_SOLID, 1, RGB(m_borderColor.r, m_borderColor.g, m_borderColor.b));
        HPEN oldPen = (HPEN)SelectObject(hdc, borderPen);
        HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));
        Rectangle(hdc, bounds.left, bounds.top, bounds.right, bounds.bottom);
        SelectObject(hdc, oldPen);
        SelectObject(hdc, oldBrush);
        DeleteObject(borderPen);
    }
    
    // Calculate progress bar fill
    float progress = m_maxValue > 0 ? m_displayValue / m_maxValue : 0.0f;
    progress = std::max(0.0f, std::min(1.0f, progress));
    
    if (progress > 0.0f) {
        RECT fillRect = bounds;
        
        // Adjust for border
        fillRect.left += 2;
        fillRect.top += 2;
        fillRect.right -= 2;
        fillRect.bottom -= 2;
        
        if (m_orientation == Orientation::HORIZONTAL) {
            fillRect.right = fillRect.left + (int)((fillRect.right - fillRect.left) * progress);
        } else {
            fillRect.top = fillRect.bottom - (int)((fillRect.bottom - fillRect.top) * progress);
        }
        
        // Draw fill with gradient
        if (m_useGradient) {
            if (m_orientation == Orientation::HORIZONTAL) {
                Renderer::DrawHorizontalGradient(hdc, fillRect, m_gradientStart, m_gradientEnd);
            } else {
                Renderer::DrawVerticalGradient(hdc, fillRect, m_gradientEnd, m_gradientStart);
            }
        } else {
            HBRUSH fillBrush = CreateSolidBrush(RGB(m_foregroundColor.r, m_foregroundColor.g, m_foregroundColor.b));
            FillRect(hdc, &fillRect, fillBrush);
            DeleteObject(fillBrush);
        }
    }
    
    // Draw text
    if (m_showText) {
        std::wstring displayText = m_text;
        if (displayText.empty()) {
            // Format percentage text
            constexpr int BUFFER_SIZE = 32;
            wchar_t buffer[BUFFER_SIZE];
            swprintf_s(buffer, BUFFER_SIZE, L"%.0f%%", GetPercentage());
            displayText = buffer;
        }
        
        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, RGB(50, 50, 50));
        
        RECT textRect = bounds;
        DrawTextW(hdc, displayText.c_str(), -1, &textRect, 
            DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    }
}

} // namespace SDK
