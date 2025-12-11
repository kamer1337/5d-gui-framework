# Advanced Window Features Guide

This guide covers the advanced window features in the 5D GUI SDK including window grouping, snapping, and animations.

## Table of Contents

1. [Window Groups](#window-groups)
2. [Window Snapping](#window-snapping)
3. [Window Animations](#window-animations)
4. [Examples](#examples)

---

## Window Groups

Window groups allow you to link multiple windows together so they move in sync and share properties.

### Features

- **Linked Movement**: Windows move together as a group
- **Synchronized Depth Changes**: Apply depth changes to all windows in the group
- **Group Transformations**: Apply transformations (alpha, scale, offset) to all windows

### Basic Usage

```cpp
#include "SDK/SDK.h"

// Create a window group
auto group = std::make_shared<SDK::WindowGroup>(L"MyGroup");

// Add windows to the group
auto window1 = SDK::WindowManager::GetInstance().GetWindow(hwnd1);
auto window2 = SDK::WindowManager::GetInstance().GetWindow(hwnd2);
auto window3 = SDK::WindowManager::GetInstance().GetWindow(hwnd3);

group->AddWindow(window1);
group->AddWindow(window2);
group->AddWindow(window3);

// Enable linked movement
group->SetLinkedMovement(true);
group->SetSynchronizedDepth(true);

// Move all windows together
group->MoveGroup(50, 50);  // Move 50 pixels right and down

// Change depth of all windows
group->SetGroupDepth(SDK::WindowDepth::FOREGROUND);

// Apply alpha to all windows
group->SetGroupAlpha(200);

// Apply custom transformation to all windows
group->ApplyTransformation([](std::shared_ptr<SDK::Window> window) {
    window->SetScale(0.9f);
    window->SetRoundedCorners(true, 15);
});
```

### API Reference

#### WindowGroup

```cpp
class WindowGroup {
public:
    WindowGroup(const std::wstring& name = L"");
    
    // Group management
    void AddWindow(std::shared_ptr<Window> window);
    void RemoveWindow(std::shared_ptr<Window> window);
    void ClearWindows();
    
    // Linked movement
    void SetLinkedMovement(bool enabled);
    bool IsLinkedMovementEnabled() const;
    void MoveGroup(int deltaX, int deltaY);
    
    // Synchronized depth
    void SetSynchronizedDepth(bool enabled);
    bool IsSynchronizedDepthEnabled() const;
    void SetGroupDepth(WindowDepth depth);
    void OffsetGroupDepth(int offset);
    
    // Group transformations
    void SetGroupAlpha(BYTE alpha);
    void SetGroupScale(float scale);
    void SetGroupOffset(int x, int y);
    void ApplyTransformation(std::function<void(std::shared_ptr<Window>)> transformation);
};
```

---

## Window Snapping

Window snapping provides automatic alignment of windows to screen edges, grid positions, or other windows.

### Features

- **Edge Snapping**: Snap to screen edges and monitor boundaries
- **Grid Snapping**: Snap to a virtual grid for precise positioning
- **Magnetic Windows**: Windows attract to each other when moved nearby

### Basic Usage

```cpp
#include "SDK/SDK.h"

// Get the WindowSnapping instance from WindowManager
auto& snapping = SDK::WindowManager::GetInstance().GetSnapping();

// Configure snapping
snapping.SetEdgeSnapEnabled(true);
snapping.SetGridSnapEnabled(false);
snapping.SetMagneticWindowsEnabled(true);

// Set snap thresholds (in pixels)
snapping.SetEdgeSnapThreshold(20);    // 20 pixels from edge
snapping.SetWindowSnapThreshold(15);  // 15 pixels from other windows

// Set grid size
snapping.SetGridSize(100, 100);  // 100x100 pixel grid

// Apply snapping in WM_MOVING handler
case WM_MOVING: {
    RECT* pRect = reinterpret_cast<RECT*>(lParam);
    if (pRect) {
        auto& snapping = SDK::WindowManager::GetInstance().GetSnapping();
        auto windows = SDK::WindowManager::GetInstance().GetWindowsByDepth();
        *pRect = snapping.ApplySnapping(hwnd, *pRect, windows);
    }
    return TRUE;
}
```

### API Reference

#### WindowSnapping

```cpp
class WindowSnapping {
public:
    // Configuration
    void SetEdgeSnapEnabled(bool enabled);
    bool IsEdgeSnapEnabled() const;
    
    void SetGridSnapEnabled(bool enabled);
    bool IsGridSnapEnabled() const;
    
    void SetMagneticWindowsEnabled(bool enabled);
    bool IsMagneticWindowsEnabled() const;
    
    // Thresholds
    void SetEdgeSnapThreshold(int threshold);
    int GetEdgeSnapThreshold() const;
    
    void SetWindowSnapThreshold(int threshold);
    int GetWindowSnapThreshold() const;
    
    // Grid configuration
    void SetGridSize(int width, int height);
    void GetGridSize(int& width, int& height) const;
    
    // Snap operations
    RECT SnapToEdges(HWND hwnd, const RECT& proposed);
    RECT SnapToGrid(const RECT& proposed);
    RECT SnapToWindows(HWND hwnd, const RECT& proposed, 
                       const std::vector<std::shared_ptr<Window>>& windows);
    RECT ApplySnapping(HWND hwnd, const RECT& proposed,
                       const std::vector<std::shared_ptr<Window>>& windows);
};
```

---

## Window Animations

Window animations provide smooth transitions for minimize, maximize, and restore operations.

### Features

- **Multiple Animation Types**: Fade, slide, scale, zoom
- **Easing Functions**: Linear, ease-in, ease-out, ease-in-out, bounce
- **Customizable Duration**: Control animation speed
- **Callbacks**: Execute code when animation completes

### Basic Usage

```cpp
#include "SDK/SDK.h"

// Create window animation
auto animation = std::make_shared<SDK::WindowAnimation>(hwnd);

// Configure animation
animation->SetMinimizeAnimation(SDK::WindowAnimation::AnimationType::SCALE);
animation->SetMaximizeAnimation(SDK::WindowAnimation::AnimationType::ZOOM);
animation->SetRestoreAnimation(SDK::WindowAnimation::AnimationType::FADE);

animation->SetAnimationDuration(300);  // 300 milliseconds
animation->SetEasingType(SDK::WindowAnimation::EasingType::EASE_OUT);

// Set callback
animation->SetOnAnimationComplete([]() {
    // Animation finished
    MessageBoxW(nullptr, L"Animation complete!", L"Info", MB_OK);
});

// Trigger animations
animation->AnimateMinimize();  // Animate to minimized state
animation->AnimateMaximize();  // Animate to maximized state
animation->AnimateRestore();   // Animate to normal state

// Update in message loop
while (GetMessage(&msg, nullptr, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
    
    // Update animation
    animation->Update();
}
```

### Animation Types

- **NONE**: No animation (instant)
- **FADE**: Fade in/out with alpha blending
- **SLIDE**: Slide from one position to another
- **SCALE**: Scale window size smoothly
- **ZOOM**: Zoom effect with center-point scaling

### Easing Types

- **LINEAR**: Constant speed
- **EASE_IN**: Start slow, accelerate
- **EASE_OUT**: Start fast, decelerate (recommended)
- **EASE_IN_OUT**: Smooth acceleration and deceleration
- **BOUNCE**: Bouncing effect

### API Reference

#### WindowAnimation

```cpp
class WindowAnimation {
public:
    enum class AnimationType {
        NONE, FADE, SLIDE, SCALE, ZOOM
    };
    
    enum class EasingType {
        LINEAR, EASE_IN, EASE_OUT, EASE_IN_OUT, BOUNCE
    };
    
    WindowAnimation(HWND hwnd);
    
    // Configuration
    void SetMinimizeAnimation(AnimationType type);
    AnimationType GetMinimizeAnimation() const;
    
    void SetMaximizeAnimation(AnimationType type);
    AnimationType GetMaximizeAnimation() const;
    
    void SetRestoreAnimation(AnimationType type);
    AnimationType GetRestoreAnimation() const;
    
    void SetAnimationDuration(int milliseconds);
    int GetAnimationDuration() const;
    
    void SetEasingType(EasingType easing);
    EasingType GetEasingType() const;
    
    // Animation control
    void AnimateMinimize();
    void AnimateMaximize();
    void AnimateRestore();
    void StopAnimation();
    
    bool IsAnimating() const;
    
    // Update (call from message loop)
    void Update();
    
    // Callbacks
    void SetOnAnimationComplete(std::function<void()> callback);
};
```

---

## Examples

### Complete Example: Advanced Window Features Demo

```cpp
#include "SDK/SDK.h"

HWND g_mainWindow = nullptr;
std::shared_ptr<SDK::WindowGroup> g_group;
std::shared_ptr<SDK::WindowAnimation> g_animation;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_KEYDOWN:
            if (wParam == 'M') {
                g_animation->AnimateMinimize();
            } else if (wParam == 'X') {
                g_animation->AnimateMaximize();
            } else if (wParam == 'R') {
                g_animation->AnimateRestore();
            }
            return 0;
            
        case WM_MOVING: {
            RECT* pRect = reinterpret_cast<RECT*>(lParam);
            if (pRect) {
                auto& snapping = SDK::WindowManager::GetInstance().GetSnapping();
                auto windows = SDK::WindowManager::GetInstance().GetWindowsByDepth();
                *pRect = snapping.ApplySnapping(hwnd, *pRect, windows);
            }
            return TRUE;
        }
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    SDK::Initialize();
    
    // Create windows...
    
    // Setup window group
    g_group = std::make_shared<SDK::WindowGroup>(L"Demo");
    g_group->AddWindow(SDK::WindowManager::GetInstance().GetWindow(g_mainWindow));
    g_group->SetLinkedMovement(true);
    
    // Setup animation
    g_animation = std::make_shared<SDK::WindowAnimation>(g_mainWindow);
    g_animation->SetAnimationDuration(300);
    g_animation->SetEasingType(SDK::WindowAnimation::EasingType::EASE_OUT);
    
    // Setup snapping
    auto& snapping = SDK::WindowManager::GetInstance().GetSnapping();
    snapping.SetEdgeSnapEnabled(true);
    snapping.SetMagneticWindowsEnabled(true);
    
    // Message loop
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
        g_animation->Update();
    }
    
    SDK::Shutdown();
    return 0;
}
```

### Tips and Best Practices

1. **Window Groups**
   - Keep groups small for better performance
   - Disable linked movement when not needed
   - Use custom transformations for complex operations

2. **Window Snapping**
   - Adjust thresholds based on user feedback
   - Grid snapping works best for layout applications
   - Magnetic windows enhance user experience

3. **Window Animations**
   - Use EASE_OUT easing for most animations (feels natural)
   - Keep duration between 200-400ms for best UX
   - Update animation in the message loop
   - Stop animations before closing windows

### Performance Considerations

- **Window Groups**: Minimal overhead, scales well with many windows
- **Window Snapping**: Very efficient, only runs during window movement
- **Window Animations**: Uses CPU for rendering, avoid many simultaneous animations

---

## Integration with Existing Features

All advanced window features work seamlessly with existing SDK features:

- **5D Depth System**: Animations and groups respect depth levels
- **Theming**: Windows maintain their themes during animations
- **Widgets**: Widgets are preserved and transformed with windows
- **Layered Windows**: All features work with alpha transparency

---

## Platform Support

All advanced window features are supported on:
- Windows 7 and later
- x86 and x64 architectures
- Compatible with all existing SDK features

---

For more information, see:
- [API Reference](API.md)
- [Architecture Guide](ARCHITECTURE.md)
- [Widget Guide](WIDGET_GUIDE.md)
- [Main README](README.md)
