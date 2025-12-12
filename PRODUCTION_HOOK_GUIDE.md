# Production Hook and Hardware Acceleration - Usage Guide

This guide explains how to use the newly implemented production-ready hook system and hardware acceleration features in the 5D GUI SDK.

## Production Hook Implementation

### Overview

The SDK now includes a production-ready inline hooking system with:
- **Instruction Length Disassembler**: Ensures hooks don't split instructions
- **Hook Verification**: Validates hooks are installed correctly
- **Thread Safety**: Mutex-protected initialization
- **Safety Checks**: Memory protection validation

### Basic Usage

```cpp
#include "SDK/SDK.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    // Initialize SDK
    SDK::Initialize();
    
    // Initialize hooking system with inline hook
    SDK::WindowHook::GetInstance().Initialize(SDK::WindowHook::HookType::INLINE);
    
    // Register callback for window creation
    SDK::WindowHook::GetInstance().RegisterCreateCallback([](HWND hwnd) {
        // This callback is triggered for every window created
        // Automatically enhance windows with SDK features
        auto window = SDK::WindowManager::GetInstance().RegisterWindow(hwnd);
        auto theme = std::make_shared<SDK::Theme>(SDK::Theme::CreateModernTheme());
        window->SetTheme(theme);
        window->UpdateAppearance();
    });
    
    // Now any CreateWindowExW call will be intercepted
    HWND hwnd = CreateWindowExW(
        WS_EX_LAYERED,
        L"MyWindowClass",
        L"My Window",
        WS_OVERLAPPEDWINDOW,
        100, 100, 800, 600,
        nullptr, nullptr, hInstance, nullptr
    );
    
    ShowWindow(hwnd, nCmdShow);
    
    // Message loop...
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    // Cleanup
    SDK::Shutdown();
    return 0;
}
```

### Hook Types

The SDK supports two hook types:

#### 1. Inline Hook (Recommended)
```cpp
SDK::WindowHook::GetInstance().Initialize(SDK::WindowHook::HookType::INLINE);
```

**Advantages:**
- Works globally for the process
- Most reliable for system API hooking
- Proper instruction boundary detection

**Limitations:**
- May trigger antivirus software (false positive)
- Requires instruction disassembly

#### 2. IAT Hook
```cpp
SDK::WindowHook::GetInstance().Initialize(SDK::WindowHook::HookType::IAT);
```

**Advantages:**
- Less likely to trigger antivirus
- Faster to install

**Limitations:**
- Only affects calls through Import Address Table
- May not catch all window creation

### Hook Verification

```cpp
// Check if hook is installed
if (SDK::WindowHook::GetInstance().IsHooked()) {
    std::cout << "Hook is active\n";
}

// Verify hook integrity
if (SDK::WindowHook::GetInstance().VerifyHook()) {
    std::cout << "Hook is valid\n";
}

// Check if hook is safe
if (SDK::WindowHook::GetInstance().IsHookSafe()) {
    std::cout << "Hook is safe to use\n";
}
```

### Best Practices

1. **Initialize Early**: Install hooks during application startup before creating threads
2. **Error Handling**: Check return value of Initialize()
3. **Cleanup**: Call Shutdown() before application exit
4. **Testing**: Test with different hook types if one fails

```cpp
// Robust initialization
bool hookInstalled = false;

// Try inline hook first
hookInstalled = SDK::WindowHook::GetInstance().Initialize(
    SDK::WindowHook::HookType::INLINE
);

// Fallback to IAT if inline fails
if (!hookInstalled) {
    hookInstalled = SDK::WindowHook::GetInstance().Initialize(
        SDK::WindowHook::HookType::IAT
    );
}

if (!hookInstalled) {
    // Manual window registration fallback
    MessageBox(nullptr, L"Hook installation failed. Using manual mode.", 
               L"Warning", MB_ICONWARNING);
}
```

## Hardware Acceleration

### Overview

The SDK now supports multiple rendering backends:
- **GDI Backend**: Software rendering (compatible with all Windows versions)
- **Direct2D Backend**: GPU-accelerated rendering (Windows 7+ with Platform Update)
- **Auto Selection**: Automatically chooses best available backend

### Basic Usage

```cpp
#include "SDK/SDK.h"
#include "SDK/RenderBackend.h"

// In your window class
class MyWindow {
private:
    std::unique_ptr<SDK::RenderBackend> m_renderBackend;
    
public:
    bool Initialize(HWND hwnd) {
        // Create render backend (auto-selects best option)
        m_renderBackend = SDK::RenderBackend::Create(
            SDK::RenderBackend::BackendType::AUTO
        );
        
        if (!m_renderBackend->Initialize(hwnd)) {
            return false;
        }
        
        // Check capabilities
        auto caps = m_renderBackend->GetCapabilities();
        if (caps.supportsGPUAcceleration) {
            std::cout << "GPU acceleration enabled!\n";
        }
        
        return true;
    }
    
    void Render() {
        // Begin drawing
        m_renderBackend->BeginDraw();
        
        // Clear background
        m_renderBackend->Clear(SDK::Color{240, 240, 240, 255});
        
        // Draw shapes
        RECT rect = {100, 100, 300, 200};
        m_renderBackend->DrawRoundedRectangle(
            rect, 10.0f,
            SDK::Color{100, 150, 255, 255},  // Fill
            SDK::Color{50, 100, 200, 255},   // Border
            2.0f                             // Border width
        );
        
        // Draw gradient
        RECT gradRect = {100, 250, 300, 300};
        m_renderBackend->DrawLinearGradient(
            gradRect,
            SDK::Color{255, 100, 100, 255},  // Start
            SDK::Color{100, 100, 255, 255},  // End
            true                             // Horizontal
        );
        
        // End drawing
        m_renderBackend->EndDraw();
    }
};
```

### Backend Selection

#### Automatic Selection (Recommended)
```cpp
auto backend = SDK::RenderBackend::Create(SDK::RenderBackend::BackendType::AUTO);
```

Automatically selects:
1. Direct2D if available (GPU accelerated)
2. GDI as fallback (software rendering)

#### Explicit Backend Selection

```cpp
// Force Direct2D (GPU accelerated)
auto d2dBackend = SDK::RenderBackend::Create(
    SDK::RenderBackend::BackendType::DIRECT2D
);

// Force GDI (software rendering)
auto gdiBackend = SDK::RenderBackend::Create(
    SDK::RenderBackend::BackendType::GDI
);
```

### Feature Detection

```cpp
auto backend = SDK::RenderBackend::Create(SDK::RenderBackend::BackendType::AUTO);
backend->Initialize(hwnd);

auto caps = backend->GetCapabilities();

std::cout << "GPU Acceleration: " << caps.supportsGPUAcceleration << "\n";
std::cout << "Advanced Effects: " << caps.supportsAdvancedEffects << "\n";
std::cout << "Antialiasing: " << caps.supportsAntialiasing << "\n";
std::cout << "Transparency: " << caps.supportsTransparency << "\n";
std::cout << "Max Texture Size: " << caps.maxTextureSize << "\n";

// Check backend type
if (backend->IsHardwareAccelerated()) {
    std::cout << "Using GPU rendering\n";
} else {
    std::cout << "Using software rendering\n";
}
```

### Available Drawing Operations

#### Basic Shapes
```cpp
// Rectangle
RECT rect = {10, 10, 100, 100};
backend->DrawRectangle(rect, fillColor, borderColor, borderWidth);

// Rounded rectangle
backend->DrawRoundedRectangle(rect, radius, fillColor, borderColor, borderWidth);

// Line
backend->DrawLine(x1, y1, x2, y2, color, width);

// Ellipse
backend->DrawEllipse(cx, cy, rx, ry, fillColor, borderColor, borderWidth);
```

#### Text Rendering
```cpp
RECT textRect = {10, 10, 200, 50};
backend->DrawText(
    L"Hello, World!",
    textRect,
    SDK::Color{0, 0, 0, 255},
    L"Arial",
    16.0f,
    FW_NORMAL
);
```

#### Gradients
```cpp
// Linear gradient
backend->DrawLinearGradient(rect, startColor, endColor, horizontal);

// Radial gradient
backend->DrawRadialGradient(rect, centerColor, edgeColor, cx, cy);
```

#### Effects
```cpp
// Shadow
backend->DrawShadow(rect, offsetX, offsetY, blur, shadowColor);

// Glow
backend->DrawGlow(rect, radius, glowColor);

// GPU-accelerated effects (if available)
if (backend->SupportsGPUEffects()) {
    backend->ApplyBlur(rect, blurRadius);
    backend->ApplyBloom(rect, threshold, intensity);
}
```

### Performance Considerations

#### GPU vs CPU Rendering

**Direct2D (GPU) Benefits:**
- 5-10x faster for complex scenes
- Hardware-accelerated effects
- Better for animations and dynamic content
- Lower CPU usage

**GDI (CPU) Benefits:**
- Works on all Windows versions
- More predictable performance
- Better for simple static UIs
- No GPU driver dependencies

#### Best Practices

1. **Use AUTO mode**: Let SDK choose best backend
2. **Cache resources**: Reuse brushes, pens, and fonts
3. **Minimize state changes**: Group similar drawing operations
4. **Use effects wisely**: GPU effects are fast, but don't overuse

```cpp
// Good: Batch similar operations
backend->BeginDraw();
for (auto& rect : rectangles) {
    backend->DrawRectangle(rect, fillColor, borderColor, 1.0f);
}
backend->EndDraw();

// Avoid: Too many Begin/End pairs
for (auto& rect : rectangles) {
    backend->BeginDraw();
    backend->DrawRectangle(rect, fillColor, borderColor, 1.0f);
    backend->EndDraw();
}
```

### Compatibility

**Direct2D Requirements:**
- Windows 7 with Platform Update
- Windows 8/8.1
- Windows 10/11
- Compatible graphics driver

**GDI Requirements:**
- Any Windows version (7+)
- No special requirements

### Example: Complete Application

```cpp
#include "SDK/SDK.h"
#include "SDK/RenderBackend.h"

class RenderWindow {
private:
    HWND m_hwnd;
    std::unique_ptr<SDK::RenderBackend> m_backend;
    
public:
    bool Create(HINSTANCE hInstance) {
        // Register window class
        WNDCLASSEXW wc = {};
        wc.cbSize = sizeof(wc);
        wc.lpfnWndProc = WindowProc;
        wc.hInstance = hInstance;
        wc.lpszClassName = L"RenderWindowClass";
        RegisterClassExW(&wc);
        
        // Create window
        m_hwnd = CreateWindowExW(
            WS_EX_LAYERED,
            L"RenderWindowClass",
            L"Hardware Accelerated Window",
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
            nullptr, nullptr, hInstance, this
        );
        
        if (!m_hwnd) return false;
        
        // Initialize render backend
        m_backend = SDK::RenderBackend::Create(
            SDK::RenderBackend::BackendType::AUTO
        );
        
        if (!m_backend->Initialize(m_hwnd)) {
            return false;
        }
        
        ShowWindow(m_hwnd, SW_SHOW);
        return true;
    }
    
    void Render() {
        m_backend->BeginDraw();
        
        // Clear
        m_backend->Clear(SDK::Color{255, 255, 255, 255});
        
        // Draw content
        RECT rect = {50, 50, 750, 550};
        m_backend->DrawRoundedRectangle(
            rect, 20.0f,
            SDK::Color{100, 150, 255, 255},
            SDK::Color{50, 100, 200, 255},
            3.0f
        );
        
        // Draw text
        RECT textRect = {100, 100, 700, 150};
        m_backend->DrawText(
            L"Hardware Accelerated Rendering",
            textRect,
            SDK::Color{255, 255, 255, 255},
            L"Arial",
            24.0f,
            FW_BOLD
        );
        
        m_backend->EndDraw();
    }
    
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
        RenderWindow* pThis = nullptr;
        
        if (msg == WM_CREATE) {
            CREATESTRUCT* pCreate = (CREATESTRUCT*)lParam;
            pThis = (RenderWindow*)pCreate->lpCreateParams;
            SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pThis);
        } else {
            pThis = (RenderWindow*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
        }
        
        if (pThis) {
            switch (msg) {
                case WM_PAINT:
                    pThis->Render();
                    ValidateRect(hwnd, nullptr);
                    return 0;
                    
                case WM_DESTROY:
                    PostQuitMessage(0);
                    return 0;
            }
        }
        
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int) {
    SDK::Initialize();
    
    RenderWindow window;
    if (!window.Create(hInstance)) {
        MessageBox(nullptr, L"Failed to create window", L"Error", MB_ICONERROR);
        return 1;
    }
    
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    SDK::Shutdown();
    return (int)msg.wParam;
}
```

## Migration Guide

### From Old Hook System

**Before:**
```cpp
// Old placeholder hook
SDK::WindowHook::GetInstance().Initialize();
// Limited functionality, potential crashes
```

**After:**
```cpp
// Production-ready hook with verification
if (!SDK::WindowHook::GetInstance().Initialize(SDK::WindowHook::HookType::INLINE)) {
    // Fallback to IAT or manual registration
    SDK::WindowHook::GetInstance().Initialize(SDK::WindowHook::HookType::IAT);
}

// Verify installation
if (!SDK::WindowHook::GetInstance().VerifyHook()) {
    // Handle error
}
```

### From Direct GDI Rendering

**Before:**
```cpp
HDC hdc = GetDC(hwnd);
// Direct GDI calls
Rectangle(hdc, 10, 10, 100, 100);
ReleaseDC(hwnd, hdc);
```

**After:**
```cpp
auto backend = SDK::RenderBackend::Create(SDK::RenderBackend::BackendType::AUTO);
backend->Initialize(hwnd);

backend->BeginDraw();
RECT rect = {10, 10, 100, 100};
backend->DrawRectangle(rect, fillColor, borderColor, 1.0f);
backend->EndDraw();
```

## Troubleshooting

### Hook Installation Fails

**Symptom:** Initialize() returns false

**Solutions:**
1. Try different hook type (INLINE vs IAT)
2. Check for conflicting hooks from other software
3. Run with administrator privileges
4. Disable antivirus temporarily for testing
5. Use manual window registration as fallback

### Direct2D Not Available

**Symptom:** AUTO mode falls back to GDI

**Solutions:**
1. Update to Windows 7 Platform Update or later
2. Update graphics drivers
3. Check if Direct2D is disabled in system
4. Use GDI backend explicitly if GPU not needed

### Rendering Performance Issues

**Solutions:**
1. Profile with GetCapabilities()
2. Reduce number of draw calls
3. Cache backend resources
4. Use simpler effects
5. Consider GDI for simple UIs

## Additional Resources

- See `examples/` directory for complete demos
- Check `ARCHITECTURE.md` for technical details
- Refer to `API.md` for full API reference
- Visit GitHub issues for community support
