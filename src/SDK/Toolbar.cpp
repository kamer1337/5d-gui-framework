#include "../../include/SDK/Toolbar.h"
#include "../../include/SDK/Renderer.h"
#include <algorithm>
#include <cmath>

namespace SDK {

Toolbar::Toolbar()
    : Widget()
    , m_orientation(Orientation::HORIZONTAL)
    , m_autoHide(false)
    , m_currentlyVisible(true)
    , m_autoHideDelay(0.5f)
    , m_hideTimer(0.0f)
    , m_triggerZoneSize(3)
    , m_itemWidth(80)
    , m_itemHeight(40)
    , m_padding(5)
    , m_itemSpacing(2)
    , m_backgroundColor(Color(45, 45, 48, 255))
    , m_itemColor(Color(60, 60, 65, 255))
    , m_itemHoverColor(Color(70, 70, 75, 255))
    , m_itemPressedColor(Color(50, 50, 55, 255))
    , m_textColor(Color(255, 255, 255, 255))
    , m_separatorColor(Color(80, 80, 85, 255))
    , m_visibilityAlpha(1.0f)
    , m_slideOffset(0.0f)
    , m_hoveredItemId(-1)
    , m_pressedItemId(-1)
{
}

Toolbar::~Toolbar() {
}

void Toolbar::AddItem(int id, const std::wstring& text, const std::wstring& tooltip) {
    ToolbarItem item(id, text);
    item.tooltip = tooltip;
    m_items.push_back(item);
    CalculateLayout();
}

void Toolbar::AddSeparator() {
    ToolbarItem item;
    item.separator = true;
    m_items.push_back(item);
    CalculateLayout();
}

void Toolbar::RemoveItem(int id) {
    m_items.erase(
        std::remove_if(m_items.begin(), m_items.end(),
            [id](const ToolbarItem& item) { return item.id == id; }),
        m_items.end()
    );
    CalculateLayout();
}

void Toolbar::ClearItems() {
    m_items.clear();
    m_itemLayouts.clear();
}

void Toolbar::SetItemEnabled(int id, bool enabled) {
    for (auto& item : m_items) {
        if (item.id == id) {
            item.enabled = enabled;
            break;
        }
    }
}

void Toolbar::SetItemIcon(int id, HBITMAP icon) {
    for (auto& item : m_items) {
        if (item.id == id) {
            item.icon = icon;
            break;
        }
    }
}

void Toolbar::SetOrientation(Orientation orientation) {
    m_orientation = orientation;
    CalculateLayout();
}

void Toolbar::SetAutoHide(bool autoHide) {
    m_autoHide = autoHide;
    if (!autoHide) {
        m_currentlyVisible = true;
        m_visibilityAlpha = 1.0f;
        m_slideOffset = 0.0f;
    }
}

void Toolbar::SetItemSize(int width, int height) {
    m_itemWidth = width;
    m_itemHeight = height;
    CalculateLayout();
}

void Toolbar::GetItemSize(int& width, int& height) const {
    width = m_itemWidth;
    height = m_itemHeight;
}

void Toolbar::CalculateLayout() {
    m_itemLayouts.clear();
    
    if (m_items.empty()) {
        return;
    }
    
    int currentX = m_padding;
    int currentY = m_padding;
    
    for (auto& item : m_items) {
        ItemLayout layout;
        layout.item = &item;
        layout.hovered = false;
        layout.pressed = false;
        
        if (item.separator) {
            // Separator is smaller
            if (m_orientation == Orientation::HORIZONTAL) {
                layout.rect.left = currentX;
                layout.rect.top = m_padding;
                layout.rect.right = currentX + 2;
                layout.rect.bottom = m_height - m_padding;
                currentX += 2 + m_itemSpacing;
            } else {
                layout.rect.left = m_padding;
                layout.rect.top = currentY;
                layout.rect.right = m_width - m_padding;
                layout.rect.bottom = currentY + 2;
                currentY += 2 + m_itemSpacing;
            }
        } else {
            if (m_orientation == Orientation::HORIZONTAL) {
                layout.rect.left = currentX;
                layout.rect.top = m_padding;
                layout.rect.right = currentX + m_itemWidth;
                layout.rect.bottom = m_padding + m_itemHeight;
                currentX += m_itemWidth + m_itemSpacing;
            } else {
                layout.rect.left = m_padding;
                layout.rect.top = currentY;
                layout.rect.right = m_padding + m_itemWidth;
                layout.rect.bottom = currentY + m_itemHeight;
                currentY += m_itemHeight + m_itemSpacing;
            }
        }
        
        m_itemLayouts.push_back(layout);
    }
}

void Toolbar::Render(HDC hdc) {
    if (!m_visible) {
        return;
    }
    
    // Apply auto-hide visibility
    if (m_autoHide && m_visibilityAlpha <= 0.0f) {
        return;
    }
    
    // Save DC state
    int savedDC = SaveDC(hdc);
    
    // Apply slide offset for auto-hide animation
    if (m_autoHide && m_slideOffset != 0.0f) {
        XFORM xform = {0};
        xform.eM11 = 1.0f;
        xform.eM22 = 1.0f;
        
        if (m_orientation == Orientation::HORIZONTAL) {
            xform.eDy = m_slideOffset;
        } else {
            xform.eDx = m_slideOffset;
        }
        
        SetGraphicsMode(hdc, GM_ADVANCED);
        SetWorldTransform(hdc, &xform);
    }
    
    // Render based on orientation
    if (m_orientation == Orientation::HORIZONTAL) {
        RenderHorizontal(hdc);
    } else {
        RenderVertical(hdc);
    }
    
    // Restore DC state
    RestoreDC(hdc, savedDC);
}

void Toolbar::RenderHorizontal(HDC hdc) {
    // Draw background
    RECT bgRect = {m_x, m_y, m_x + m_width, m_y + m_height};
    
    // Apply alpha for auto-hide
    Color bgColor = m_backgroundColor;
    if (m_autoHide) {
        bgColor.a = static_cast<BYTE>(bgColor.a * m_visibilityAlpha);
    }
    
    HBRUSH bgBrush = CreateSolidBrush(RGB(bgColor.r, bgColor.g, bgColor.b));
    FillRect(hdc, &bgRect, bgBrush);
    DeleteObject(bgBrush);
    
    // Draw items
    for (auto& layout : m_itemLayouts) {
        RECT itemRect = layout.rect;
        itemRect.left += m_x;
        itemRect.right += m_x;
        itemRect.top += m_y;
        itemRect.bottom += m_y;
        
        layout.rect = itemRect;
        RenderItem(hdc, layout);
    }
}

void Toolbar::RenderVertical(HDC hdc) {
    // Draw background
    RECT bgRect = {m_x, m_y, m_x + m_width, m_y + m_height};
    
    // Apply alpha for auto-hide
    Color bgColor = m_backgroundColor;
    if (m_autoHide) {
        bgColor.a = static_cast<BYTE>(bgColor.a * m_visibilityAlpha);
    }
    
    HBRUSH bgBrush = CreateSolidBrush(RGB(bgColor.r, bgColor.g, bgColor.b));
    FillRect(hdc, &bgRect, bgBrush);
    DeleteObject(bgBrush);
    
    // Draw items
    for (auto& layout : m_itemLayouts) {
        RECT itemRect = layout.rect;
        itemRect.left += m_x;
        itemRect.right += m_x;
        itemRect.top += m_y;
        itemRect.bottom += m_y;
        
        layout.rect = itemRect;
        RenderItem(hdc, layout);
    }
}

void Toolbar::RenderItem(HDC hdc, const ItemLayout& layout) {
    if (layout.item->separator) {
        // Draw separator
        Color sepColor = m_separatorColor;
        if (m_autoHide) {
            sepColor.a = static_cast<BYTE>(sepColor.a * m_visibilityAlpha);
        }
        
        HBRUSH sepBrush = CreateSolidBrush(RGB(sepColor.r, sepColor.g, sepColor.b));
        FillRect(hdc, &layout.rect, sepBrush);
        DeleteObject(sepBrush);
        return;
    }
    
    // Determine item color
    Color itemColor = m_itemColor;
    if (layout.pressed) {
        itemColor = m_itemPressedColor;
    } else if (layout.hovered && layout.item->enabled) {
        itemColor = m_itemHoverColor;
    }
    
    // Apply alpha for auto-hide
    if (m_autoHide) {
        itemColor.a = static_cast<BYTE>(itemColor.a * m_visibilityAlpha);
    }
    
    // Darken if disabled
    if (!layout.item->enabled) {
        itemColor.r = static_cast<BYTE>(itemColor.r * 0.5);
        itemColor.g = static_cast<BYTE>(itemColor.g * 0.5);
        itemColor.b = static_cast<BYTE>(itemColor.b * 0.5);
    }
    
    // Draw item background
    HBRUSH itemBrush = CreateSolidBrush(RGB(itemColor.r, itemColor.g, itemColor.b));
    FillRect(hdc, &layout.rect, itemBrush);
    DeleteObject(itemBrush);
    
    // Draw icon if present
    if (layout.item->icon) {
        HDC hdcMem = CreateCompatibleDC(hdc);
        HBITMAP oldBitmap = (HBITMAP)SelectObject(hdcMem, layout.item->icon);
        
        BITMAP bm;
        GetObject(layout.item->icon, sizeof(BITMAP), &bm);
        
        int iconX = layout.rect.left + (layout.rect.right - layout.rect.left - bm.bmWidth) / 2;
        int iconY = layout.rect.top + 5;
        
        BitBlt(hdc, iconX, iconY, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCCOPY);
        
        SelectObject(hdcMem, oldBitmap);
        DeleteDC(hdcMem);
    }
    
    // Draw text
    if (!layout.item->text.empty()) {
        SetBkMode(hdc, TRANSPARENT);
        
        Color textColor = m_textColor;
        if (m_autoHide) {
            textColor.a = static_cast<BYTE>(textColor.a * m_visibilityAlpha);
        }
        if (!layout.item->enabled) {
            textColor.r = static_cast<BYTE>(textColor.r * 0.5);
            textColor.g = static_cast<BYTE>(textColor.g * 0.5);
            textColor.b = static_cast<BYTE>(textColor.b * 0.5);
        }
        
        SetTextColor(hdc, RGB(textColor.r, textColor.g, textColor.b));
        
        RECT textRect = layout.rect;
        if (layout.item->icon) {
            textRect.top += 30;
        }
        
        DrawTextW(hdc, layout.item->text.c_str(), -1, &textRect,
            DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    }
    
    // Draw border for hovered/pressed items
    if (layout.hovered || layout.pressed) {
        HPEN borderPen = CreatePen(PS_SOLID, 1, RGB(100, 150, 255));
        HPEN oldPen = (HPEN)SelectObject(hdc, borderPen);
        HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));
        
        Rectangle(hdc, layout.rect.left, layout.rect.top,
            layout.rect.right, layout.rect.bottom);
        
        SelectObject(hdc, oldPen);
        SelectObject(hdc, oldBrush);
        DeleteObject(borderPen);
    }
}

void Toolbar::Update(float deltaTime) {
    Widget::Update(deltaTime);
    
    if (!m_autoHide) {
        return;
    }
    
    // Get cursor position
    POINT cursorPos;
    GetCursorPos(&cursorPos);
    
    // Convert to client coordinates if needed
    // (Assuming m_x, m_y are in client coordinates)
    
    UpdateAutoHideState(cursorPos.x, cursorPos.y, deltaTime);
}

void Toolbar::UpdateAutoHideState(int mouseX, int mouseY, float deltaTime) {
    if (!m_autoHide) {
        return;
    }
    
    bool inTriggerZone = IsInTriggerZone(mouseX, mouseY);
    bool mouseOverToolbar = HitTest(mouseX, mouseY);
    
    if (inTriggerZone || mouseOverToolbar) {
        // Show toolbar
        m_hideTimer = 0.0f;
        m_currentlyVisible = true;
        
        // Animate in
        if (m_visibilityAlpha < 1.0f) {
            m_visibilityAlpha += deltaTime * 5.0f;
            if (m_visibilityAlpha > 1.0f) {
                m_visibilityAlpha = 1.0f;
            }
        }
        
        if (m_slideOffset != 0.0f) {
            float slideSpeed = 500.0f * deltaTime;
            if (m_orientation == Orientation::HORIZONTAL) {
                m_slideOffset += slideSpeed;
                if (m_slideOffset > 0.0f) {
                    m_slideOffset = 0.0f;
                }
            } else {
                m_slideOffset += slideSpeed;
                if (m_slideOffset > 0.0f) {
                    m_slideOffset = 0.0f;
                }
            }
        }
    } else {
        // Start hide timer
        m_hideTimer += deltaTime;
        
        if (m_hideTimer >= m_autoHideDelay) {
            // Hide toolbar
            m_currentlyVisible = false;
            
            // Animate out
            if (m_visibilityAlpha > 0.0f) {
                m_visibilityAlpha -= deltaTime * 5.0f;
                if (m_visibilityAlpha < 0.0f) {
                    m_visibilityAlpha = 0.0f;
                }
            }
            
            float hideOffset = (m_orientation == Orientation::HORIZONTAL) ? 
                static_cast<float>(-m_height) : static_cast<float>(-m_width);
            
            if (m_slideOffset > hideOffset) {
                float slideSpeed = 500.0f * deltaTime;
                m_slideOffset -= slideSpeed;
                if (m_slideOffset < hideOffset) {
                    m_slideOffset = hideOffset;
                }
            }
        }
    }
}

bool Toolbar::IsInTriggerZone(int x, int y) const {
    if (!m_autoHide) {
        return false;
    }
    
    RECT triggerZone;
    
    if (m_orientation == Orientation::HORIZONTAL) {
        // Horizontal toolbar at top or bottom
        if (m_y == 0) {
            // Top toolbar
            triggerZone.left = m_x;
            triggerZone.right = m_x + m_width;
            triggerZone.top = 0;
            triggerZone.bottom = m_triggerZoneSize;
        } else {
            // Bottom toolbar (or arbitrary position - trigger above)
            triggerZone.left = m_x;
            triggerZone.right = m_x + m_width;
            triggerZone.top = m_y - m_triggerZoneSize;
            triggerZone.bottom = m_y;
        }
    } else {
        // Vertical toolbar at left or right
        if (m_x == 0) {
            // Left toolbar
            triggerZone.left = 0;
            triggerZone.right = m_triggerZoneSize;
            triggerZone.top = m_y;
            triggerZone.bottom = m_y + m_height;
        } else {
            // Right toolbar (or arbitrary position - trigger to the left)
            triggerZone.left = m_x - m_triggerZoneSize;
            triggerZone.right = m_x;
            triggerZone.top = m_y;
            triggerZone.bottom = m_y + m_height;
        }
    }
    
    POINT pt = {x, y};
    return PtInRect(&triggerZone, pt) != 0;
}

Toolbar::ItemLayout* Toolbar::GetItemAt(int x, int y) {
    for (auto& layout : m_itemLayouts) {
        POINT pt = {x, y};
        if (PtInRect(&layout.rect, pt)) {
            return &layout;
        }
    }
    return nullptr;
}

bool Toolbar::HandleMouseMove(int x, int y) {
    if (!m_visible) {
        return false;
    }
    
    // Update hover states
    bool anyHovered = false;
    for (auto& layout : m_itemLayouts) {
        POINT pt = {x, y};
        layout.hovered = !layout.item->separator && 
                         layout.item->enabled &&
                         PtInRect(&layout.rect, pt) != 0;
        if (layout.hovered) {
            anyHovered = true;
            m_hoveredItemId = layout.item->id;
        }
    }
    
    if (!anyHovered) {
        m_hoveredItemId = -1;
    }
    
    m_hovered = HitTest(x, y);
    
    return m_hovered;
}

bool Toolbar::HandleMouseDown(int x, int y, int button) {
    if (!m_visible || button != 0) { // 0 = left button
        return false;
    }
    
    ItemLayout* item = GetItemAt(x, y);
    if (item && !item->item->separator && item->item->enabled) {
        item->pressed = true;
        m_pressedItemId = item->item->id;
        return true;
    }
    
    return false;
}

bool Toolbar::HandleMouseUp(int x, int y, int button) {
    if (!m_visible || button != 0) {
        return false;
    }
    
    bool handled = false;
    
    for (auto& layout : m_itemLayouts) {
        if (layout.pressed) {
            layout.pressed = false;
            
            POINT pt = {x, y};
            if (PtInRect(&layout.rect, pt) && layout.item->enabled) {
                // Item was clicked
                if (m_itemClickCallback) {
                    m_itemClickCallback(layout.item->id);
                }
                TriggerEvent(WidgetEvent::CLICK, &layout.item->id);
                handled = true;
            }
        }
    }
    
    m_pressedItemId = -1;
    
    return handled;
}

} // namespace SDK
