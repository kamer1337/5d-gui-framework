#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include "Widget.h"
#include "Theme.h"

namespace SDK {

/**
 * Toolbar - A customizable toolbar widget supporting horizontal/vertical orientation and auto-hide
 * Provides a container for toolbar items (buttons) with optional icons and text
 */
class Toolbar : public Widget {
public:
    enum class Orientation {
        HORIZONTAL,
        VERTICAL
    };
    
    struct ToolbarItem {
        int id;
        std::wstring text;
        std::wstring tooltip;
        HBITMAP icon;
        bool enabled;
        bool separator;
        void* userData;
        
        ToolbarItem(int id = 0, const std::wstring& text = L"")
            : id(id), text(text), icon(nullptr), enabled(true), 
              separator(false), userData(nullptr) {}
    };
    
    using ItemClickCallback = std::function<void(int itemId)>;
    
    Toolbar();
    virtual ~Toolbar();
    
    // Item management
    void AddItem(int id, const std::wstring& text, const std::wstring& tooltip = L"");
    void AddSeparator();
    void RemoveItem(int id);
    void ClearItems();
    
    void SetItemEnabled(int id, bool enabled);
    void SetItemIcon(int id, HBITMAP icon);
    
    // Orientation
    void SetOrientation(Orientation orientation);
    Orientation GetOrientation() const { return m_orientation; }
    
    // Auto-hide functionality
    void SetAutoHide(bool autoHide);
    bool IsAutoHide() const { return m_autoHide; }
    
    void SetAutoHideDelay(float seconds) { m_autoHideDelay = seconds; }
    float GetAutoHideDelay() const { return m_autoHideDelay; }
    
    void SetTriggerZoneSize(int pixels) { m_triggerZoneSize = pixels; }
    int GetTriggerZoneSize() const { return m_triggerZoneSize; }
    
    bool IsVisible() const { return m_currentlyVisible; }
    
    // Appearance
    void SetItemSize(int width, int height);
    void GetItemSize(int& width, int& height) const;
    
    void SetBackgroundColor(const Color& color) { m_backgroundColor = color; }
    void SetItemColor(const Color& color) { m_itemColor = color; }
    void SetItemHoverColor(const Color& color) { m_itemHoverColor = color; }
    void SetItemPressedColor(const Color& color) { m_itemPressedColor = color; }
    void SetTextColor(const Color& color) { m_textColor = color; }
    void SetSeparatorColor(const Color& color) { m_separatorColor = color; }
    
    void SetPadding(int padding) { m_padding = padding; }
    int GetPadding() const { return m_padding; }
    
    void SetItemSpacing(int spacing) { m_itemSpacing = spacing; }
    int GetItemSpacing() const { return m_itemSpacing; }
    
    // Callbacks
    void SetItemClickCallback(ItemClickCallback callback) { m_itemClickCallback = callback; }
    
    // Widget overrides
    void Render(HDC hdc) override;
    void Update(float deltaTime) override;
    bool HandleMouseMove(int x, int y) override;
    bool HandleMouseDown(int x, int y, int button) override;
    bool HandleMouseUp(int x, int y, int button) override;
    
private:
    struct ItemLayout {
        RECT rect;
        ToolbarItem* item;
        bool hovered;
        bool pressed;
    };
    
    void CalculateLayout();
    void RenderHorizontal(HDC hdc);
    void RenderVertical(HDC hdc);
    void RenderBackground(HDC hdc);
    void RenderItems(HDC hdc);
    void RenderItem(HDC hdc, const ItemLayout& layout);
    
    bool IsInTriggerZone(int x, int y) const;
    void UpdateAutoHideState(int mouseX, int mouseY, float deltaTime);
    
    ItemLayout* GetItemAt(int x, int y);
    
    std::vector<ToolbarItem> m_items;
    std::vector<ItemLayout> m_itemLayouts;
    
    Orientation m_orientation;
    bool m_autoHide;
    bool m_currentlyVisible;
    float m_autoHideDelay;
    float m_hideTimer;
    int m_triggerZoneSize;
    
    int m_itemWidth;
    int m_itemHeight;
    int m_padding;
    int m_itemSpacing;
    
    Color m_backgroundColor;
    Color m_itemColor;
    Color m_itemHoverColor;
    Color m_itemPressedColor;
    Color m_textColor;
    Color m_separatorColor;
    
    ItemClickCallback m_itemClickCallback;
    
    // Animation state for auto-hide
    float m_visibilityAlpha;
    float m_slideOffset;
    
    int m_hoveredItemId;
    int m_pressedItemId;
};

} // namespace SDK
