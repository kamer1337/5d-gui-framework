#pragma once


#include "Platform.h"
#include <string>
#include <memory>
#include <functional>
#include <vector>
#include "Theme.h"

namespace SDK {

// Forward declarations
class Window;

// Widget event types
enum class WidgetEvent {
    CLICK,
    DOUBLE_CLICK,
    MOUSE_ENTER,
    MOUSE_LEAVE,
    MOUSE_MOVE,
    KEY_PRESS,
    KEY_RELEASE,
    TEXT_CHANGED,
    FOCUS_GAINED,
    FOCUS_LOST,
    VALUE_CHANGED
};

// Widget alignment types
enum class WidgetAlignment {
    NONE,
    LEFT,
    RIGHT,
    TOP,
    BOTTOM,
    CENTER,
    TOP_LEFT,
    TOP_RIGHT,
    BOTTOM_LEFT,
    BOTTOM_RIGHT
};

/**
 * Widget - Base class for UI components
 * Provides common functionality for all widgets like position, size, visibility, etc.
 */
class Widget {
public:
    Widget();
    virtual ~Widget();
    
    // Basic properties
    void SetPosition(int x, int y);
    void GetPosition(int& x, int& y) const;
    
    void SetSize(int width, int height);
    void GetSize(int& width, int& height) const;
    
    void SetBounds(int x, int y, int width, int height);
    void GetBounds(RECT& rect) const;
    
    // Visibility and enabled state
    void SetVisible(bool visible);
    bool IsVisible() const { return m_visible; }
    
    void SetEnabled(bool enabled);
    bool IsEnabled() const { return m_enabled; }
    
    // Hover state
    void SetHovered(bool hovered);
    bool IsHovered() const { return m_hovered; }
    
    // Hit testing
    virtual bool HitTest(int x, int y) const;
    
    // Rendering
    virtual void Render(HDC hdc) = 0;
    
    // Update (for animations)
    virtual void Update(float deltaTime);
    
    // Mouse events
    virtual void OnMouseEnter();
    virtual void OnMouseLeave();
    virtual void OnMouseMove(int x, int y);
    virtual void OnMouseDown(int x, int y);
    virtual void OnMouseUp(int x, int y);
    virtual void OnClick();
    
    // ID for identification
    void SetId(int id) { m_id = id; }
    int GetId() const { return m_id; }
    
    // Tag for custom data
    void SetTag(void* tag) { m_tag = tag; }
    void* GetTag() const { return m_tag; }
    
    // Name for identification
    void SetName(const std::wstring& name) { m_name = name; }
    std::wstring GetName() const { return m_name; }
    
    // Padding properties
    void SetPadding(int padding);
    void SetPadding(int left, int top, int right, int bottom);
    void GetPadding(int& left, int& top, int& right, int& bottom) const;
    
    // Margin properties
    void SetMargin(int margin);
    void SetMargin(int left, int top, int right, int bottom);
    void GetMargin(int& left, int& top, int& right, int& bottom) const;
    
    // Size constraints
    void SetMinSize(int minWidth, int minHeight);
    void GetMinSize(int& minWidth, int& minHeight) const;
    void SetMaxSize(int maxWidth, int maxHeight);
    void GetMaxSize(int& maxWidth, int& maxHeight) const;
    
    // Opacity
    void SetOpacity(float opacity);
    float GetOpacity() const { return m_opacity; }
    
    // Border properties
    void SetBorderWidth(int width);
    int GetBorderWidth() const { return m_borderWidth; }
    void SetBorderRadius(int radius);
    int GetBorderRadius() const { return m_borderRadius; }
    
    // Tooltip
    void SetTooltipText(const std::wstring& text) { m_tooltipText = text; }
    std::wstring GetTooltipText() const { return m_tooltipText; }
    
    // Cursor
    void SetCursor(HCURSOR cursor) { m_cursor = cursor; }
    HCURSOR GetCursor() const { return m_cursor; }
    
    // Z-index for layering
    void SetZIndex(int zIndex) { m_zIndex = zIndex; }
    int GetZIndex() const { return m_zIndex; }
    
    // Font properties
    void SetFontFamily(const std::wstring& family) { m_fontFamily = family; }
    std::wstring GetFontFamily() const { return m_fontFamily; }
    void SetFontSize(int size);
    int GetFontSize() const { return m_fontSize; }
    void SetFontBold(bool bold) { m_fontBold = bold; }
    bool IsFontBold() const { return m_fontBold; }
    void SetFontItalic(bool italic) { m_fontItalic = italic; }
    bool IsFontItalic() const { return m_fontItalic; }
    
    // Focus management
    void SetFocused(bool focused);
    bool IsFocused() const { return m_focused; }
    
    // Hierarchy
    void SetParent(Widget* parent);
    Widget* GetParent() const { return m_parent; }
    
    void AddChild(std::shared_ptr<Widget> child);
    void RemoveChild(std::shared_ptr<Widget> child);
    const std::vector<std::shared_ptr<Widget>>& GetChildren() const { return m_children; }
    
    // Event handling
    using EventCallback = std::function<void(Widget*, WidgetEvent, void* data)>;
    void SetEventCallback(EventCallback callback) { m_eventCallback = callback; }
    
    // Input handling
    virtual bool HandleMouseMove(int x, int y);
    virtual bool HandleMouseDown(int x, int y, int button);
    virtual bool HandleMouseUp(int x, int y, int button);
    virtual bool HandleKeyDown(int keyCode);
    virtual bool HandleKeyUp(int keyCode);
    virtual bool HandleChar(wchar_t ch);
    
    // Theme support
    void SetTheme(std::shared_ptr<Theme> theme) { m_theme = theme; }
    std::shared_ptr<Theme> GetTheme() const { return m_theme; }
    
    // Alignment properties
    void SetAlignment(WidgetAlignment alignment) { m_alignment = alignment; }
    WidgetAlignment GetAlignment() const { return m_alignment; }
    void AlignToWidget(Widget* target, WidgetAlignment alignment, int spacing = 0);
    void AlignToParent(WidgetAlignment alignment, int margin = 0);
    
    // Overlap detection
    bool CheckOverlap(const Widget* other) const;
    bool CheckOverlap(int x, int y, int width, int height) const;
    void ResolveOverlap(Widget* other, int spacing = 5);
    
protected:
    void TriggerEvent(WidgetEvent event, void* data = nullptr);
    
    int m_x, m_y;
    int m_width, m_height;
    bool m_visible;
    bool m_enabled;
    bool m_focused;
    bool m_hovered;
    int m_id;
    void* m_tag;
    
    Widget* m_parent;
    std::vector<std::shared_ptr<Widget>> m_children;
    
    EventCallback m_eventCallback;
    std::shared_ptr<Theme> m_theme;
    
    // New properties
    std::wstring m_name;
    int m_paddingLeft, m_paddingTop, m_paddingRight, m_paddingBottom;
    int m_marginLeft, m_marginTop, m_marginRight, m_marginBottom;
    int m_minWidth, m_minHeight;
    int m_maxWidth, m_maxHeight;
    float m_opacity;
    int m_borderWidth;
    int m_borderRadius;
    std::wstring m_tooltipText;
    HCURSOR m_cursor;
    int m_zIndex;
    std::wstring m_fontFamily;
    int m_fontSize;
    bool m_fontBold;
    bool m_fontItalic;
    WidgetAlignment m_alignment;
};

// Button widget
class Button : public Widget {
public:
    Button(const std::wstring& text = L"");
    virtual ~Button();
    
    void SetText(const std::wstring& text) { m_text = text; }
    std::wstring GetText() const { return m_text; }
    
    void SetBackgroundColor(const Color& color) { m_backgroundColor = color; }
    void SetTextColor(const Color& color) { m_textColor = color; }
    void SetHoverColor(const Color& color) { m_hoverColor = color; }
    void SetPressColor(const Color& color) { m_pressColor = color; }
    
    void Render(HDC hdc) override;
    bool HandleMouseMove(int x, int y) override;
    bool HandleMouseDown(int x, int y, int button) override;
    bool HandleMouseUp(int x, int y, int button) override;
    
private:
    std::wstring m_text;
    Color m_backgroundColor;
    Color m_textColor;
    Color m_hoverColor;
    Color m_pressColor;
    bool m_pressed;
};

// Label widget
class Label : public Widget {
public:
    Label(const std::wstring& text = L"");
    virtual ~Label();
    
    void SetText(const std::wstring& text) { m_text = text; }
    std::wstring GetText() const { return m_text; }
    
    void SetTextColor(const Color& color) { m_textColor = color; }
    void SetTextAlignment(UINT alignment) { m_textAlignment = alignment; }
    
    void Render(HDC hdc) override;
    
private:
    std::wstring m_text;
    Color m_textColor;
    UINT m_textAlignment;
};

// TextBox widget
class TextBox : public Widget {
public:
    TextBox();
    virtual ~TextBox();
    
    void SetText(const std::wstring& text);
    std::wstring GetText() const { return m_text; }
    
    void SetPlaceholder(const std::wstring& placeholder) { m_placeholder = placeholder; }
    void SetMaxLength(int maxLength) { m_maxLength = maxLength; }
    
    void SetBackgroundColor(const Color& color) { m_backgroundColor = color; }
    void SetTextColor(const Color& color) { m_textColor = color; }
    void SetBorderColor(const Color& color) { m_borderColor = color; }
    
    void Render(HDC hdc) override;
    bool HandleMouseDown(int x, int y, int button) override;
    bool HandleKeyDown(int keyCode) override;
    bool HandleChar(wchar_t ch) override;
    
private:
    void UpdateCursorPosition();
    
    std::wstring m_text;
    std::wstring m_placeholder;
    int m_maxLength;
    int m_cursorPosition;
    bool m_showCursor;
    float m_cursorBlinkTime;
    
    Color m_backgroundColor;
    Color m_textColor;
    Color m_borderColor;
};

// CheckBox widget
class CheckBox : public Widget {
public:
    CheckBox(const std::wstring& text = L"");
    virtual ~CheckBox();
    
    void SetText(const std::wstring& text) { m_text = text; }
    std::wstring GetText() const { return m_text; }
    
    void SetChecked(bool checked);
    bool IsChecked() const { return m_checked; }
    
    void Render(HDC hdc) override;
    bool HandleMouseDown(int x, int y, int button) override;
    
private:
    std::wstring m_text;
    bool m_checked;
    Color m_checkColor;
    Color m_boxColor;
};

// Separator widget
class Separator : public Widget {
public:
    enum class Orientation {
        HORIZONTAL,
        VERTICAL
    };
    
    Separator(Orientation orientation = Orientation::HORIZONTAL);
    virtual ~Separator();
    
    void SetOrientation(Orientation orientation) { m_orientation = orientation; }
    void SetColor(const Color& color) { m_color = color; }
    
    void Render(HDC hdc) override;
    
private:
    Orientation m_orientation;
    Color m_color;
};

// Image widget
class Image : public Widget {
public:
    Image();
    virtual ~Image();
    
    bool LoadFromFile(const std::wstring& filename);
    bool LoadFromResource(HINSTANCE hInstance, int resourceId);
    void SetHBITMAP(HBITMAP bitmap);
    
    void SetStretchMode(bool stretch) { m_stretch = stretch; }
    
    void Render(HDC hdc) override;
    
private:
    HBITMAP m_bitmap;
    bool m_stretch;
    int m_imageWidth;
    int m_imageHeight;
};

// Slider widget
class Slider : public Widget {
public:
    enum class Orientation {
        HORIZONTAL,
        VERTICAL
    };
    
    Slider(Orientation orientation = Orientation::HORIZONTAL);
    virtual ~Slider();
    
    void SetValue(float value);
    float GetValue() const { return m_value; }
    
    void SetRange(float minValue, float maxValue);
    void GetRange(float& minValue, float& maxValue) const;
    
    void SetOrientation(Orientation orientation) { m_orientation = orientation; }
    Orientation GetOrientation() const { return m_orientation; }
    
    void SetTrackColor(const Color& color) { m_trackColor = color; }
    void SetThumbColor(const Color& color) { m_thumbColor = color; }
    void SetFillColor(const Color& color) { m_fillColor = color; }
    
    void Render(HDC hdc) override;
    bool HandleMouseDown(int x, int y, int button) override;
    bool HandleMouseMove(int x, int y) override;
    bool HandleMouseUp(int x, int y, int button) override;
    
private:
    void UpdateValueFromPosition(int x, int y);
    
    Orientation m_orientation;
    float m_value;
    float m_minValue;
    float m_maxValue;
    bool m_dragging;
    
    Color m_trackColor;
    Color m_thumbColor;
    Color m_fillColor;
};

// RadioButton widget
class RadioButton : public Widget {
public:
    RadioButton(const std::wstring& text = L"", int groupId = 0);
    virtual ~RadioButton();
    
    void SetText(const std::wstring& text) { m_text = text; }
    std::wstring GetText() const { return m_text; }
    
    void SetChecked(bool checked);
    bool IsChecked() const { return m_checked; }
    
    void SetGroupId(int groupId) { m_groupId = groupId; }
    int GetGroupId() const { return m_groupId; }
    
    void Render(HDC hdc) override;
    bool HandleMouseDown(int x, int y, int button) override;
    
private:
    std::wstring m_text;
    bool m_checked;
    int m_groupId;
    Color m_checkColor;
    Color m_circleColor;
};

// Panel widget (container for grouping widgets)
class Panel : public Widget {
public:
    enum class CollapseOrientation {
        VERTICAL,
        HORIZONTAL
    };
    
    Panel();
    virtual ~Panel();
    
    void SetTitle(const std::wstring& title) { m_title = title; }
    std::wstring GetTitle() const { return m_title; }
    
    void SetBackgroundColor(const Color& color) { m_backgroundColor = color; }
    void SetBorderColor(const Color& color) { m_borderColor = color; }
    void SetTitleBarColor(const Color& color) { m_titleBarColor = color; }
    
    // Collapse/expand functionality
    void SetCollapsible(bool collapsible) { m_collapsible = collapsible; }
    bool IsCollapsible() const { return m_collapsible; }
    void SetCollapsed(bool collapsed);
    bool IsCollapsed() const { return m_collapsed; }
    void ToggleCollapsed();
    void SetCollapseOrientation(CollapseOrientation orientation) { m_collapseOrientation = orientation; }
    CollapseOrientation GetCollapseOrientation() const { return m_collapseOrientation; }
    
    // Boundary constraints
    void SetConstrainChildren(bool constrain) { m_constrainChildren = constrain; }
    bool IsConstrainChildren() const { return m_constrainChildren; }
    void ClampChildPosition(Widget* child);
    
    void Render(HDC hdc) override;
    bool HandleMouseDown(int x, int y, int button) override;
    
    // Override to enforce boundaries on child widgets
    void AddChild(std::shared_ptr<Widget> child);
    
private:
    void RenderCollapseButton(HDC hdc, const RECT& buttonRect);
    RECT GetCollapseButtonRect() const;
    int GetCollapsedSize() const;
    
    std::wstring m_title;
    Color m_backgroundColor;
    Color m_borderColor;
    Color m_titleBarColor;
    int m_titleBarHeight;
    
    // New collapse/expand properties
    bool m_collapsible;
    bool m_collapsed;
    CollapseOrientation m_collapseOrientation;
    int m_expandedSize;  // Store original size when collapsed
    std::vector<bool> m_childrenVisibilityState;  // Store original visibility of children
    
    // Boundary constraint properties
    bool m_constrainChildren;
};

// SpinBox widget (numeric input with up/down buttons)
class SpinBox : public Widget {
public:
    SpinBox();
    virtual ~SpinBox();
    
    void SetValue(int value);
    int GetValue() const { return m_value; }
    
    void SetRange(int minValue, int maxValue);
    void GetRange(int& minValue, int& maxValue) const;
    
    void SetStep(int step) { m_step = step; }
    int GetStep() const { return m_step; }
    
    void Render(HDC hdc) override;
    bool HandleMouseDown(int x, int y, int button) override;
    bool HandleKeyDown(int keyCode) override;
    
private:
    int m_value;
    int m_minValue;
    int m_maxValue;
    int m_step;
    Color m_backgroundColor;
    Color m_textColor;
    Color m_buttonColor;
};

} // namespace SDK
