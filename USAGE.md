# 5D GUI SDK Usage Guide

This guide provides practical examples and patterns for using the 5D GUI SDK in your applications.

## Quick Start

### Minimal Example

```cpp
#include "SDK/SDK.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    // 1. Initialize SDK
    SDK::Initialize();
    
    // 2. Create your window normally
    HWND hwnd = CreateWindowEx(
        WS_EX_LAYERED,
        L"MyClass", L"My App",
        WS_OVERLAPPEDWINDOW,
        100, 100, 800, 600,
        nullptr, nullptr, hInstance, nullptr
    );
    
    // 3. Register with SDK
    auto window = SDK::WindowManager::GetInstance().RegisterWindow(hwnd);
    
    // 4. Show window
    ShowWindow(hwnd, nCmdShow);
    
    // 5. Message loop
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    // 6. Cleanup
    SDK::Shutdown();
    return 0;
}
```

## Common Use Cases

### 1. Create a Modern Themed Window

```cpp
// Create window
HWND hwnd = CreateWindowEx(WS_EX_LAYERED, ...);

// Register and apply modern theme
auto window = SDK::WindowManager::GetInstance().RegisterWindow(hwnd);
auto theme = std::make_shared<SDK::Theme>(SDK::Theme::CreateModernTheme());
window->SetTheme(theme);
window->UpdateAppearance();
```

### 2. Create Layered Windows (Book Stack Effect)

```cpp
// Create 5 windows at different depths
for (int i = 0; i < 5; i++) {
    HWND layered = CreateWindowEx(
        WS_EX_LAYERED | WS_EX_TOPMOST,
        L"MyClass",
        (L"Layer " + std::to_wstring(i)).c_str(),
        WS_POPUP | WS_VISIBLE,
        100 + i * 20,
        100 + i * 20,
        400, 300,
        nullptr, nullptr, hInstance, nullptr
    );
    
    auto window = SDK::WindowManager::GetInstance().RegisterWindow(layered);
    window->SetDepth((SDK::WindowDepth)i);
    
    ShowWindow(layered, SW_SHOW);
}
```

### 3. Custom Rendering

```cpp
window->SetRenderCallback([](HDC hdc) {
    RECT rect;
    GetClientRect(WindowFromDC(hdc), &rect);
    
    // Draw custom gradient background
    RECT bgRect = rect;
    bgRect.top = 50;
    SDK::Renderer::DrawHorizontalGradient(hdc, bgRect,
        SDK::Color(138, 43, 226, 255),   // Purple
        SDK::Color(255, 20, 147, 255));  // Pink
    
    // Draw icons
    SDK::Renderer::DrawIcon(hdc, SDK::Renderer::IconType::STAR,
        100, 100, 40, SDK::Color(255, 215, 0, 255));
    
    // Draw text
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(255, 255, 255));
    DrawTextW(hdc, L"Custom Content", -1, &rect, DT_CENTER | DT_VCENTER);
});
```

### 4. Particle Effects

```cpp
// Global particle storage
std::vector<SDK::Renderer::Particle> g_particles;

// On mouse click - create particles
void OnMouseClick(int x, int y) {
    auto newParticles = SDK::Renderer::CreateParticleEmission(
        x, y, 30, SDK::Color(255, 215, 0, 255)
    );
    g_particles.insert(g_particles.end(), 
        newParticles.begin(), newParticles.end());
}

// In update loop
void Update(float deltaTime) {
    SDK::Renderer::UpdateParticles(g_particles, deltaTime);
}

// In render callback
void RenderParticles(HDC hdc) {
    SDK::Renderer::DrawParticles(hdc, g_particles);
}
```

### 5. Modal Dialogs

```cpp
// Show modal dialog
HWND dialog = CreateWindowEx(...);  // Create dialog window
auto dialogWindow = SDK::WindowManager::GetInstance().RegisterWindow(dialog);
dialogWindow->SetDepth(SDK::WindowDepth::FOREGROUND);

// Set as active modal
SDK::WindowManager::GetInstance().SetActiveModal(dialog);

// Disable parent window
EnableWindow(parentHwnd, FALSE);

// ... handle dialog ...

// When closing dialog
SDK::WindowManager::GetInstance().ClearActiveModal();
EnableWindow(parentHwnd, TRUE);
DestroyWindow(dialog);
```

### 6. Animated Window Transitions

```cpp
// Enable depth animation
SDK::WindowManager::GetInstance().EnableDepthAnimation(true);

// In your message loop
DWORD lastTime = GetTickCount();
while (true) {
    // Process messages...
    
    // Calculate delta time
    DWORD currentTime = GetTickCount();
    float deltaTime = (currentTime - lastTime) / 1000.0f;
    lastTime = currentTime;
    
    // Update animations
    SDK::WindowManager::GetInstance().Update(deltaTime);
    
    Sleep(16);  // ~60 FPS
}
```

### 7. Custom Theme Creation

```cpp
// Create custom theme
SDK::Theme customTheme("Ocean");

// Title bar with horizontal gradient
SDK::Gradient titleGradient(
    SDK::GradientType::HORIZONTAL,
    SDK::Color(0, 119, 190, 255),    // Ocean blue
    SDK::Color(0, 180, 216, 255)     // Cyan
);
customTheme.SetTitleBarGradient(titleGradient);
customTheme.SetTitleBarHeight(35);

// Window colors
customTheme.SetBackgroundColor(SDK::Color(240, 248, 255, 255));  // Alice blue
customTheme.SetBorderColor(SDK::Color(70, 130, 180, 255));       // Steel blue
customTheme.SetBorderWidth(2);

// Rounded corners
customTheme.SetCornerRadius(20);

// Shadow settings
customTheme.SetShadowColor(SDK::Color(0, 0, 0, 120));
customTheme.SetShadowOffset(6, 6);
customTheme.SetShadowBlur(12);

// Effects
customTheme.SetGlowEnabled(true);
customTheme.SetGlowColor(SDK::Color(0, 180, 216, 80));
customTheme.SetGlassEffect(true);

// Apply to window
window->SetTheme(std::make_shared<SDK::Theme>(customTheme));
```

### 8. Icon System with Hover Effects

```cpp
struct IconButton {
    int x, y, size;
    SDK::Renderer::IconType type;
    SDK::Color color;
    bool hovered;
};

std::vector<IconButton> icons;

// Initialize icons at different depths
void InitIcons() {
    icons = {
        {50, 100, 30, SDK::Renderer::IconType::CIRCLE, SDK::Color(255, 0, 0, 255), false},
        {130, 100, 36, SDK::Renderer::IconType::SQUARE, SDK::Color(0, 255, 0, 255), false},
        {210, 100, 42, SDK::Renderer::IconType::DIAMOND, SDK::Color(0, 0, 255, 255), false},
        {290, 100, 48, SDK::Renderer::IconType::FOLDER, SDK::Color(255, 255, 0, 255), false},
        {370, 100, 54, SDK::Renderer::IconType::STAR, SDK::Color(255, 0, 255, 255), false}
    };
}

// Handle mouse move
void OnMouseMove(int mx, int my) {
    for (auto& icon : icons) {
        int dx = mx - icon.x;
        int dy = my - icon.y;
        int dist = (int)sqrt(dx*dx + dy*dy);
        icon.hovered = (dist < icon.size);
    }
}

// Render icons with hover effect
void RenderIcons(HDC hdc) {
    for (const auto& icon : icons) {
        float scale = icon.hovered ? 1.2f : 1.0f;
        float alpha = icon.hovered ? 1.0f : 0.8f;
        
        SDK::Renderer::DrawIcon(hdc, icon.type, 
            icon.x, icon.y, (int)(icon.size * scale), 
            icon.color, alpha);
        
        // Draw glow on hover
        if (icon.hovered) {
            RECT glowRect = {
                icon.x - icon.size, icon.y - icon.size,
                icon.x + icon.size, icon.y + icon.size
            };
            SDK::Renderer::DrawGlow(hdc, glowRect, 10, 
                SDK::Color(icon.color.r, icon.color.g, icon.color.b, 50));
        }
    }
}
```

### 9. Multi-Window Application

```cpp
class Application {
    std::vector<HWND> windows;
    
public:
    void CreateMainWindow() {
        HWND main = CreateWindowEx(...);
        auto window = SDK::WindowManager::GetInstance().RegisterWindow(main);
        window->SetDepth(SDK::WindowDepth::FOREGROUND);
        
        auto theme = std::make_shared<SDK::Theme>(
            SDK::Theme::CreateModernTheme()
        );
        window->SetTheme(theme);
        
        windows.push_back(main);
    }
    
    void CreateToolWindow() {
        HWND tool = CreateWindowEx(
            WS_EX_LAYERED | WS_EX_TOOLWINDOW,
            ...
        );
        
        auto window = SDK::WindowManager::GetInstance().RegisterWindow(tool);
        window->SetDepth(SDK::WindowDepth::MID_FRONT);
        window->SetAlpha(240);
        
        windows.push_back(tool);
    }
    
    void ArrangeWindows() {
        // Automatic depth sorting
        SDK::WindowManager::GetInstance().UpdateWindowDepths();
        
        // Or manual arrangement
        int depth = 0;
        for (auto hwnd : windows) {
            auto window = SDK::WindowManager::GetInstance().GetWindow(hwnd);
            if (window) {
                window->SetDepth((SDK::WindowDepth)(depth++ % 5));
            }
        }
    }
};
```

## Advanced Techniques

### Double-Buffered Rendering

```cpp
void RenderWithDoubleBuffer(HWND hwnd) {
    HDC screenDC = GetDC(hwnd);
    
    RECT rect;
    GetClientRect(hwnd, &rect);
    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;
    
    // Create memory DC
    HBITMAP bitmap;
    HDC memDC = SDK::Renderer::CreateMemoryDC(width, height, &bitmap);
    
    // Render to memory DC
    auto window = SDK::WindowManager::GetInstance().GetWindow(hwnd);
    if (window) {
        window->Render(memDC);
    }
    
    // Blit to screen
    BitBlt(screenDC, 0, 0, width, height, memDC, 0, 0, SRCCOPY);
    
    // Cleanup
    SDK::Renderer::DeleteMemoryDC(memDC, bitmap);
    ReleaseDC(hwnd, screenDC);
}
```

### Custom Gradient Patterns

```cpp
// Rainbow gradient
void DrawRainbow(HDC hdc, const RECT& rect) {
    int width = rect.right - rect.left;
    int segmentWidth = width / 6;
    
    SDK::Color colors[] = {
        SDK::Color(255, 0, 0, 255),     // Red
        SDK::Color(255, 127, 0, 255),   // Orange
        SDK::Color(255, 255, 0, 255),   // Yellow
        SDK::Color(0, 255, 0, 255),     // Green
        SDK::Color(0, 0, 255, 255),     // Blue
        SDK::Color(75, 0, 130, 255),    // Indigo
        SDK::Color(148, 0, 211, 255)    // Violet
    };
    
    for (int i = 0; i < 6; i++) {
        RECT segment = rect;
        segment.left = rect.left + i * segmentWidth;
        segment.right = segment.left + segmentWidth;
        
        SDK::Renderer::DrawHorizontalGradient(hdc, segment,
            colors[i], colors[i + 1]);
    }
}
```

### Performance Monitoring

```cpp
class PerformanceMonitor {
    DWORD lastFrame;
    float fps;
    
public:
    PerformanceMonitor() : lastFrame(GetTickCount()), fps(0.0f) {}
    
    void Update() {
        DWORD current = GetTickCount();
        float delta = (current - lastFrame) / 1000.0f;
        lastFrame = current;
        
        fps = 1.0f / delta;
    }
    
    void Render(HDC hdc, int x, int y) {
        wchar_t buffer[64];
        swprintf_s(buffer, L"FPS: %.1f", fps);
        
        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, RGB(255, 255, 255));
        TextOutW(hdc, x, y, buffer, wcslen(buffer));
    }
};
```

## Best Practices

### 1. Window Lifecycle

```cpp
// Good: Register window immediately after creation
HWND hwnd = CreateWindowEx(...);
auto window = SDK::WindowManager::GetInstance().RegisterWindow(hwnd);

// Configure before showing
window->SetTheme(theme);
window->SetDepth(depth);

// Show window
ShowWindow(hwnd, SW_SHOW);

// Before destroying
SDK::WindowManager::GetInstance().UnregisterWindow(hwnd);
DestroyWindow(hwnd);
```

### 2. Theme Management

```cpp
// Good: Reuse theme objects
auto modernTheme = std::make_shared<SDK::Theme>(
    SDK::Theme::CreateModernTheme()
);

// Apply to multiple windows
window1->SetTheme(modernTheme);
window2->SetTheme(modernTheme);
window3->SetTheme(modernTheme);

// Or set as default
SDK::WindowManager::GetInstance().SetDefaultTheme(modernTheme);
```

### 3. Render Optimization

```cpp
// Good: Only render when needed
case WM_PAINT: {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps);
    
    // Only render if window is visible
    if (IsWindowVisible(hwnd)) {
        auto window = SDK::WindowManager::GetInstance().GetWindow(hwnd);
        if (window) {
            window->Render(hdc);
        }
    }
    
    EndPaint(hwnd, &ps);
    return 0;
}

// Don't call InvalidateRect() unnecessarily
```

### 4. Memory Management

```cpp
// Good: Clean up resources
std::vector<SDK::Renderer::Particle> particles;

// Remove dead particles regularly
void CleanupParticles() {
    particles.erase(
        std::remove_if(particles.begin(), particles.end(),
            [](const SDK::Renderer::Particle& p) {
                return p.life <= 0.0f;
            }),
        particles.end()
    );
}
```

## Troubleshooting

### Window Not Enhanced

```cpp
// Check if window is registered
auto window = SDK::WindowManager::GetInstance().GetWindow(hwnd);
if (!window) {
    // Not registered - register it
    window = SDK::WindowManager::GetInstance().RegisterWindow(hwnd);
}

// Check if layered mode is enabled
window->EnableLayeredMode();
```

### Rendering Issues

```cpp
// Force redraw
InvalidateRect(hwnd, nullptr, TRUE);
UpdateWindow(hwnd);

// Check if render callback is set
window->SetRenderCallback([](HDC hdc) {
    // Your rendering code
});
```

### Performance Issues

```cpp
// Reduce particle count
auto particles = SDK::Renderer::CreateParticleEmission(x, y, 10, color);  // Was 100

// Disable expensive effects
theme->SetGlowEnabled(false);
theme->SetParticlesEnabled(false);

// Reduce update frequency
Sleep(33);  // 30 FPS instead of 60
```

## Integration with Existing Code

### Retrofitting Existing Application

```cpp
// In WinMain, after window creation
HWND hwnd = CreateWindowEx(...);  // Your existing code

// Add SDK enhancement
if (SDK::Initialize()) {
    auto window = SDK::WindowManager::GetInstance().RegisterWindow(hwnd);
    // Window is now enhanced with 5D rendering
}

// Rest of your code unchanged...
```

### Gradual Migration

```cpp
// Start with one window
HWND mainWindow = ...;
SDK::WindowManager::GetInstance().RegisterWindow(mainWindow);

// Add more windows over time
HWND dialog = ...;
SDK::WindowManager::GetInstance().RegisterWindow(dialog);
```

## Summary

The 5D GUI SDK provides:
- **Easy Integration**: Drop-in enhancement for existing windows
- **Flexible Theming**: Predefined or custom themes
- **Rich Effects**: Gradients, particles, shadows, glow
- **Depth System**: 5-level layering with automatic scaling
- **Performance**: Optimized GDI rendering
- **Zero Dependencies**: Pure Win32 API

Start with the basic examples and gradually add advanced features as needed!
