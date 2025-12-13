#include "../../include/SDK/Tooltip.h"
#include "../../include/SDK/Renderer.h"
#include <algorithm>

namespace SDK {

Tooltip::Tooltip()
    : Widget()
    , m_text(L"")
    , m_positionMode(Position::AUTO)
    , m_targetX(0)
    , m_targetY(0)
    , m_isShowing(false)
    , m_opacity(0.0f)
    , m_fadeEnabled(true)
    , m_fadeSpeed(5.0f)
    , m_showDelay(0.5f)
    , m_delayTimer(0.0f)
    , m_backgroundColor(Color(50, 50, 50, 240))
    , m_textColor(Color(255, 255, 255, 255))
    , m_borderColor(Color(100, 100, 100, 255))
    , m_shadowEnabled(true)
    , m_cornerRadius(6)
    , m_padding(8)
{
    m_width = 100;
    m_height = 30;
    m_visible = false;
}

void Tooltip::SetText(const std::wstring& text) {
    m_text = text;
    ParseMultilineText();
}

void Tooltip::SetLines(const std::vector<std::wstring>& lines) {
    m_lines = lines;
    
    // Rebuild text from lines
    m_text.clear();
    for (size_t i = 0; i < lines.size(); i++) {
        m_text += lines[i];
        if (i < lines.size() - 1) {
            m_text += L'\n';
        }
    }
}

void Tooltip::ParseMultilineText() {
    m_lines.clear();
    
    std::wstring line;
    for (wchar_t ch : m_text) {
        if (ch == L'\n') {
            m_lines.push_back(line);
            line.clear();
        } else {
            line += ch;
        }
    }
    
    if (!line.empty() || m_text.empty()) {
        m_lines.push_back(line);
    }
}

void Tooltip::SetPositionMode(Position mode) {
    m_positionMode = mode;
}

void Tooltip::Show(int x, int y) {
    m_targetX = x;
    m_targetY = y;
    
    // Only reset timer if we're not already showing
    if (!m_isShowing) {
        m_delayTimer = 0.0f;
    }
    
    m_isShowing = true;
    m_visible = true;
    UpdatePosition();
}

void Tooltip::ShowNearWidget(const Widget* widget) {
    if (!widget) return;
    
    RECT bounds;
    widget->GetBounds(bounds);
    
    // Position below the widget by default
    Show(bounds.left, bounds.bottom + 5);
}

void Tooltip::ShowAtCursor() {
    POINT pt;
    GetCursorPos(&pt);
    Show(pt.x + 10, pt.y + 10);
}

void Tooltip::Hide() {
    m_isShowing = false;
    m_delayTimer = 0.0f;
}

void Tooltip::SetFadeEnabled(bool enabled) {
    m_fadeEnabled = enabled;
    if (!enabled) {
        m_opacity = m_isShowing ? 1.0f : 0.0f;
    }
}

void Tooltip::SetFadeSpeed(float speed) {
    m_fadeSpeed = std::max(0.1f, speed);
}

void Tooltip::SetShowDelay(float seconds) {
    m_showDelay = std::max(0.0f, seconds);
}

void Tooltip::SetBackgroundColor(Color color) {
    m_backgroundColor = color;
}

void Tooltip::SetTextColor(Color color) {
    m_textColor = color;
}

void Tooltip::SetBorderColor(Color color) {
    m_borderColor = color;
}

void Tooltip::SetShadowEnabled(bool enabled) {
    m_shadowEnabled = enabled;
}

void Tooltip::SetCornerRadius(int radius) {
    m_cornerRadius = std::max(0, radius);
}

void Tooltip::SetPadding(int padding) {
    m_padding = std::max(0, padding);
}

void Tooltip::AutoSize(HDC hdc) {
    if (m_lines.empty()) {
        m_width = 100;
        m_height = 30;
        return;
    }
    
    // Calculate required size for text
    int maxWidth = 0;
    int totalHeight = 0;
    
    SIZE sz;
    for (const auto& line : m_lines) {
        if (GetTextExtentPoint32W(hdc, line.c_str(), (int)line.length(), &sz)) {
            maxWidth = std::max(maxWidth, (int)sz.cx);
            totalHeight += sz.cy;
        }
    }
    
    m_width = maxWidth + m_padding * 2;
    m_height = totalHeight + m_padding * 2;
    
    UpdatePosition();
}

void Tooltip::UpdatePosition() {
    switch (m_positionMode) {
        case Position::AUTO:
        case Position::CURSOR:
            // Position near target with screen bounds checking
            m_x = m_targetX;
            m_y = m_targetY;
            
            // Check screen bounds and adjust if needed
            {
                int screenWidth = GetSystemMetrics(SM_CXSCREEN);
                int screenHeight = GetSystemMetrics(SM_CYSCREEN);
                
                // Ensure tooltip doesn't go off right edge
                if (m_x + m_width > screenWidth) {
                    m_x = screenWidth - m_width - 5;
                }
                
                // Ensure tooltip doesn't go off left edge
                if (m_x < 0) {
                    m_x = 5;
                }
                
                // Ensure tooltip doesn't go off bottom edge
                if (m_y + m_height > screenHeight) {
                    m_y = screenHeight - m_height - 5;
                }
                
                // Ensure tooltip doesn't go off top edge
                if (m_y < 0) {
                    m_y = 5;
                }
            }
            break;
            
        case Position::ABOVE:
            m_x = m_targetX;
            m_y = m_targetY - m_height - 5;
            break;
            
        case Position::BELOW:
            m_x = m_targetX;
            m_y = m_targetY + 5;
            break;
            
        case Position::LEFT:
            m_x = m_targetX - m_width - 5;
            m_y = m_targetY;
            break;
            
        case Position::RIGHT:
            m_x = m_targetX + 5;
            m_y = m_targetY;
            break;
    }
}

void Tooltip::Update(float deltaTime) {
    if (m_isShowing) {
        // Handle show delay
        if (m_showDelay > 0.0f && m_delayTimer < m_showDelay) {
            m_delayTimer += deltaTime;
            return;
        }
        
        // Fade in
        if (m_fadeEnabled && m_opacity < 1.0f) {
            m_opacity += m_fadeSpeed * deltaTime;
            m_opacity = std::min(1.0f, m_opacity);
        } else if (!m_fadeEnabled) {
            m_opacity = 1.0f;
        }
    } else {
        // Fade out
        if (m_fadeEnabled && m_opacity > 0.0f) {
            m_opacity -= m_fadeSpeed * deltaTime;
            m_opacity = std::max(0.0f, m_opacity);
            
            if (m_opacity <= 0.0f) {
                m_visible = false;
            }
        } else if (!m_fadeEnabled) {
            m_opacity = 0.0f;
            m_visible = false;
        }
    }
}

void Tooltip::Render(HDC hdc) {
    if (!m_visible || m_opacity <= 0.0f) return;
    
    // Skip if delay not passed
    if (m_delayTimer < m_showDelay) return;
    
    RECT bounds;
    GetBounds(bounds);
    
    // Apply opacity to colors
    Color bgColor = m_backgroundColor;
    bgColor.a = (BYTE)(m_backgroundColor.a * m_opacity);
    
    Color borderColor = m_borderColor;
    borderColor.a = (BYTE)(m_borderColor.a * m_opacity);
    
    // Note: Text color opacity is limited by GDI's SetTextColor which doesn't support alpha.
    // For full text fade effect, would need to render text to a memory bitmap and alpha blend.
    Color textColor = m_textColor;
    
    // Draw shadow
    if (m_shadowEnabled) {
        RECT shadowRect = bounds;
        shadowRect.left += 2;
        shadowRect.top += 2;
        shadowRect.right += 2;
        shadowRect.bottom += 2;
        
        Color shadowColor(0, 0, 0, (BYTE)(100 * m_opacity));
        Renderer::DrawShadow(hdc, shadowRect, 2, 2, 4, shadowColor);
    }
    
    // Draw background
    if (m_cornerRadius > 0) {
        Renderer::DrawRoundedRect(hdc, bounds, m_cornerRadius, 
            bgColor, borderColor, 1);
    } else {
        HBRUSH bgBrush = CreateSolidBrush(RGB(bgColor.r, bgColor.g, bgColor.b));
        FillRect(hdc, &bounds, bgBrush);
        DeleteObject(bgBrush);
        
        HPEN borderPen = CreatePen(PS_SOLID, 1, RGB(borderColor.r, borderColor.g, borderColor.b));
        HPEN oldPen = (HPEN)SelectObject(hdc, borderPen);
        HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));
        Rectangle(hdc, bounds.left, bounds.top, bounds.right, bounds.bottom);
        SelectObject(hdc, oldPen);
        SelectObject(hdc, oldBrush);
        DeleteObject(borderPen);
    }
    
    // Draw text
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(textColor.r, textColor.g, textColor.b));
    
    if (m_lines.size() == 1) {
        // Single line - center it
        RECT textRect = bounds;
        textRect.left += m_padding;
        textRect.top += m_padding;
        textRect.right -= m_padding;
        textRect.bottom -= m_padding;
        
        DrawTextW(hdc, m_lines[0].c_str(), -1, &textRect, 
            DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    } else {
        // Multi-line
        int y = bounds.top + m_padding;
        
        for (const auto& line : m_lines) {
            RECT lineRect = {
                bounds.left + m_padding, 
                y, 
                bounds.right - m_padding, 
                y + 20
            };
            
            DrawTextW(hdc, line.c_str(), -1, &lineRect, 
                DT_LEFT | DT_TOP | DT_SINGLELINE);
            
            SIZE sz;
            if (GetTextExtentPoint32W(hdc, line.c_str(), (int)line.length(), &sz)) {
                y += sz.cy;
            }
        }
    }
}

} // namespace SDK
