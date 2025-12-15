# 5D GUI SDK - Custom Complex C++ GUI System

A custom complex C++ GUI SDK for Windows with inline hooking, complete widget system, multidimensional rendering, and advanced theming capabilities. **Zero external dependencies** - pure Win32 API implementation.

## Overview

The 5D GUI SDK provides an enhanced window rendering system with 5-depth layering, modern visual effects, comprehensive widget system, and multidimensional (3D/4D/5D/6D) rendering support. **Version 2.0.0** adds per-monitor DPI awareness and multi-monitor support for modern display configurations. Windows are automatically enhanced through inline hooking or explicit registration, enabling advanced features without modifying existing code.

## Features

### Core Capabilities
- **Inline Hooking**: Production-ready CreateWindowExW interception with trampoline method
- **Complete Widget System**: 15+ widgets including buttons, text inputs, lists, tabs, and more
- **Multimodal Window Support**: Advanced window management with modal states
- **5D Depth System**: 5 distinct depth levels (FAR_BACKGROUND to FOREGROUND)
- **Layered Windows**: "Book stack" effect with perspective scaling
- **Widget System**: ProgressBar, Tooltip, and extensible widget framework
- **Neural Network GUI Creation**: AI-powered natural language window/widget generation
- **ML Renderer Optimization**: Machine learning for adaptive rendering performance
- **Prompt Window Builder**: Template-based window generation (extensible for AI)
- **Multidimensional Rendering**: 3D/4D/5D/6D rendering with software projection
- **Zero Dependencies**: Pure Win32 API - no external libraries required

### Modern Display Support (v2.0)
- **High DPI Support**: Per-monitor DPI awareness with automatic scaling
- **DPI Change Detection**: Automatic handling of DPI changes when moving between monitors
- **Vector Graphics**: Scalable rendering that adapts to different DPI settings
- **Multi-Monitor Management**: Complete multi-monitor window management system
- **Monitor Enumeration**: Automatic detection and tracking of all connected monitors
- **Monitor-Specific Themes**: Apply different themes to windows on different monitors
- **Window Migration**: Seamlessly move windows between monitors with automatic DPI adjustment
- **Monitor Change Detection**: Automatic detection when windows move to different monitors

### Widget System
- **Basic Widgets**: Button, Label, TextBox, CheckBox, Separator, Image, Slider, RadioButton, SpinBox
- **Container Widgets**: Panel (grouping with optional title bar)
- **Advanced Widgets**: ComboBox, ListBox, ListView (with checkboxes), TabControl, Toolbar, ProgressBar, Tooltip
- **Complex Widgets**: FileTree, FileExplorer, SyntaxHighlightTextEditor (with C++ syntax highlighting)
- **Menu System (v1.3)**: MenuBar, ContextMenu, MenuItem with icons, shortcuts, and submenus
- **Rich Text (v1.3)**: RichTextBox with formatting (bold, italic, underline), colors, fonts, and Markdown support
- **Layout System (v1.3)**: Grid, Flow, and Stack layouts for automatic widget arrangement
- **Advanced Layout (v1.3.1)**: Automatic layout engine with heuristics and constraint-based positioning
- **Toolbar Features**: Horizontal/vertical orientation, auto-hide functionality, customizable items
- **Event System**: Comprehensive event handling (click, hover, focus, value changes)
- **Hierarchy Support**: Parent-child widget relationships
- **Rich Properties**: Padding, margin, min/max size, opacity, border radius, font styling, tooltips, z-index, custom cursors

### Visual Effects
- **Multi-directional Gradients**: Vertical, horizontal, and radial gradients
- **Alpha Transparency**: Per-window and per-element alpha blending
- **Rounded Corners**: Anti-aliased rounded rectangles with configurable radius
- **Depth-Aware Shadows**: Shadow intensity scales with depth level
- **Particle System**: Physics-based particle effects with emission
- **Glow Effects**: Expanding glow halos for highlights
- **Glass Effect**: Translucent window surfaces
- **Advanced Effects (v1.3)**: Depth-of-field, motion blur, chromatic aberration
- **Effect Presets (v1.3)**: CINEMATIC, GAME_UI, RETRO, DREAMY, MOTION - apply complex effect combinations instantly
- **GPU Acceleration (v1.2+)**: Hardware-accelerated rendering via Direct2D when available

### Theming System
- **Predefined Themes**: Default, Modern, Aurora, Dark, and Light themes
- **Custom Themes**: Full control over colors, gradients, and effects
- **Theme Inheritance**: Apply themes to multiple windows
- **Dynamic Theming**: Change themes at runtime
- **Widget Theme Support**: Widgets automatically adapt to window theme

### Multidimensional Rendering
- **3D Rendering**: Perspective projection, cubes, lines, and points
- **4D Rendering**: Hypercube visualization with time animation
- **5D Rendering**: Depth-aware 3D scenes with layer management
- **6D Rendering**: Multi-timeline path visualization
- **Projection System**: Automatic dimension reduction with visual effects

### Camera Controller & 3D Widgets
- **3D Camera Controller**: Full mouse and keyboard camera controls
- **Camera Modes**: Orbit, pan, and zoom with smooth animations
- **3D Widget Placement**: Place interactive UI widgets in 3D space
- **Ray Casting**: Automatic 3D mouse picking for widget interaction
- **Billboard Mode**: Widgets always face camera for optimal visibility
- **Depth Sorting**: Proper rendering order for 3D widgets

### Advanced Window Features
- **Window Groups**: Linked window movement and synchronized transformations
- **Window Snapping**: Edge snapping, grid snapping, and magnetic windows
- **Window Animations**: Smooth minimize/maximize/restore transitions with multiple animation types
- **Easing Functions**: Linear, ease-in, ease-out, ease-in-out, bounce, elastic, and back effects
- **Bezier Curves (v1.3)**: Custom animation curves with control points
- **Animation Control (v1.3)**: Pause, resume, and reverse animations
- **Animation Groups (v1.3)**: Sequence or parallel animation playback

## Quick Start

### DPI and Multi-Monitor Support (v2.0)

The SDK automatically handles DPI awareness and multi-monitor configurations:

```cpp
#include "SDK/SDK.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    // Initialize SDK (automatically enables per-monitor DPI awareness)
    SDK::Initialize();
    
    // Get managers
    auto& dpiMgr = SDK::DPIManager::GetInstance();
    auto& monitorMgr = SDK::MonitorManager::GetInstance();
    
    // Get system DPI for initial window sizing
    SDK::DPIScaleInfo systemDPI = dpiMgr.GetSystemDPI();
    int windowWidth = dpiMgr.ScaleValueX(800, systemDPI);
    int windowHeight = dpiMgr.ScaleValueY(600, systemDPI);
    
    // Create window with DPI-scaled dimensions
    HWND hwnd = CreateWindowExW(WS_EX_LAYERED, L"MyClass", L"DPI-Aware App",
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
        windowWidth, windowHeight, nullptr, nullptr, hInstance, nullptr);
    
    // Register with SDK
    auto window = SDK::WindowManager::GetInstance().RegisterWindow(hwnd);
    
    // Track window for monitor changes
    monitorMgr.TrackWindow(hwnd);
    
    // Register DPI change callback
    dpiMgr.RegisterDPIChangeCallback(hwnd, [window](HWND hwnd, 
        const SDK::DPIScaleInfo& oldDPI, const SDK::DPIScaleInfo& newDPI) {
        window->HandleDPIChange(oldDPI, newDPI);
    });
    
    // Register monitor change callback  
    monitorMgr.RegisterMonitorChangeCallback(hwnd, [window](HWND hwnd,
        HMONITOR oldMonitor, HMONITOR newMonitor) {
        window->HandleMonitorChange(oldMonitor, newMonitor);
    });
    
    // Set monitor-specific theme (optional)
    const SDK::MonitorInfo* primary = monitorMgr.GetPrimaryMonitor();
    if (primary) {
        auto theme = std::make_shared<SDK::Theme>(SDK::Theme::CreateModernTheme());
        monitorMgr.SetMonitorTheme(primary->hMonitor, theme);
    }
    
    ShowWindow(hwnd, nCmdShow);
    
    // Message loop with monitor update
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
        monitorMgr.Update(); // Check for window monitor changes
    }
    
    SDK::Shutdown();
    return 0;
}

// Handle WM_DPICHANGED in window procedure
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_DPICHANGED) {
        SDK::DPIManager::GetInstance().HandleDPIChange(hwnd, wParam, lParam);
        return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}
```

### Basic Window with Widgets (Universal Function)

The SDK now provides a universal `CreateWidgetsWindow` function that simplifies window creation:

```cpp
#include "SDK/SDK.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    // Initialize SDK
    SDK::Initialize();
    
    // Configure window
    SDK::PromptWindowBuilder::WindowConfig config;
    config.className = L"MyClass";
    config.title = L"Widget App";
    config.width = 800;
    config.height = 600;
    config.theme = std::make_shared<SDK::Theme>(SDK::Theme::CreateDarkTheme());
    config.depth = SDK::WindowDepth::FOREGROUND;
    config.roundedCorners = true;
    
    // Create window with automatic SDK registration
    HWND hwnd = SDK::PromptWindowBuilder::CreateWidgetsWindow(config, hInstance);
    
    // Get the SDK window to add widgets
    auto window = SDK::WindowManager::GetInstance().GetWindow(hwnd);
    
    // Add widgets
    auto button = std::make_shared<SDK::Button>(L"Click Me");
    button->SetPosition(50, 50);
    button->SetSize(150, 40);
    button->SetEventCallback([](SDK::Widget* w, SDK::WidgetEvent e, void* data) {
        if (e == SDK::WidgetEvent::CLICK) {
            MessageBoxW(nullptr, L"Button clicked!", L"Info", MB_OK);
        }
    });
    window->AddWidget(button);
    
    auto textBox = std::make_shared<SDK::TextBox>();
    textBox->SetPosition(50, 100);
    textBox->SetSize(300, 30);
    window->AddWidget(textBox);
    
    ShowWindow(hwnd, nCmdShow);
    
    // Message loop...
    
    SDK::Shutdown();
    return 0;
}
```

### Neural Network GUI Creation (New!)

Create windows and widgets using natural language prompts:

```cpp
#include "SDK/SDK.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    // Initialize SDK
    SDK::Initialize();
    
    // Create neural prompt builder
    SDK::NeuralPromptBuilder builder;
    
    // Create window from natural language prompt
    HWND hwnd = builder.BuildFromPrompt(
        L"Create a window 800x600 called 'My App' with a button and textbox",
        hInstance
    );
    
    // Register with SDK and apply theme
    auto window = SDK::WindowManager::GetInstance().RegisterWindow(hwnd);
    auto theme = std::make_shared<SDK::Theme>(SDK::Theme::CreateModernTheme());
    window->SetTheme(theme);
    window->UpdateAppearance();
    
    ShowWindow(hwnd, nCmdShow);
    
    // Message loop...
    
    SDK::Shutdown();
    return 0;
}
```

More examples:
```cpp
// Login window
builder.BuildFromPrompt(
    L"Create window 400x300 'Login' with textbox for username, textbox for password, and button 'Sign In'",
    hInstance
);

// Settings dialog
builder.BuildFromPrompt(
    L"Create dialog 500x400 'Settings' with checkbox 'Enable notifications' and button 'Apply'",
    hInstance
);

// Generate callback from description
auto callback = builder.GenerateCallback(L"Show message on click");
button->SetEventCallback(callback);
```

See [Neural Network Guide](NEURAL_NETWORK_GUIDE.md) for complete documentation.

### Basic Window with Widgets (Manual Registration)

You can also create windows manually if you need more control:

```cpp
#include "SDK/SDK.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    // Initialize SDK with inline hooking
    SDK::Initialize();
    SDK::WindowHook::GetInstance().Initialize(SDK::WindowHook::HookType::INLINE);
    
    // Create window
    HWND hwnd = CreateWindowEx(WS_EX_LAYERED, L"MyClass", L"Widget App",
        WS_OVERLAPPEDWINDOW, 100, 100, 800, 600,
        nullptr, nullptr, hInstance, nullptr);
    
    // Enhance with SDK
    auto window = SDK::WindowManager::GetInstance().RegisterWindow(hwnd);
    auto theme = std::make_shared<SDK::Theme>(SDK::Theme::CreateDarkTheme());
    window->SetTheme(theme);
    
    // Add widgets
    auto button = std::make_shared<SDK::Button>(L"Click Me");
    button->SetPosition(50, 50);
    button->SetSize(150, 40);
    button->SetEventCallback([](SDK::Widget* w, SDK::WidgetEvent e, void* data) {
        if (e == SDK::WidgetEvent::CLICK) {
            MessageBoxW(nullptr, L"Button clicked!", L"Info", MB_OK);
        }
    });
    window->AddWidget(button);
    
    auto textBox = std::make_shared<SDK::TextBox>();
    textBox->SetPosition(50, 100);
    textBox->SetSize(300, 30);
    window->AddWidget(textBox);
    
    ShowWindow(hwnd, nCmdShow);
    
    // Message loop...
    
    SDK::Shutdown();
    return 0;
}
```

### Using Widgets

```cpp
#include "SDK/SDK.h"

// Create widget manager
auto widgetMgr = std::make_shared<SDK::WidgetManager>();

// Add a progress bar
auto progressBar = std::make_shared<SDK::ProgressBar>();
progressBar->SetBounds(20, 20, 400, 30);
progressBar->SetValue(75.0f);
progressBar->SetAnimated(true);
widgetMgr->AddWidget(progressBar);

// Add a tooltip
auto tooltip = std::make_shared<SDK::Tooltip>();
tooltip->SetText(L"Progress: 75%");
widgetMgr->AddWidget(tooltip);

// In WM_PAINT: widgetMgr->RenderAll(hdc);
// In main loop: widgetMgr->UpdateAll(deltaTime);
```

### Prompt-Based Window Generation

```cpp
#include "SDK/SDK.h"

SDK::PromptWindowBuilder builder;

// Create window from text prompt
HWND hwnd = builder.BuildFromPrompt(
    L"window 600x400 'My App' with progressbar and tooltip",
    hInstance);

auto widgetMgr = builder.GetLastWidgetManager();
ShowWindow(hwnd, SW_SHOW);
```

## Documentation

### Core Documentation
- **[Getting Started](USAGE.md)**: Practical examples and patterns
- **[Build Guide](BUILD.md)**: Compilation instructions for all platforms
- **[Demo Guide](DEMO_GUIDE.md)**: Complete guide to all 18 demo applications
- **[API Reference](API.md)**: Complete API documentation with examples
- **[Quick Reference](QUICKREF.md)**: Quick syntax reference and common patterns

### Feature Guides
- **[Widget Guide](WIDGET_GUIDE.md)**: Complete widget system documentation
- **[Advanced Layout Guide](ADVANCED_LAYOUT_GUIDE.md)**: Automatic layout engine and constraint solver
- **[Neural Network Guide](NEURAL_NETWORK_GUIDE.md)**: AI-powered GUI creation with natural language
- **[Optimization Guide](OPTIMIZATION_GUIDE.md)**: Rendering and memory optimization features
- **[ML Renderer Optimization](ML_RENDERER_OPTIMIZATION.md)**: Machine learning-based renderer optimization
- **[Camera Controller Guide](CAMERA_GUIDE.md)**: 3D camera controls and widget placement
- **[Advanced Window Features](ADVANCED_WINDOW_FEATURES.md)**: Window groups, snapping, and animations

### Technical Documentation
- **[Architecture](ARCHITECTURE.md)**: Technical design and internals
- **[Architecture Diagram](ARCHITECTURE_DIAGRAM.md)**: Visual system architecture
- **[Optimizations Completed](OPTIMIZATIONS_COMPLETED.md)**: Implementation details and benchmarks

### Platform-Specific
- **[Linux Support](LINUX_SUPPORT.md)**: Linux platform support information

### Project Information
- **[Roadmap](ROADMAP.md)**: Future features and development plans
- **[TODO](TODO.md)**: Active tasks and improvements
- **[Contributing](CONTRIBUTING.md)**: Guidelines for contributors

## Building

### CMake (Recommended)

#### Windows
```cmd
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .
```

#### Linux
```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .
```

**Linux Requirements:**
- X11 development libraries: `sudo apt-get install libx11-dev`
- C++17 compiler (GCC 7+, Clang)

### Makefile (MinGW - Windows only)
```cmd
mingw32-make all
```

### Requirements

#### Windows
- Windows 7 or later
- C++17 compiler (MSVC, MinGW, Clang)
- CMake 3.10+ (optional)

#### Linux
- Linux with X11 or Wayland
- C++17 compiler (GCC 7+, Clang)
- CMake 3.10+ (required)
- X11 development libraries (`libx11-dev` on Ubuntu/Debian)

## Project Structure

```
MaterialGameEngine/
├── include/SDK/          # Public headers
│   ├── SDK.h            # Main SDK header
│   ├── Window.h         # Window class
│   ├── WindowManager.h  # Window management
│   ├── WindowHook.h     # CreateWindowExW hooking
│   ├── Theme.h          # Theming system
│   ├── Renderer.h       # Rendering utilities
│   ├── Widget.h         # Base widget class
│   ├── ProgressBar.h    # ProgressBar widget
│   ├── Tooltip.h        # Tooltip widget
│   ├── WidgetManager.h  # Widget management
│   ├── PromptWindowBuilder.h  # Prompt-based generation
│   ├── NeuralNetwork.h        # Neural network for NLP
│   ├── NeuralPromptBuilder.h  # AI-powered window generation
│   ├── AdvancedWidgets.h     # Advanced widget components
│   ├── CameraController.h    # 3D camera controls
│   ├── Widget3D.h            # 3D widget placement
│   ├── Toolbar.h             # Toolbar widget
│   ├── Layout.h              # Layout system (Grid, Flow, Stack, Engine, Constraints)
│   ├── WindowGroup.h         # Window grouping
│   ├── WindowSnapping.h      # Window snapping
│   └── WindowAnimation.h     # Window animations
├── src/SDK/             # Implementation files
├── examples/            # Demo applications
│   ├── demo.cpp              # Original 5D rendering demo
│   ├── widget_demo.cpp       # Widget system demo
│   ├── widget_showcase.cpp   # Complete widget showcase
│   ├── neural_network_demo.cpp  # Neural network GUI creation demo
│   ├── camera_demo.cpp       # 3D camera controller demo
│   ├── toolbar_demo.cpp      # Toolbar widget demo
│   ├── layout_engine_demo.cpp   # Advanced layout engine demo
│   └── advanced_window_demo.cpp  # Advanced window features demo
├── build/               # Build output (generated)
├── CMakeLists.txt       # CMake build file
├── Makefile             # Alternative build system
└── README.md            # This file
```

## Platform Support

### Windows
- **Windows 7**: Full support (requires Platform Update)
- **Windows 8/8.1**: Full support
- **Windows 10/11**: Full support with enhanced effects
- **Architecture**: x86 and x64
- **Features**: All features fully supported including window hooking, layered windows, and advanced effects

### Linux
- **Linux**: Full X11 backend support (v1.3.0+)
- **Display Servers**: X11 (fully implemented), Wayland (planned)
- **Architecture**: x86_64, ARM64
- **Features**: 
  - ✅ Core SDK compilation
  - ✅ Neural network support (platform-independent)
  - ✅ Widget system (platform-independent)
  - ✅ X11 window creation and management
  - ✅ X11 rendering backend (shapes, text, gradients)
  - ✅ Event handling (mouse, keyboard, window events)
  - ✅ Demo applications (2 Linux demos available)
  - ❌ Window hooking (not available on Linux)
  - ❌ Advanced effects (GPU-accelerated effects not yet available)

**Note**: Linux X11 backend is fully functional with window creation, rendering, and event handling. See [Linux Support Guide](LINUX_SUPPORT.md) for details.

#### Linux Screenshots

**Basic Rendering Demo:**
![Linux X11 Demo](https://github.com/user-attachments/assets/49167026-46e9-430e-b5eb-5478f5f24b58)

**Widget System Demo:**
![Linux Widget Demo](https://github.com/user-attachments/assets/a4e519d3-3445-45ad-a41d-989d5a45c5e7)

## Dependencies

### Windows
**None!** The SDK uses only:
- windows.h (Win32 API)
- dwmapi.lib (Desktop Window Manager)
- gdi32.lib (Graphics Device Interface)
- user32.lib (Window management)

All libraries are standard Windows SDK components.

### Linux
- X11 libraries (libX11)
- pthread (threading support)
- Standard C++ library

## License

MIT License - See [LICENSE](LICENSE) file for details.

---

# Enhanced 5D Rendering - Visual Specifications

This section describes the visual capabilities of the enhanced 5D rendering system.

### Visual Description:
This screenshot demonstrates the improved window rendering with alpha transparency and enhanced shadows.

```
┌────────────────────────────────────────────────────────────────────────┐
│  [Full HD 1920x1080]                               │
│                                                                          │
│   ┌─────── Front Window ───────┐  ╔════════ Back Window (80% α) ══════╗│
│   │ █████████████████████████  │  ║  ░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░  ║│
│   │ Title Bar (Blue Gradient)  │  ║  Semi-transparent content shows   ║│
│   │ [Minimize] [Maximize] [X]  │  ║  through to desktop background    ║│
│   ├────────────────────────────┤  ║                                   ║│
│   │  • Sharp corners → Rounded │  ║  • Softer shadow (3px offset)    ║│
│   │  • Strong shadow (5px off) │  ║  • Rounded corners with alpha     ║│
│   │  • Full opacity (100%)     │  ║  • Perspective offset applied     ║│
│   │  • Vivid blue gradient     │  ╚═══════════════════════════════════╝│
│   │                            │                                        │
│   │  Content area with         │    ╭─── Mid Window (90% α) ────╮      │
│   │  alpha-blended elements    │    │  ▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒  │      │
│   └────────────────────────────┘    │  Medium shadow & scale    │      │
│                                      ╰────────────────────────────╯      │
│                                                                          │
│  Visual Hierarchy:                                                      │
│  • Focused window: Full color, strong shadow, sharp appearance         │
│  • Background windows: Transparency reveals depth, softer shadows      │
│  • All corners rounded (12px radius) for modern aesthetic             │
└────────────────────────────────────────────────────────────────────────┘
```

**Key Features Shown**:
- Three windows at different depth levels
- Alpha transparency creating layered effect
- Variable shadow intensity based on depth
- Rounded corners with smooth anti-aliasing
- Perspective offset creating 3D illusion

---
### Visual Description:
This screenshot showcases the "book page" or "card stack" effect with 5 windows rendered at different depths.

```
┌────────────────────────────────────────────────────────────────────────┐
│  [Layered Window Demonstration]                            │
│                                                                          │
│        ╔════════════════════════════════════╗  (Far background)        │
│        ║  Back Window (70% scale)          ║  • 70% scale              │
│        ║  ░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░  ║  • +15px right, +9px down │
│        ╚════════════════════════════════════╝  • α: 170               │
│      ╔══════════════════════════════════════╗   (Background)           │
│      ║  Mid-Back Window (80% scale)        ║   • 80% scale             │
│      ║  ▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒  ║   • +10px right, +6px down│
│      ╚══════════════════════════════════════╝   • α: 200               │
│    ╔════════════════════════════════════════╗   (Middle)               │
│    ║  Middle Window (90% scale)             ║   • 90% scale            │
│    ║  ▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓  ║   • +5px right, +3px down│
│    ╚════════════════════════════════════════╝   • α: 230               │
│  ┌──────────────────────────────────────────┐   (Mid-front)            │
│  │  Mid-Front Window (95% scale)            │   • 95% scale            │
│  │  ████████████████████████████████████    │   • +2px right, +1px down│
│  └──────────────────────────────────────────┘   • α: 245               │
│ ┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓   (Foreground)            │
│ ┃  Front Window (100% scale)               ┃   • 100% scale           │
│ ┃  ███████████████████████████████████████ ┃   • No offset            │
│ ┃                                           ┃   • α: 255               │
│ ┃  Click "Layered Windows" button to see   ┃   • Subtle glow effect   │
│ ┃  this beautiful depth effect in action!  ┃                           │
│ ┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛                           │
│                                                                          │
│  Animation: Windows slowly breathe with wave effect                    │
└────────────────────────────────────────────────────────────────────────┘
```

**Key Features Shown**:
- Five distinct depth layers
- Perspective transformation (scaling + offset)
- Alpha gradient from back (170) to front (255)
- Shadow intensity decreases with depth
- Creates illusion of pages stacked in 3D space
- Subtle animation showing depth wave effect

---

### Visual Description:
This screenshot demonstrates the icon system with various depth levels and interactive effects.

```
┌────────────────────────────────────────────────────────────────────────┐
│  [5D Icon System Showcase]                                │
│                                                                          │
│  Icon Depth Demonstration:                                             │
│                                                                          │
│     ▓▓     ▒▒▒     ░░░░      ▓▓▓      ███                             │
│    ▓▓▓▓   ▒▒▒▒▒   ░░░░░░    ▓▓▓▓▓    █████    ← Icons at depth       │
│     ▓▓     ▒▒▒     ░░░░      ▓▓▓      ███                             │
│     ↓       ↓        ↓         ↓        ↓                              │
│    FAR    BACK    NORMAL   NORMAL   FRONT                             │
│   (70%)   (80%)    (90%)    (90%)   (100%)  ← Scale percentages       │
│                                                                          │
│  Interactive States:                                                    │
│                                                                          │
│   Normal State:          Hover State:                                  │
│      ███                    ╭─◉─╮  ← Glow effect                      │
│     █████                   │███│                                      │
│      ███                    ╰─◉─╯  ← 110% scale                       │
│                               ✦    ← Particle emission                 │
│                                                                          │
│  Icon Types Generated (0-9):                                           │
│                                                                          │
│   ⬤  Circular    □  Square     ◇  Diamond    📁 Folder    ★  Star    │
│   gradient      border                                                  │
│                                                                          │
│  Depth Effects Applied:                                                │
│  • Foreground (100%): Full opacity, strong shadow, particles on hover │
│  • Normal (90%): Slight transparency, medium shadow                    │
│  • Background (80%): More transparent, soft shadow, offset right       │
│  • Far (70%): Maximum transparency, minimal shadow, larger offset      │
│                                                                          │
│  [ Hover over icons to see glow and particle effects ]                │
│                                                                          │
└────────────────────────────────────────────────────────────────────────┘
```

**Key Features Shown**:
- Five icons at different depth levels
- Procedurally generated icon shapes (circle, square, diamond, folder, star)
- Hover state with glow halo
- Particle emission on foreground icon hover
- Scale and alpha variations by depth
- Position offset creating perspective
- Shadow strength correlated to depth

---

```
┌────────────────────────────────────────────────────────────────────────┐
│  Enhanced 5D Rendering Complete Showcase                   │
├────────────────────────────────────────────────────────────────────────┤
│                                                                          │
│  ╔══════════════════════════════════════════════════════════════════╗ │
│  ║ Enhanced 5D Rendering Demo Window                         [-][□][X]║ │
│  ║ ████████████████████████████ ← Horizontal gradient title bar      ║ │
│  ╠══════════════════════════════════════════════════════════════════╣ │
│  ║                                                                    ║ │
│  ║  Button Row 1:                                                    ║ │
│  ║  [Gradient] [Particles] [Glow] [Glass]                          ║ │
│  ║   3D/3D      5D          5D     5D                                ║ │
│  ║                                                                    ║ │
│  ║  Button Row 2 (NEW):                                              ║ │
│  ║  [Layered Windows] [5D Icons] [H/R Gradients] [Animation]       ║ │
│  ║   ← Book effect    ← Depth    ← Horizontal    ← 4D Easing        ║ │
│  ║                                  & Radial                          ║ │
│  ║  ┌────────────────────────────────────────────────────────────┐  ║ │
│  ║  │  Live Effect Display Area:                                 │  ║ │
│  ║  │                                                             │  ║ │
│  ║  │  ┏━━━━━━┓  ┏━━━━━━┓  ┏━━━━━━┓  ← Layered windows        │  ║ │
│  ║  │  ┃      ┃  ┃      ┃  ┃      ┃    with depth               │  ║ │
│  ║  │  ┗━━━━━━┛  ┗━━━━━━┛  ┗━━━━━━┛                            │  ║ │
│  ║  │                                                             │  ║ │
│  ║  │   ⬤  ⬤  ⬤  ⬤  ⬤   ← 5D icons at depths                  │  ║ │
│  ║  │   ✦ (particles)                                            │  ║ │
│  ║  │                                                             │  ║ │
│  ║  │   ▓▓▓▓▓▓▓▓▓▓░░░   ← Horizontal gradient                  │  ║ │
│  ║  │                                                             │  ║ │
│  ║  │       ◉          ← Radial gradient                        │  ║ │
│  ║  │      ◉◉◉                                                   │  ║ │
│  ║  │       ◉                                                    │  ║ │
│  ║  └────────────────────────────────────────────────────────────┘  ║ │
│  ║                                                                    ║ │
│  ║  Features Demonstrated:                                           ║ │
│  ║  ✓ Multi-layer shadows with depth awareness                      ║ │
│  ║  ✓ Alpha-blended rounded corners                                 ║ │
│  ║  ✓ Layered window rendering (book effect)                        ║ │
│  ║  ✓ 5D icon system with hover & particles                         ║ │
│  ║  ✓ Horizontal & radial gradients                                 ║ │
│  ║  ✓ Interactive particle emissions                                ║ │
│  ╚══════════════════════════════════════════════════════════════════╝ │
│                                                                          │
│  ╭─────────────────────────────────╮                                   │
│  │ 5D Rendering Features Info      │  ← Info window with              │
│  │ ───────────────────────────────│    rounded corners                 │
│  │ Enhanced 3D Depth:              │                                    │
│  │ * Multi-layer shadows           │                                    │
│  │ * Horizontal gradients          │                                    │
│  │ * Radial gradients              │                                    │
│  │ * Rounded alpha corners         │                                    │
│  │                                 │                                    │
│  │ 5D Interactive:                 │                                    │
│  │ * Layered window depth          │                                    │
│  │ * Icon depth system             │                                    │
│  │ * Perspective scaling           │                                    │
│  ╰─────────────────────────────────╯                                   │
│                                                                          │
└────────────────────────────────────────────────────────────────────────┘
```

**Key Features Shown**:
- All enhanced features in one screen
- Demo window with enhanced styling
- Live effect display area showing:
  - Layered windows at 3 depths
  - 5D icons with hover particle effects
  - Horizontal gradient bar
  - Radial gradient circle
- Info panel with rounded corners
- Multiple shadow layers creating depth
- Interactive elements with visual feedback

---

## Screenshot 5: Horizontal & Radial Gradients Detail
**Filename**: `aurora-os-enhanced-gradients.png`

### Visual Description:
Close-up showcase of the new gradient types.

```
┌────────────────────────────────────────────────────────────────────────┐
│  Gradient Enhancement Showcase                                         │
│                                                                          │
│  1. HORIZONTAL GRADIENT (Left to Right):                               │
│  ╔═══════════════════════════════════════════════════════════════════╗│
│  ║█▓▓▒▒▒▒░░░░  ░░░░▒▒▒▒▓▓█                                          ║│
│  ║   Red  →→→  Smooth transition  →→→  Blue                          ║│
│  ╚═══════════════════════════════════════════════════════════════════╝│
│                                                                          │
│  2. VERTICAL GRADIENT (Top to Bottom) - Original:                      │
│  ╔═══════════════════════════════════════════════════════════════════╗│
│  ║████████████████████  ← Dark Blue (top)                            ║│
│  ║▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓                                             ║│
│  ║▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒  ↓ Smooth transition                         ║│
│  ║░░░░░░░░░░░░░░░░░░░░                                               ║│
│  ║                      ← Light Blue (bottom)                         ║│
│  ╚═══════════════════════════════════════════════════════════════════╝│
│                                                                          │
│  3. RADIAL GRADIENT (Center to Edge):                                  │
│                                                                          │
│              ░░░░                                                       │
│            ░▒▒▒▒▒░                                                     │
│           ░▒▓███▓▒░     ← Golden center                               │
│            ░▒▒▒▒▒░        fading to red edges                         │
│              ░░░░                                                       │
│                                                                          │
│  Applications:                                                          │
│  • Horizontal: Progress bars, modern panels, navigation                │
│  • Vertical: Window title bars, menus, backgrounds                     │
│  • Radial: Spotlights, buttons, attention grabbers                     │
│                                                                          │
│  Technical:                                                             │
│  • Per-pixel interpolation for smooth transitions                      │
│  • Full RGBA support with alpha blending                               │
│  • Optimized rendering (line-by-line for H/V, pixel for radial)       │
└────────────────────────────────────────────────────────────────────────┘
```

**Key Features Shown**:
- Three gradient types demonstrated
- Smooth color transitions
- Different use cases highlighted
- Technical details included

---

## Summary of Visual Improvements

### Compared to Original System:

**Before Enhanced 5D**:
- Simple vertical gradients only
- No window depth layering
- Icons at single depth level
- Basic shadows without depth awareness
- Rectangular corners only

**After Enhanced 5D**:
- Multi-directional gradients (vertical, horizontal, radial)
- Layered window rendering with perspective
- Icon depth system with 4 levels
- Depth-aware shadows with variable intensity
- Rounded corners with alpha transparency
- Interactive hover effects with particles
- Glow effects around UI elements
- Perspective scaling for depth illusion

### Visual Impact:
The enhanced 5D rendering system creates a **modern, depth-aware interface** that feels more like a contemporary OS (macOS, Windows 11) while maintaining the unique Aurora OS aesthetic. The combination of transparency, depth layering, and interactive effects creates a **visually rich environment** that engages users and provides clear visual hierarchy.
