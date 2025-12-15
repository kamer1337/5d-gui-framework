# Linux Support Guide

This document provides information about Linux support in the 5D GUI SDK.

## Current Status

The 5D GUI SDK now includes **X11 backend support** as of version 1.3.0. This means:

- ✅ **SDK compiles on Linux** - The core SDK library builds successfully
- ✅ **Neural network fully functional** - All AI-powered GUI creation features work
- ✅ **Cross-platform code** - Platform abstraction layer implemented
- ✅ **X11 window support** - Full X11 window creation and event handling
- ✅ **X11 rendering backend** - Complete rendering implementation with Xlib
- ✅ **Demo applications** - Linux demos available (demo_linux, widget_demo_linux)

## Features Available on Linux

### Fully Functional
- **Neural Network System**: Complete natural language GUI parsing
  - Intent classification
  - Entity extraction
  - Vocabulary processing (2100+ words)
  - Callback generation
  - All features identical to Windows

- **Widget System**: Platform-independent widget definitions
  - Button, Label, TextBox, CheckBox
  - ProgressBar, Tooltip, Slider
  - ComboBox, ListBox, TabControl
  - All widget classes compile and instantiate

- **Theme System**: Theme definitions and color management
  - Theme creation
  - Color schemes
  - Gradient definitions

### Fully Functional on Linux
- **X11 Window Management**: Complete window system
  - X11 window creation and destruction
  - Window positioning and sizing
  - Window show/hide operations
  - Event loop and message processing

- **X11 Rendering Backend**: Full rendering capabilities
  - Basic shapes (rectangles, lines, ellipses)
  - Rounded rectangles with anti-aliasing
  - Linear gradients
  - Text rendering with fonts
  - Color management with alpha blending
  - Double-buffered rendering for smooth updates

- **Event Handling**: Complete event system
  - Mouse movement tracking
  - Mouse button events
  - Keyboard input
  - Window close events
  - Expose/repaint events

### Partially Implemented
- **Advanced Effects**: Limited on X11
  - Basic shadows (simplified)
  - Gradients (linear only, radial simplified)
  - No GPU-accelerated effects (blur, bloom, etc.)

### Not Yet Available
- **Layered Windows**: X11 doesn't support Windows-style layered windows
  - Compositing requires different approach

- **Window Hooking**: Linux doesn't support inline hooking like Windows
  - Alternative approaches needed (LD_PRELOAD, etc.)

## Building on Linux

### Prerequisites

**Ubuntu/Debian:**
```bash
sudo apt-get update
sudo apt-get install build-essential cmake libx11-dev
```

**Fedora/RHEL:**
```bash
sudo dnf install gcc-c++ cmake libX11-devel
```

**Arch Linux:**
```bash
sudo pacman -S base-devel cmake libx11
```

### Build Instructions

```bash
# Clone the repository
git clone https://github.com/kamer1337/5d-gui-framework.git
cd 5d-gui-framework

# Create build directory
mkdir build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build the SDK
make -j$(nproc)

# The output will be lib5DGUI_SDK.a
```

### Expected Output

After successful build:
```
build/
├── lib5DGUI_SDK.a           # Static library
├── 5DGUI_Demo_Linux         # Basic rendering demo
├── 5DGUI_WidgetDemo_Linux   # Widget system demo
└── CMakeFiles/              # Build artifacts
```

## Running Demo Applications

### Linux X11 Demos

The SDK includes two demo applications for Linux:

**Basic Rendering Demo:**
```bash
cd build
./5DGUI_Demo_Linux
```

This demo showcases:
- X11 window creation
- Basic shape rendering (rectangles, rounded rectangles, ellipses, lines)
- Linear gradients
- Text rendering
- Event handling

**Widget System Demo:**
```bash
cd build
./5DGUI_WidgetDemo_Linux
```

This demo showcases:
- Progress bars with gradient fills
- Interactive buttons
- Checkboxes with state management
- Slider controls
- Click event handling
- Real-time animation

### Screenshots

**Basic Demo:**
![Linux X11 Demo](https://github.com/user-attachments/assets/49167026-46e9-430e-b5eb-5478f5f24b58)
*X11 backend demonstrating basic shape rendering, rounded corners, gradients, and text*

**Widget Demo:**
![Linux Widget Demo](https://github.com/user-attachments/assets/a4e519d3-3445-45ad-a41d-989d5a45c5e7)
*Interactive widget system with progress bars, buttons, checkboxes, and sliders*

## Using the SDK on Linux

### X11 Window Creation (Fully Functional)

```cpp
#include "SDK/WindowX11.h"
#include "SDK/X11WindowManager.h"

int main() {
    // Create window
    auto window = SDK::X11WindowManager::GetInstance().CreateWindow(
        L"My Window",
        100, 100,  // Position
        800, 600   // Size
    );
    
    if (!window) {
        return 1;
    }
    
    // Set up callbacks
    window->SetPaintCallback([]() {
        // Paint window content
    });
    
    window->SetCloseCallback([]() {
        SDK::X11WindowManager::GetInstance().Quit();
    });
    
    // Show window
    window->Show();
    
    // Run event loop
    SDK::X11WindowManager::GetInstance().RunEventLoop();
    
    return 0;
}
```

### X11 Rendering (Fully Functional)

```cpp
#include "SDK/WindowX11.h"
#include "SDK/X11RenderBackend.h"

void OnPaint() {
    auto renderBackend = window->GetRenderBackend();
    
    window->BeginPaint();
    
    // Clear background
    renderBackend->Clear(SDK::Color(240, 240, 245, 255));
    
    // Draw rectangle
    RECT rect = {50, 50, 200, 150};
    renderBackend->DrawRectangle(rect,
        SDK::Color(100, 149, 237, 255),  // Fill
        SDK::Color(65, 105, 225, 255),   // Border
        2.0f);
    
    // Draw text
    RECT textRect = {50, 160, 200, 180};
    renderBackend->DrawText(L"Hello X11!", textRect,
        SDK::Color(50, 50, 50, 255), L"", 14, 400);
    
    window->EndPaint();
}
```

### Neural Network Features (Fully Functional)

```cpp
#include "SDK/NeuralNetwork.h"
#include "SDK/NeuralPromptBuilder.h"

int main() {
    // Create neural network
    auto nn = std::make_shared<SDK::NeuralNetwork>();
    nn->Initialize();
    
    // Parse natural language prompts
    auto parsed = nn->ParsePrompt(L"Create window 800x600 with button and textbox");
    
    // Check results
    if (parsed.confidence > 0.8f) {
        int width = parsed.GetWidth();        // 800
        int height = parsed.GetHeight();      // 600
        auto intent = parsed.intent;          // CREATE_WINDOW
    }
    
    return 0;
}
```

### Widget Definitions (Platform Independent)

```cpp
#include "SDK/Widget.h"
#include "SDK/AdvancedWidgets.h"

int main() {
    // Create widgets (definitions work cross-platform)
    auto button = std::make_shared<SDK::Button>(L"Click Me");
    button->SetPosition(50, 50);
    button->SetSize(120, 40);
    
    auto textBox = std::make_shared<SDK::TextBox>();
    textBox->SetPosition(50, 100);
    textBox->SetSize(200, 30);
    
    // Widget properties are set, but rendering requires platform implementation
    return 0;
}
```

### Compiling Your Application

```bash
# Compile with the SDK
g++ -std=c++17 your_app.cpp -I/path/to/5d-gui-framework/include \
    -L/path/to/5d-gui-framework/build -l5DGUI_SDK \
    -lX11 -lpthread -o your_app
```

## Platform Differences

### Windows vs Linux

| Feature | Windows | Linux |
|---------|---------|-------|
| Neural Network | ✅ Full | ✅ Full |
| Widget Definitions | ✅ Full | ✅ Full |
| Theme System | ✅ Full | ✅ Full |
| Window Creation | ✅ Full | ✅ Full (X11) |
| Window Rendering | ✅ Full | ✅ Full (X11) |
| Basic Shapes | ✅ Full | ✅ Full |
| Gradients | ✅ Full | ✅ Linear, ⚠️ Radial (simplified) |
| Text Rendering | ✅ Full | ✅ Full |
| Event Handling | ✅ Full | ✅ Full |
| Layered Windows | ✅ Full | ❌ Different approach needed |
| GPU Effects | ✅ Direct2D | ❌ Not available |
| Window Hooking | ✅ Full | ❌ Not applicable |
| Demo Applications | ✅ Full | ✅ Available (2 demos) |

### Type Compatibility

The Platform.h header provides type aliases for cross-platform compatibility:

```cpp
#ifdef SDK_PLATFORM_LINUX
    typedef void* HWND;           // Window handle
    typedef unsigned char BYTE;    // Byte type
    typedef unsigned long DWORD;   // Double word
    // ... and more
#endif
```

## Development Roadmap

### Phase 1: Foundation (✅ Complete)
- [x] Platform detection and abstraction
- [x] CMake build system for Linux
- [x] Type definitions and aliases
- [x] Neural network cross-platform support
- [x] Documentation

### Phase 2: X11 Integration (✅ Complete)
- [x] X11 window creation
- [x] Basic rendering with Xlib
- [x] Event handling (mouse, keyboard)
- [x] Window management (move, resize, close)
- [x] X11RenderBackend implementation
- [x] WindowX11 window wrapper
- [x] X11WindowManager for event loop

### Phase 3: Demo Applications (✅ Complete)
- [x] Demo applications for Linux
- [x] demo_linux.cpp - Basic rendering showcase
- [x] widget_demo_linux.cpp - Interactive widgets
- [x] Cross-platform build system

### Phase 4: Advanced Features (Future)
- [ ] Compositing and effects with X Composite extension
- [ ] Wayland support
- [ ] GTK integration (optional)
- [ ] Hardware acceleration with OpenGL
- [ ] GPU-accelerated effects

### Phase 5: Feature Parity (Long-term)
- [ ] All Windows features on Linux
- [ ] Performance optimization
- [ ] Comprehensive testing
- [ ] Full documentation

## X11 Backend Implementation

### Architecture

The Linux X11 backend consists of three main components:

**1. X11RenderBackend** (`include/SDK/X11RenderBackend.h`, `src/SDK/X11RenderBackend.cpp`)
- Implements the `RenderBackend` interface using Xlib
- Provides drawing primitives (rectangles, lines, ellipses, text)
- Supports gradients and basic effects
- Uses double-buffering for smooth rendering
- Manages font cache for text rendering

**2. WindowX11** (`include/SDK/WindowX11.h`, `src/SDK/WindowX11.cpp`)
- Wraps X11 window creation and management
- Handles window properties (title, position, size)
- Manages event processing for individual windows
- Provides callback system for events (paint, close, mouse, keyboard)

**3. X11WindowManager** (`include/SDK/WindowX11.h`, `src/SDK/WindowX11.cpp`)
- Manages multiple X11 windows
- Provides event loop implementation
- Handles window lifecycle
- Singleton pattern for easy access

### Key Features

**Double Buffering:**
All rendering is performed on an off-screen pixmap (back buffer) and then copied to the window, eliminating flicker.

**Event System:**
Complete event handling with callbacks:
- Paint events for rendering
- Mouse events (move, button press/release)
- Keyboard events (key press/release)
- Window close events

**Color Management:**
RGB color support with alpha blending. Alpha values are pre-blended with a white background due to X11 limitations.

**Font Rendering:**
Dynamic font loading with caching. Falls back to "fixed" font if specific fonts are unavailable.

### Rendering Capabilities

| Feature | Status | Notes |
|---------|--------|-------|
| Rectangles | ✅ Full | With border and fill colors |
| Rounded Rectangles | ✅ Full | Using X11 arcs for corners |
| Lines | ✅ Full | With thickness control |
| Ellipses/Circles | ✅ Full | With border and fill colors |
| Text | ✅ Full | UTF-8 support, font caching |
| Linear Gradients | ✅ Full | Horizontal and vertical |
| Radial Gradients | ⚠️ Simplified | Solid color fallback |
| Shadows | ⚠️ Basic | Offset rectangles |
| Blur/Bloom/DOF | ❌ Not available | Requires GPU or complex CPU work |

### Platform Differences

**Type Compatibility:**
The `Platform.h` header provides type aliases for cross-platform code:
- `HWND` → `void*` (casts to X11 Window)
- `HDC` → `void*` (casts to X11 Window for compatibility)
- `RECT`, `POINT`, `COLORREF` → Native structs/types

**Entry Point:**
Linux demos use standard `main()` instead of Windows' `WinMain()`:
```cpp
int main() {
    // Cross-platform code
}
```

## Technical Challenges

### Window Hooking
**Challenge**: Linux doesn't support inline hooking like Windows CreateWindowExW

**Solutions Being Considered**:
- LD_PRELOAD for library interposition
- Custom window manager integration
- Compositor plugins (for Wayland)
- Explicit window registration (no hooking)

### Layered Windows
**Challenge**: Linux/X11 doesn't have Windows' layered window API

**Solutions Being Considered**:
- Compositing with X Composite extension
- Direct ARGB window rendering
- OpenGL overlay windows
- Wayland subsurfaces

### DWM Effects
**Challenge**: Linux doesn't have Desktop Window Manager like Windows

**Solutions Being Considered**:
- Custom compositor effects
- Picom/Compton integration
- KWin effects (KDE)
- Compiz plugins

## Contributing

We welcome contributions to improve Linux support! Priority areas:

1. **X11 Backend Implementation**: Core window system integration
2. **Rendering System**: Basic drawing and painting
3. **Event Handling**: Mouse, keyboard, and window events
4. **Testing**: Cross-platform testing framework
5. **Documentation**: Linux-specific guides and examples

### How to Contribute

1. Check the [CONTRIBUTING.md](CONTRIBUTING.md) guide
2. Look for issues tagged "linux" or "cross-platform"
3. Discuss major changes in GitHub Discussions
4. Follow the existing code style and patterns
5. Test on multiple Linux distributions
6. Update documentation as needed

## Known Issues

1. **Build System**: Demo applications don't build on Linux (intentional for now)
2. **Type Definitions**: Some Windows-specific types are aliased but not fully implemented
3. **Header Dependencies**: Some headers still have Windows-specific includes

## FAQ

### Can I use the neural network on Linux?
**Yes!** The neural network is fully functional on Linux with identical features to Windows.

### Can I create windows on Linux?
**Partially.** You can define window properties and use the neural network to parse window specifications, but actual window creation requires X11/Wayland implementation (coming soon).

### Will Linux have feature parity with Windows?
**Eventually.** The goal is to achieve feature parity where platform limitations allow. Some Windows-specific features (like DWM effects) will have Linux equivalents using different technologies.

### How can I help with Linux support?
Check the Contributing section above and look for "linux" tagged issues on GitHub. We especially need help with X11/Wayland integration.

### Does this work with Wayland?
**Not yet.** Current focus is on X11 support. Wayland support is planned for a future release.

## Resources

- [Build Guide](BUILD.md) - Complete build instructions for all platforms
- [API Documentation](API.md) - Cross-platform API reference
- [Neural Network Guide](NEURAL_NETWORK_GUIDE.md) - AI features (fully cross-platform)
- [GitHub Issues](https://github.com/kamer1337/5d-gui-framework/issues) - Bug reports and feature requests

## Support

For Linux-specific questions:
1. Check this document first
2. Look for existing GitHub issues tagged "linux"
3. Create a new issue with [Linux] in the title
4. Join discussions in GitHub Discussions

---

**Last Updated**: December 2025  
**Next Review**: March 2026  
**X11 Integration**: ✅ Complete (v1.3.0)
