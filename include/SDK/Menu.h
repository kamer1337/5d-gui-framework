#pragma once

#include "Widget.h"
#include "Theme.h"
#include <string>
#include <vector>
#include <memory>
#include <functional>

namespace SDK {

/**
 * MenuItem - Represents a single menu item
 */
class MenuItem {
public:
    enum class Type {
        NORMAL,
        SEPARATOR,
        SUBMENU,
        CHECKBOX,
        RADIO
    };
    
    MenuItem(const std::wstring& text, Type type = Type::NORMAL);
    ~MenuItem() = default;
    
    // Properties
    void SetText(const std::wstring& text) { m_text = text; }
    std::wstring GetText() const { return m_text; }
    
    void SetType(Type type) { m_type = type; }
    Type GetType() const { return m_type; }
    
    void SetEnabled(bool enabled) { m_enabled = enabled; }
    bool IsEnabled() const { return m_enabled; }
    
    void SetChecked(bool checked) { m_checked = checked; }
    bool IsChecked() const { return m_checked; }
    
    void SetShortcut(const std::wstring& shortcut) { m_shortcut = shortcut; }
    std::wstring GetShortcut() const { return m_shortcut; }
    
    void SetIcon(HICON icon) { m_icon = icon; }
    HICON GetIcon() const { return m_icon; }
    
    // Submenu
    void AddSubMenuItem(std::shared_ptr<MenuItem> item);
    std::vector<std::shared_ptr<MenuItem>>& GetSubMenuItems() { return m_subItems; }
    const std::vector<std::shared_ptr<MenuItem>>& GetSubMenuItems() const { return m_subItems; }
    bool HasSubMenu() const { return !m_subItems.empty(); }
    
    // Callback
    void SetOnClick(std::function<void()> callback) { m_onClick = callback; }
    void Click();
    
private:
    std::wstring m_text;
    Type m_type;
    bool m_enabled;
    bool m_checked;
    std::wstring m_shortcut;
    HICON m_icon;
    
    std::vector<std::shared_ptr<MenuItem>> m_subItems;
    std::function<void()> m_onClick;
};

/**
 * Menu - Base menu class
 */
class Menu : public Widget {
public:
    Menu();
    ~Menu() override = default;
    
    // Menu items
    void AddItem(std::shared_ptr<MenuItem> item);
    void RemoveItem(std::shared_ptr<MenuItem> item);
    void Clear();
    
    std::vector<std::shared_ptr<MenuItem>>& GetItems() { return m_items; }
    const std::vector<std::shared_ptr<MenuItem>>& GetItems() const { return m_items; }
    
    // Widget overrides
    void Render(HDC hdc) override;
    void HandleEvent(WidgetEvent event, void* data) override;
    
    // Menu appearance
    void SetItemHeight(int height) { m_itemHeight = height; }
    int GetItemHeight() const { return m_itemHeight; }
    
    void SetShowIcons(bool show) { m_showIcons = show; }
    bool GetShowIcons() const { return m_showIcons; }
    
protected:
    virtual void RenderItem(HDC hdc, const RECT& rect, std::shared_ptr<MenuItem> item, bool hovered, bool selected);
    
    std::vector<std::shared_ptr<MenuItem>> m_items;
    int m_itemHeight;
    bool m_showIcons;
    int m_hoveredIndex;
    int m_selectedIndex;
};

/**
 * ContextMenu - Popup menu that appears at a specific location
 */
class ContextMenu : public Menu {
public:
    ContextMenu();
    ~ContextMenu() override = default;
    
    // Show/hide
    void Show(int x, int y);
    void Hide();
    
    bool IsVisible() const { return m_visible; }
    
    // Auto-hide on focus loss
    void SetAutoHide(bool autoHide) { m_autoHide = autoHide; }
    bool IsAutoHide() const { return m_autoHide; }
    
    // Widget overrides
    void Render(HDC hdc) override;
    void HandleEvent(WidgetEvent event, void* data) override;
    
private:
    bool m_visible;
    bool m_autoHide;
};

/**
 * MenuBar - Horizontal menu bar widget
 */
class MenuBar : public Widget {
public:
    MenuBar();
    ~MenuBar() override = default;
    
    // Menus
    void AddMenu(const std::wstring& title, std::shared_ptr<Menu> menu);
    void RemoveMenu(const std::wstring& title);
    void Clear();
    
    // Widget overrides
    void Render(HDC hdc) override;
    void HandleEvent(WidgetEvent event, void* data) override;
    
    // Appearance
    void SetMenuHeight(int height) { m_menuHeight = height; }
    int GetMenuHeight() const { return m_menuHeight; }
    
private:
    struct MenuEntry {
        std::wstring title;
        std::shared_ptr<Menu> menu;
        RECT bounds;
    };
    
    std::vector<MenuEntry> m_menus;
    int m_menuHeight;
    int m_hoveredIndex;
    int m_activeIndex;
    
    void LayoutMenus();
    void ShowMenu(int index);
    void HideActiveMenu();
};

} // namespace SDK
