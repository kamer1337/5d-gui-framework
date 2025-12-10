#include "../../include/SDK/WidgetManager.h"
#include <algorithm>

namespace SDK {

WidgetManager::WidgetManager()
    : m_hoveredWidget(nullptr)
    , m_pressedWidget(nullptr)
{
}

WidgetManager::~WidgetManager() {
    Clear();
}

void WidgetManager::AddWidget(std::shared_ptr<Widget> widget) {
    if (widget) {
        m_widgets.push_back(widget);
    }
}

void WidgetManager::RemoveWidget(std::shared_ptr<Widget> widget) {
    if (!widget) return;
    
    auto it = std::find(m_widgets.begin(), m_widgets.end(), widget);
    if (it != m_widgets.end()) {
        if (m_hoveredWidget == widget) {
            m_hoveredWidget = nullptr;
        }
        if (m_pressedWidget == widget) {
            m_pressedWidget = nullptr;
        }
        m_widgets.erase(it);
    }
}

void WidgetManager::RemoveWidgetById(int id) {
    auto widget = GetWidgetById(id);
    if (widget) {
        RemoveWidget(widget);
    }
}

void WidgetManager::Clear() {
    m_widgets.clear();
    m_hoveredWidget = nullptr;
    m_pressedWidget = nullptr;
}

std::shared_ptr<Widget> WidgetManager::GetWidgetById(int id) const {
    for (const auto& widget : m_widgets) {
        if (widget->GetId() == id) {
            return widget;
        }
    }
    return nullptr;
}

std::shared_ptr<Widget> WidgetManager::GetWidgetAt(int x, int y) const {
    // Search in reverse order (top to bottom)
    for (auto it = m_widgets.rbegin(); it != m_widgets.rend(); ++it) {
        if ((*it)->HitTest(x, y)) {
            return *it;
        }
    }
    return nullptr;
}

void WidgetManager::RenderAll(HDC hdc) {
    for (const auto& widget : m_widgets) {
        widget->Render(hdc);
    }
}

void WidgetManager::UpdateAll(float deltaTime) {
    for (const auto& widget : m_widgets) {
        widget->Update(deltaTime);
    }
}

void WidgetManager::HandleMouseMove(int x, int y) {
    auto widget = GetWidgetAt(x, y);
    
    // Handle hover state changes
    if (widget != m_hoveredWidget) {
        if (m_hoveredWidget) {
            m_hoveredWidget->SetHovered(false);
        }
        
        m_hoveredWidget = widget;
        
        if (m_hoveredWidget) {
            m_hoveredWidget->SetHovered(true);
        }
    }
    
    // Send mouse move to hovered widget
    if (m_hoveredWidget) {
        m_hoveredWidget->OnMouseMove(x, y);
    }
}

void WidgetManager::HandleMouseDown(int x, int y) {
    auto widget = GetWidgetAt(x, y);
    
    if (widget && widget->IsEnabled()) {
        m_pressedWidget = widget;
        widget->OnMouseDown(x, y);
    }
}

void WidgetManager::HandleMouseUp(int x, int y) {
    if (m_pressedWidget) {
        m_pressedWidget->OnMouseUp(x, y);
        
        // Check if mouse is still over the widget for click
        if (m_pressedWidget->HitTest(x, y)) {
            m_pressedWidget->OnClick();
        }
        
        m_pressedWidget = nullptr;
    }
}

void WidgetManager::SetAllEnabled(bool enabled) {
    for (const auto& widget : m_widgets) {
        widget->SetEnabled(enabled);
    }
}

} // namespace SDK
