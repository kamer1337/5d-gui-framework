#include "../../include/SDK/Widget.h"

namespace SDK {

Widget::Widget()
    : m_x(0), m_y(0)
    , m_width(100), m_height(30)
    , m_visible(true)
    , m_enabled(true)
    , m_hovered(false)
    , m_id(0)
    , m_tag(nullptr)
{
}

void Widget::SetPosition(int x, int y) {
    m_x = x;
    m_y = y;
}

void Widget::GetPosition(int& x, int& y) const {
    x = m_x;
    y = m_y;
}

void Widget::SetSize(int width, int height) {
    m_width = width;
    m_height = height;
}

void Widget::GetSize(int& width, int& height) const {
    width = m_width;
    height = m_height;
}

void Widget::SetBounds(int x, int y, int width, int height) {
    m_x = x;
    m_y = y;
    m_width = width;
    m_height = height;
}

void Widget::GetBounds(RECT& rect) const {
    rect.left = m_x;
    rect.top = m_y;
    rect.right = m_x + m_width;
    rect.bottom = m_y + m_height;
}

void Widget::SetVisible(bool visible) {
    m_visible = visible;
}

void Widget::SetEnabled(bool enabled) {
    m_enabled = enabled;
}

void Widget::SetHovered(bool hovered) {
    if (m_hovered != hovered) {
        m_hovered = hovered;
        if (hovered) {
            OnMouseEnter();
        } else {
            OnMouseLeave();
        }
    }
}

bool Widget::HitTest(int x, int y) const {
    if (!m_visible) return false;
    return x >= m_x && x < m_x + m_width &&
           y >= m_y && y < m_y + m_height;
}

void Widget::Update(float deltaTime) {
    // Base implementation does nothing
    // Derived classes can override for animations
}

void Widget::OnMouseEnter() {
    // Override in derived classes
}

void Widget::OnMouseLeave() {
    // Override in derived classes
}

void Widget::OnMouseMove(int x, int y) {
    // Override in derived classes
}

void Widget::OnMouseDown(int x, int y) {
    // Override in derived classes
}

void Widget::OnMouseUp(int x, int y) {
    // Override in derived classes
}

void Widget::OnClick() {
    // Override in derived classes
}

} // namespace SDK
