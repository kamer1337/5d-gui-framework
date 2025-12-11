# 5D GUI SDK API Reference

Complete API documentation for the 5D GUI SDK.

## Table of Contents

1. [Initialization](#initialization)
2. [Window Management](#window-management)
3. [Window Class](#window-class)
4. [Theming](#theming)
5. [Rendering](#rendering)
6. [Window Hooking](#window-hooking)

---

## Initialization

### SDK::Initialize()

Initialize the 5D GUI SDK. Must be called before creating any windows.

```cpp
bool SDK::Initialize();
```

**Returns**: `true` on success, `false` on failure

**Example**:
```cpp
if (!SDK::Initialize()) {
    // Handle error
    return -1;
}
```

### SDK::Shutdown()

Cleanup and shutdown the SDK. Call before application exit.

```cpp
void SDK::Shutdown();
```

**Example**:
```cpp
SDK::Shutdown();
```

### SDK::GetVersion()

Get SDK version string.

```cpp
const char* SDK::GetVersion();
```

**Returns**: Version string (e.g., "5D GUI SDK v1.2.0")

### SDK::IsInitialized()

Check if SDK is currently initialized.

```cpp
bool SDK::IsInitialized();
```

**Returns**: `true` if initialized

---

## Window Management

### WindowManager (Singleton)

Central manager for all enhanced windows.

```cpp
SDK::WindowManager& manager = SDK::WindowManager::GetInstance();
```

### RegisterWindow()

Register a window with the SDK for enhancement.

```cpp
std::shared_ptr<Window> RegisterWindow(HWND hwnd);
```

**Parameters**:
- `hwnd`: Handle to existing window

**Returns**: Shared pointer to Window object, or `nullptr` on failure

**Example**:
```cpp
HWND hwnd = CreateWindowEx(...);
auto window = SDK::WindowManager::GetInstance().RegisterWindow(hwnd);
if (window) {
    window->SetDepth(SDK::WindowDepth::FOREGROUND);
}
```

### UnregisterWindow()

Remove window from SDK management.

```cpp
void UnregisterWindow(HWND hwnd);
```

**Parameters**:
- `hwnd`: Handle to window

### GetWindow()

Get Window object for existing HWND.

```cpp
std::shared_ptr<Window> GetWindow(HWND hwnd);
```

**Returns**: Window object or `nullptr` if not registered

### GetWindowsByDepth()

Get all windows sorted by depth (back to front).

```cpp
std::vector<std::shared_ptr<Window>> GetWindowsByDepth() const;
```

**Returns**: Vector of windows in rendering order

### Modal Window Management

```cpp
void SetActiveModal(HWND hwnd);
HWND GetActiveModal() const;
void ClearActiveModal();
```

**Example**:
```cpp
// Show modal dialog
SDK::WindowManager::GetInstance().SetActiveModal(dialogHwnd);

// ... user interaction ...

// Clear modal
SDK::WindowManager::GetInstance().ClearActiveModal();
```

### Default Theme

```cpp
void SetDefaultTheme(std::shared_ptr<Theme> theme);
std::shared_ptr<Theme> GetDefaultTheme() const;
```

**Example**:
```cpp
auto theme = std::make_shared<SDK::Theme>(SDK::Theme::CreateModernTheme());
SDK::WindowManager::GetInstance().SetDefaultTheme(theme);
```

### Animation

```cpp
void EnableDepthAnimation(bool enabled);
bool IsDepthAnimationEnabled() const;
void Update(float deltaTime);
```

**Example**:
```cpp
// Enable breathing animation
SDK::WindowManager::GetInstance().EnableDepthAnimation(true);

// In game loop
float deltaTime = /* calculate */;
SDK::WindowManager::GetInstance().Update(deltaTime);
```

---

## Window Class

Represents an enhanced window with 5D rendering capabilities.

### Depth Management

```cpp
void SetDepth(WindowDepth depth);
WindowDepth GetDepth() const;
```

**WindowDepth Enum**:
```cpp
enum class WindowDepth {
    FAR_BACKGROUND = 0,  // 70% scale, alpha 170
    BACKGROUND = 1,       // 80% scale, alpha 200
    MIDDLE = 2,           // 90% scale, alpha 230
    MID_FRONT = 3,        // 95% scale, alpha 245
    FOREGROUND = 4        // 100% scale, alpha 255
};
```

**Example**:
```cpp
window->SetDepth(SDK::WindowDepth::BACKGROUND);
```

### Alpha Transparency

```cpp
void SetAlpha(BYTE alpha);
BYTE GetAlpha() const;
```

**Parameters**:
- `alpha`: 0-255 (0=transparent, 255=opaque)

**Example**:
```cpp
window->SetAlpha(200);  // 78% opacity
```

### Scaling

```cpp
void SetScale(float scale);
float GetScale() const;
```

**Parameters**:
- `scale`: Scaling factor (1.0 = 100%)

**Example**:
```cpp
window->SetScale(0.8f);  // 80% of original size
```

### Depth Offset

```cpp
void SetDepthOffset(int x, int y);
void GetDepthOffset(int& x, int& y) const;
```

Creates perspective effect by offsetting window position.

**Example**:
```cpp
window->SetDepthOffset(10, 6);  // Offset right and down
```

### Theming

```cpp
void SetTheme(std::shared_ptr<Theme> theme);
std::shared_ptr<Theme> GetTheme() const;
```

**Example**:
```cpp
auto auroraTheme = std::make_shared<SDK::Theme>(
    SDK::Theme::CreateAuroraTheme()
);
window->SetTheme(auroraTheme);
```

### Rounded Corners

```cpp
void SetRoundedCorners(bool enabled, int radius = 12);
bool HasRoundedCorners() const;
int GetCornerRadius() const;
```

**Example**:
```cpp
window->SetRoundedCorners(true, 16);  // 16px corner radius
```

### Shadow

```cpp
void SetShadowEnabled(bool enabled);
bool IsShadowEnabled() const;
void SetShadowIntensity(float intensity);
float GetShadowIntensity() const;
```

**Parameters**:
- `intensity`: 0.0-1.0 multiplier for shadow strength

**Example**:
```cpp
window->SetShadowEnabled(true);
window->SetShadowIntensity(0.8f);
```

### Custom Rendering

```cpp
void SetRenderCallback(std::function<void(HDC)> callback);
void Render(HDC hdc);
```

**Example**:
```cpp
window->SetRenderCallback([](HDC hdc) {
    // Custom drawing code
    RECT rect;
    GetClientRect(WindowFromDC(hdc), &rect);
    
    SDK::Renderer::DrawGradient(hdc, rect, 
        SDK::Gradient(SDK::GradientType::HORIZONTAL,
                      SDK::Color(255, 0, 0, 255),
                      SDK::Color(0, 0, 255, 255)));
});
```

### Utility

```cpp
HWND GetHandle() const;
bool IsValid() const;
void UpdateAppearance();
void EnableLayeredMode();
```

---

## Theming

### Theme Class

Defines visual appearance of windows.

### Constructor

```cpp
Theme();
Theme(const std::string& name);
```

**Example**:
```cpp
SDK::Theme myTheme("Custom");
```

### Properties

```cpp
void SetName(const std::string& name);
std::string GetName() const;
```

### Title Bar

```cpp
void SetTitleBarGradient(const Gradient& gradient);
Gradient GetTitleBarGradient() const;
void SetTitleBarHeight(int height);
int GetTitleBarHeight() const;
```

**Example**:
```cpp
SDK::Gradient gradient(
    SDK::GradientType::HORIZONTAL,
    SDK::Color(100, 149, 237, 255),  // Cornflower blue
    SDK::Color(65, 105, 225, 255)     // Royal blue
);
theme.SetTitleBarGradient(gradient);
theme.SetTitleBarHeight(40);
```

### Colors

```cpp
void SetBackgroundColor(const Color& color);
Color GetBackgroundColor() const;
void SetBorderColor(const Color& color);
Color GetBorderColor() const;
void SetBorderWidth(int width);
int GetBorderWidth() const;
```

**Color Structure**:
```cpp
struct Color {
    BYTE r, g, b, a;
    
    Color(BYTE red, BYTE green, BYTE blue, BYTE alpha = 255);
    COLORREF ToCOLORREF() const;
    static Color FromRGB(BYTE r, BYTE g, BYTE b);
};
```

**Example**:
```cpp
theme.SetBackgroundColor(SDK::Color(245, 245, 245, 255));
theme.SetBorderColor(SDK::Color(100, 100, 100, 255));
theme.SetBorderWidth(2);
```

### Corners and Shadow

```cpp
void SetCornerRadius(int radius);
int GetCornerRadius() const;

void SetShadowColor(const Color& color);
Color GetShadowColor() const;
void SetShadowOffset(int x, int y);
void GetShadowOffset(int& x, int& y) const;
void SetShadowBlur(int blur);
int GetShadowBlur() const;
```

### Effects

```cpp
void SetGlowEnabled(bool enabled);
bool IsGlowEnabled() const;
void SetGlowColor(const Color& color);
Color GetGlowColor() const;

void SetParticlesEnabled(bool enabled);
bool AreParticlesEnabled() const;

void SetGlassEffect(bool enabled);
bool IsGlassEffectEnabled() const;
```

### Predefined Themes

```cpp
static Theme CreateDefaultTheme();
static Theme CreateModernTheme();
static Theme CreateAuroraTheme();
```

**Example**:
```cpp
auto theme = SDK::Theme::CreateModernTheme();
window->SetTheme(std::make_shared<SDK::Theme>(theme));
```

### Gradient Types

```cpp
enum class GradientType {
    VERTICAL,    // Top to bottom
    HORIZONTAL,  // Left to right
    RADIAL       // Center to edges
};

struct Gradient {
    GradientType type;
    Color startColor;
    Color endColor;
    int centerX, centerY;  // For radial only
    
    Gradient(GradientType t, Color start, Color end);
};
```

---

## Rendering

### Renderer Class

Static utility class for drawing operations.

### Gradients

```cpp
static void DrawGradient(HDC hdc, const RECT& rect, const Gradient& gradient);
static void DrawVerticalGradient(HDC hdc, const RECT& rect, Color startColor, Color endColor);
static void DrawHorizontalGradient(HDC hdc, const RECT& rect, Color startColor, Color endColor);
static void DrawRadialGradient(HDC hdc, const RECT& rect, Color centerColor, Color edgeColor, int cx, int cy);
```

**Example**:
```cpp
RECT rect = {10, 10, 200, 100};
SDK::Renderer::DrawHorizontalGradient(hdc, rect,
    SDK::Color(255, 0, 0, 255),
    SDK::Color(0, 0, 255, 255));
```

### Shapes

```cpp
static void DrawRoundedRect(HDC hdc, const RECT& rect, int radius, 
                           Color fillColor, Color borderColor, int borderWidth);
```

**Example**:
```cpp
SDK::Renderer::DrawRoundedRect(hdc, rect, 12,
    SDK::Color(255, 255, 255, 255),  // White fill
    SDK::Color(100, 100, 100, 255),  // Gray border
    2);  // 2px border
```

### Effects

```cpp
static void DrawShadow(HDC hdc, const RECT& rect, int offsetX, int offsetY, 
                      int blur, Color shadowColor);
static void DrawGlow(HDC hdc, const RECT& rect, int radius, Color glowColor);
```

**Example**:
```cpp
SDK::Renderer::DrawShadow(hdc, rect, 5, 5, 10,
    SDK::Color(0, 0, 0, 100));  // Soft black shadow
```

### Particles

```cpp
struct Particle {
    float x, y;      // Position
    float vx, vy;    // Velocity
    float life;      // Remaining lifetime
    Color color;
};

static void DrawParticles(HDC hdc, const std::vector<Particle>& particles);
static void UpdateParticles(std::vector<Particle>& particles, float deltaTime);
static std::vector<Particle> CreateParticleEmission(int x, int y, int count, Color color);
```

**Example**:
```cpp
// Create particle burst
auto particles = SDK::Renderer::CreateParticleEmission(100, 100, 50,
    SDK::Color(255, 215, 0, 255));  // Gold particles

// In update loop
SDK::Renderer::UpdateParticles(particles, deltaTime);

// In render loop
SDK::Renderer::DrawParticles(hdc, particles);
```

### Icons

```cpp
enum class IconType {
    CIRCLE,
    SQUARE,
    DIAMOND,
    FOLDER,
    STAR
};

static void DrawIcon(HDC hdc, IconType type, int x, int y, int size, 
                    Color color, float alpha = 1.0f);
```

**Example**:
```cpp
SDK::Renderer::DrawIcon(hdc, SDK::Renderer::IconType::STAR,
    150, 150, 30,
    SDK::Color(255, 215, 0, 255),  // Gold
    0.8f);  // 80% alpha
```

### Utilities

```cpp
static Color InterpolateColor(Color c1, Color c2, float t);
static BYTE BlendAlpha(BYTE src, BYTE dst, BYTE alpha);
static HDC CreateMemoryDC(int width, int height, HBITMAP* outBitmap);
static void DeleteMemoryDC(HDC hdc, HBITMAP bitmap);
```

---

## Window Hooking

### WindowHook (Singleton)

Low-level hooking system for CreateWindowExW.

```cpp
SDK::WindowHook& hook = SDK::WindowHook::GetInstance();
```

### Methods

```cpp
bool Initialize();
void Shutdown();
void RegisterCreateCallback(CreateWindowCallback callback);
bool IsHooked() const;
```

**Example**:
```cpp
SDK::WindowHook::GetInstance().RegisterCreateCallback([](HWND hwnd) {
    // Called for every new window
    OutputDebugString(L"New window created!\n");
});
```

**Note**: The current implementation provides a framework. Production use requires integration with a hooking library like Microsoft Detours.

---

## Complete Example

```cpp
#include "SDK/SDK.h"

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (uMsg == WM_PAINT) {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        
        auto window = SDK::WindowManager::GetInstance().GetWindow(hwnd);
        if (window) {
            window->Render(hdc);
        }
        
        EndPaint(hwnd, &ps);
        return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    // Initialize SDK
    SDK::Initialize();
    
    // Register window class
    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"MyApp";
    RegisterClassEx(&wc);
    
    // Create window
    HWND hwnd = CreateWindowEx(WS_EX_LAYERED, L"MyApp", L"5D GUI Demo",
        WS_OVERLAPPEDWINDOW, 100, 100, 800, 600,
        nullptr, nullptr, hInstance, nullptr);
    
    // Enhance with SDK
    auto window = SDK::WindowManager::GetInstance().RegisterWindow(hwnd);
    auto theme = std::make_shared<SDK::Theme>(SDK::Theme::CreateModernTheme());
    window->SetTheme(theme);
    window->SetDepth(SDK::WindowDepth::FOREGROUND);
    
    // Custom rendering
    window->SetRenderCallback([](HDC hdc) {
        RECT rect;
        GetClientRect(WindowFromDC(hdc), &rect);
        rect.top = 50;
        rect.bottom = 100;
        
        SDK::Renderer::DrawHorizontalGradient(hdc, rect,
            SDK::Color(255, 0, 0, 255),
            SDK::Color(0, 0, 255, 255));
    });
    
    ShowWindow(hwnd, nCmdShow);
    
    // Message loop
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    SDK::Shutdown();
    return 0;
}
```

---

## Widget System

### Widget Base Class

Base class for all UI widgets. All widgets inherit from `SDK::Widget`.

#### Position and Size

```cpp
void SetPosition(int x, int y);
void GetPosition(int& x, int& y) const;

void SetSize(int width, int height);
void GetSize(int& width, int& height) const;

void SetBounds(int x, int y, int width, int height);
void GetBounds(RECT& rect) const;
```

**Example**:
```cpp
auto button = std::make_shared<SDK::Button>(L"Click Me");
button->SetPosition(50, 50);
button->SetSize(120, 40);
// Or use SetBounds to set all at once
button->SetBounds(50, 50, 120, 40);
```

#### State Properties

```cpp
void SetVisible(bool visible);
bool IsVisible() const;

void SetEnabled(bool enabled);
bool IsEnabled() const;

void SetFocused(bool focused);
bool IsFocused() const;

void SetHovered(bool hovered);
bool IsHovered() const;
```

#### Layout Properties

```cpp
// Padding (internal spacing)
void SetPadding(int padding);  // All sides
void SetPadding(int left, int top, int right, int bottom);
void GetPadding(int& left, int& top, int& right, int& bottom) const;

// Margin (external spacing)
void SetMargin(int margin);  // All sides
void SetMargin(int left, int top, int right, int bottom);
void GetMargin(int& left, int& top, int& right, int& bottom) const;

// Size constraints
void SetMinSize(int minWidth, int minHeight);
void GetMinSize(int& minWidth, int& minHeight) const;
void SetMaxSize(int maxWidth, int maxHeight);
void GetMaxSize(int& maxWidth, int& maxHeight) const;
```

**Example**:
```cpp
auto textBox = std::make_shared<SDK::TextBox>();
textBox->SetPadding(10);  // 10px padding on all sides
textBox->SetMargin(5, 10, 5, 10);  // Different margins
textBox->SetMinSize(100, 30);  // Minimum dimensions
textBox->SetMaxSize(500, 100);  // Maximum dimensions
```

#### Visual Properties

```cpp
// Opacity (0.0 = transparent, 1.0 = opaque)
void SetOpacity(float opacity);
float GetOpacity() const;

// Border
void SetBorderWidth(int width);
int GetBorderWidth() const;
void SetBorderRadius(int radius);
int GetBorderRadius() const;
```

**Example**:
```cpp
auto panel = std::make_shared<SDK::Panel>();
panel->SetOpacity(0.8f);  // 80% opaque
panel->SetBorderWidth(2);  // 2px border
panel->SetBorderRadius(8);  // Rounded corners
```

#### Font Properties

```cpp
void SetFontFamily(const std::wstring& family);
std::wstring GetFontFamily() const;

void SetFontSize(int size);
int GetFontSize() const;

void SetFontBold(bool bold);
bool IsFontBold() const;

void SetFontItalic(bool italic);
bool IsFontItalic() const;
```

**Example**:
```cpp
auto label = std::make_shared<SDK::Label>(L"Important Text");
label->SetFontFamily(L"Arial");
label->SetFontSize(16);
label->SetFontBold(true);
label->SetFontItalic(false);
```

#### Interaction Properties

```cpp
// Tooltip
void SetTooltipText(const std::wstring& text);
std::wstring GetTooltipText() const;

// Cursor
void SetCursor(HCURSOR cursor);
HCURSOR GetCursor() const;
```

**Example**:
```cpp
auto button = std::make_shared<SDK::Button>(L"Submit");
button->SetTooltipText(L"Click to submit the form");
button->SetCursor(LoadCursor(nullptr, IDC_HAND));
```

#### Identification Properties

```cpp
// Numeric ID
void SetId(int id);
int GetId() const;

// String name
void SetName(const std::wstring& name);
std::wstring GetName() const;

// Custom data pointer
void SetTag(void* tag);
void* GetTag() const;

// Z-index for layering
void SetZIndex(int zIndex);
int GetZIndex() const;
```

**Example**:
```cpp
auto widget = std::make_shared<SDK::Button>(L"OK");
widget->SetId(1001);
widget->SetName(L"OkButton");
widget->SetZIndex(10);  // Higher z-index renders on top
widget->SetTag(customData);  // Store custom data
```

#### Event Handling

```cpp
using EventCallback = std::function<void(Widget*, WidgetEvent, void* data)>;
void SetEventCallback(EventCallback callback);

// Event types
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
```

**Example**:
```cpp
auto button = std::make_shared<SDK::Button>(L"Click Me");
button->SetEventCallback([](SDK::Widget* w, SDK::WidgetEvent e, void* data) {
    if (e == SDK::WidgetEvent::CLICK) {
        MessageBoxW(nullptr, L"Button clicked!", L"Info", MB_OK);
    }
});
```

#### Hierarchy

```cpp
void SetParent(Widget* parent);
Widget* GetParent() const;

void AddChild(std::shared_ptr<Widget> child);
void RemoveChild(std::shared_ptr<Widget> child);
const std::vector<std::shared_ptr<Widget>>& GetChildren() const;
```

**Example**:
```cpp
auto panel = std::make_shared<SDK::Panel>();
auto button1 = std::make_shared<SDK::Button>(L"Button 1");
auto button2 = std::make_shared<SDK::Button>(L"Button 2");

panel->AddChild(button1);
panel->AddChild(button2);

// Get all children
const auto& children = panel->GetChildren();
```

#### Theme Support

```cpp
void SetTheme(std::shared_ptr<Theme> theme);
std::shared_ptr<Theme> GetTheme() const;
```

**Example**:
```cpp
auto theme = std::make_shared<SDK::Theme>(SDK::Theme::CreateDarkTheme());
button->SetTheme(theme);
```

### Property Defaults

All widgets are initialized with the following defaults:

- **Position**: (0, 0)
- **Size**: 100x30
- **Visible**: true
- **Enabled**: true
- **Padding**: 0 on all sides
- **Margin**: 0 on all sides
- **Min Size**: 0x0 (no minimum)
- **Max Size**: INT_MAX x INT_MAX (no maximum)
- **Opacity**: 1.0 (fully opaque)
- **Border Width**: 0 (no border)
- **Border Radius**: 0 (square corners)
- **Z-Index**: 0
- **Font Family**: "Segoe UI"
- **Font Size**: 12pt
- **Font Bold**: false
- **Font Italic**: false

### Complete Widget Example

```cpp
#include "SDK/SDK.h"

// Create and fully configure a widget
auto createStyledButton() {
    auto button = std::make_shared<SDK::Button>(L"Styled Button");
    
    // Layout
    button->SetBounds(50, 50, 150, 40);
    button->SetPadding(12, 8, 12, 8);
    button->SetMargin(10);
    button->SetMinSize(100, 30);
    button->SetMaxSize(200, 50);
    
    // Visual
    button->SetOpacity(0.95f);
    button->SetBorderWidth(1);
    button->SetBorderRadius(6);
    
    // Font
    button->SetFontFamily(L"Segoe UI");
    button->SetFontSize(14);
    button->SetFontBold(true);
    
    // Interaction
    button->SetTooltipText(L"Click to perform action");
    button->SetCursor(LoadCursor(nullptr, IDC_HAND));
    
    // Identification
    button->SetName(L"ActionButton");
    button->SetId(1001);
    button->SetZIndex(5);
    
    // Event handling
    button->SetEventCallback([](SDK::Widget* w, SDK::WidgetEvent e, void* d) {
        if (e == SDK::WidgetEvent::CLICK) {
            // Handle click
        }
    });
    
    return button;
}
```

