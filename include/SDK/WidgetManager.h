#pragma once

#include "Widget.h"
#include <vector>
#include <memory>

namespace SDK {

/**
 * WidgetManager - Manages widgets within a window
 * Handles widget rendering, events, and lifecycle
 */
class WidgetManager {
public:
    WidgetManager();
    ~WidgetManager();
    
    // Add/remove widgets
    void AddWidget(std::shared_ptr<Widget> widget);
    void RemoveWidget(std::shared_ptr<Widget> widget);
    void RemoveWidgetById(int id);
    void Clear();
    
    // Find widgets
    std::shared_ptr<Widget> GetWidgetById(int id) const;
    std::shared_ptr<Widget> GetWidgetAt(int x, int y) const;
    
    // Render all widgets
    void RenderAll(HDC hdc);
    
    // Update all widgets (for animations)
    void UpdateAll(float deltaTime);
    
    // Mouse event handling
    void HandleMouseMove(int x, int y);
    void HandleMouseDown(int x, int y);
    void HandleMouseUp(int x, int y);
    
    // Get all widgets
    const std::vector<std::shared_ptr<Widget>>& GetWidgets() const { return m_widgets; }
    
    // Enable/disable all widgets
    void SetAllEnabled(bool enabled);
    
private:
    std::vector<std::shared_ptr<Widget>> m_widgets;
    std::shared_ptr<Widget> m_hoveredWidget;
    std::shared_ptr<Widget> m_pressedWidget;
};

} // namespace SDK
