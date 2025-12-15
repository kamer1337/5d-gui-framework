# 5D GUI SDK - Demo Applications Guide

This guide provides an overview of all demo applications included with the 5D GUI SDK, explaining what each demonstrates and how to run them.

## Overview

The SDK includes **18 Windows demo applications** and **2 Linux demo applications** that showcase various features and capabilities. Each demo is designed to highlight specific aspects of the SDK.

## Windows Demo Applications

### Core Demonstrations

#### 1. 5DGUI_Demo (`demo.cpp`)
**Main 5D Rendering Demo**

Demonstrates:
- 5-level depth system (FAR_BACKGROUND to FOREGROUND)
- Layered window rendering with alpha transparency
- Multi-directional gradients (vertical, horizontal, radial)
- Rounded corners with anti-aliasing
- Depth-aware shadow rendering
- Particle system with physics
- Glow effects
- Icon system
- Interactive buttons for each effect

**How to Run:**
```cmd
build\bin\5DGUI_Demo.exe
```

**Key Features:**
- Click buttons to toggle different visual effects
- Watch windows render at different depth levels
- See particle effects in action
- Experience the "book stack" layering effect

---

#### 2. Widget_Demo (`widget_demo.cpp`)
**Widget System Demo**

Demonstrates:
- Basic widgets (Button, Label, TextBox, CheckBox)
- Container widgets (Panel)
- Advanced widgets (ComboBox, ListBox, Slider)
- Widget event handling
- Theme application to widgets

**How to Run:**
```cmd
build\bin\5DGUI_WidgetDemo.exe
```

**Key Features:**
- Interactive buttons with hover/press states
- Text input with custom rendering
- Checkbox with checked/unchecked states
- Dropdown menus and list boxes
- Slider controls

---

#### 3. Widget_Showcase (`widget_showcase.cpp`)
**Complete Widget Showcase**

Demonstrates:
- All 15+ built-in widgets
- Widget properties (padding, margin, opacity)
- Rich text rendering
- Menu systems
- Layout managers
- File browsers

**How to Run:**
```cmd
build\bin\Widget_Showcase.exe
```

**Key Features:**
- Comprehensive display of all widgets
- Property customization examples
- Interactive widget gallery
- Real-world widget usage patterns

---

### Advanced Feature Demonstrations

#### 4. Camera_Demo (`camera_demo.cpp`)
**3D Camera Controller Demo**

Demonstrates:
- 3D camera controls (orbit, pan, zoom)
- Mouse and keyboard navigation
- 3D widget placement in space
- Billboard mode for UI widgets
- Ray casting for 3D mouse picking
- Depth sorting for proper rendering

**How to Run:**
```cmd
build\bin\Camera_Demo.exe
```

**Key Features:**
- Left mouse drag: Rotate camera
- Right mouse drag: Pan camera
- Mouse wheel: Zoom in/out
- Interactive 3D UI widgets in space

---

#### 5. Advanced_Window_Demo (`advanced_window_demo.cpp`)
**Advanced Window Features Demo**

Demonstrates:
- Window groups (linked movement)
- Window snapping (edge and grid)
- Window animations (minimize/maximize/restore)
- Multiple animation types and easing functions
- Synchronized window transformations

**How to Run:**
```cmd
build\bin\Advanced_Window_Demo.exe
```

**Key Features:**
- Create and manage window groups
- Drag windows to see snapping behavior
- Animate window transitions
- Test different easing functions

---

#### 6. Toolbar_Demo (`toolbar_demo.cpp`)
**Toolbar Widget Demo**

Demonstrates:
- Horizontal and vertical toolbars
- Toolbar items with icons
- Auto-hide functionality
- Customizable toolbar appearance
- Toolbar event handling

**How to Run:**
```cmd
build\bin\Toolbar_Demo.exe
```

**Key Features:**
- Add/remove toolbar items
- Toggle auto-hide mode
- Switch between horizontal/vertical orientation
- Custom toolbar styling

---

### Neural Network & AI Features

#### 7. Neural_Network_Demo (`neural_network_demo.cpp`)
**Neural Network GUI Creation Demo**

Demonstrates:
- Natural language window generation
- AI-powered widget creation
- Prompt-based UI design
- Dynamic layout generation

**How to Run:**
```cmd
build\bin\Neural_Network_Demo.exe
```

**Key Features:**
- Type natural language prompts to create windows
- Generate buttons, text boxes, and other widgets from descriptions
- Automatic layout and positioning
- Real-time UI generation

**Example Prompts:**
- "Create a window 800x600 called 'My App' with a button and textbox"
- "Create dialog 500x400 'Settings' with checkbox 'Enable notifications' and button 'Apply'"

---

#### 8. Neural_Network_Advanced_Demo (`neural_network_advanced_demo.cpp`)
**Advanced Neural Network Demo**

Demonstrates:
- Advanced natural language understanding
- Complex widget hierarchies
- Multi-window generation
- Callback generation from descriptions

**How to Run:**
```cmd
build\bin\Neural_Network_Advanced_Demo.exe
```

**Key Features:**
- More sophisticated prompt parsing
- Complex UI layouts from single prompt
- Context-aware widget suggestions
- Dynamic callback generation

---

### Performance & Optimization

#### 9. Optimization_Demo (`optimization_demo.cpp`)
**Rendering Optimization Demo**

Demonstrates:
- Dirty rectangle tracking
- Occlusion culling
- Render caching
- Multi-threaded particle updates
- Performance metrics display

**How to Run:**
```cmd
build\bin\Optimization_Demo.exe
```

**Key Features:**
- Toggle optimization features on/off
- See performance impact in real-time
- Compare frame rates with/without optimizations
- Monitor render times

---

#### 10. ML_Optimizer_Demo (`ml_optimizer_demo.cpp`)
**Machine Learning Renderer Optimization Demo**

Demonstrates:
- ML-based renderer optimization
- Adaptive rendering strategies
- Intelligent LOD (Level of Detail) management
- Smart caching decisions
- Online learning from performance metrics

**How to Run:**
```cmd
build\bin\ML_Optimizer_Demo.exe
```

**Key Features:**
- Watch ML adapt to rendering patterns
- See performance predictions
- Monitor optimization decisions
- Compare ML vs. traditional optimization

---

#### 11. Hardware_Accel_Demo (`hardware_accel_demo.cpp`)
**Hardware Acceleration Demo**

Demonstrates:
- Direct2D GPU acceleration
- GDI software rendering fallback
- Automatic backend selection
- Performance comparison
- GPU-accelerated effects (blur, bloom, shadows)

**How to Run:**
```cmd
build\bin\Hardware_Accel_Demo.exe
```

**Key Features:**
- Switch between GDI and Direct2D backends
- Compare performance metrics
- Test GPU-accelerated effects
- View backend capabilities

---

### Layout & UI Components

#### 12. Layout_Engine_Demo (`layout_engine_demo.cpp`)
**Advanced Layout Engine Demo**

Demonstrates:
- Grid layout manager
- Flow layout manager
- Stack layout manager
- Automatic layout engine with heuristics
- Constraint-based positioning
- Dynamic layout adjustment

**How to Run:**
```cmd
build\bin\Layout_Engine_Demo.exe
```

**Key Features:**
- Compare different layout algorithms
- See automatic widget arrangement
- Test responsive layouts
- Constraint solving visualization

---

#### 13. New_Widgets_Demo (`new_widgets_demo.cpp`)
**New Widgets Demo (v1.2+)**

Demonstrates:
- RadioButton with group support
- SpinBox for numeric input
- Panel widget for grouping
- Advanced ComboBox features
- FileTree widget
- ListView with checkboxes

**How to Run:**
```cmd
build\bin\New_Widgets_Demo.exe
```

**Key Features:**
- Test all v1.2+ widgets
- Interactive examples
- Property customization
- Real-world usage scenarios

---

#### 14. Widget_Properties_Demo (`widget_properties_demo.cpp`)
**Widget Properties Demo**

Demonstrates:
- Padding and margin settings
- Min/max size constraints
- Opacity and transparency
- Border radius
- Font styling
- Tooltips
- Z-index layering
- Custom cursors

**How to Run:**
```cmd
build\bin\Widget_Properties_Demo.exe
```

**Key Features:**
- Adjust widget properties in real-time
- See immediate visual feedback
- Test property combinations
- Learn property API usage

---

#### 15. Widget_Enhancements_Demo (`widget_enhancements_demo.cpp`)
**Widget Enhancements Demo**

Demonstrates:
- Enhanced widget features
- Advanced styling options
- Custom widget behaviors
- Widget composition
- Complex widget interactions

**How to Run:**
```cmd
build\bin\Widget_Enhancements_Demo.exe
```

**Key Features:**
- Test enhanced widget capabilities
- Custom widget examples
- Advanced interaction patterns
- Composition examples

---

### Rich Text & Data Management

#### 16. RichText_DataGrid_Demo (`richtext_datagrid_demo.cpp`)
**Rich Text and Data Grid Demo**

Demonstrates:
- RichTextBox with formatting
- Text styling (bold, italic, underline)
- Color and font customization
- Markdown rendering
- DataGrid with sortable columns
- Filtering capabilities
- Cell editing
- Virtual scrolling

**How to Run:**
```cmd
build\bin\RichText_DataGrid_Demo.exe
```

**Key Features:**
- Rich text editing with toolbar
- Markdown preview
- Sortable data grid
- Filter and search
- Edit cells inline

---

### Version-Specific Showcases

#### 17. V13_Showcase_Demo (`v13_showcase_demo.cpp`)
**Version 1.3 Features Showcase**

Demonstrates:
- Advanced visual effects (depth-of-field, motion blur, chromatic aberration)
- Effect preset system (CINEMATIC, GAME_UI, RETRO, DREAMY, MOTION)
- Enhanced animation system with Bezier curves
- Animation pause/resume/reverse controls
- Menu system (MenuBar, ContextMenu)
- Animation grouping with sequence and parallel modes

**How to Run:**
```cmd
build\bin\V13_Showcase_Demo.exe
```

**Key Features:**
- Toggle between effect presets
- Control animation playback
- Test new v1.3 features
- Experience advanced visual effects

---

#### 18. DPI_MultiMonitor_Demo (`dpi_multimonitor_demo.cpp`)
**DPI and Multi-Monitor Demo (v2.0)**

Demonstrates:
- Per-monitor DPI awareness
- Automatic DPI scaling
- DPI change detection
- Multi-monitor window management
- Monitor enumeration
- Monitor-specific themes
- Window migration between monitors

**How to Run:**
```cmd
build\bin\DPI_MultiMonitor_Demo.exe
```

**Key Features:**
- Move window between monitors to see DPI scaling
- Test different DPI settings
- Monitor information display
- Per-monitor theme support

---

## Linux Demo Applications

### 1. 5DGUI_Demo_Linux (`demo_linux.cpp`)
**Linux X11 Rendering Demo**

Demonstrates:
- X11 window creation
- Basic rendering with X11
- Cross-platform SDK core
- Neural network features (platform-independent)

**How to Run:**
```bash
./build/5DGUI_Demo_Linux
```

**Requirements:**
- X11 libraries (`libx11-dev`)
- Linux with X11 display server

---

### 2. 5DGUI_WidgetDemo_Linux (`widget_demo_linux.cpp`)
**Linux Widget Demo**

Demonstrates:
- Platform-independent widget system
- Neural network GUI generation
- Cross-platform AI features

**How to Run:**
```bash
./build/5DGUI_WidgetDemo_Linux
```

---

## Building the Demos

### Windows

```cmd
# Using CMake with Visual Studio
mkdir build
cd build
cmake .. -G "Visual Studio 16 2019" -A x64
cmake --build . --config Release

# Using CMake with MinGW
mkdir build
cd build
cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
cmake --build .
```

### Linux

```bash
# Install dependencies
sudo apt-get install build-essential cmake libx11-dev

# Build
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

---

## Demo Categories

### By Feature Area

**Core Rendering:**
- 5DGUI_Demo
- Hardware_Accel_Demo
- Optimization_Demo
- ML_Optimizer_Demo

**Widget System:**
- Widget_Demo
- Widget_Showcase
- New_Widgets_Demo
- Widget_Properties_Demo
- Widget_Enhancements_Demo

**Advanced Features:**
- Camera_Demo
- Advanced_Window_Demo
- Toolbar_Demo
- Layout_Engine_Demo

**AI & Neural Networks:**
- Neural_Network_Demo
- Neural_Network_Advanced_Demo

**Rich Content:**
- RichText_DataGrid_Demo

**Version Showcases:**
- V13_Showcase_Demo
- DPI_MultiMonitor_Demo

**Linux:**
- 5DGUI_Demo_Linux
- 5DGUI_WidgetDemo_Linux

---

## Troubleshooting

### Windows

**Demo won't start:**
- Ensure Windows 7 or later
- Install Visual C++ Redistributable if needed
- Check that DWM (Aero) is enabled

**Visual effects not working:**
- Update graphics drivers
- Verify hardware acceleration is available
- Try running as administrator

### Linux

**X11 errors:**
- Ensure X11 display server is running
- Check `$DISPLAY` environment variable
- Verify X11 libraries are installed

**Build failures:**
- Install all dependencies
- Check compiler version (GCC 7+ or Clang 5+)
- Verify CMake version (3.10+)

---

## Learning Path

Recommended order for exploring demos:

1. **Start Here:** `5DGUI_Demo` - Learn core rendering concepts
2. **Basic Widgets:** `Widget_Demo` - Understand widget basics
3. **Comprehensive Overview:** `Widget_Showcase` - See all widgets
4. **AI Features:** `Neural_Network_Demo` - Natural language UI creation
5. **Advanced Widgets:** `New_Widgets_Demo` - Explore v1.2+ widgets
6. **Layout Systems:** `Layout_Engine_Demo` - Learn automatic layouts
7. **Visual Effects:** `V13_Showcase_Demo` - Experience advanced effects
8. **3D Features:** `Camera_Demo` - 3D camera and widgets
9. **Performance:** `ML_Optimizer_Demo` - ML-based optimization
10. **Modern Display:** `DPI_MultiMonitor_Demo` - Multi-monitor and HiDPI

---

## Source Code

All demo source files are located in the `examples/` directory. Each demo is well-commented and can serve as a template for your own applications.

**Example Structure:**
```cpp
#include "../include/SDK/SDK.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    // Initialize SDK
    SDK::Initialize();
    
    // Create and configure window
    // Add widgets
    // Setup event handlers
    
    // Message loop
    
    // Cleanup
    SDK::Shutdown();
    return 0;
}
```

---

## Additional Resources

- **[README.md](README.md)** - Project overview and quick start
- **[API.md](API.md)** - Complete API documentation
- **[BUILD.md](BUILD.md)** - Detailed build instructions
- **[USAGE.md](USAGE.md)** - Practical usage examples
- **[WIDGET_GUIDE.md](WIDGET_GUIDE.md)** - Widget system guide
- **[NEURAL_NETWORK_GUIDE.md](NEURAL_NETWORK_GUIDE.md)** - AI features guide
- **[OPTIMIZATION_GUIDE.md](OPTIMIZATION_GUIDE.md)** - Performance optimization
- **[TODO.md](TODO.md)** - Active tasks and roadmap

---

## Contributing

Found a bug in a demo or want to add a new one? See [CONTRIBUTING.md](CONTRIBUTING.md) for guidelines.

---

**Last Updated:** December 2025  
**SDK Version:** 2.0.0
