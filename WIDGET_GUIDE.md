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
```cpp
auto fileTree = std::make_shared<SDK::FileTree>();
fileTree->SetPosition(50, 50);
fileTree->SetSize(300, 400);
fileTree->SetRootPath(L"C:\\");
window->AddWidget(fileTree);
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
- More widget types (sliders, spinners, date pickers)
- Advanced text editor features (code completion, find/replace)
- Hardware-accelerated 3D/4D rendering
- Network-enabled widgets
- Plugin system for custom widgets
