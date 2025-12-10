#include "../../include/SDK/Widget.h"
#include "../../include/SDK/Renderer.h"
#include <algorithm>

namespace SDK {

// Widget base class implementation
Widget::Widget()
    : m_x(0), m_y(0), m_width(100), m_height(30)
    , m_visible(true), m_enabled(true), m_focused(false), m_hovered(false)
    , m_parent(nullptr)
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
    m_width = width;
    m_height = height;
}

void Widget::GetSize(int& width, int& height) const {
    width = m_width;
    height = m_height;
}

RECT Widget::GetBounds() const {
    RECT bounds;
    bounds.left = m_x;
    bounds.top = m_y;
    bounds.right = m_x + m_width;
    bounds.bottom = m_y + m_height;
    return bounds;
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

void Widget::Update(float deltaTime) {
    if (!m_visible) return;
    
    // Update children
    for (auto& child : m_children) {
        child->Update(deltaTime);
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

bool Widget::HitTest(int x, int y) const {
    RECT bounds = GetBounds();
    return x >= bounds.left && x < bounds.right && y >= bounds.top && y < bounds.bottom;
}

void Widget::TriggerEvent(WidgetEvent event, void* data) {
    if (m_eventCallback) {
        m_eventCallback(this, event, data);
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
    
    RECT bounds = GetBounds();
    
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
    
    RECT bounds = GetBounds();
    
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
    
    RECT bounds = GetBounds();
    
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
    
    RECT bounds = GetBounds();
    
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
    
    RECT bounds = GetBounds();
    
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

} // namespace SDK
