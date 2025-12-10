# 5D GUI SDK Quick Reference

Fast reference for common SDK operations.

## Initialization

```cpp
#include "SDK/SDK.h"

// Initialize at startup
SDK::Initialize();

// Cleanup at exit
SDK::Shutdown();
```

## Window Management

```cpp
// Register window
HWND hwnd = CreateWindowEx(...);
auto window = SDK::WindowManager::GetInstance().RegisterWindow(hwnd);

// Unregister window
SDK::WindowManager::GetInstance().UnregisterWindow(hwnd);

// Get window
auto window = SDK::WindowManager::GetInstance().GetWindow(hwnd);
```

## Window Properties

```cpp
// Set depth (0-4)
window->SetDepth(SDK::WindowDepth::FOREGROUND);

// Set alpha (0-255)
window->SetAlpha(200);

// Set scale (0.0-1.0+)
window->SetScale(0.9f);

// Set depth offset
window->SetDepthOffset(10, 6);

// Enable layered mode
window->EnableLayeredMode();

// Update appearance
window->UpdateAppearance();
```

## WindowDepth Enum

```cpp
WindowDepth::FAR_BACKGROUND  // 0: 70% scale, alpha 170
WindowDepth::BACKGROUND      // 1: 80% scale, alpha 200
WindowDepth::MIDDLE          // 2: 90% scale, alpha 230
WindowDepth::MID_FRONT       // 3: 95% scale, alpha 245
WindowDepth::FOREGROUND      // 4: 100% scale, alpha 255
```

## Theming

```cpp
// Create theme
auto theme = std::make_shared<SDK::Theme>("MyTheme");

// Apply predefined theme
auto theme = std::make_shared<SDK::Theme>(SDK::Theme::CreateModernTheme());
window->SetTheme(theme);

// Set default theme for all windows
SDK::WindowManager::GetInstance().SetDefaultTheme(theme);
```

## Theme Configuration

```cpp
// Title bar
SDK::Gradient gradient(SDK::GradientType::HORIZONTAL,
    SDK::Color(255, 0, 0, 255),    // Start
    SDK::Color(0, 0, 255, 255));   // End
theme->SetTitleBarGradient(gradient);
theme->SetTitleBarHeight(35);

// Colors
theme->SetBackgroundColor(SDK::Color(255, 255, 255, 255));
theme->SetBorderColor(SDK::Color(100, 100, 100, 255));
theme->SetBorderWidth(2);

// Corners
theme->SetCornerRadius(12);

// Shadow
theme->SetShadowColor(SDK::Color(0, 0, 0, 100));
theme->SetShadowOffset(5, 5);
theme->SetShadowBlur(10);

// Effects
theme->SetGlowEnabled(true);
theme->SetParticlesEnabled(true);
theme->SetGlassEffect(true);
```

## Color Structure

```cpp
// Create color
SDK::Color color(255, 128, 0, 255);  // R, G, B, A

// Predefined colors
SDK::Color red(255, 0, 0, 255);
SDK::Color green(0, 255, 0, 255);
SDK::Color blue(0, 0, 255, 255);
SDK::Color white(255, 255, 255, 255);
SDK::Color black(0, 0, 0, 255);

// Convert to COLORREF
COLORREF cr = color.ToCOLORREF();
```

## Gradient Types

```cpp
// Vertical gradient (top to bottom)
SDK::GradientType::VERTICAL

// Horizontal gradient (left to right)
SDK::GradientType::HORIZONTAL

// Radial gradient (center to edges)
SDK::GradientType::RADIAL
```

## Rendering

```cpp
// Custom rendering callback
window->SetRenderCallback([](HDC hdc) {
    RECT rect;
    GetClientRect(WindowFromDC(hdc), &rect);
    
    // Your rendering code
});

// In WM_PAINT handler
case WM_PAINT: {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps);
    
    auto window = SDK::WindowManager::GetInstance().GetWindow(hwnd);
    if (window) {
        window->Render(hdc);
    }
    
    EndPaint(hwnd, &ps);
    return 0;
}
```

## Renderer Functions

```cpp
// Gradients
SDK::Renderer::DrawVerticalGradient(hdc, rect, color1, color2);
SDK::Renderer::DrawHorizontalGradient(hdc, rect, color1, color2);
SDK::Renderer::DrawRadialGradient(hdc, rect, centerColor, edgeColor, cx, cy);

// Shapes
SDK::Renderer::DrawRoundedRect(hdc, rect, radius, fillColor, borderColor, borderWidth);

// Effects
SDK::Renderer::DrawShadow(hdc, rect, offsetX, offsetY, blur, shadowColor);
SDK::Renderer::DrawGlow(hdc, rect, radius, glowColor);

// Icons
SDK::Renderer::DrawIcon(hdc, iconType, x, y, size, color, alpha);
```

## Icon Types

```cpp
SDK::Renderer::IconType::CIRCLE
SDK::Renderer::IconType::SQUARE
SDK::Renderer::IconType::DIAMOND
SDK::Renderer::IconType::FOLDER
SDK::Renderer::IconType::STAR
```

## Particles

```cpp
// Create particle emission
auto particles = SDK::Renderer::CreateParticleEmission(x, y, count, color);

// Update particles (in game loop)
SDK::Renderer::UpdateParticles(particles, deltaTime);

// Render particles
SDK::Renderer::DrawParticles(hdc, particles);

// Particle structure
struct Particle {
    float x, y;      // Position
    float vx, vy;    // Velocity
    float life;      // Remaining lifetime
    Color color;     // Particle color
};
```

## Animation

```cpp
// Enable depth animation
SDK::WindowManager::GetInstance().EnableDepthAnimation(true);

// Update in game loop
float deltaTime = /* calculate */;
SDK::WindowManager::GetInstance().Update(deltaTime);
```

## Modal Windows

```cpp
// Set active modal
SDK::WindowManager::GetInstance().SetActiveModal(dialogHwnd);

// Get active modal
HWND modal = SDK::WindowManager::GetInstance().GetActiveModal();

// Clear modal
SDK::WindowManager::GetInstance().ClearActiveModal();
```

## Utility Functions

```cpp
// Color interpolation
SDK::Color result = SDK::Renderer::InterpolateColor(color1, color2, 0.5f);

// Alpha blending
BYTE blended = SDK::Renderer::BlendAlpha(src, dst, alpha);

// Memory DC
HBITMAP bitmap;
HDC memDC = SDK::Renderer::CreateMemoryDC(width, height, &bitmap);
// ... render to memDC ...
SDK::Renderer::DeleteMemoryDC(memDC, bitmap);
```

## Common Patterns

### Layered Windows
```cpp
for (int i = 0; i < 5; i++) {
    HWND hwnd = CreateWindowEx(WS_EX_LAYERED, ...);
    auto window = SDK::WindowManager::GetInstance().RegisterWindow(hwnd);
    window->SetDepth((SDK::WindowDepth)i);
    ShowWindow(hwnd, SW_SHOW);
}
```

### Particle Burst on Click
```cpp
case WM_LBUTTONDOWN: {
    int x = LOWORD(lParam);
    int y = HIWORD(lParam);
    auto particles = SDK::Renderer::CreateParticleEmission(x, y, 30, color);
    // Store and update particles...
}
```

### Custom Theme
```cpp
SDK::Theme theme("Custom");
theme.SetTitleBarGradient(SDK::Gradient(
    SDK::GradientType::HORIZONTAL,
    SDK::Color(138, 43, 226, 255),
    SDK::Color(219, 112, 147, 255)
));
theme.SetBackgroundColor(SDK::Color(250, 250, 250, 255));
theme.SetCornerRadius(16);
window->SetTheme(std::make_shared<SDK::Theme>(theme));
```

## Build Commands

### CMake
```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .
```

### Makefile
```bash
mingw32-make all    # Build everything
mingw32-make sdk    # SDK only
mingw32-make demo   # Demo only
mingw32-make clean  # Clean
```

## Documentation Files

- **README.md**: Overview and quick start
- **API.md**: Complete API reference
- **USAGE.md**: Practical examples and patterns
- **ARCHITECTURE.md**: Technical design
- **BUILD.md**: Building instructions
- **CONTRIBUTING.md**: Contribution guidelines
- **ROADMAP.md**: Future plans
- **QUICKREF.md**: This file

## SDK Version

```cpp
const char* version = SDK::GetVersion();  // "5D GUI SDK v1.2.0"
bool initialized = SDK::IsInitialized();
```

## Error Handling

```cpp
// Check return values
if (!SDK::Initialize()) {
    // Handle initialization error
}

auto window = SDK::WindowManager::GetInstance().RegisterWindow(hwnd);
if (!window) {
    // Handle registration error
}

// Check window validity
if (!window->IsValid()) {
    // Window is invalid
}
```

## Performance Tips

1. **Reuse theme objects** - Don't create new themes per window
2. **Limit particles** - More than 1000 can impact performance
3. **Batch rendering** - Use memory DC for complex scenes
4. **Disable effects** when not needed - Turn off glow/particles
5. **Update only when needed** - Don't invalidate unnecessarily

## Common Issues

**Window not enhanced?**
- Call `RegisterWindow()` after creation
- Ensure `WS_EX_LAYERED` style is set
- Call `EnableLayeredMode()`

**Rendering not showing?**
- Handle `WM_PAINT` message
- Call `window->Render(hdc)` in paint handler
- Check `SetRenderCallback()` is set

**Performance issues?**
- Reduce particle count
- Disable animation
- Check for excessive `InvalidateRect()` calls
- Profile with Windows Performance Analyzer

---

**Quick Links**:
- GitHub: https://github.com/kamer1337/MaterialGameEngine
- Issues: Report bugs and request features
- Discussions: Ask questions and share ideas
