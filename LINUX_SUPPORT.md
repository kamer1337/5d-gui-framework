# Linux Support Guide

This document provides information about Linux support in the 5D GUI SDK.

## Current Status

The 5D GUI SDK now includes **foundation-level Linux support** as of version 1.2.1. This means:

- ✅ **SDK compiles on Linux** - The core SDK library builds successfully
- ✅ **Neural network fully functional** - All AI-powered GUI creation features work
- ✅ **Cross-platform code** - Platform abstraction layer implemented
- ⚠️ **Basic window support** - Window system integration is in foundation stage
- ❌ **Demo applications** - Windows-only for now

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

### Partially Implemented
- **Window Management**: Basic window type definitions
  - Window handle abstraction (HWND type alias)
  - Window depth enumeration
  - Window properties

### Not Yet Available
- **Window Rendering**: Platform-specific rendering code needed
  - X11 window creation
  - Drawing and painting
  - Event handling
  - Visual effects

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
├── lib5DGUI_SDK.a      # Static library
└── CMakeFiles/         # Build artifacts
```

## Using the SDK on Linux

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
| Window Creation | ✅ Full | ⚠️ Basic |
| Window Rendering | ✅ Full | ❌ Planned |
| Layered Windows | ✅ Full | ❌ Different approach needed |
| Window Hooking | ✅ Full | ❌ Not applicable |
| Demo Applications | ✅ Full | ❌ Planned |

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

### Phase 2: X11 Integration (Planned)
- [ ] X11 window creation
- [ ] Basic rendering with Xlib
- [ ] Event handling (mouse, keyboard)
- [ ] Window management (move, resize, close)

### Phase 3: Advanced Features (Future)
- [ ] Compositing and effects
- [ ] Wayland support
- [ ] GTK integration (optional)
- [ ] Hardware acceleration with OpenGL
- [ ] Demo applications for Linux

### Phase 4: Feature Parity (Long-term)
- [ ] All Windows features on Linux
- [ ] Performance optimization
- [ ] Comprehensive testing
- [ ] Full documentation

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
**Target for X11 Integration**: Q2 2026
