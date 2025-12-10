# Camera Controller and 3D Widget Guide

This guide covers the camera controller system and 3D widget placement features in the 5D GUI SDK.

## Table of Contents

1. [Overview](#overview)
2. [Camera Controller](#camera-controller)
3. [3D Widgets](#3d-widgets)
4. [Widget3D Manager](#widget3d-manager)
5. [Complete Examples](#complete-examples)
6. [Best Practices](#best-practices)

## Overview

The 5D GUI SDK now includes a comprehensive camera controller system and support for placing interactive widgets in 3D space. This enables creating immersive 3D/4D/5D applications with familiar 2D UI controls positioned in 3D scenes.

### Key Features

- **Camera Controller**: Full 3D camera with orbit, pan, and zoom controls
- **Mouse Controls**: Left-click orbit, middle-click pan, right-click/wheel zoom
- **Keyboard Controls**: WASD/Arrow keys for navigation, Q/E for vertical movement
- **3D Widgets**: Place buttons, labels, and panels in 3D space
- **Ray Casting**: Automatic mouse picking in 3D space
- **Billboard Mode**: Widgets always face the camera
- **Depth Sorting**: Proper rendering order for 3D widgets

## Camera Controller

The `CameraController` class provides a complete 3D camera system with intuitive mouse and keyboard controls.

### Creating a Camera

```cpp
#include "SDK/SDK.h"

// Create camera controller
auto camera = std::make_shared<SDK::CameraController>();

// Set initial position and target
camera->SetPosition(0.0f, 0.0f, -300.0f);
camera->SetTarget(0.0f, 0.0f, 0.0f);

// Set camera mode (3D, 4D, 5D, or 6D)
camera->SetCameraMode(SDK::CameraController::CameraMode::MODE_3D);
```

### Camera Modes

The camera supports multiple rendering dimensions:

- **MODE_3D**: Standard 3D camera (default)
- **MODE_4D**: 4D visualization with time dimension
- **MODE_5D**: 5D rendering with depth layers
- **MODE_6D**: 6D multi-timeline rendering

### Mouse Controls

The camera automatically handles mouse input for intuitive navigation:

```cpp
// In your WM_MOUSEMOVE handler
case WM_MOUSEMOVE: {
    int x = LOWORD(lParam);
    int y = HIWORD(lParam);
    camera->HandleMouseMove(x, y);
    return 0;
}

// In your WM_LBUTTONDOWN handler (orbit)
case WM_LBUTTONDOWN: {
    int x = LOWORD(lParam);
    int y = HIWORD(lParam);
    camera->HandleMouseDown(x, y, 0); // 0 = left button
    return 0;
}

// In your WM_MBUTTONDOWN handler (pan)
case WM_MBUTTONDOWN: {
    int x = LOWORD(lParam);
    int y = HIWORD(lParam);
    camera->HandleMouseDown(x, y, 1); // 1 = middle button
    return 0;
}

// In your WM_RBUTTONDOWN handler (zoom)
case WM_RBUTTONDOWN: {
    int x = LOWORD(lParam);
    int y = HIWORD(lParam);
    camera->HandleMouseDown(x, y, 2); // 2 = right button
    return 0;
}

// In your WM_MOUSEWHEEL handler
case WM_MOUSEWHEEL: {
    int delta = GET_WHEEL_DELTA_WPARAM(wParam);
    camera->HandleMouseWheel(delta);
    return 0;
}
```

**Mouse Button Functions**:
- **Left Button**: Orbit camera around target
- **Middle Button**: Pan camera (move parallel to view plane)
- **Right Button**: Zoom camera (or use mouse wheel)
- **Mouse Wheel**: Zoom in/out

### Keyboard Controls

The camera supports keyboard navigation:

```cpp
// In your WM_KEYDOWN handler
case WM_KEYDOWN: {
    camera->HandleKeyDown((int)wParam);
    return 0;
}

// In your WM_KEYUP handler
case WM_KEYUP: {
    camera->HandleKeyUp((int)wParam);
    return 0;
}
```

**Keyboard Keys**:
- **W / Up Arrow**: Move forward
- **S / Down Arrow**: Move backward
- **A / Left Arrow**: Move left
- **D / Right Arrow**: Move right
- **Q**: Move down
- **E**: Move up
- **R**: Reset camera to default position

### Updating the Camera

Update the camera in your game loop for smooth animations:

```cpp
void UpdateScene(float deltaTime) {
    camera->Update(deltaTime);
}
```

### Camera Properties

```cpp
// Set camera distance from target
camera->SetDistance(500.0f);

// Set field of view
camera->SetFieldOfView(600.0f);

// Set near and far clipping planes
camera->SetNearPlane(1.0f);
camera->SetFarPlane(2000.0f);

// Enable/disable camera smoothing
camera->SetSmoothingEnabled(true);
camera->SetSmoothingFactor(0.15f); // 0.0 to 1.0

// Get camera vectors
SDK::Renderer::Vector3D forward = camera->GetForwardVector();
SDK::Renderer::Vector3D right = camera->GetRightVector();
SDK::Renderer::Vector3D up = camera->GetUpVector();
```

### Rendering with Camera

Apply camera transformations when rendering 3D objects:

```cpp
void RenderScene(HDC hdc, int width, int height) {
    int originX = width / 2;
    int originY = height / 2;
    
    // Render 3D cube with camera
    SDK::Renderer::Vector3D cubePos(0.0f, 0.0f, 0.0f);
    SDK::Renderer::Render3DCube(hdc, cubePos, 50.0f, originX, originY,
                                SDK::Color(100, 150, 255, 255),
                                0.0f, 0.0f, 0.0f);
    
    // Or manually project points
    SDK::Renderer::Vector3D worldPoint(100.0f, 50.0f, 0.0f);
    int screenX, screenY;
    camera->ApplyToProjection(worldPoint, screenX, screenY, originX, originY, 1.0f);
    
    // Draw at screen position
    Ellipse(hdc, screenX-5, screenY-5, screenX+5, screenY+5);
}
```

## 3D Widgets

The SDK now supports placing interactive widgets in 3D space. These widgets automatically handle 3D-to-2D projection and provide click detection via ray casting.

### Available 3D Widgets

- **Button3D**: Clickable button in 3D space
- **Label3D**: Text label in 3D space
- **Panel3D**: Container panel in 3D space

### Creating 3D Widgets

```cpp
// Create a 3D button
auto button = std::make_shared<SDK::Button3D>(L"Click Me!");
button->SetPosition3D(100.0f, 50.0f, 0.0f); // X, Y, Z in world space
button->SetSize(150, 40); // Width, height in pixels

// Customize appearance
button->SetBackgroundColor(SDK::Color(70, 130, 180, 200));
button->SetTextColor(SDK::Color(255, 255, 255, 255));
button->SetHoverColor(SDK::Color(100, 160, 210, 200));

// Add click event
button->SetEventCallback([](SDK::Widget* w, SDK::WidgetEvent e, void* data) {
    if (e == SDK::WidgetEvent::CLICK) {
        MessageBoxW(nullptr, L"Button clicked in 3D!", L"Event", MB_OK);
    }
});
```

```cpp
// Create a 3D label
auto label = std::make_shared<SDK::Label3D>(L"3D Text");
label->SetPosition3D(0.0f, 100.0f, 0.0f);
label->SetTextColor(SDK::Color(255, 255, 255, 255));
label->SetTransparent(false); // Show background
```

```cpp
// Create a 3D panel
auto panel = std::make_shared<SDK::Panel3D>();
panel->SetPosition3D(-100.0f, 0.0f, 50.0f);
panel->SetSize(200, 150);
panel->SetBackgroundColor(SDK::Color(50, 50, 50, 200));
panel->SetRounded(true);
panel->SetCornerRadius(8);
```

### Widget 3D Properties

```cpp
// 3D positioning
widget->SetPosition3D(x, y, z);
widget->GetPosition3D(x, y, z);

// 3D rotation (in degrees)
widget->SetRotation3D(pitch, yaw, roll);
widget->GetRotation3D(pitch, yaw, roll);

// 3D scale
widget->SetScale3D(1.5f); // 1.0 = normal size

// Billboard mode (always face camera)
widget->SetBillboard(true); // Default: true for UI elements

// Depth testing
widget->SetDepthTest(true); // Enable depth sorting
```

## Widget3D Manager

The `Widget3DManager` class handles all 3D widget rendering and interaction, including ray casting for mouse picking.

### Creating a Widget Manager

```cpp
auto widgetManager = std::make_shared<SDK::Widget3DManager>();
```

### Adding Widgets

```cpp
// Add widgets to manager
widgetManager->AddWidget(button);
widgetManager->AddWidget(label);
widgetManager->AddWidget(panel);

// Remove widget
widgetManager->RemoveWidget(button);

// Clear all widgets
widgetManager->ClearWidgets();
```

### Handling Input

The manager automatically handles mouse picking via ray casting:

```cpp
// In your WM_MOUSEMOVE handler
case WM_MOUSEMOVE: {
    int x = LOWORD(lParam);
    int y = HIWORD(lParam);
    
    RECT clientRect;
    GetClientRect(hwnd, &clientRect);
    int width = clientRect.right - clientRect.left;
    int height = clientRect.bottom - clientRect.top;
    
    widgetManager->HandleMouseMove(x, y, width, height, camera.get());
    return 0;
}

// In your WM_LBUTTONDOWN handler
case WM_LBUTTONDOWN: {
    int x = LOWORD(lParam);
    int y = HIWORD(lParam);
    
    RECT clientRect;
    GetClientRect(hwnd, &clientRect);
    int width = clientRect.right - clientRect.left;
    int height = clientRect.bottom - clientRect.top;
    
    // Try to click widget first
    if (!widgetManager->HandleMouseDown(x, y, width, height, 0, camera.get())) {
        // If no widget clicked, handle camera
        camera->HandleMouseDown(x, y, 0);
    }
    return 0;
}

// In your WM_LBUTTONUP handler
case WM_LBUTTONUP: {
    int x = LOWORD(lParam);
    int y = HIWORD(lParam);
    
    RECT clientRect;
    GetClientRect(hwnd, &clientRect);
    int width = clientRect.right - clientRect.left;
    int height = clientRect.bottom - clientRect.top;
    
    widgetManager->HandleMouseUp(x, y, width, height, 0, camera.get());
    return 0;
}
```

### Updating and Rendering

```cpp
void UpdateScene(float deltaTime) {
    // Update camera
    camera->Update(deltaTime);
    
    // Update widgets (updates screen positions)
    RECT clientRect;
    GetClientRect(hwnd, &clientRect);
    int width = clientRect.right - clientRect.left;
    int height = clientRect.bottom - clientRect.top;
    int originX = width / 2;
    int originY = height / 2;
    
    widgetManager->UpdateAll(deltaTime, camera.get(), originX, originY);
}

void RenderScene(HDC hdc, int width, int height) {
    int originX = width / 2;
    int originY = height / 2;
    
    // Render all 3D widgets
    widgetManager->RenderAll(hdc, camera.get(), originX, originY);
}
```

## Complete Examples

### Example 1: Basic 3D Button

```cpp
#include "SDK/SDK.h"

std::shared_ptr<SDK::CameraController> g_camera;
std::shared_ptr<SDK::Widget3DManager> g_widgetManager;

void InitializeScene() {
    // Create camera
    g_camera = std::make_shared<SDK::CameraController>();
    g_camera->SetPosition(0.0f, 0.0f, -300.0f);
    
    // Create widget manager
    g_widgetManager = std::make_shared<SDK::Widget3DManager>();
    
    // Create button in 3D space
    auto button = std::make_shared<SDK::Button3D>(L"Click Me!");
    button->SetPosition3D(0.0f, 0.0f, 0.0f);
    button->SetEventCallback([](SDK::Widget* w, SDK::WidgetEvent e, void* data) {
        if (e == SDK::WidgetEvent::CLICK) {
            MessageBoxW(nullptr, L"3D Button clicked!", L"Event", MB_OK);
        }
    });
    
    g_widgetManager->AddWidget(button);
}
```

### Example 2: Multiple 3D Widgets

```cpp
void CreateWidgetCircle() {
    g_widgetManager = std::make_shared<SDK::Widget3DManager>();
    
    // Create buttons in a circle
    int numButtons = 6;
    float radius = 150.0f;
    
    for (int i = 0; i < numButtons; i++) {
        float angle = (float)i / numButtons * 2.0f * 3.14159f;
        float x = std::cos(angle) * radius;
        float z = std::sin(angle) * radius;
        
        auto button = std::make_shared<SDK::Button3D>(
            (L"Button " + std::to_wstring(i + 1)).c_str()
        );
        button->SetPosition3D(x, 0.0f, z);
        
        g_widgetManager->AddWidget(button);
    }
    
    // Add center label
    auto label = std::make_shared<SDK::Label3D>(L"Center");
    label->SetPosition3D(0.0f, 0.0f, 0.0f);
    g_widgetManager->AddWidget(label);
}
```

### Example 3: 3D UI Panel with Controls

```cpp
void CreateControlPanel() {
    // Create background panel
    auto panel = std::make_shared<SDK::Panel3D>();
    panel->SetPosition3D(0.0f, 0.0f, 0.0f);
    panel->SetSize(300, 200);
    g_widgetManager->AddWidget(panel);
    
    // Add title label
    auto title = std::make_shared<SDK::Label3D>(L"Control Panel");
    title->SetPosition3D(0.0f, 80.0f, 1.0f); // Slightly in front of panel
    title->SetTransparent(true);
    g_widgetManager->AddWidget(title);
    
    // Add buttons
    auto button1 = std::make_shared<SDK::Button3D>(L"Option 1");
    button1->SetPosition3D(-70.0f, 20.0f, 1.0f);
    g_widgetManager->AddWidget(button1);
    
    auto button2 = std::make_shared<SDK::Button3D>(L"Option 2");
    button2->SetPosition3D(70.0f, 20.0f, 1.0f);
    g_widgetManager->AddWidget(button2);
    
    auto button3 = std::make_shared<SDK::Button3D>(L"Confirm");
    button3->SetPosition3D(0.0f, -40.0f, 1.0f);
    g_widgetManager->AddWidget(button3);
}
```

## Best Practices

### 1. Widget Positioning

- Use **world space coordinates** for widget 3D positions
- Keep widgets at reasonable distances from the camera (50-500 units)
- Use **billboard mode** for UI elements that should always face the camera

### 2. Event Handling

- Handle widget clicks **before** camera controls to prevent clicking through widgets
- Use the widget manager's `HandleMouseDown` return value to determine if a widget was clicked

```cpp
if (!widgetManager->HandleMouseDown(x, y, width, height, 0, camera.get())) {
    // No widget clicked, handle camera
    camera->HandleMouseDown(x, y, 0);
}
```

### 3. Performance

- Update widget screen positions only when camera or widgets move
- Use depth sorting for proper rendering order
- Limit the number of 3D widgets to maintain good performance (< 100 widgets)

### 4. Visual Hierarchy

- Use **scale** to emphasize important widgets
- Use **depth** to organize widgets in 3D space
- Use **alpha transparency** for background elements

### 5. Camera Setup

- Set appropriate **field of view** for your scene (default: 500)
- Adjust **move speed** based on scene scale (default: 100 units/sec)
- Enable **smoothing** for smoother camera movement (default: enabled)

## Ray Casting Details

The widget manager uses ray casting for 3D mouse picking:

1. Screen coordinates are converted to a ray in world space
2. The ray is tested against all visible widgets
3. The closest widget hit by the ray is selected
4. Hit testing uses a sphere around each widget by default

You can implement custom hit testing by overriding `HitTest3D` in your widget class:

```cpp
class MyWidget3D : public SDK::Widget3D {
public:
    bool HitTest3D(const SDK::Renderer::Vector3D& rayOrigin, 
                   const SDK::Renderer::Vector3D& rayDirection,
                   float& distance) const override {
        // Custom hit testing logic here
        // Return true if ray intersects widget
        return false;
    }
};
```

## Integration with Existing SDK Features

The camera controller and 3D widgets integrate seamlessly with existing SDK features:

- **Themes**: 3D widgets respect theme colors
- **Events**: Full event system support (click, hover, focus)
- **Rendering**: Compatible with all SDK rendering features
- **Window Management**: Works with multiple windows

## Troubleshooting

### Widgets not visible
- Check widget `SetVisible(true)` is called
- Verify widget position is within camera view
- Check widget scale is reasonable (default: 1.0)

### Clicks not working
- Ensure widget manager `HandleMouseDown` is called
- Verify screen dimensions are correct
- Check widget is enabled (`SetEnabled(true)`)

### Camera not moving
- Verify `Update(deltaTime)` is called each frame
- Check keyboard handlers are connected
- Ensure mouse buttons are released properly

## See Also

- [Widget Guide](WIDGET_GUIDE.md) - Complete 2D widget documentation
- [API Reference](API.md) - Full API documentation
- [Renderer Documentation](API.md#renderer) - 3D/4D/5D rendering details
- [Examples](examples/) - Complete example applications
