# Widget System Documentation

The 5D GUI SDK now includes a comprehensive widget system for building rich user interfaces. This document describes the available widgets and how to use them.

## Overview

The widget system provides reusable UI components that can be easily added to windows. All widgets inherit from the base `Widget` class and share common functionality.

## Base Widget Class

All widgets inherit from `SDK::Widget` which provides:

- **Position and Size**: `SetPosition()`, `SetSize()`, `SetBounds()`, `GetBounds()`
- **Visibility**: `SetVisible()`, `IsVisible()`
- **Enabled State**: `SetEnabled()`, `IsEnabled()`
- **Hover State**: `SetHovered()`, `IsHovered()`
- **Hit Testing**: `HitTest(x, y)` - Check if a point is inside the widget
- **Rendering**: `Render(HDC)` - Draw the widget (pure virtual)
- **Animation**: `Update(deltaTime)` - Update widget state for animations
- **Mouse Events**: `OnMouseEnter()`, `OnMouseLeave()`, `OnMouseMove()`, `OnMouseDown()`, `OnMouseUp()`, `OnClick()`
- **Identification**: `SetId()`, `GetId()`, `SetTag()`, `GetTag()`

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
    
    // Get last created widget manager
    std::shared_ptr<WidgetManager> GetLastWidgetManager() const;
    
    // Register custom widget factory
    using WidgetFactory = std::function<std::shared_ptr<Widget>(const std::wstring&)>;
    void RegisterWidgetFactory(const std::wstring& widgetType, WidgetFactory factory);
};
```

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
