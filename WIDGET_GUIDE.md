# Widget System Documentation

The 5D GUI SDK now includes a comprehensive widget system for building rich user interfaces. This document describes the available widgets and how to use them.

## Overview

The widget system provides reusable UI components that can be easily added to windows. All widgets inherit from the base `Widget` class and share common functionality.

## Base Widget Class

All widgets inherit from `SDK::Widget` which provides:

### Basic Properties
- **Position and Size**: `SetPosition()`, `SetSize()`, `SetBounds()`, `GetBounds()`
- **Visibility**: `SetVisible()`, `IsVisible()`
- **Enabled State**: `SetEnabled()`, `IsEnabled()`
- **Hover State**: `SetHovered()`, `IsHovered()`
- **Focus State**: `SetFocused()`, `IsFocused()`

### Layout Properties
- **Padding**: `SetPadding(int)`, `SetPadding(left, top, right, bottom)`, `GetPadding()`
- **Margin**: `SetMargin(int)`, `SetMargin(left, top, right, bottom)`, `GetMargin()`
- **Size Constraints**: `SetMinSize()`, `GetMinSize()`, `SetMaxSize()`, `GetMaxSize()`
- **Z-Index**: `SetZIndex()`, `GetZIndex()` - Widget stacking order

### Visual Properties
- **Opacity**: `SetOpacity()`, `GetOpacity()` - Alpha transparency (0.0-1.0)
- **Border**: `SetBorderWidth()`, `GetBorderWidth()`, `SetBorderRadius()`, `GetBorderRadius()`
- **Theme**: `SetTheme()`, `GetTheme()` - Apply custom themes

### Font Properties
- **Font Family**: `SetFontFamily()`, `GetFontFamily()` - Font name (default: "Segoe UI")
- **Font Size**: `SetFontSize()`, `GetFontSize()` - Font size in points (default: 12)
- **Font Style**: `SetFontBold()`, `IsFontBold()`, `SetFontItalic()`, `IsFontItalic()`

### Interaction Properties
- **Tooltip**: `SetTooltipText()`, `GetTooltipText()` - Hover tooltip text
- **Cursor**: `SetCursor()`, `GetCursor()` - Custom mouse cursor

### Identification
- **ID**: `SetId()`, `GetId()` - Numeric identifier
- **Name**: `SetName()`, `GetName()` - String name for identification
- **Tag**: `SetTag()`, `GetTag()` - Custom data pointer

### Event Handling
- **Hit Testing**: `HitTest(x, y)` - Check if a point is inside the widget
- **Mouse Events**: `OnMouseEnter()`, `OnMouseLeave()`, `OnMouseMove()`, `OnMouseDown()`, `OnMouseUp()`, `OnClick()`
- **Keyboard Events**: `HandleKeyDown()`, `HandleKeyUp()`, `HandleChar()`
- **Event Callbacks**: `SetEventCallback()` - Custom event handlers

### Hierarchy
- **Parent/Child**: `SetParent()`, `GetParent()`, `AddChild()`, `RemoveChild()`, `GetChildren()`

### Alignment and Layout (New!)
- **Auto-Alignment**: `SetAlignment()`, `GetAlignment()` - Set widget alignment type
- **Align to Widget**: `AlignToWidget(target, alignment, spacing)` - Align relative to another widget
- **Align to Parent**: `AlignToParent(alignment, margin)` - Align within parent bounds
- **Alignment Types**: `NONE`, `LEFT`, `RIGHT`, `TOP`, `BOTTOM`, `CENTER`, `TOP_LEFT`, `TOP_RIGHT`, `BOTTOM_LEFT`, `BOTTOM_RIGHT`

### Overlap Detection (New!)
- **Check Overlap**: `CheckOverlap(widget)` or `CheckOverlap(x, y, width, height)` - Detect if widgets overlap
- **Resolve Overlap**: `ResolveOverlap(widget, spacing)` - Automatically move to resolve overlap

#### Alignment Examples

```cpp
// Align button to the right of another button
auto btn1 = std::make_shared<SDK::Button>(L"First");
btn1->SetPosition(20, 20);
btn1->SetSize(100, 30);

auto btn2 = std::make_shared<SDK::Button>(L"Second");
btn2->SetSize(100, 30);
btn2->AlignToWidget(btn1.get(), SDK::WidgetAlignment::RIGHT, 10); // 10px spacing

// Center a widget within its parent
auto label = std::make_shared<SDK::Label>(L"Centered");
label->SetParent(panel.get());
label->AlignToParent(SDK::WidgetAlignment::CENTER, 0);

// Check for overlaps and resolve
if (btn1->CheckOverlap(btn2.get())) {
    btn2->ResolveOverlap(btn1.get(), 5); // Move btn2 with 5px spacing
}
```

### Rendering
- **Render**: `Render(HDC)` - Draw the widget (pure virtual)
- **Update**: `Update(deltaTime)` - Update widget state for animations

## Available Widgets

### ProgressBar

A visual widget for displaying progress with gradient fills and smooth animations.

#### Features
- Horizontal and vertical orientations
- Gradient fill with customizable colors
- Smooth animated transitions
- Text display (percentage or custom)
- Rounded corners
- Border customization

#### Basic Usage

```cpp
#include "SDK/SDK.h"

// Create a progress bar
auto progressBar = std::make_shared<SDK::ProgressBar>();
progressBar->SetBounds(20, 20, 300, 30);
progressBar->SetValue(50.0f);  // 50%

// Customize colors
progressBar->SetGradientColors(
    SDK::Color(100, 149, 237, 255),  // Start color
    SDK::Color(65, 105, 225, 255)    // End color
);

// Enable smooth animation
progressBar->SetAnimated(true);
progressBar->SetAnimationSpeed(5.0f);

// Render in your WM_PAINT handler
progressBar->Render(hdc);

// Update in your main loop
progressBar->Update(deltaTime);
```

#### API Reference

**Value Management:**
```cpp
void SetValue(float value);          // Set progress value (0 to max)
float GetValue() const;
void SetMaxValue(float maxValue);    // Default: 100.0
float GetMaxValue() const;
void SetPercentage(float percentage); // Set as percentage (0-100)
float GetPercentage() const;
```

**Orientation:**
```cpp
enum class Orientation { HORIZONTAL, VERTICAL };
void SetOrientation(Orientation orientation);
Orientation GetOrientation() const;
```

**Colors:**
```cpp
void SetBackgroundColor(Color color);
void SetForegroundColor(Color color);
void SetBorderColor(Color color);
```

**Gradient:**
```cpp
void SetGradient(bool enabled);
bool HasGradient() const;
void SetGradientColors(Color startColor, Color endColor);
```

**Text Display:**
```cpp
void SetShowText(bool show);         // Show/hide percentage text
bool ShowsText() const;
void SetText(const std::wstring& text); // Custom text
std::wstring GetText() const;
```

**Animation:**
```cpp
void SetAnimated(bool animated);     // Enable smooth transitions
bool IsAnimated() const;
void SetAnimationSpeed(float speed); // Speed factor (default: 5.0)
```

**Appearance:**
```cpp
void SetCornerRadius(int radius);    // Rounded corners
int GetCornerRadius() const;
```

### Tooltip

A popup text widget that appears on hover with fade animations.

#### Features
- Auto-positioning near cursor or widgets
- Fade in/out animations with configurable speed
- Multi-line text support
- Rounded corners and shadows
- Delay before showing
- Multiple positioning modes

#### Basic Usage

```cpp
#include "SDK/SDK.h"

// Create a tooltip
auto tooltip = std::make_shared<SDK::Tooltip>();
tooltip->SetText(L"This is a tooltip!");

// Show at cursor position
tooltip->ShowAtCursor();

// Show near a widget
tooltip->ShowNearWidget(someWidget.get());

// Show at specific position
tooltip->Show(100, 100);

// Hide tooltip
tooltip->Hide();

// Render and update
tooltip->Render(hdc);
tooltip->Update(deltaTime);
```

#### API Reference

**Text Content:**
```cpp
void SetText(const std::wstring& text);
std::wstring GetText() const;

// Multi-line text
void SetLines(const std::vector<std::wstring>& lines);
const std::vector<std::wstring>& GetLines() const;
```

**Positioning:**
```cpp
enum class Position {
    AUTO,    // Auto position near cursor
    ABOVE,   // Above target
    BELOW,   // Below target
    LEFT,    // Left of target
    RIGHT,   // Right of target
    CURSOR   // Follow cursor
};

void SetPositionMode(Position mode);
Position GetPositionMode() const;

void Show(int x, int y);
void ShowNearWidget(const Widget* widget);
void ShowAtCursor();
void Hide();
```

**Animation:**
```cpp
void SetFadeEnabled(bool enabled);
bool IsFadeEnabled() const;
void SetFadeSpeed(float speed);      // Default: 5.0
float GetFadeSpeed() const;
void SetShowDelay(float seconds);    // Delay before showing
float GetShowDelay() const;
```

**Appearance:**
```cpp
void SetBackgroundColor(Color color);
Color GetBackgroundColor() const;
void SetTextColor(Color color);
Color GetTextColor() const;
void SetBorderColor(Color color);
Color GetBorderColor() const;

void SetShadowEnabled(bool enabled);
bool IsShadowEnabled() const;

void SetCornerRadius(int radius);
int GetCornerRadius() const;

void SetPadding(int padding);
int GetPadding() const;
```

**State:**
```cpp
bool IsShowing() const;
float GetOpacity() const;  // Current fade opacity (0.0 - 1.0)

void AutoSize(HDC hdc);    // Auto-size to fit text
```

### Toolbar

A customizable toolbar widget supporting horizontal/vertical orientation and auto-hide functionality.

#### Features
- Horizontal and vertical orientations
- Auto-hide capability with trigger zones
- Customizable toolbar items (buttons)
- Icon and text support for items
- Separators between items
- Hover and pressed states
- Click callbacks
- Smooth show/hide animations

#### Basic Usage

```cpp
#include "SDK/SDK.h"

// Create a horizontal toolbar
auto toolbar = std::make_shared<SDK::Toolbar>();
toolbar->SetBounds(0, 0, 800, 50);
toolbar->SetOrientation(SDK::Toolbar::Orientation::HORIZONTAL);

// Add items
toolbar->AddItem(1, L"File", L"File operations");
toolbar->AddItem(2, L"Edit", L"Edit operations");
toolbar->AddSeparator();
toolbar->AddItem(3, L"View", L"View options");

// Set click callback
toolbar->SetItemClickCallback([](int itemId) {
    // Handle item click
    switch (itemId) {
        case 1:
            // Handle File click
            break;
        case 2:
            // Handle Edit click
            break;
    }
});

// Render in your WM_PAINT handler
toolbar->Render(hdc);

// Update in your main loop for auto-hide animation
toolbar->Update(deltaTime);
```

#### API Reference

**Item Management:**
```cpp
void AddItem(int id, const std::wstring& text, const std::wstring& tooltip = L"");
void AddSeparator();
void RemoveItem(int id);
void ClearItems();

void SetItemEnabled(int id, bool enabled);
void SetItemIcon(int id, HBITMAP icon);
```

**Orientation:**
```cpp
enum class Orientation {
    HORIZONTAL,
    VERTICAL
};

void SetOrientation(Orientation orientation);
Orientation GetOrientation() const;
```

**Auto-Hide Functionality:**
```cpp
void SetAutoHide(bool autoHide);         // Enable/disable auto-hide
bool IsAutoHide() const;

void SetAutoHideDelay(float seconds);    // Delay before hiding (default: 0.5s)
float GetAutoHideDelay() const;

void SetTriggerZoneSize(int pixels);     // Size of trigger zone (default: 3px)
int GetTriggerZoneSize() const;

bool IsVisible() const;                  // Check if currently visible
```

**Appearance:**
```cpp
void SetItemSize(int width, int height);
void GetItemSize(int& width, int& height) const;

void SetBackgroundColor(const Color& color);
void SetItemColor(const Color& color);
void SetItemHoverColor(const Color& color);
void SetItemPressedColor(const Color& color);
void SetTextColor(const Color& color);
void SetSeparatorColor(const Color& color);

void SetPadding(int padding);
int GetPadding() const;

void SetItemSpacing(int spacing);
int GetItemSpacing() const;
```

**Callbacks:**
```cpp
using ItemClickCallback = std::function<void(int itemId)>;
void SetItemClickCallback(ItemClickCallback callback);
```

#### Examples

**Horizontal Toolbar at Top:**
```cpp
auto topToolbar = std::make_shared<SDK::Toolbar>();
topToolbar->SetBounds(0, 0, 800, 50);
topToolbar->SetOrientation(SDK::Toolbar::Orientation::HORIZONTAL);
topToolbar->AddItem(1, L"New");
topToolbar->AddItem(2, L"Open");
topToolbar->AddItem(3, L"Save");
```

**Vertical Toolbar on Side:**
```cpp
auto sideToolbar = std::make_shared<SDK::Toolbar>();
sideToolbar->SetBounds(0, 0, 80, 600);
sideToolbar->SetOrientation(SDK::Toolbar::Orientation::VERTICAL);
sideToolbar->SetItemSize(70, 60);
sideToolbar->AddItem(10, L"Home");
sideToolbar->AddItem(11, L"Search");
sideToolbar->AddItem(12, L"Settings");
```

**Auto-Hide Toolbar:**
```cpp
auto autoHideToolbar = std::make_shared<SDK::Toolbar>();
autoHideToolbar->SetBounds(750, 0, 50, 600);
autoHideToolbar->SetOrientation(SDK::Toolbar::Orientation::VERTICAL);
autoHideToolbar->SetAutoHide(true);
autoHideToolbar->SetAutoHideDelay(0.5f);
autoHideToolbar->SetTriggerZoneSize(5);
autoHideToolbar->AddItem(20, L"Tools");
autoHideToolbar->AddItem(21, L"Help");

// Toolbar will automatically hide after 0.5 seconds
// and show when mouse enters 5px trigger zone at edge
```

## Widget Manager

The `WidgetManager` class helps manage multiple widgets within a window.

### Basic Usage

```cpp
#include "SDK/SDK.h"

// Create widget manager
auto widgetManager = std::make_shared<SDK::WidgetManager>();

// Add widgets
widgetManager->AddWidget(progressBar);
widgetManager->AddWidget(tooltip);

// In WM_PAINT handler
widgetManager->RenderAll(hdc);

// In main loop
widgetManager->UpdateAll(deltaTime);

// In WM_MOUSEMOVE handler
widgetManager->HandleMouseMove(x, y);

// In WM_LBUTTONDOWN handler
widgetManager->HandleMouseDown(x, y);

// In WM_LBUTTONUP handler
widgetManager->HandleMouseUp(x, y);
```

### API Reference

**Widget Management:**
```cpp
void AddWidget(std::shared_ptr<Widget> widget);
void RemoveWidget(std::shared_ptr<Widget> widget);
void RemoveWidgetById(int id);
void Clear();
```

**Finding Widgets:**
```cpp
std::shared_ptr<Widget> GetWidgetById(int id) const;
std::shared_ptr<Widget> GetWidgetAt(int x, int y) const;
const std::vector<std::shared_ptr<Widget>>& GetWidgets() const;
```

**Batch Operations:**
```cpp
void RenderAll(HDC hdc);
void UpdateAll(float deltaTime);
void SetAllEnabled(bool enabled);
```

**Event Handling:**
```cpp
void HandleMouseMove(int x, int y);
void HandleMouseDown(int x, int y);
void HandleMouseUp(int x, int y);
```

## Prompt Window Builder

The `PromptWindowBuilder` allows you to create windows with widgets using simple text prompts. This provides a foundation for future neural network integration.

### Basic Usage

```cpp
#include "SDK/SDK.h"

SDK::PromptWindowBuilder builder;

// Build window from prompt
std::wstring prompt = L"window 800x600 'My App' with progressbar and tooltip";
HWND hwnd = builder.BuildFromPrompt(prompt, hInstance);

// Get the created widget manager
auto widgetMgr = builder.GetLastWidgetManager();

// Show the window
ShowWindow(hwnd, SW_SHOW);
```

### Prompt Syntax

The prompt parser supports a simple syntax for describing windows:

**Basic Window:**
```
window [dimensions] 'title' with widget1 and widget2
```

**Examples:**
```
"window 800x600 'My App' with progressbar"
"window 600x400 'Settings' with progressbar and tooltip"
"dialog with 3 progressbars"
"create window with progressbar and tooltip and subwindow 'Child'"
```

**Supported Patterns:**
- `window [width]x[height]` - Window with dimensions
- `'title'` - Window title in quotes
- `with widget` - Add a widget
- `and widget` - Add another widget
- `3 progressbars` - Add multiple widgets (number + widget type)
- `subwindow 'title'` - Add child window
- `dialog` - Create dialog-style window

**Supported Widget Types:**
- `progressbar` (or `progressbars`)
- `tooltip` (or `tooltips`)
- Custom widgets via `RegisterWidgetFactory()`

### API Reference

```cpp
class PromptWindowBuilder {
public:
    // Parse prompt into window specification
    WindowSpec ParsePrompt(const std::wstring& prompt);
    
    // Create window from specification
    HWND CreateWindowFromSpec(const WindowSpec& spec, 
                              HINSTANCE hInstance, 
                              HWND parent = nullptr);
    
    // Create widgets from specification
    std::vector<std::shared_ptr<Widget>> CreateWidgetsFromSpec(const WindowSpec& spec);
    
    // Build everything in one call
    HWND BuildFromPrompt(const std::wstring& prompt,
                         HINSTANCE hInstance,
                         HWND parent = nullptr);
    
    // Universal window creation function (NEW)
    // Creates window, registers with SDK, applies theme, and configures settings
    static HWND CreateWidgetsWindow(const WindowConfig& config,
                                    HINSTANCE hInstance);
    
    // Get last created widget manager
    std::shared_ptr<WidgetManager> GetLastWidgetManager() const;
    
    // Register custom widget factory
    using WidgetFactory = std::function<std::shared_ptr<Widget>(const std::wstring&)>;
    void RegisterWidgetFactory(const std::wstring& widgetType, WidgetFactory factory);
};
```

### Universal Window Creation (Recommended)

The `CreateWidgetsWindow` static function provides a unified way to create windows with automatic SDK integration:

```cpp
#include "SDK/SDK.h"

// Configure window with all options
SDK::PromptWindowBuilder::WindowConfig config;
config.className = L"MyAppClass";
config.title = L"My Application";
config.width = 800;
config.height = 600;
config.x = CW_USEDEFAULT;
config.y = CW_USEDEFAULT;
config.style = WS_OVERLAPPEDWINDOW;
config.exStyle = WS_EX_LAYERED;
config.theme = std::make_shared<SDK::Theme>(SDK::Theme::CreateModernTheme());
config.depth = SDK::WindowDepth::FOREGROUND;
config.roundedCorners = true;
config.cornerRadius = 12;
config.renderCallback = MyRenderFunction;

// Create window with automatic SDK registration
HWND hwnd = SDK::PromptWindowBuilder::CreateWidgetsWindow(config, hInstance);

// Get the SDK window to add widgets
auto window = SDK::WindowManager::GetInstance().GetWindow(hwnd);
window->AddWidget(myWidget);
```

**WindowConfig Structure:**
- `className` - Window class name (default: "5DGUIDemo")
- `title` - Window title (default: "Window")
- `width`, `height` - Window dimensions (default: 800x600)
- `x`, `y` - Window position (default: CW_USEDEFAULT)
- `style` - Window style flags (default: WS_OVERLAPPEDWINDOW)
- `exStyle` - Extended style flags (default: WS_EX_LAYERED)
- `parent` - Parent window handle (default: nullptr)
- `theme` - Theme to apply (default: ModernTheme)
- `depth` - Window depth level (default: FOREGROUND)
- `roundedCorners` - Enable rounded corners (default: false)
- `cornerRadius` - Corner radius in pixels (default: 12)
- `renderCallback` - Custom render function (default: nullptr)

**Benefits:**
- Single function call for complete window setup
- Automatic SDK registration
- Automatic theme application
- Consistent window creation across the application
- Reduces boilerplate code
- Error handling built-in

### Custom Widget Registration

You can register custom widget factories:

```cpp
builder.RegisterWidgetFactory(L"button", [](const std::wstring& params) {
    // Create custom button widget
    auto button = std::make_shared<MyCustomButton>();
    // Configure button based on params
    return button;
});

// Now you can use it in prompts
HWND hwnd = builder.BuildFromPrompt(L"window with button", hInstance);
```

## Using Widget Properties

### Layout Properties Example

```cpp
#include "SDK/SDK.h"

// Create a button with padding and margin
auto button = std::make_shared<SDK::Button>(L"Submit");
button->SetPosition(50, 50);
button->SetSize(120, 40);

// Add padding inside the button
button->SetPadding(10);  // 10px on all sides
// Or set individual padding
button->SetPadding(15, 10, 15, 10);  // left, top, right, bottom

// Add margin around the button
button->SetMargin(20);  // 20px margin on all sides

// Set size constraints
button->SetMinSize(80, 30);   // Minimum size
button->SetMaxSize(200, 60);  // Maximum size
```

### Visual Properties Example

```cpp
// Create a semi-transparent panel
auto panel = std::make_shared<SDK::Panel>();
panel->SetBounds(100, 100, 300, 200);
panel->SetOpacity(0.9f);  // 90% opaque

// Add rounded borders
panel->SetBorderWidth(2);
panel->SetBorderRadius(12);  // Rounded corners

// Set z-index for layering
panel->SetZIndex(10);  // Higher z-index = rendered on top
```

### Font Properties Example

```cpp
// Create a label with custom font
auto label = std::make_shared<SDK::Label>(L"Custom Font Text");
label->SetPosition(50, 150);

// Configure font
label->SetFontFamily(L"Arial");
label->SetFontSize(16);
label->SetFontBold(true);
label->SetFontItalic(false);
```

### Interaction Properties Example

```cpp
// Create a button with tooltip and custom cursor
auto helpButton = std::make_shared<SDK::Button>(L"?");
helpButton->SetBounds(10, 10, 30, 30);

// Add tooltip that appears on hover
helpButton->SetTooltipText(L"Click for help information");

// Set custom cursor (hand pointer)
helpButton->SetCursor(LoadCursor(nullptr, IDC_HAND));

// Set descriptive name for identification
helpButton->SetName(L"HelpButton");
helpButton->SetId(1001);
```

### Complete Widget Configuration

```cpp
// Create a fully configured text box
auto textBox = std::make_shared<SDK::TextBox>();

// Position and size
textBox->SetBounds(50, 200, 250, 35);
textBox->SetMinSize(150, 25);
textBox->SetMaxSize(400, 50);

// Layout
textBox->SetPadding(8, 5, 8, 5);
textBox->SetMargin(10);

// Visual
textBox->SetOpacity(1.0f);
textBox->SetBorderWidth(1);
textBox->SetBorderRadius(4);

// Font
textBox->SetFontFamily(L"Consolas");
textBox->SetFontSize(11);
textBox->SetFontBold(false);
textBox->SetFontItalic(false);

// Interaction
textBox->SetTooltipText(L"Enter your text here");
textBox->SetName(L"MainInputBox");
textBox->SetId(2001);
textBox->SetZIndex(5);

// Content
textBox->SetPlaceholder(L"Type something...");
textBox->SetMaxLength(100);
```

### Property Getter Example

```cpp
// Reading widget properties
auto widget = widgetManager->GetWidgetById(1001);
if (widget) {
    // Get layout info
    int left, top, right, bottom;
    widget->GetPadding(left, top, right, bottom);
    widget->GetMargin(left, top, right, bottom);
    
    // Get size constraints
    int minW, minH, maxW, maxH;
    widget->GetMinSize(minW, minH);
    widget->GetMaxSize(maxW, maxH);
    
    // Get visual properties
    float opacity = widget->GetOpacity();
    int borderWidth = widget->GetBorderWidth();
    int borderRadius = widget->GetBorderRadius();
    
    // Get font properties
    std::wstring fontFamily = widget->GetFontFamily();
    int fontSize = widget->GetFontSize();
    bool isBold = widget->IsFontBold();
    bool isItalic = widget->IsFontItalic();
    
    // Get identification
    std::wstring name = widget->GetName();
    int zIndex = widget->GetZIndex();
    std::wstring tooltip = widget->GetTooltipText();
}
```

## Complete Example

Here's a complete example showing all widgets together:

```cpp
#include "SDK/SDK.h"

HWND g_mainWindow = nullptr;
std::shared_ptr<SDK::WidgetManager> g_widgetManager;
std::shared_ptr<SDK::ProgressBar> g_progressBar;
std::shared_ptr<SDK::Tooltip> g_tooltip;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            
            // Render all widgets
            if (g_widgetManager) {
                g_widgetManager->RenderAll(hdc);
            }
            
            EndPaint(hwnd, &ps);
            return 0;
        }
        
        case WM_MOUSEMOVE: {
            int x = LOWORD(lParam);
            int y = HIWORD(lParam);
            
            if (g_widgetManager) {
                g_widgetManager->HandleMouseMove(x, y);
            }
            
            // Show tooltip on progressbar hover
            if (g_progressBar && g_progressBar->IsHovered() && g_tooltip) {
                if (!g_tooltip->IsShowing()) {
                    g_tooltip->SetText(L"Progress: " + std::to_wstring((int)g_progressBar->GetPercentage()) + L"%");
                    g_tooltip->Show(x + 10, y + 10);
                }
            }
            
# Widget System Guide

This guide covers the new widget system and multidimensional rendering features added to the MaterialGameEngine SDK.

## Overview

The SDK now includes:
- **Complete Widget System**: 15+ widgets ready to use
- **Inline Hooking**: Enhanced window creation interception
- **Multidimensional Rendering**: 3D/4D/5D/6D rendering capabilities
- **Dark & Light Themes**: Modern theme presets

## Widget System

### Basic Widgets

#### Button
```cpp
auto button = std::make_shared<SDK::Button>(L"Click Me");
button->SetPosition(50, 50);
button->SetSize(120, 35);
button->SetEventCallback([](SDK::Widget* widget, SDK::WidgetEvent event, void* data) {
    if (event == SDK::WidgetEvent::CLICK) {
        MessageBoxW(nullptr, L"Button clicked!", L"Info", MB_OK);
    }
});
window->AddWidget(button);
```

#### Label
```cpp
auto label = std::make_shared<SDK::Label>(L"Hello World");
label->SetPosition(50, 100);
label->SetTextColor(SDK::Color(0, 0, 0, 255));
window->AddWidget(label);
```

#### TextBox
```cpp
auto textBox = std::make_shared<SDK::TextBox>();
textBox->SetPosition(50, 150);
textBox->SetSize(200, 30);
textBox->SetPlaceholder(L"Enter text...");
window->AddWidget(textBox);
```

#### CheckBox
```cpp
auto checkbox = std::make_shared<SDK::CheckBox>(L"Enable feature");
checkbox->SetPosition(50, 200);
checkbox->SetEventCallback([](SDK::Widget* widget, SDK::WidgetEvent event, void* data) {
    if (event == SDK::WidgetEvent::VALUE_CHANGED) {
        bool checked = *static_cast<bool*>(data);
        // Handle checkbox state change
    }
});
window->AddWidget(checkbox);
```

### Advanced Widgets

#### ComboBox
```cpp
auto comboBox = std::make_shared<SDK::ComboBox>();
comboBox->SetPosition(50, 250);
comboBox->SetSize(200, 30);
comboBox->AddItem(L"Option 1");
comboBox->AddItem(L"Option 2");
comboBox->AddItem(L"Option 3");
comboBox->SetSelectedIndex(0);
window->AddWidget(comboBox);
```

#### ListView (with checkboxes)
```cpp
auto listView = std::make_shared<SDK::ListView>();
listView->SetPosition(50, 300);
listView->SetSize(300, 150);
listView->SetCheckboxEnabled(true);
listView->AddItem(L"Task 1", false);
listView->AddItem(L"Task 2", true);
listView->AddItem(L"Task 3", false);
window->AddWidget(listView);
```

#### TabControl
```cpp
auto tabControl = std::make_shared<SDK::TabControl>();
tabControl->SetPosition(50, 470);
tabControl->SetSize(500, 200);

auto tab1Content = std::make_shared<SDK::Label>(L"Content for Tab 1");
auto tab2Content = std::make_shared<SDK::Label>(L"Content for Tab 2");

tabControl->AddTab(L"Tab 1", tab1Content);
tabControl->AddTab(L"Tab 2", tab2Content);
tabControl->SetActiveTab(0);
window->AddWidget(tabControl);
```

### Complex Widgets

#### FileTree

A hierarchical tree widget for displaying and navigating file system structures. Now supports both vertical and horizontal orientations!

**Features:**
- Display directory and file hierarchy
- Expandable/collapsible directories
- **Vertical and horizontal orientation** (new!)
- **Expand/collapse all functionality** (new!)
- **Enhanced visual indicators** with triangle arrows (new!)
- File/folder icons
- Click to expand/collapse or select

**Basic Usage:**
```cpp
auto fileTree = std::make_shared<SDK::FileTree>();
fileTree->SetPosition(50, 50);
fileTree->SetSize(300, 400);
fileTree->SetRootPath(L"C:\\");

// Set orientation (default is VERTICAL)
fileTree->SetOrientation(SDK::FileTree::Orientation::VERTICAL);

// Expand/collapse operations
fileTree->ExpandAll();     // Expand all directories
fileTree->CollapseAll();   // Collapse all directories
fileTree->ExpandNode(L"C:\\Users");    // Expand specific path
fileTree->CollapseNode(L"C:\\Users");  // Collapse specific path

window->AddWidget(fileTree);
```

**API Reference:**
```cpp
// Path management
void SetRootPath(const std::wstring& path);
std::wstring GetRootPath() const;
void SetSelectedPath(const std::wstring& path);
std::wstring GetSelectedPath() const;

// Orientation (New!)
enum class Orientation { VERTICAL, HORIZONTAL };
void SetOrientation(Orientation orientation);
Orientation GetOrientation() const;

// Expansion control (New!)
void ExpandAll();                          // Expand all nodes recursively
void CollapseAll();                        // Collapse all nodes recursively
void ExpandNode(const std::wstring& path); // Expand specific node by path
void CollapseNode(const std::wstring& path); // Collapse specific node by path
```

**Orientation Examples:**
```cpp
// Vertical tree (traditional file browser style)
auto verticalTree = std::make_shared<SDK::FileTree>();
verticalTree->SetOrientation(SDK::FileTree::Orientation::VERTICAL);
verticalTree->SetSize(300, 400);

// Horizontal tree (timeline/flow chart style)
auto horizontalTree = std::make_shared<SDK::FileTree>();
horizontalTree->SetOrientation(SDK::FileTree::Orientation::HORIZONTAL);
horizontalTree->SetSize(600, 300);
```

#### FileExplorer
```cpp
auto fileExplorer = std::make_shared<SDK::FileExplorer>();
fileExplorer->SetPosition(50, 50);
fileExplorer->SetSize(600, 400);
fileExplorer->SetCurrentPath(L"C:\\Users");
window->AddWidget(fileExplorer);
```

#### SyntaxHighlightTextEditor
```cpp
auto editor = std::make_shared<SDK::SyntaxHighlightTextEditor>();
editor->SetPosition(50, 50);
editor->SetSize(700, 500);
editor->SetLanguage(SDK::SyntaxHighlightTextEditor::Language::CPP);
editor->SetShowLineNumbers(true);
editor->SetText(L"int main() {\n    return 0;\n}");
window->AddWidget(editor);
```

## Inline Hooking

The SDK now supports inline hooking for automatic window enhancement:

```cpp
// Initialize SDK with inline hooking
SDK::Initialize();
SDK::WindowHook::GetInstance().Initialize(SDK::WindowHook::HookType::INLINE);

// All windows created after this will be automatically hooked
HWND hwnd = CreateWindowExW(...);
```

### Hook Types

- **INLINE**: Inline code patching using trampoline method (recommended)
- **IAT**: Import Address Table hooking (simpler, less reliable)

## Multidimensional Rendering

### 3D Rendering

Render 3D objects with perspective projection:

```cpp
// Render a 3D cube
SDK::Renderer::Vector3D center(0, 0, 0);
SDK::Renderer::Render3DCube(hdc, center, 100.0f, 400, 300,
    SDK::Color(100, 149, 237, 255), rotX, rotY, rotZ);

// Render 3D points
SDK::Renderer::Vector3D point(10, 20, 30);
SDK::Renderer::Render3DPoint(hdc, point, 400, 300,
    SDK::Color(255, 0, 0, 255));

// Render 3D lines
SDK::Renderer::Vector3D start(0, 0, 0);
SDK::Renderer::Vector3D end(100, 100, 50);
SDK::Renderer::Render3DLine(hdc, start, end, 400, 300,
    SDK::Color(0, 255, 0, 255));
```

### 4D Rendering

Render 4D hypercubes with time animation:

```cpp
SDK::Renderer::Vector4D center(0, 0, 0, 0);
float time = GetTickCount() / 1000.0f;
SDK::Renderer::Render4DHypercube(hdc, center, 80.0f, time, 400, 300,
    SDK::Color(255, 100, 100, 255));
```

### 5D Rendering

Render points with depth layer awareness:

```cpp
std::vector<SDK::Renderer::Vector5D> points;
points.push_back(SDK::Renderer::Vector5D(0, 0, 0, 0, 0));   // FAR_BACKGROUND
points.push_back(SDK::Renderer::Vector5D(20, 20, 10, 0, 2)); // MIDDLE
points.push_back(SDK::Renderer::Vector5D(40, 40, 20, 0, 4)); // FOREGROUND

std::vector<SDK::Color> colors;
colors.push_back(SDK::Color(255, 0, 0, 255));
colors.push_back(SDK::Color(0, 255, 0, 255));
colors.push_back(SDK::Color(0, 0, 255, 255));

SDK::Renderer::Render5DScene(hdc, points, time, 400, 300, colors);
```

### 6D Rendering

Render paths across multiple timelines:

```cpp
std::vector<SDK::Renderer::Vector6D> path;
for (int i = 0; i < 10; i++) {
    float t = i * 0.1f;
    path.push_back(SDK::Renderer::Vector6D(
        i * 10, sin(t) * 50, cos(t) * 50,
        0, 2, t
    ));
}

SDK::Renderer::Render6DPath(hdc, path, 400, 300,
    SDK::Color(255, 215, 0, 255));
```

## Themes

### Using Dark Theme

```cpp
auto darkTheme = std::make_shared<SDK::Theme>(SDK::Theme::CreateDarkTheme());
window->SetTheme(darkTheme);
```

### Using Light Theme

```cpp
auto lightTheme = std::make_shared<SDK::Theme>(SDK::Theme::CreateLightTheme());
window->SetTheme(lightTheme);
```

### Custom Theme

```cpp
SDK::Theme customTheme("Custom");

// Title bar gradient
SDK::Gradient gradient(
    SDK::GradientType::VERTICAL,
    SDK::Color(50, 50, 200, 255),
    SDK::Color(20, 20, 100, 255)
);
customTheme.SetTitleBarGradient(gradient);

// Colors
customTheme.SetBackgroundColor(SDK::Color(30, 30, 30, 255));
customTheme.SetBorderColor(SDK::Color(100, 100, 100, 255));
customTheme.SetCornerRadius(10);

// Apply to window
window->SetTheme(std::make_shared<SDK::Theme>(customTheme));
```

## Widget Input Handling

Windows automatically handle widget input when using the SDK:

```cpp
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    auto window = SDK::WindowManager::GetInstance().GetWindow(hwnd);
    if (!window) return DefWindowProc(hwnd, uMsg, wParam, lParam);
    
    switch (uMsg) {
        case WM_MOUSEMOVE: {
            int x = LOWORD(lParam);
            int y = HIWORD(lParam);
            window->HandleWidgetMouseMove(x, y);
            InvalidateRect(hwnd, nullptr, FALSE);
            return 0;
        }
        
        case WM_LBUTTONDOWN: {
            int x = LOWORD(lParam);
            int y = HIWORD(lParam);
            
            if (g_widgetManager) {
                g_widgetManager->HandleMouseDown(x, y);
            }
            
            return 0;
        }
        
        case WM_LBUTTONUP: {
            int x = LOWORD(lParam);
            int y = HIWORD(lParam);
            
            if (g_widgetManager) {
                g_widgetManager->HandleMouseUp(x, y);
            }
            
            return 0;
        }
        
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
            window->HandleWidgetMouseDown(x, y, 0);
            InvalidateRect(hwnd, nullptr, FALSE);
            return 0;
        }
        
        case WM_CHAR: {
            window->HandleWidgetChar((wchar_t)wParam);
            InvalidateRect(hwnd, nullptr, FALSE);
            return 0;
        }
    }
    
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    SDK::Initialize();
    
    // Register window class...
    // Create window...
    
    // Create widget manager
    g_widgetManager = std::make_shared<SDK::WidgetManager>();
    
    // Create progress bar
    g_progressBar = std::make_shared<SDK::ProgressBar>();
    g_progressBar->SetBounds(20, 20, 400, 30);
    g_progressBar->SetValue(75.0f);
    g_progressBar->SetAnimated(true);
    g_widgetManager->AddWidget(g_progressBar);
    
    // Create tooltip
    g_tooltip = std::make_shared<SDK::Tooltip>();
    g_tooltip->SetFadeEnabled(true);
    g_widgetManager->AddWidget(g_tooltip);
    
    ShowWindow(g_mainWindow, nCmdShow);
    
    // Message loop
    MSG msg = {};
    DWORD lastTime = GetTickCount();
    
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
        
        // Update
        DWORD currentTime = GetTickCount();
        float deltaTime = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;
        
        if (g_widgetManager) {
            g_widgetManager->UpdateAll(deltaTime);
        }
        
        InvalidateRect(g_mainWindow, nullptr, FALSE);
    }
    
    SDK::Shutdown();
    return (int)msg.wParam;
}
```

## Integration with Neural Networks (Future)

The `PromptWindowBuilder` is designed to be extensible for future neural network integration. Here's how it could be extended:

```cpp
class NeuralPromptBuilder : public PromptWindowBuilder {
public:
    NeuralPromptBuilder(NeuralNetwork* network) : m_network(network) {}
    
    WindowSpec ParsePrompt(const std::wstring& prompt) override {
        // Use neural network to interpret natural language
        auto interpretation = m_network->Interpret(prompt);
        
        // Generate window specification from interpretation
        WindowSpec spec;
        spec.title = interpretation.windowTitle;
        spec.width = interpretation.dimensions.width;
        spec.height = interpretation.dimensions.height;
        
        for (const auto& widget : interpretation.widgets) {
            spec.widgets.push_back(widget.type);
        }
        
        return spec;
    }
    
private:
    NeuralNetwork* m_network;
};
```

This would allow prompts like:
- "Create a download manager with three progress bars showing current downloads"
- "I need a settings window with sliders for volume and brightness"
- "Make me a dashboard with gauges for CPU, RAM, and disk usage"

## Best Practices

1. **Memory Management**: Use `std::shared_ptr` for widgets to ensure proper cleanup
2. **Event Handling**: Always pass mouse events to the `WidgetManager`
3. **Updates**: Call `UpdateAll(deltaTime)` in your main loop for animations
4. **Rendering**: Call `RenderAll(hdc)` in your `WM_PAINT` handler
5. **IDs**: Assign unique IDs to widgets for easy lookup
6. **Layout**: Use `SetBounds()` to position widgets explicitly or implement auto-layout
7. **Performance**: Only invalidate the window when needed to trigger redraws

## See Also

- [API Reference](API.md) - Complete SDK API documentation
- [Usage Guide](USAGE.md) - General SDK usage examples
- [Examples](examples/) - Sample applications
```

## Best Practices

1. **Always initialize SDK first**: Call `SDK::Initialize()` before creating windows
2. **Use smart pointers**: Widgets use `std::shared_ptr` for automatic memory management
3. **Handle events**: Set up event callbacks for interactive widgets
4. **Update regularly**: Call `UpdateWidgets()` in your main loop for animations
5. **Invalidate on changes**: Call `InvalidateRect()` to redraw when widget state changes

## Examples

See the `examples/widget_showcase.cpp` file for a comprehensive demonstration of all features.

## Building

```cmd
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .
```

The build will generate:
- `5DGUI_Demo.exe` - Original demo
- `Widget_Showcase.exe` - New widget showcase

## Notes

- The inline hooking implementation is production-ready for Windows systems
- Multidimensional rendering uses software projection (no GPU required)
- All widgets support theming through the window's theme
- Widget rendering is hardware-accelerated where possible
- Zero external dependencies - pure Win32 API

## Future Enhancements

See ROADMAP.md for planned features including:
- Advanced text editor features (code completion, find/replace)
- Hardware-accelerated 3D/4D rendering
- Network-enabled widgets
- Plugin system for custom widgets

---

## New Widgets (v1.2.0)

### Slider

A widget for selecting a numeric value by dragging a thumb along a track.

#### Features
- Horizontal and vertical orientations
- Customizable value range
- Smooth dragging interaction
- Visual feedback with filled track
- Color customization for track, thumb, and fill

#### Basic Usage

```cpp
#include "SDK/SDK.h"

// Create horizontal slider
auto slider = std::make_shared<SDK::Slider>(SDK::Slider::Orientation::HORIZONTAL);
slider->SetBounds(50, 50, 200, 30);
slider->SetRange(0.0f, 100.0f);
slider->SetValue(50.0f);

// Set callback for value changes
slider->SetEventCallback([](SDK::Widget* w, SDK::WidgetEvent e, void* data) {
    if (e == SDK::WidgetEvent::VALUE_CHANGED && data) {
        float value = *(float*)data;
        // Use the value...
    }
});

window->AddWidget(slider);
```

#### API Reference

```cpp
// Constructor
Slider(Orientation orientation = Orientation::HORIZONTAL);

// Value management
void SetValue(float value);
float GetValue() const;
void SetRange(float minValue, float maxValue);
void GetRange(float& minValue, float& maxValue) const;

// Orientation
void SetOrientation(Orientation orientation);
Orientation GetOrientation() const;

// Colors
void SetTrackColor(const Color& color);
void SetThumbColor(const Color& color);
void SetFillColor(const Color& color);
```

### RadioButton

A widget for selecting one option from a group of mutually exclusive choices.

#### Features
- Group support (radio buttons in the same group are mutually exclusive)
- Visual indication of selected state
- Customizable colors
- Text labels

#### Basic Usage

```cpp
#include "SDK/SDK.h"

// Create radio button group (group ID = 0)
auto radio1 = std::make_shared<SDK::RadioButton>(L"Option 1", 0);
radio1->SetBounds(50, 50, 200, 20);
radio1->SetChecked(true);

auto radio2 = std::make_shared<SDK::RadioButton>(L"Option 2", 0);
radio2->SetBounds(50, 80, 200, 20);

auto radio3 = std::make_shared<SDK::RadioButton>(L"Option 3", 0);
radio3->SetBounds(50, 110, 200, 20);

// Add to parent so group management works
panel->AddChild(radio1);
panel->AddChild(radio2);
panel->AddChild(radio3);

window->AddWidget(radio1);
window->AddWidget(radio2);
window->AddWidget(radio3);
```

#### API Reference

```cpp
// Constructor
RadioButton(const std::wstring& text = L"", int groupId = 0);

// Text management
void SetText(const std::wstring& text);
std::wstring GetText() const;

// State management
void SetChecked(bool checked);
bool IsChecked() const;

// Group management
void SetGroupId(int groupId);
int GetGroupId() const;
```

### Panel

A container widget for visually grouping related widgets with an optional title bar. Now includes collapsible functionality and boundary constraints!

#### Features
- Optional title bar
- **Collapsible panels** with triangle indicator (new!)
- **Vertical or horizontal collapse** direction (new!)
- **Boundary constraints** to keep children within panel (new!)
- Rounded corners and borders
- Customizable colors
- Can contain child widgets

#### Basic Usage

```cpp
#include "SDK/SDK.h"

// Create panel
auto panel = std::make_shared<SDK::Panel>();
panel->SetBounds(50, 50, 300, 200);
panel->SetTitle(L"Settings");
panel->SetBackgroundColor(SDK::Color(245, 245, 245, 255));

// Enable collapsible feature
panel->SetCollapsible(true);
panel->SetCollapseOrientation(SDK::Panel::CollapseOrientation::VERTICAL);

// Enable boundary constraints
panel->SetConstrainChildren(true);
panel->SetPadding(10);

// Add widgets to panel
auto label = std::make_shared<SDK::Label>(L"Volume:");
label->SetPosition(70, 90);
panel->AddChild(label);

// Click the triangle button in title bar to collapse/expand!

window->AddWidget(panel);
window->AddWidget(label);
```

#### API Reference

```cpp
// Constructor
Panel();

// Title management
void SetTitle(const std::wstring& title);
std::wstring GetTitle() const;

// Colors
void SetBackgroundColor(const Color& color);
void SetBorderColor(const Color& color);
void SetTitleBarColor(const Color& color);

// Collapse/Expand functionality (New!)
void SetCollapsible(bool collapsible);        // Enable collapse button
bool IsCollapsible() const;
void SetCollapsed(bool collapsed);            // Set collapsed state
bool IsCollapsed() const;
void ToggleCollapsed();                       // Toggle between collapsed/expanded
void SetCollapseOrientation(CollapseOrientation orientation);
CollapseOrientation GetCollapseOrientation() const;

// Orientation enum: VERTICAL (collapses to title bar height) or HORIZONTAL (collapses to narrow width)

// Boundary Constraints (New!)
void SetConstrainChildren(bool constrain);    // Keep children within panel bounds
bool IsConstrainChildren() const;
void ClampChildPosition(Widget* child);       // Manually clamp child position
```

#### Examples

**Collapsible Panel:**
```cpp
auto panel = std::make_shared<SDK::Panel>();
panel->SetTitle(L"Options");
panel->SetCollapsible(true);  // Shows triangle button
panel->SetCollapseOrientation(SDK::Panel::CollapseOrientation::VERTICAL);

// User clicks triangle to toggle, or use code:
panel->ToggleCollapsed();
```

**Constrained Children:**
```cpp
auto panel = std::make_shared<SDK::Panel>();
panel->SetConstrainChildren(true);
panel->SetPadding(10);

auto button = std::make_shared<SDK::Button>(L"Button");
button->SetPosition(1000, 1000);  // Way outside panel
panel->AddChild(button);  // Automatically clamped within bounds!
```

### SpinBox

A widget for entering numeric values with increment/decrement buttons.

#### Features
- Up/down buttons for value adjustment
- Keyboard support (arrow keys)
- Customizable value range and step
- Visual feedback on button press

#### Basic Usage

```cpp
#include "SDK/SDK.h"

// Create spinbox
auto spinBox = std::make_shared<SDK::SpinBox>();
spinBox->SetBounds(50, 50, 120, 30);
spinBox->SetRange(0, 100);
spinBox->SetStep(5);
spinBox->SetValue(50);

// Set callback for value changes
spinBox->SetEventCallback([](SDK::Widget* w, SDK::WidgetEvent e, void* data) {
    if (e == SDK::WidgetEvent::VALUE_CHANGED && data) {
        int value = *(int*)data;
        // Use the value...
    }
});

window->AddWidget(spinBox);
```

#### API Reference

```cpp
// Constructor
SpinBox();

// Value management
void SetValue(int value);
int GetValue() const;
void SetRange(int minValue, int maxValue);
void GetRange(int& minValue, int& maxValue) const;

// Step management
void SetStep(int step);
int GetStep() const;
```

### Example: Complete Form with New Widgets

```cpp
#include "SDK/SDK.h"

// Create a settings form
auto panel = std::make_shared<SDK::Panel>();
panel->SetBounds(20, 20, 400, 300);
panel->SetTitle(L"Application Settings");

// Volume slider
auto volumeLabel = std::make_shared<SDK::Label>(L"Volume:");
volumeLabel->SetPosition(40, 70);
auto volumeSlider = std::make_shared<SDK::Slider>();
volumeSlider->SetBounds(120, 65, 250, 30);
volumeSlider->SetRange(0.0f, 100.0f);

// Quality radio buttons
auto qualityLabel = std::make_shared<SDK::Label>(L"Quality:");
qualityLabel->SetPosition(40, 120);

auto lowQuality = std::make_shared<SDK::RadioButton>(L"Low", 0);
lowQuality->SetBounds(120, 120, 100, 20);
auto medQuality = std::make_shared<SDK::RadioButton>(L"Medium", 0);
medQuality->SetBounds(220, 120, 100, 20);
auto highQuality = std::make_shared<SDK::RadioButton>(L"High", 0);
highQuality->SetBounds(320, 120, 100, 20);
highQuality->SetChecked(true);

panel->AddChild(lowQuality);
panel->AddChild(medQuality);
panel->AddChild(highQuality);

// Max connections spinbox
auto connectLabel = std::make_shared<SDK::Label>(L"Max Connections:");
connectLabel->SetPosition(40, 170);
auto connectSpin = std::make_shared<SDK::SpinBox>();
connectSpin->SetBounds(180, 165, 120, 30);
connectSpin->SetRange(1, 100);
connectSpin->SetValue(10);

// Add all to window
window->AddWidget(panel);
window->AddWidget(volumeLabel);
window->AddWidget(volumeSlider);
window->AddWidget(qualityLabel);
window->AddWidget(lowQuality);
window->AddWidget(medQuality);
window->AddWidget(highQuality);
window->AddWidget(connectLabel);
window->AddWidget(connectSpin);
```

See `examples/new_widgets_demo.cpp` for a complete working example.

## RichText Widgets

The SDK provides a rich text system for displaying and editing formatted text with support for bold, italic, underline, hyperlinks, and Markdown rendering.

### RichTextDocument

A container for rich text content with support for text spans and formatting.

#### Features
- Multiple text spans with individual formatting
- Bold, italic, underline, and strikethrough support
- Font family and size per span
- Foreground and background colors
- Hyperlink support
- Markdown parsing
- Export to plain text and HTML

#### Basic Usage

```cpp
#include "SDK/RichText.h"

// Create a document
auto doc = std::make_shared<SDK::RichTextDocument>();

// Add formatted text
doc->AddText(L"This is regular text. ", false, false);
doc->AddText(L"This is bold. ", true, false);
doc->AddText(L"This is italic. ", false, true);

// Add a heading
doc->AddHeading(L"Section Title", 1);

// Add a hyperlink
doc->AddLink(L"Click here", L"https://example.com");

// Parse Markdown
doc->ParseMarkdown(
    L"# Heading\n\n"
    L"This is **bold** and *italic* text.\n\n"
    L"[Link text](https://example.com)"
);

// Export to plain text
std::wstring plainText = doc->ToPlainText();

// Export to HTML
std::wstring html = doc->ToHtml();
```

#### API Reference

**Content Management:**
```cpp
void Clear();                                    // Clear all spans
void AddSpan(const TextSpan& span);             // Add a text span
void InsertSpan(size_t index, const TextSpan& span);
void RemoveSpan(size_t index);
std::vector<TextSpan>& GetSpans();
```

**Quick Text Addition:**
```cpp
void AddText(const std::wstring& text, bool bold = false, bool italic = false);
void AddHeading(const std::wstring& text, int level = 1);
void AddLink(const std::wstring& text, const std::wstring& url);
void AddParagraph(const std::wstring& text);
```

**Markdown Support:**
```cpp
void ParseMarkdown(const std::wstring& markdown);  // Parse and render Markdown
```

**Export:**
```cpp
std::wstring ToPlainText() const;  // Export as plain text
std::wstring ToHtml() const;       // Export as HTML
```

### RichTextBox

An editable widget for displaying and editing rich text.

#### Features
- Editable or read-only mode
- Text selection and formatting
- Scrollable content
- Hyperlink clicking
- Line and paragraph spacing
- Mouse interaction

#### Basic Usage

```cpp
// Create a rich text box
auto richTextBox = std::make_shared<SDK::RichTextBox>();
richTextBox->SetBounds(20, 20, 400, 300);

// Set content
richTextBox->SetText(L"Some text");

// Or use a document
auto doc = richTextBox->GetDocument();
doc->AddText(L"Formatted ", true, false);
doc->AddText(L"text", false, true);

// Enable editing
richTextBox->SetEditable(true);
richTextBox->SetReadOnly(false);

// Handle link clicks
richTextBox->SetEventCallback([](SDK::Widget* w, SDK::WidgetEvent e, void* data) {
    if (e == SDK::WidgetEvent::CLICK && data) {
        const wchar_t* url = static_cast<const wchar_t*>(data);
        // Handle link click
    }
});

window->AddWidget(richTextBox);
```

#### API Reference

**Document:**
```cpp
void SetDocument(std::shared_ptr<RichTextDocument> document);
std::shared_ptr<RichTextDocument> GetDocument() const;
```

**Text Operations:**
```cpp
void SetText(const std::wstring& text);
std::wstring GetText() const;
void AppendText(const std::wstring& text);
void Clear();
```

**Editing:**
```cpp
void SetReadOnly(bool readOnly);
bool IsReadOnly() const;
void SetEditable(bool editable);
bool IsEditable() const;
```

**Selection:**
```cpp
void SelectAll();
void ClearSelection();
bool HasSelection() const;
void SetSelectionRange(size_t start, size_t end);
```

**Formatting:**
```cpp
void SetSelectionBold(bool bold);
void SetSelectionItalic(bool italic);
void SetSelectionUnderline(bool underline);
void SetSelectionColor(Color color);
void SetSelectionFontSize(int size);
```

**Scrolling:**
```cpp
void SetScrollable(bool scrollable);
void ScrollToTop();
void ScrollToBottom();
void ScrollToPosition(int position);
```

**Appearance:**
```cpp
void SetLineSpacing(float spacing);      // Default: 1.2
void SetParagraphSpacing(int spacing);   // Default: 5
```

### RichTextLabel

A simple read-only rich text display widget (inherits from RichTextBox).

```cpp
auto label = std::make_shared<SDK::RichTextLabel>();
label->SetBounds(20, 20, 300, 100);
label->GetDocument()->ParseMarkdown(L"**Bold** and *italic* text");
window->AddWidget(label);
```

## DataGrid Widget

A powerful table widget with sorting, filtering, editing, and virtual scrolling support.

### Features
- Sortable columns (ascending/descending)
- Global and per-column filtering
- Cell editing support
- Virtual scrolling for large datasets
- Single and multi-row selection
- Customizable appearance
- Event callbacks

### Basic Usage

```cpp
#include "SDK/DataGrid.h"

// Create a data grid
auto grid = std::make_shared<SDK::DataGrid>();
grid->SetBounds(20, 20, 800, 500);

// Add columns
grid->AddColumn(L"ID", 60);
grid->AddColumn(L"Name", 200);
grid->AddColumn(L"Age", 80);
grid->AddColumn(L"Department", 150);

// Make columns editable
grid->SetColumnEditable(1, true);  // Name
grid->SetColumnEditable(2, true);  // Age

// Add rows
grid->AddRow({L"1", L"John Doe", L"30", L"Engineering"});
grid->AddRow({L"2", L"Jane Smith", L"28", L"Marketing"});
grid->AddRow({L"3", L"Bob Johnson", L"35", L"Sales"});

// Set selection mode
grid->SetSelectionMode(SDK::DataGrid::SelectionMode::SINGLE);

// Enable virtual scrolling
grid->SetVirtualScrolling(true);

window->AddWidget(grid);
```

### API Reference

#### Column Management

```cpp
void AddColumn(const std::wstring& header, int width = 100);
void AddColumn(const Column& column);
void RemoveColumn(int index);
void ClearColumns();

Column& GetColumn(int index);
int GetColumnCount() const;

void SetColumnWidth(int columnIndex, int width);
void SetColumnSortable(int columnIndex, bool sortable);
void SetColumnEditable(int columnIndex, bool editable);
```

#### Row Management

```cpp
void AddRow(const std::vector<std::wstring>& values);
void AddRow(const Row& row);
void InsertRow(int index, const std::vector<std::wstring>& values);
void RemoveRow(int index);
void ClearRows();

Row& GetRow(int index);
int GetRowCount() const;
```

#### Cell Access

```cpp
void SetCellValue(int row, int column, const std::wstring& value);
std::wstring GetCellValue(int row, int column) const;

Cell& GetCell(int row, int column);
const Cell& GetCell(int row, int column) const;
```

#### Sorting

```cpp
enum class SortOrder { NONE, ASCENDING, DESCENDING };

void SortByColumn(int columnIndex, SortOrder order = SortOrder::ASCENDING);
void ToggleSort(int columnIndex);  // Cycles through NONE -> ASC -> DESC -> NONE

int GetSortColumn() const;
SortOrder GetSortOrder() const;
```

#### Filtering

```cpp
void SetFilter(const std::wstring& filterText);           // Global filter
void SetColumnFilter(int columnIndex, const std::wstring& filterText);
void ClearFilter();

std::wstring GetFilter() const;
bool IsFiltered() const;
```

#### Selection

```cpp
enum class SelectionMode { NONE, SINGLE, MULTI };

void SetSelectionMode(SelectionMode mode);
SelectionMode GetSelectionMode() const;

void SelectRow(int index, bool selected = true);
void SelectAll();
void ClearSelection();

bool IsRowSelected(int index) const;
std::vector<int> GetSelectedRows() const;
int GetSelectedRowCount() const;
```

#### Editing

```cpp
void BeginEdit(int row, int column);
void EndEdit(bool commit = true);
void CancelEdit();

bool IsEditing() const;
int GetEditingRow() const;
int GetEditingColumn() const;
```

Users can press **F2** to begin editing a selected cell, **Enter** to commit, and **Escape** to cancel.

#### Virtual Scrolling

```cpp
void SetVirtualScrolling(bool enabled);
bool IsVirtualScrolling() const;

void SetVisibleRowCount(int count);
int GetVisibleRowCount() const;

void ScrollToRow(int index);
int GetFirstVisibleRow() const;
```

#### Appearance

```cpp
void SetHeaderHeight(int height);        // Default: 30
void SetRowHeight(int height);           // Default: 25
void SetGridLineColor(Color color);      // Default: (200, 200, 200)
void SetHeaderColor(Color color);        // Default: (240, 240, 240)
void SetAlternateRowColor(Color color);  // Default: (250, 250, 250)
void SetSelectionColor(Color color);     // Default: (200, 220, 255)
```

#### Event Callbacks

```cpp
// Called when a cell is clicked
void SetCellClickCallback(std::function<void(int row, int column)> callback);

// Called when a cell value is changed
void SetCellEditCallback(std::function<void(int row, int column, 
    const std::wstring& oldValue, const std::wstring& newValue)> callback);

// Called when sorting changes
void SetSortCallback(std::function<void(int column, SortOrder order)> callback);
```

### Complete Example

```cpp
// Create data grid
auto grid = std::make_shared<SDK::DataGrid>();
grid->SetBounds(20, 20, 700, 400);

// Configure columns
grid->AddColumn(L"ID", 50);
grid->AddColumn(L"Name", 200);
grid->AddColumn(L"Email", 250);
grid->AddColumn(L"Status", 100);

// Make Name and Email editable
grid->SetColumnEditable(1, true);
grid->SetColumnEditable(2, true);

// Add data
for (int i = 1; i <= 100; i++) {
    grid->AddRow({
        std::to_wstring(i),
        L"User " + std::to_wstring(i),
        L"user" + std::to_wstring(i) + L"@example.com",
        (i % 2 == 0) ? L"Active" : L"Inactive"
    });
}

// Enable virtual scrolling for performance
grid->SetVirtualScrolling(true);
grid->SetSelectionMode(SDK::DataGrid::SelectionMode::MULTI);

// Customize appearance
grid->SetHeaderColor(SDK::Color(70, 130, 180, 255));
grid->SetSelectionColor(SDK::Color(135, 206, 250, 255));
grid->SetAlternateRowColor(SDK::Color(240, 248, 255, 255));

// Set up callbacks
grid->SetCellEditCallback([](int row, int col, 
    const std::wstring& oldVal, const std::wstring& newVal) {
    // Handle cell edit
    std::wcout << L"Row " << row << L" Col " << col 
               << L" changed from '" << oldVal 
               << L"' to '" << newVal << L"'" << std::endl;
});

grid->SetSortCallback([](int col, SDK::DataGrid::SortOrder order) {
    // Handle sort change
    std::wcout << L"Sorted by column " << col << std::endl;
});

// Add filter
auto filterBox = std::make_shared<SDK::TextBox>();
filterBox->SetBounds(20, 430, 300, 30);
filterBox->SetPlaceholder(L"Type to filter...");
filterBox->SetEventCallback([grid](SDK::Widget* w, SDK::WidgetEvent e, void* data) {
    if (e == SDK::WidgetEvent::TEXT_CHANGED) {
        auto textBox = dynamic_cast<SDK::TextBox*>(w);
        if (textBox) {
            grid->SetFilter(textBox->GetText());
        }
    }
});

window->AddWidget(grid);
window->AddWidget(filterBox);
```

### Keyboard Navigation

The DataGrid supports keyboard navigation:

- **Arrow Keys**: Navigate between rows
- **F2**: Begin editing selected cell
- **Enter**: Commit edit
- **Escape**: Cancel edit
- **Ctrl+A**: Select all (in multi-select mode)

See `examples/richtext_datagrid_demo.cpp` for a complete working example.
