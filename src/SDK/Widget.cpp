#include "../../include/SDK/Widget.h"
#include "../../include/SDK/Renderer.h"
#include <algorithm>

namespace SDK {

// Widget base class implementation
Widget::Widget()
    : m_x(0), m_y(0), m_width(100), m_height(30)
    , m_visible(true), m_enabled(true), m_focused(false), m_hovered(false)
    , m_parent(nullptr)
    , m_name(L"")
    , m_paddingLeft(0), m_paddingTop(0), m_paddingRight(0), m_paddingBottom(0)
    , m_marginLeft(0), m_marginTop(0), m_marginRight(0), m_marginBottom(0)
    , m_minWidth(0), m_minHeight(0)
    , m_maxWidth(65535), m_maxHeight(65535)
    , m_opacity(1.0f)
    , m_borderWidth(0)
    , m_borderRadius(0)
    , m_tooltipText(L"")
    , m_cursor(nullptr)
    , m_zIndex(0)
    , m_fontFamily(L"Segoe UI")
    , m_fontSize(12)
    , m_fontBold(false)
    , m_fontItalic(false)
    , m_alignment(WidgetAlignment::NONE)
{
}

Widget::~Widget() {
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
    // Apply min/max constraints
    if (width < m_minWidth) width = m_minWidth;
    if (height < m_minHeight) height = m_minHeight;
    if (width > m_maxWidth) width = m_maxWidth;
    if (height > m_maxHeight) height = m_maxHeight;
    
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
    RECT bounds; GetBounds(bounds);
    return x >= bounds.left && x < bounds.right && y >= bounds.top && y < bounds.bottom;
}

void Widget::Update(float deltaTime) {
    if (!m_visible) return;
    
    // Update children
    for (auto& child : m_children) {
        child->Update(deltaTime);
    }
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

void Widget::SetFocused(bool focused) {
    if (m_focused != focused) {
        m_focused = focused;
        TriggerEvent(focused ? WidgetEvent::FOCUS_GAINED : WidgetEvent::FOCUS_LOST);
    }
}

void Widget::SetParent(Widget* parent) {
    m_parent = parent;
}

void Widget::AddChild(std::shared_ptr<Widget> child) {
    child->SetParent(this);
    m_children.push_back(child);
}

void Widget::RemoveChild(std::shared_ptr<Widget> child) {
    auto it = std::find(m_children.begin(), m_children.end(), child);
    if (it != m_children.end()) {
        (*it)->SetParent(nullptr);
        m_children.erase(it);
    }
}

void Widget::Render(HDC hdc) {
    if (!m_visible) return;
    
    // Render children
    for (auto& child : m_children) {
        child->Render(hdc);
    }
}

bool Widget::HandleMouseMove(int x, int y) {
    if (!m_visible || !m_enabled) return false;
    
    bool wasHovered = m_hovered;
    m_hovered = HitTest(x, y);
    
    if (m_hovered && !wasHovered) {
        TriggerEvent(WidgetEvent::MOUSE_ENTER);
    } else if (!m_hovered && wasHovered) {
        TriggerEvent(WidgetEvent::MOUSE_LEAVE);
    }
    
    if (m_hovered) {
        TriggerEvent(WidgetEvent::MOUSE_MOVE);
    }
    
    // Check children
    for (auto& child : m_children) {
        if (child->HandleMouseMove(x, y)) {
            return true;
        }
    }
    
    return m_hovered;
}

bool Widget::HandleMouseDown(int x, int y, int button) {
    if (!m_visible || !m_enabled) return false;
    
    // Check children first
    for (auto& child : m_children) {
        if (child->HandleMouseDown(x, y, button)) {
            return true;
        }
    }
    
    if (HitTest(x, y)) {
        TriggerEvent(WidgetEvent::CLICK);
        return true;
    }
    
    return false;
}

bool Widget::HandleMouseUp(int x, int y, int button) {
    if (!m_visible || !m_enabled) return false;
    
    // Check children
    for (auto& child : m_children) {
        if (child->HandleMouseUp(x, y, button)) {
            return true;
        }
    }
    
    return HitTest(x, y);
}

bool Widget::HandleKeyDown(int keyCode) {
    if (!m_visible || !m_enabled || !m_focused) return false;
    
    TriggerEvent(WidgetEvent::KEY_PRESS, &keyCode);
    return true;
}

bool Widget::HandleKeyUp(int keyCode) {
    if (!m_visible || !m_enabled || !m_focused) return false;
    
    TriggerEvent(WidgetEvent::KEY_RELEASE, &keyCode);
    return true;
}

bool Widget::HandleChar(wchar_t ch) {
    if (!m_visible || !m_enabled || !m_focused) return false;
    
    // Base implementation does nothing but returns true if focused
    // Derived classes should override this method
    return false;
}

void Widget::TriggerEvent(WidgetEvent event, void* data) {
    if (m_eventCallback) {
        m_eventCallback(this, event, data);
    }
}

// Alignment methods
void Widget::AlignToWidget(Widget* target, WidgetAlignment alignment, int spacing) {
    if (!target) return;
    
    RECT targetBounds;
    target->GetBounds(targetBounds);
    
    int newX = m_x;
    int newY = m_y;
    
    switch (alignment) {
        case WidgetAlignment::LEFT:
            newX = targetBounds.left - m_width - spacing;
            newY = targetBounds.top;
            break;
        case WidgetAlignment::RIGHT:
            newX = targetBounds.right + spacing;
            newY = targetBounds.top;
            break;
        case WidgetAlignment::TOP:
            newX = targetBounds.left;
            newY = targetBounds.top - m_height - spacing;
            break;
        case WidgetAlignment::BOTTOM:
            newX = targetBounds.left;
            newY = targetBounds.bottom + spacing;
            break;
        case WidgetAlignment::CENTER:
            newX = targetBounds.left + (targetBounds.right - targetBounds.left - m_width) / 2;
            newY = targetBounds.top + (targetBounds.bottom - targetBounds.top - m_height) / 2;
            break;
        case WidgetAlignment::TOP_LEFT:
            newX = targetBounds.left;
            newY = targetBounds.top;
            break;
        case WidgetAlignment::TOP_RIGHT:
            newX = targetBounds.right - m_width;
            newY = targetBounds.top;
            break;
        case WidgetAlignment::BOTTOM_LEFT:
            newX = targetBounds.left;
            newY = targetBounds.bottom - m_height;
            break;
        case WidgetAlignment::BOTTOM_RIGHT:
            newX = targetBounds.right - m_width;
            newY = targetBounds.bottom - m_height;
            break;
        default:
            break;
    }
    
    SetPosition(newX, newY);
}

void Widget::AlignToParent(WidgetAlignment alignment, int margin) {
    if (!m_parent) return;
    
    RECT parentBounds;
    m_parent->GetBounds(parentBounds);
    
    int newX = m_x;
    int newY = m_y;
    
    switch (alignment) {
        case WidgetAlignment::LEFT:
            newX = parentBounds.left + margin;
            newY = m_y;
            break;
        case WidgetAlignment::RIGHT:
            newX = parentBounds.right - m_width - margin;
            newY = m_y;
            break;
        case WidgetAlignment::TOP:
            newX = m_x;
            newY = parentBounds.top + margin;
            break;
        case WidgetAlignment::BOTTOM:
            newX = m_x;
            newY = parentBounds.bottom - m_height - margin;
            break;
        case WidgetAlignment::CENTER:
            newX = parentBounds.left + (parentBounds.right - parentBounds.left - m_width) / 2;
            newY = parentBounds.top + (parentBounds.bottom - parentBounds.top - m_height) / 2;
            break;
        case WidgetAlignment::TOP_LEFT:
            newX = parentBounds.left + margin;
            newY = parentBounds.top + margin;
            break;
        case WidgetAlignment::TOP_RIGHT:
            newX = parentBounds.right - m_width - margin;
            newY = parentBounds.top + margin;
            break;
        case WidgetAlignment::BOTTOM_LEFT:
            newX = parentBounds.left + margin;
            newY = parentBounds.bottom - m_height - margin;
            break;
        case WidgetAlignment::BOTTOM_RIGHT:
            newX = parentBounds.right - m_width - margin;
            newY = parentBounds.bottom - m_height - margin;
            break;
        default:
            break;
    }
    
    SetPosition(newX, newY);
}

// Overlap detection methods
bool Widget::CheckOverlap(const Widget* other) const {
    if (!other) return false;
    
    RECT thisBounds, otherBounds;
    GetBounds(thisBounds);
    other->GetBounds(otherBounds);
    
    return !(thisBounds.right <= otherBounds.left ||
             thisBounds.left >= otherBounds.right ||
             thisBounds.bottom <= otherBounds.top ||
             thisBounds.top >= otherBounds.bottom);
}

bool Widget::CheckOverlap(int x, int y, int width, int height) const {
    RECT thisBounds;
    GetBounds(thisBounds);
    
    RECT otherBounds = {x, y, x + width, y + height};
    
    return !(thisBounds.right <= otherBounds.left ||
             thisBounds.left >= otherBounds.right ||
             thisBounds.bottom <= otherBounds.top ||
             thisBounds.top >= otherBounds.bottom);
}

void Widget::ResolveOverlap(Widget* other, int spacing) {
    if (!other || !CheckOverlap(other)) return;
    
    RECT thisBounds, otherBounds;
    GetBounds(thisBounds);
    other->GetBounds(otherBounds);
    
    // Calculate overlap amounts in each direction
    int overlapLeft = thisBounds.right - otherBounds.left;
    int overlapRight = otherBounds.right - thisBounds.left;
    int overlapTop = thisBounds.bottom - otherBounds.top;
    int overlapBottom = otherBounds.bottom - thisBounds.top;
    
    // Find minimum overlap direction
    int minOverlap = overlapLeft;
    int direction = 0; // 0=left, 1=right, 2=top, 3=bottom
    
    if (overlapRight < minOverlap) { minOverlap = overlapRight; direction = 1; }
    if (overlapTop < minOverlap) { minOverlap = overlapTop; direction = 2; }
    if (overlapBottom < minOverlap) { minOverlap = overlapBottom; direction = 3; }
    
    // Move this widget in the direction of minimum overlap
    switch (direction) {
        case 0: // Move left
            SetPosition(otherBounds.left - m_width - spacing, m_y);
            break;
        case 1: // Move right
            SetPosition(otherBounds.right + spacing, m_y);
            break;
        case 2: // Move up
            SetPosition(m_x, otherBounds.top - m_height - spacing);
            break;
        case 3: // Move down
            SetPosition(m_x, otherBounds.bottom + spacing);
            break;
    }
}

// Button implementation
Button::Button(const std::wstring& text)
    : Widget()
    , m_text(text)
    , m_backgroundColor(100, 149, 237, 255)
    , m_textColor(255, 255, 255, 255)
    , m_hoverColor(135, 206, 250, 255)
    , m_pressColor(65, 105, 225, 255)
    , m_pressed(false)
{
    m_height = 35;
}

Button::~Button() {
}

void Button::Render(HDC hdc) {
    if (!m_visible) return;
    
    RECT bounds; GetBounds(bounds);
    
    // Choose color based on state
    Color bgColor = m_backgroundColor;
    if (m_pressed) {
        bgColor = m_pressColor;
    } else if (m_hovered) {
        bgColor = m_hoverColor;
    }
    
    // Draw button background
    Renderer::DrawRoundedRect(hdc, bounds, 8, bgColor, Color(0, 0, 0, 100), 1);
    
    // Draw text
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, m_textColor.ToCOLORREF());
    DrawTextW(hdc, m_text.c_str(), -1, &bounds, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    
    // Render children
    Widget::Render(hdc);
}

bool Button::HandleMouseMove(int x, int y) {
    bool result = Widget::HandleMouseMove(x, y);
    return result;
}

bool Button::HandleMouseDown(int x, int y, int button) {
    if (!m_visible || !m_enabled) return false;
    
    if (HitTest(x, y) && button == 0) {
        m_pressed = true;
        return true;
    }
    
    return false;
}

bool Button::HandleMouseUp(int x, int y, int button) {
    if (!m_visible || !m_enabled) return false;
    
    if (m_pressed && button == 0) {
        m_pressed = false;
        if (HitTest(x, y)) {
            TriggerEvent(WidgetEvent::CLICK);
        }
        return true;
    }
    
    return false;
}

// Label implementation
Label::Label(const std::wstring& text)
    : Widget()
    , m_text(text)
    , m_textColor(50, 50, 50, 255)
    , m_alignment(DT_LEFT | DT_VCENTER | DT_SINGLELINE)
{
    m_height = 20;
}

Label::~Label() {
}

void Label::Render(HDC hdc) {
    if (!m_visible) return;
    
    RECT bounds; GetBounds(bounds);
    
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, m_textColor.ToCOLORREF());
    DrawTextW(hdc, m_text.c_str(), -1, &bounds, m_alignment);
    
    Widget::Render(hdc);
}

// TextBox implementation
TextBox::TextBox()
    : Widget()
    , m_maxLength(256)
    , m_cursorPosition(0)
    , m_showCursor(true)
    , m_cursorBlinkTime(0.0f)
    , m_backgroundColor(255, 255, 255, 255)
    , m_textColor(0, 0, 0, 255)
    , m_borderColor(128, 128, 128, 255)
{
    m_height = 30;
}

TextBox::~TextBox() {
}

void TextBox::SetText(const std::wstring& text) {
    if (m_maxLength > 0 && text.length() > (size_t)m_maxLength) {
        m_text = text.substr(0, m_maxLength);
    } else {
        m_text = text;
    }
    m_cursorPosition = (int)m_text.length();
    TriggerEvent(WidgetEvent::TEXT_CHANGED);
}

void TextBox::Render(HDC hdc) {
    if (!m_visible) return;
    
    RECT bounds; GetBounds(bounds);
    
    // Draw background and border
    Renderer::DrawRoundedRect(hdc, bounds, 4, m_backgroundColor, m_borderColor, m_focused ? 2 : 1);
    
    // Draw text or placeholder
    RECT textRect = bounds;
    textRect.left += 5;
    textRect.right -= 5;
    
    SetBkMode(hdc, TRANSPARENT);
    
    if (m_text.empty() && !m_placeholder.empty()) {
        SetTextColor(hdc, RGB(150, 150, 150));
        DrawTextW(hdc, m_placeholder.c_str(), -1, &textRect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    } else {
        SetTextColor(hdc, m_textColor.ToCOLORREF());
        DrawTextW(hdc, m_text.c_str(), -1, &textRect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
        
        // Draw cursor if focused
        if (m_focused && m_showCursor) {
            SIZE textSize;
            GetTextExtentPoint32W(hdc, m_text.c_str(), m_cursorPosition, &textSize);
            int cursorX = textRect.left + textSize.cx;
            int cursorY1 = textRect.top + 5;
            int cursorY2 = textRect.bottom - 5;
            
            HPEN pen = CreatePen(PS_SOLID, 1, m_textColor.ToCOLORREF());
            HPEN oldPen = (HPEN)SelectObject(hdc, pen);
            MoveToEx(hdc, cursorX, cursorY1, nullptr);
            LineTo(hdc, cursorX, cursorY2);
            SelectObject(hdc, oldPen);
            DeleteObject(pen);
        }
    }
    
    Widget::Render(hdc);
}

bool TextBox::HandleMouseDown(int x, int y, int button) {
    if (!m_visible || !m_enabled) return false;
    
    if (HitTest(x, y)) {
        SetFocused(true);
        return true;
    }
    
    return false;
}

bool TextBox::HandleKeyDown(int keyCode) {
    if (!Widget::HandleKeyDown(keyCode)) return false;
    
    if (keyCode == VK_BACK && m_cursorPosition > 0) {
        m_text.erase(m_cursorPosition - 1, 1);
        m_cursorPosition--;
        TriggerEvent(WidgetEvent::TEXT_CHANGED);
        return true;
    } else if (keyCode == VK_DELETE && m_cursorPosition < (int)m_text.length()) {
        m_text.erase(m_cursorPosition, 1);
        TriggerEvent(WidgetEvent::TEXT_CHANGED);
        return true;
    } else if (keyCode == VK_LEFT && m_cursorPosition > 0) {
        m_cursorPosition--;
        return true;
    } else if (keyCode == VK_RIGHT && m_cursorPosition < (int)m_text.length()) {
        m_cursorPosition++;
        return true;
    } else if (keyCode == VK_HOME) {
        m_cursorPosition = 0;
        return true;
    } else if (keyCode == VK_END) {
        m_cursorPosition = (int)m_text.length();
        return true;
    }
    
    return false;
}

bool TextBox::HandleChar(wchar_t ch) {
    // Check if widget can handle input
    if (!m_visible || !m_enabled || !m_focused) return false;
    
    if (ch >= 32 && ch != 127) { // Printable character
        if (m_maxLength <= 0 || (int)m_text.length() < m_maxLength) {
            m_text.insert(m_cursorPosition, 1, ch);
            m_cursorPosition++;
            TriggerEvent(WidgetEvent::TEXT_CHANGED);
            return true;
        }
    }
    
    return false;
}

void TextBox::UpdateCursorPosition() {
    // Cursor blink animation can be handled in Update()
}

// CheckBox implementation
CheckBox::CheckBox(const std::wstring& text)
    : Widget()
    , m_text(text)
    , m_checked(false)
    , m_checkColor(0, 128, 0, 255)
    , m_boxColor(200, 200, 200, 255)
{
    m_height = 20;
    m_width = 20;
}

CheckBox::~CheckBox() {
}

void CheckBox::SetChecked(bool checked) {
    if (m_checked != checked) {
        m_checked = checked;
        TriggerEvent(WidgetEvent::VALUE_CHANGED, &m_checked);
    }
}

void CheckBox::Render(HDC hdc) {
    if (!m_visible) return;
    
    RECT boxRect = {m_x, m_y, m_x + 20, m_y + 20};
    
    // Draw checkbox box
    Renderer::DrawRoundedRect(hdc, boxRect, 3, Color(255, 255, 255, 255), m_boxColor, 2);
    
    // Draw checkmark if checked
    if (m_checked) {
        HPEN pen = CreatePen(PS_SOLID, 2, m_checkColor.ToCOLORREF());
        HPEN oldPen = (HPEN)SelectObject(hdc, pen);
        
        // Draw checkmark
        MoveToEx(hdc, m_x + 4, m_y + 10, nullptr);
        LineTo(hdc, m_x + 8, m_y + 14);
        LineTo(hdc, m_x + 16, m_y + 6);
        
        SelectObject(hdc, oldPen);
        DeleteObject(pen);
    }
    
    // Draw label
    if (!m_text.empty()) {
        RECT textRect = {m_x + 25, m_y, m_x + m_width, m_y + m_height};
        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, RGB(50, 50, 50));
        DrawTextW(hdc, m_text.c_str(), -1, &textRect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    }
    
    Widget::Render(hdc);
}

bool CheckBox::HandleMouseDown(int x, int y, int button) {
    if (!m_visible || !m_enabled) return false;
    
    if (HitTest(x, y) && button == 0) {
        SetChecked(!m_checked);
        return true;
    }
    
    return false;
}

// Separator implementation
Separator::Separator(Orientation orientation)
    : Widget()
    , m_orientation(orientation)
    , m_color(200, 200, 200, 255)
{
    if (orientation == Orientation::HORIZONTAL) {
        m_height = 1;
        m_width = 100;
    } else {
        m_height = 100;
        m_width = 1;
    }
}

Separator::~Separator() {
}

void Separator::Render(HDC hdc) {
    if (!m_visible) return;
    
    RECT bounds; GetBounds(bounds);
    
    HBRUSH brush = CreateSolidBrush(m_color.ToCOLORREF());
    FillRect(hdc, &bounds, brush);
    DeleteObject(brush);
    
    Widget::Render(hdc);
}

// Image implementation
Image::Image()
    : Widget()
    , m_bitmap(nullptr)
    , m_stretch(false)
    , m_imageWidth(0)
    , m_imageHeight(0)
{
}

Image::~Image() {
    if (m_bitmap) {
        DeleteObject(m_bitmap);
    }
}

bool Image::LoadFromFile(const std::wstring& filename) {
    if (m_bitmap) {
        DeleteObject(m_bitmap);
        m_bitmap = nullptr;
    }
    
    m_bitmap = (HBITMAP)LoadImageW(nullptr, filename.c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    
    if (m_bitmap) {
        BITMAP bm;
        GetObject(m_bitmap, sizeof(BITMAP), &bm);
        m_imageWidth = bm.bmWidth;
        m_imageHeight = bm.bmHeight;
        return true;
    }
    
    return false;
}

bool Image::LoadFromResource(HINSTANCE hInstance, int resourceId) {
    if (m_bitmap) {
        DeleteObject(m_bitmap);
        m_bitmap = nullptr;
    }
    
    m_bitmap = LoadBitmap(hInstance, MAKEINTRESOURCE(resourceId));
    
    if (m_bitmap) {
        BITMAP bm;
        GetObject(m_bitmap, sizeof(BITMAP), &bm);
        m_imageWidth = bm.bmWidth;
        m_imageHeight = bm.bmHeight;
        return true;
    }
    
    return false;
}

void Image::SetHBITMAP(HBITMAP bitmap) {
    if (m_bitmap) {
        DeleteObject(m_bitmap);
    }
    
    m_bitmap = bitmap;
    
    if (m_bitmap) {
        BITMAP bm;
        GetObject(m_bitmap, sizeof(BITMAP), &bm);
        m_imageWidth = bm.bmWidth;
        m_imageHeight = bm.bmHeight;
    }
}

void Image::Render(HDC hdc) {
    if (!m_visible || !m_bitmap) return;
    
    RECT bounds; GetBounds(bounds);
    
    HDC memDC = CreateCompatibleDC(hdc);
    HBITMAP oldBitmap = (HBITMAP)SelectObject(memDC, m_bitmap);
    
    if (m_stretch) {
        StretchBlt(hdc, bounds.left, bounds.top, m_width, m_height,
                   memDC, 0, 0, m_imageWidth, m_imageHeight, SRCCOPY);
    } else {
        BitBlt(hdc, bounds.left, bounds.top, m_imageWidth, m_imageHeight,
               memDC, 0, 0, SRCCOPY);
    }
    
    SelectObject(memDC, oldBitmap);
    DeleteDC(memDC);
    
    Widget::Render(hdc);
}

// Slider implementation
Slider::Slider(Orientation orientation)
    : Widget()
    , m_orientation(orientation)
    , m_value(0.0f)
    , m_minValue(0.0f)
    , m_maxValue(100.0f)
    , m_dragging(false)
    , m_trackColor(200, 200, 200, 255)
    , m_thumbColor(100, 149, 237, 255)
    , m_fillColor(135, 206, 250, 255)
{
    if (orientation == Orientation::HORIZONTAL) {
        m_width = 200;
        m_height = 30;
    } else {
        m_width = 30;
        m_height = 200;
    }
}

Slider::~Slider() {
}

void Slider::SetValue(float value) {
    if (value < m_minValue) value = m_minValue;
    if (value > m_maxValue) value = m_maxValue;
    
    if (m_value != value) {
        m_value = value;
        TriggerEvent(WidgetEvent::VALUE_CHANGED, &m_value);
    }
}

void Slider::SetRange(float minValue, float maxValue) {
    // Validate and swap if necessary
    if (minValue > maxValue) {
        float temp = minValue;
        minValue = maxValue;
        maxValue = temp;
    }
    
    m_minValue = minValue;
    m_maxValue = maxValue;
    if (m_value < m_minValue) m_value = m_minValue;
    if (m_value > m_maxValue) m_value = m_maxValue;
}

void Slider::GetRange(float& minValue, float& maxValue) const {
    minValue = m_minValue;
    maxValue = m_maxValue;
}

void Slider::Render(HDC hdc) {
    if (!m_visible) return;
    
    RECT bounds; GetBounds(bounds);
    
    // Protect against division by zero
    if (m_maxValue == m_minValue) {
        // Draw track only
        if (m_orientation == Orientation::HORIZONTAL) {
            int trackHeight = 4;
            int trackY = (bounds.top + bounds.bottom) / 2 - trackHeight / 2;
            RECT trackRect = {bounds.left + 10, trackY, bounds.right - 10, trackY + trackHeight};
            Renderer::DrawRoundedRect(hdc, trackRect, 2, m_trackColor, m_trackColor, 0);
        } else {
            int trackWidth = 4;
            int trackX = (bounds.left + bounds.right) / 2 - trackWidth / 2;
            RECT trackRect = {trackX, bounds.top + 10, trackX + trackWidth, bounds.bottom - 10};
            Renderer::DrawRoundedRect(hdc, trackRect, 2, m_trackColor, m_trackColor, 0);
        }
        Widget::Render(hdc);
        return;
    }
    
    if (m_orientation == Orientation::HORIZONTAL) {
        // Draw track
        int trackHeight = 4;
        int trackY = (bounds.top + bounds.bottom) / 2 - trackHeight / 2;
        RECT trackRect = {bounds.left + 10, trackY, bounds.right - 10, trackY + trackHeight};
        Renderer::DrawRoundedRect(hdc, trackRect, 2, m_trackColor, m_trackColor, 0);
        
        // Draw filled portion
        float ratio = (m_value - m_minValue) / (m_maxValue - m_minValue);
        int fillWidth = (int)((bounds.right - bounds.left - 20) * ratio);
        RECT fillRect = {bounds.left + 10, trackY, bounds.left + 10 + fillWidth, trackY + trackHeight};
        Renderer::DrawRoundedRect(hdc, fillRect, 2, m_fillColor, m_fillColor, 0);
        
        // Draw thumb
        int thumbX = bounds.left + 10 + fillWidth - 8;
        int thumbY = (bounds.top + bounds.bottom) / 2 - 8;
        RECT thumbRect = {thumbX, thumbY, thumbX + 16, thumbY + 16};
        Renderer::DrawRoundedRect(hdc, thumbRect, 8, m_thumbColor, Color(50, 50, 50, 255), 1);
    } else {
        // Vertical slider
        int trackWidth = 4;
        int trackX = (bounds.left + bounds.right) / 2 - trackWidth / 2;
        RECT trackRect = {trackX, bounds.top + 10, trackX + trackWidth, bounds.bottom - 10};
        Renderer::DrawRoundedRect(hdc, trackRect, 2, m_trackColor, m_trackColor, 0);
        
        // Draw filled portion
        float ratio = (m_value - m_minValue) / (m_maxValue - m_minValue);
        int fillHeight = (int)((bounds.bottom - bounds.top - 20) * ratio);
        RECT fillRect = {trackX, bounds.bottom - 10 - fillHeight, trackX + trackWidth, bounds.bottom - 10};
        Renderer::DrawRoundedRect(hdc, fillRect, 2, m_fillColor, m_fillColor, 0);
        
        // Draw thumb
        int thumbX = (bounds.left + bounds.right) / 2 - 8;
        int thumbY = bounds.bottom - 10 - fillHeight - 8;
        RECT thumbRect = {thumbX, thumbY, thumbX + 16, thumbY + 16};
        Renderer::DrawRoundedRect(hdc, thumbRect, 8, m_thumbColor, Color(50, 50, 50, 255), 1);
    }
    
    Widget::Render(hdc);
}

bool Slider::HandleMouseDown(int x, int y, int button) {
    if (!m_visible || !m_enabled) return false;
    
    if (HitTest(x, y) && button == 0) {
        m_dragging = true;
        UpdateValueFromPosition(x, y);
        return true;
    }
    
    return false;
}

bool Slider::HandleMouseMove(int x, int y) {
    if (!m_visible || !m_enabled) return false;
    
    bool result = Widget::HandleMouseMove(x, y);
    
    if (m_dragging) {
        UpdateValueFromPosition(x, y);
    }
    
    return result || m_dragging;
}

bool Slider::HandleMouseUp(int x, int y, int button) {
    if (!m_visible || !m_enabled) return false;
    
    if (m_dragging && button == 0) {
        m_dragging = false;
        return true;
    }
    
    return false;
}

void Slider::UpdateValueFromPosition(int x, int y) {
    RECT bounds; GetBounds(bounds);
    
    if (m_orientation == Orientation::HORIZONTAL) {
        int trackLeft = bounds.left + 10;
        int trackRight = bounds.right - 10;
        int trackWidth = trackRight - trackLeft;
        
        if (trackWidth <= 0 || m_maxValue == m_minValue) return;
        
        int relX = x - trackLeft;
        if (relX < 0) relX = 0;
        if (relX > trackWidth) relX = trackWidth;
        
        float ratio = (float)relX / trackWidth;
        SetValue(m_minValue + ratio * (m_maxValue - m_minValue));
    } else {
        int trackTop = bounds.top + 10;
        int trackBottom = bounds.bottom - 10;
        int trackHeight = trackBottom - trackTop;
        
        if (trackHeight <= 0 || m_maxValue == m_minValue) return;
        
        int relY = trackBottom - y;
        if (relY < 0) relY = 0;
        if (relY > trackHeight) relY = trackHeight;
        
        float ratio = (float)relY / trackHeight;
        SetValue(m_minValue + ratio * (m_maxValue - m_minValue));
    }
}

// RadioButton implementation
RadioButton::RadioButton(const std::wstring& text, int groupId)
    : Widget()
    , m_text(text)
    , m_checked(false)
    , m_groupId(groupId)
    , m_checkColor(100, 149, 237, 255)
    , m_circleColor(200, 200, 200, 255)
{
    m_height = 20;
    m_width = 20;
}

RadioButton::~RadioButton() {
}

void RadioButton::SetChecked(bool checked) {
    if (m_checked != checked) {
        m_checked = checked;
        
        // Uncheck other radio buttons in the same group
        if (checked && m_parent) {
            for (auto& child : m_parent->GetChildren()) {
                RadioButton* radio = dynamic_cast<RadioButton*>(child.get());
                if (radio && radio != this && radio->GetGroupId() == m_groupId) {
                    radio->SetChecked(false);
                }
            }
        }
        
        TriggerEvent(WidgetEvent::VALUE_CHANGED, &m_checked);
    }
}

void RadioButton::Render(HDC hdc) {
    if (!m_visible) return;
    
    int radius = 10;
    RECT circleRect = {m_x, m_y, m_x + 20, m_y + 20};
    
    // Draw outer circle
    HBRUSH outerBrush = CreateSolidBrush(RGB(255, 255, 255));
    HPEN outerPen = CreatePen(PS_SOLID, 2, m_circleColor.ToCOLORREF());
    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, outerBrush);
    HPEN oldPen = (HPEN)SelectObject(hdc, outerPen);
    
    Ellipse(hdc, circleRect.left, circleRect.top, circleRect.right, circleRect.bottom);
    
    // Draw inner circle if checked
    if (m_checked) {
        RECT innerCircleRect = {m_x + 5, m_y + 5, m_x + 15, m_y + 15};
        HBRUSH innerBrush = CreateSolidBrush(m_checkColor.ToCOLORREF());
        SelectObject(hdc, innerBrush);
        SelectObject(hdc, GetStockObject(NULL_PEN));
        
        Ellipse(hdc, innerCircleRect.left, innerCircleRect.top, innerCircleRect.right, innerCircleRect.bottom);
        
        DeleteObject(innerBrush);
    }
    
    SelectObject(hdc, oldBrush);
    SelectObject(hdc, oldPen);
    DeleteObject(outerBrush);
    DeleteObject(outerPen);
    
    // Draw label
    if (!m_text.empty()) {
        RECT textRect = {m_x + 25, m_y, m_x + m_width, m_y + m_height};
        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, RGB(50, 50, 50));
        DrawTextW(hdc, m_text.c_str(), -1, &textRect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    }
    
    Widget::Render(hdc);
}

bool RadioButton::HandleMouseDown(int x, int y, int button) {
    if (!m_visible || !m_enabled) return false;
    
    if (HitTest(x, y) && button == 0) {
        SetChecked(true);
        return true;
    }
    
    return false;
}

// Panel implementation
Panel::Panel()
    : Widget()
    , m_backgroundColor(240, 240, 240, 255)
    , m_borderColor(180, 180, 180, 255)
    , m_titleBarColor(200, 200, 200, 255)
    , m_titleBarHeight(25)
    , m_collapsible(false)
    , m_collapsed(false)
    , m_collapseOrientation(CollapseOrientation::VERTICAL)
    , m_expandedSize(0)
    , m_constrainChildren(false)
{
    m_width = 200;
    m_height = 150;
}

Panel::~Panel() {
}

void Panel::SetCollapsed(bool collapsed) {
    if (m_collapsed == collapsed || !m_collapsible) return;
    
    m_collapsed = collapsed;
    
    if (collapsed) {
        // Store expanded size before collapsing
        if (m_collapseOrientation == CollapseOrientation::VERTICAL) {
            m_expandedSize = m_height;
            m_height = m_titleBarHeight;
        } else {
            m_expandedSize = m_width;
            m_width = 30; // Collapsed width for horizontal
        }
        
        // Hide all children
        for (auto& child : m_children) {
            child->SetVisible(false);
        }
    } else {
        // Restore expanded size
        if (m_collapseOrientation == CollapseOrientation::VERTICAL) {
            m_height = m_expandedSize > 0 ? m_expandedSize : 150;
        } else {
            m_width = m_expandedSize > 0 ? m_expandedSize : 200;
        }
        
        // Show all children
        for (auto& child : m_children) {
            child->SetVisible(true);
        }
    }
}

void Panel::ToggleCollapsed() {
    SetCollapsed(!m_collapsed);
}

void Panel::ClampChildPosition(Widget* child) {
    if (!child || !m_constrainChildren) return;
    
    RECT panelBounds;
    GetBounds(panelBounds);
    
    // Account for title bar if present
    if (!m_title.empty()) {
        panelBounds.top += m_titleBarHeight;
    }
    
    // Add padding
    panelBounds.left += m_paddingLeft;
    panelBounds.top += m_paddingTop;
    panelBounds.right -= m_paddingRight;
    panelBounds.bottom -= m_paddingBottom;
    
    int childX, childY, childWidth, childHeight;
    child->GetPosition(childX, childY);
    child->GetSize(childWidth, childHeight);
    
    // Clamp child position within panel bounds
    if (childX < panelBounds.left) childX = panelBounds.left;
    if (childY < panelBounds.top) childY = panelBounds.top;
    if (childX + childWidth > panelBounds.right) childX = panelBounds.right - childWidth;
    if (childY + childHeight > panelBounds.bottom) childY = panelBounds.bottom - childHeight;
    
    child->SetPosition(childX, childY);
}

void Panel::AddChild(std::shared_ptr<Widget> child) {
    Widget::AddChild(child);
    
    // Apply boundary constraints if enabled
    if (m_constrainChildren) {
        ClampChildPosition(child.get());
    }
}

RECT Panel::GetCollapseButtonRect() const {
    RECT bounds;
    GetBounds(bounds);
    
    int buttonSize = m_titleBarHeight - 6;
    RECT buttonRect;
    buttonRect.right = bounds.right - 5;
    buttonRect.left = buttonRect.right - buttonSize;
    buttonRect.top = bounds.top + 3;
    buttonRect.bottom = buttonRect.top + buttonSize;
    
    return buttonRect;
}

int Panel::GetCollapsedSize() const {
    if (m_collapseOrientation == CollapseOrientation::VERTICAL) {
        return m_titleBarHeight;
    } else {
        return 30;
    }
}

void Panel::RenderCollapseButton(HDC hdc, const RECT& buttonRect) {
    // Draw button background
    Color btnColor = m_hovered ? Color(220, 220, 220, 255) : Color(200, 200, 200, 255);
    Renderer::DrawRoundedRect(hdc, buttonRect, 3, btnColor, Color(150, 150, 150, 255), 1);
    
    // Draw triangle indicator
    int centerX = (buttonRect.left + buttonRect.right) / 2;
    int centerY = (buttonRect.top + buttonRect.bottom) / 2;
    int size = 5;
    
    POINT triangle[3];
    if (m_collapsed) {
        if (m_collapseOrientation == CollapseOrientation::VERTICAL) {
            // Down-pointing triangle (collapsed state for vertical)
            triangle[0] = {centerX, centerY + size};
            triangle[1] = {centerX - size, centerY - size};
            triangle[2] = {centerX + size, centerY - size};
        } else {
            // Right-pointing triangle (collapsed state for horizontal)
            triangle[0] = {centerX + size, centerY};
            triangle[1] = {centerX - size, centerY - size};
            triangle[2] = {centerX - size, centerY + size};
        }
    } else {
        if (m_collapseOrientation == CollapseOrientation::VERTICAL) {
            // Up-pointing triangle (expanded state for vertical)
            triangle[0] = {centerX, centerY - size};
            triangle[1] = {centerX - size, centerY + size};
            triangle[2] = {centerX + size, centerY + size};
        } else {
            // Left-pointing triangle (expanded state for horizontal)
            triangle[0] = {centerX - size, centerY};
            triangle[1] = {centerX + size, centerY - size};
            triangle[2] = {centerX + size, centerY + size};
        }
    }
    
    HBRUSH brush = CreateSolidBrush(RGB(80, 80, 80));
    HPEN pen = CreatePen(PS_SOLID, 1, RGB(80, 80, 80));
    HGDIOBJ oldBrush = SelectObject(hdc, brush);
    HGDIOBJ oldPen = SelectObject(hdc, pen);
    
    Polygon(hdc, triangle, 3);
    
    SelectObject(hdc, oldBrush);
    SelectObject(hdc, oldPen);
    DeleteObject(brush);
    DeleteObject(pen);
}

bool Panel::HandleMouseDown(int x, int y, int button) {
    if (!m_visible || !m_enabled) return false;
    
    // Check if clicking collapse button
    if (m_collapsible && !m_title.empty()) {
        RECT buttonRect = GetCollapseButtonRect();
        if (x >= buttonRect.left && x < buttonRect.right &&
            y >= buttonRect.top && y < buttonRect.bottom) {
            ToggleCollapsed();
            return true;
        }
    }
    
    // Check children
    for (auto& child : m_children) {
        if (child->HandleMouseDown(x, y, button)) {
            return true;
        }
    }
    
    return HitTest(x, y);
}

void Panel::Render(HDC hdc) {
    if (!m_visible) return;
    
    RECT bounds; GetBounds(bounds);
    
    // Draw border and background
    Renderer::DrawRoundedRect(hdc, bounds, 8, m_backgroundColor, m_borderColor, 2);
    
    // Draw title bar if title is set
    if (!m_title.empty()) {
        RECT titleBarRect = {bounds.left, bounds.top, bounds.right, bounds.top + m_titleBarHeight};
        Renderer::DrawRoundedRect(hdc, titleBarRect, 8, m_titleBarColor, m_titleBarColor, 0);
        
        // Draw title text
        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, RGB(50, 50, 50));
        RECT titleTextRect = titleBarRect;
        titleTextRect.left += 10;
        titleTextRect.right -= (m_collapsible ? m_titleBarHeight : 0);
        DrawTextW(hdc, m_title.c_str(), -1, &titleTextRect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
        
        // Draw collapse button if collapsible
        if (m_collapsible) {
            RECT buttonRect = GetCollapseButtonRect();
            RenderCollapseButton(hdc, buttonRect);
        }
    }
    
    Widget::Render(hdc);
}

// SpinBox implementation
SpinBox::SpinBox()
    : Widget()
    , m_value(0)
    , m_minValue(0)
    , m_maxValue(100)
    , m_step(1)
    , m_backgroundColor(255, 255, 255, 255)
    , m_textColor(0, 0, 0, 255)
    , m_buttonColor(220, 220, 220, 255)
{
    m_width = 120;
    m_height = 30;
}

SpinBox::~SpinBox() {
}

void SpinBox::SetValue(int value) {
    if (value < m_minValue) value = m_minValue;
    if (value > m_maxValue) value = m_maxValue;
    
    if (m_value != value) {
        m_value = value;
        TriggerEvent(WidgetEvent::VALUE_CHANGED, &m_value);
    }
}

void SpinBox::SetRange(int minValue, int maxValue) {
    // Validate and swap if necessary
    if (minValue > maxValue) {
        int temp = minValue;
        minValue = maxValue;
        maxValue = temp;
    }
    
    m_minValue = minValue;
    m_maxValue = maxValue;
    if (m_value < m_minValue) m_value = m_minValue;
    if (m_value > m_maxValue) m_value = m_maxValue;
}

void SpinBox::GetRange(int& minValue, int& maxValue) const {
    minValue = m_minValue;
    maxValue = m_maxValue;
}

void SpinBox::Render(HDC hdc) {
    if (!m_visible) return;
    
    RECT bounds; GetBounds(bounds);
    
    // Draw background
    int buttonWidth = 20;
    RECT textRect = {bounds.left, bounds.top, bounds.right - buttonWidth, bounds.bottom};
    Renderer::DrawRoundedRect(hdc, textRect, 4, m_backgroundColor, Color(128, 128, 128, 255), 1);
    
    // Draw value text
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, m_textColor.ToCOLORREF());
    std::wstring valueText = std::to_wstring(m_value);
    RECT valueTextRect = textRect;
    valueTextRect.left += 5;
    DrawTextW(hdc, valueText.c_str(), -1, &valueTextRect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    
    // Draw up button
    RECT upButtonRect = {bounds.right - buttonWidth, bounds.top, bounds.right, bounds.top + m_height / 2};
    Renderer::DrawRoundedRect(hdc, upButtonRect, 4, m_buttonColor, Color(128, 128, 128, 255), 1);
    
    // Draw up arrow
    int arrowCenterX = upButtonRect.left + buttonWidth / 2;
    int arrowCenterY = upButtonRect.top + (upButtonRect.bottom - upButtonRect.top) / 2;
    HPEN arrowPen = CreatePen(PS_SOLID, 2, RGB(50, 50, 50));
    HPEN oldPen = (HPEN)SelectObject(hdc, arrowPen);
    
    MoveToEx(hdc, arrowCenterX - 4, arrowCenterY + 2, nullptr);
    LineTo(hdc, arrowCenterX, arrowCenterY - 2);
    LineTo(hdc, arrowCenterX + 4, arrowCenterY + 2);
    
    // Draw down button
    RECT downButtonRect = {bounds.right - buttonWidth, bounds.top + m_height / 2, bounds.right, bounds.bottom};
    Renderer::DrawRoundedRect(hdc, downButtonRect, 4, m_buttonColor, Color(128, 128, 128, 255), 1);
    
    // Draw down arrow
    arrowCenterY = downButtonRect.top + (downButtonRect.bottom - downButtonRect.top) / 2;
    
    MoveToEx(hdc, arrowCenterX - 4, arrowCenterY - 2, nullptr);
    LineTo(hdc, arrowCenterX, arrowCenterY + 2);
    LineTo(hdc, arrowCenterX + 4, arrowCenterY - 2);
    
    SelectObject(hdc, oldPen);
    DeleteObject(arrowPen);
    
    Widget::Render(hdc);
}

bool SpinBox::HandleMouseDown(int x, int y, int button) {
    if (!m_visible || !m_enabled) return false;
    
    if (HitTest(x, y) && button == 0) {
        RECT bounds; GetBounds(bounds);
        int buttonWidth = 20;
        
        // Check if clicking up button
        if (x >= bounds.right - buttonWidth && y < bounds.top + m_height / 2) {
            SetValue(m_value + m_step);
            return true;
        }
        // Check if clicking down button
        else if (x >= bounds.right - buttonWidth && y >= bounds.top + m_height / 2) {
            SetValue(m_value - m_step);
            return true;
        }
        
        SetFocused(true);
        return true;
    }
    
    return false;
}

bool SpinBox::HandleKeyDown(int keyCode) {
    if (!Widget::HandleKeyDown(keyCode)) return false;
    
    if (keyCode == VK_UP) {
        SetValue(m_value + m_step);
        return true;
    } else if (keyCode == VK_DOWN) {
        SetValue(m_value - m_step);
        return true;
    }
    
    return false;
}

// New property implementations

void Widget::SetPadding(int padding) {
    m_paddingLeft = m_paddingTop = m_paddingRight = m_paddingBottom = padding;
}

void Widget::SetPadding(int left, int top, int right, int bottom) {
    m_paddingLeft = left;
    m_paddingTop = top;
    m_paddingRight = right;
    m_paddingBottom = bottom;
}

void Widget::GetPadding(int& left, int& top, int& right, int& bottom) const {
    left = m_paddingLeft;
    top = m_paddingTop;
    right = m_paddingRight;
    bottom = m_paddingBottom;
}

void Widget::SetMargin(int margin) {
    m_marginLeft = m_marginTop = m_marginRight = m_marginBottom = margin;
}

void Widget::SetMargin(int left, int top, int right, int bottom) {
    m_marginLeft = left;
    m_marginTop = top;
    m_marginRight = right;
    m_marginBottom = bottom;
}

void Widget::GetMargin(int& left, int& top, int& right, int& bottom) const {
    left = m_marginLeft;
    top = m_marginTop;
    right = m_marginRight;
    bottom = m_marginBottom;
}

void Widget::SetMinSize(int minWidth, int minHeight) {
    m_minWidth = minWidth;
    m_minHeight = minHeight;
    
    // Apply constraints to current size
    if (m_width < m_minWidth) m_width = m_minWidth;
    if (m_height < m_minHeight) m_height = m_minHeight;
}

void Widget::GetMinSize(int& minWidth, int& minHeight) const {
    minWidth = m_minWidth;
    minHeight = m_minHeight;
}

void Widget::SetMaxSize(int maxWidth, int maxHeight) {
    m_maxWidth = maxWidth;
    m_maxHeight = maxHeight;
    
    // Apply constraints to current size
    if (m_width > m_maxWidth) m_width = m_maxWidth;
    if (m_height > m_maxHeight) m_height = m_maxHeight;
}

void Widget::GetMaxSize(int& maxWidth, int& maxHeight) const {
    maxWidth = m_maxWidth;
    maxHeight = m_maxHeight;
}

void Widget::SetOpacity(float opacity) {
    // Clamp opacity to valid range [0.0, 1.0]
    m_opacity = std::min(std::max(opacity, 0.0f), 1.0f);
}

void Widget::SetBorderWidth(int width) {
    if (width < 0) width = 0;
    m_borderWidth = width;
}

void Widget::SetBorderRadius(int radius) {
    if (radius < 0) radius = 0;
    m_borderRadius = radius;
}

void Widget::SetFontSize(int size) {
    if (size < 1) size = 1;
    m_fontSize = size;
}

} // namespace SDK
