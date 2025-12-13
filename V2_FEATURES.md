# Version 2.0.0 Features Guide

This guide covers the new features introduced in version 2.0.0 of the 5D GUI SDK: High DPI Support and Multi-Monitor Management.

## Table of Contents

1. [High DPI Support](#high-dpi-support)
2. [Multi-Monitor Management](#multi-monitor-management)
3. [Integration Examples](#integration-examples)
4. [Best Practices](#best-practices)
5. [Troubleshooting](#troubleshooting)

---

## High DPI Support

Version 2.0.0 introduces comprehensive per-monitor DPI awareness, allowing your applications to render sharply on all displays regardless of their DPI settings.

### DPI Awareness Levels

The SDK supports four levels of DPI awareness:

- **UNAWARE**: System-scaled (blurry on high DPI displays)
- **SYSTEM_AWARE**: Single DPI for all monitors
- **PER_MONITOR_V1**: Per-monitor DPI awareness (Windows 8.1+)
- **PER_MONITOR_V2**: Enhanced per-monitor DPI v2 (Windows 10 1703+, recommended)

The SDK automatically initializes with PER_MONITOR_V2 when available, falling back gracefully to older APIs.

### Using the DPI Manager

```cpp
#include "SDK/SDK.h"

// Initialize SDK (DPI manager is automatically initialized)
SDK::Initialize();

// Get DPI manager instance
auto& dpiMgr = SDK::DPIManager::GetInstance();

// Get DPI information for a window
SDK::DPIScaleInfo dpi = dpiMgr.GetDPIForWindow(hwnd);
std::wcout << L"DPI: " << dpi.dpi << std::endl;
std::wcout << L"Scale: " << dpi.scaleX << L"x" << std::endl;

// Get system DPI (primary monitor)
SDK::DPIScaleInfo systemDPI = dpiMgr.GetSystemDPI();

// Get DPI for a specific monitor
HMONITOR hMonitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
SDK::DPIScaleInfo monitorDPI = dpiMgr.GetDPIForMonitor(hMonitor);
```

### DPI Scaling Functions

The DPI manager provides convenient scaling functions:

```cpp
auto& dpiMgr = SDK::DPIManager::GetInstance();
SDK::DPIScaleInfo dpi = dpiMgr.GetDPIForWindow(hwnd);

// Scale individual values
int scaledWidth = dpiMgr.ScaleValueX(100, dpi);   // Scale 100 pixels
int scaledHeight = dpiMgr.ScaleValueY(100, dpi);

// Unscale values
int originalWidth = dpiMgr.UnscaleValueX(scaledWidth, dpi);

// Scale structures
RECT rect = {0, 0, 800, 600};
RECT scaledRect = dpiMgr.ScaleRect(rect, dpi);

SIZE size = {800, 600};
SIZE scaledSize = dpiMgr.ScaleSize(size, dpi);

POINT pt = {100, 100};
POINT scaledPt = dpiMgr.ScalePoint(pt, dpi);
```

### Handling DPI Changes

Windows can change DPI when moved between monitors with different scaling:

```cpp
// Register DPI change callback
dpiMgr.RegisterDPIChangeCallback(hwnd, 
    [](HWND hwnd, const SDK::DPIScaleInfo& oldDPI, const SDK::DPIScaleInfo& newDPI) {
        // DPI changed from oldDPI to newDPI
        std::wcout << L"DPI changed from " << oldDPI.dpi 
                   << L" to " << newDPI.dpi << std::endl;
        
        // Window is automatically resized by the system
        // Update your UI elements here
    });

// In your window procedure, handle WM_DPICHANGED
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_DPICHANGED) {
        SDK::DPIManager::GetInstance().HandleDPIChange(hwnd, wParam, lParam);
        return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}
```

### SDK Window Integration

SDK windows automatically handle DPI changes:

```cpp
auto window = SDK::WindowManager::GetInstance().RegisterWindow(hwnd);

// The window will automatically:
// - Track its current DPI
// - Scale widgets when DPI changes
// - Update rendering

// Manually trigger DPI update if needed
window->UpdateForDPI();

// Get current DPI scale for the window
SDK::DPIScaleInfo windowDPI = window->GetDPIScale();
```

---

## Multi-Monitor Management

Version 2.0.0 provides comprehensive multi-monitor support, including monitor enumeration, window tracking, and monitor-specific themes.

### Monitor Manager

```cpp
#include "SDK/SDK.h"

// Initialize SDK (Monitor manager is automatically initialized)
SDK::Initialize();

// Get monitor manager instance
auto& monitorMgr = SDK::MonitorManager::GetInstance();

// Refresh monitor list (automatically called on initialization)
monitorMgr.RefreshMonitors();

// Get all monitors
const std::vector<SDK::MonitorInfo>& monitors = monitorMgr.GetMonitors();
int count = monitorMgr.GetMonitorCount();

std::wcout << L"Found " << count << L" monitor(s)" << std::endl;
```

### Monitor Information

Each monitor provides detailed information:

```cpp
const SDK::MonitorInfo* monitor = monitorMgr.GetPrimaryMonitor();
if (monitor) {
    std::wcout << L"Name: " << monitor->name << std::endl;
    std::wcout << L"Device: " << monitor->deviceName << std::endl;
    std::wcout << L"Resolution: " << (monitor->bounds.right - monitor->bounds.left)
               << L"x" << (monitor->bounds.bottom - monitor->bounds.top) << std::endl;
    std::wcout << L"Refresh Rate: " << monitor->refreshRate << L" Hz" << std::endl;
    std::wcout << L"DPI: " << monitor->dpiInfo.dpi << std::endl;
    std::wcout << L"Primary: " << (monitor->isPrimary ? L"Yes" : L"No") << std::endl;
}
```

### Finding Monitors

```cpp
// Get monitor for a window
const SDK::MonitorInfo* monitor = monitorMgr.GetMonitorForWindow(hwnd);

// Get monitor at a point
POINT pt = {100, 100};
const SDK::MonitorInfo* monitor = monitorMgr.GetMonitorAtPoint(pt);

// Get primary monitor
const SDK::MonitorInfo* primary = monitorMgr.GetPrimaryMonitor();

// Get monitor handle
HMONITOR hMonitor = monitorMgr.GetMonitorHandleForWindow(hwnd);

// Get monitor bounds
RECT bounds = monitorMgr.GetMonitorBounds(hMonitor);
RECT workArea = monitorMgr.GetMonitorWorkArea(hMonitor); // Excludes taskbar
```

### Window Tracking

Track windows as they move between monitors:

```cpp
// Start tracking a window
monitorMgr.TrackWindow(hwnd);

// Check if window changed monitors
if (monitorMgr.HasWindowChangedMonitor(hwnd)) {
    HMONITOR currentMonitor = monitorMgr.GetTrackedMonitorForWindow(hwnd);
    // Handle monitor change
}

// Register monitor change callback
monitorMgr.RegisterMonitorChangeCallback(hwnd,
    [](HWND hwnd, HMONITOR oldMonitor, HMONITOR newMonitor) {
        std::wcout << L"Window moved to different monitor" << std::endl;
        // Update window settings for new monitor
    });

// In your message loop, update tracking
MSG msg;
while (GetMessage(&msg, nullptr, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
    monitorMgr.Update(); // Check for monitor changes
}

// Stop tracking when done
monitorMgr.UntrackWindow(hwnd);
```

### Window Migration

Move windows between monitors programmatically:

```cpp
// Get target monitor
const SDK::MonitorInfo* targetMonitor = monitorMgr.GetMonitors()[1]; // Second monitor

// Migrate window (with centering)
monitorMgr.MigrateWindowToMonitor(hwnd, targetMonitor->hMonitor, true);

// Migrate window (preserve relative position)
monitorMgr.MigrateWindowToMonitor(hwnd, targetMonitor->hMonitor, false);

// Center window on a monitor
monitorMgr.CenterWindowOnMonitor(hwnd, targetMonitor->hMonitor);
```

### Monitor-Specific Themes

Apply different themes to windows based on their monitor:

```cpp
// Set theme for primary monitor
const SDK::MonitorInfo* primary = monitorMgr.GetPrimaryMonitor();
if (primary) {
    auto lightTheme = std::make_shared<SDK::Theme>(SDK::Theme::CreateLightTheme());
    monitorMgr.SetMonitorTheme(primary->hMonitor, lightTheme);
}

// Set theme for secondary monitor
if (monitorMgr.GetMonitorCount() > 1) {
    const SDK::MonitorInfo* secondary = &monitorMgr.GetMonitors()[1];
    auto darkTheme = std::make_shared<SDK::Theme>(SDK::Theme::CreateDarkTheme());
    monitorMgr.SetMonitorTheme(secondary->hMonitor, darkTheme);
}

// Get theme for a window's monitor
auto theme = monitorMgr.GetEffectiveThemeForWindow(hwnd);
if (theme) {
    window->SetTheme(theme);
}

// Clear monitor-specific themes
monitorMgr.ClearMonitorTheme(hMonitor);
monitorMgr.ClearAllMonitorThemes();
```

---

## Integration Examples

### Complete DPI-Aware Application

```cpp
#include "SDK/SDK.h"

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    // Initialize SDK
    SDK::Initialize();
    
    auto& dpiMgr = SDK::DPIManager::GetInstance();
    auto& monitorMgr = SDK::MonitorManager::GetInstance();
    
    // Register window class
    WNDCLASSEXW wc = {0};
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"DPIAwareApp";
    RegisterClassExW(&wc);
    
    // Get system DPI for initial sizing
    SDK::DPIScaleInfo systemDPI = dpiMgr.GetSystemDPI();
    int width = dpiMgr.ScaleValueX(800, systemDPI);
    int height = dpiMgr.ScaleValueY(600, systemDPI);
    
    // Create window
    HWND hwnd = CreateWindowExW(WS_EX_LAYERED, L"DPIAwareApp", L"My App",
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
        width, height, nullptr, nullptr, hInstance, nullptr);
    
    // Register with SDK
    auto window = SDK::WindowManager::GetInstance().RegisterWindow(hwnd);
    window->SetTheme(std::make_shared<SDK::Theme>(SDK::Theme::CreateModernTheme()));
    
    // Track for monitor changes
    monitorMgr.TrackWindow(hwnd);
    
    // Register callbacks
    dpiMgr.RegisterDPIChangeCallback(hwnd, [window](HWND hwnd,
        const SDK::DPIScaleInfo& oldDPI, const SDK::DPIScaleInfo& newDPI) {
        window->HandleDPIChange(oldDPI, newDPI);
    });
    
    monitorMgr.RegisterMonitorChangeCallback(hwnd, [window](HWND hwnd,
        HMONITOR oldMonitor, HMONITOR newMonitor) {
        window->HandleMonitorChange(oldMonitor, newMonitor);
    });
    
    ShowWindow(hwnd, nCmdShow);
    
    // Message loop
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
        monitorMgr.Update();
    }
    
    // Cleanup
    monitorMgr.UntrackWindow(hwnd);
    SDK::Shutdown();
    return 0;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_DPICHANGED:
            SDK::DPIManager::GetInstance().HandleDPIChange(hwnd, wParam, lParam);
            return 0;
            
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}
```

### Multi-Monitor Dashboard

```cpp
// Create a window on each monitor
auto& monitorMgr = SDK::MonitorManager::GetInstance();
const auto& monitors = monitorMgr.GetMonitors();

for (const auto& monitor : monitors) {
    // Create window
    HWND hwnd = CreateWindowExW(...);
    
    // Center on this monitor
    monitorMgr.CenterWindowOnMonitor(hwnd, monitor.hMonitor);
    
    // Apply monitor-specific theme
    auto theme = monitor.isPrimary ? 
        std::make_shared<SDK::Theme>(SDK::Theme::CreateLightTheme()) :
        std::make_shared<SDK::Theme>(SDK::Theme::CreateDarkTheme());
    
    auto window = SDK::WindowManager::GetInstance().RegisterWindow(hwnd);
    window->SetTheme(theme);
    
    ShowWindow(hwnd, SW_SHOW);
}
```

---

## Best Practices

### 1. Always Initialize SDK First

```cpp
// Always call Initialize before using DPI or Monitor managers
SDK::Initialize();
```

### 2. Use DPI-Scaled Sizes

```cpp
// Bad: Hard-coded pixel sizes
CreateWindowExW(..., 100, 100, 800, 600, ...);

// Good: DPI-scaled sizes
auto& dpiMgr = SDK::DPIManager::GetInstance();
SDK::DPIScaleInfo dpi = dpiMgr.GetSystemDPI();
int width = dpiMgr.ScaleValueX(800, dpi);
int height = dpiMgr.ScaleValueY(600, dpi);
CreateWindowExW(..., CW_USEDEFAULT, CW_USEDEFAULT, width, height, ...);
```

### 3. Handle WM_DPICHANGED

```cpp
// Always handle WM_DPICHANGED in your window procedure
case WM_DPICHANGED:
    SDK::DPIManager::GetInstance().HandleDPIChange(hwnd, wParam, lParam);
    return 0;
```

### 4. Update Monitor Tracking

```cpp
// Call Update() in your message loop if tracking windows
MSG msg;
while (GetMessage(&msg, nullptr, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
    SDK::MonitorManager::GetInstance().Update();
}
```

### 5. Use SDK Windows for Automatic Handling

```cpp
// SDK windows automatically handle DPI and monitor changes
auto window = SDK::WindowManager::GetInstance().RegisterWindow(hwnd);
// No manual DPI scaling needed for widgets
```

---

## Troubleshooting

### Blurry Rendering on High DPI

**Problem**: Window appears blurry on high DPI displays.

**Solution**: Ensure SDK is initialized before creating windows. The SDK automatically sets per-monitor DPI awareness.

```cpp
// Make sure Initialize is called first
SDK::Initialize();
// Then create windows
```

### Window Size Incorrect After DPI Change

**Problem**: Window size doesn't update when moved to different DPI monitor.

**Solution**: Handle WM_DPICHANGED message and let SDK handle the resize.

```cpp
case WM_DPICHANGED:
    SDK::DPIManager::GetInstance().HandleDPIChange(hwnd, wParam, lParam);
    return 0; // Don't call DefWindowProc
```

### Widgets Not Scaling with DPI

**Problem**: Widgets remain the same size when DPI changes.

**Solution**: Use SDK windows which automatically scale widgets.

```cpp
auto window = SDK::WindowManager::GetInstance().RegisterWindow(hwnd);
// Widgets will automatically scale with DPI changes
```

### Monitor Changes Not Detected

**Problem**: Window monitor changes aren't detected.

**Solution**: Call Update() in your message loop.

```cpp
while (GetMessage(&msg, nullptr, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
    monitorMgr.Update(); // Add this
}
```

### Wrong DPI on Application Start

**Problem**: Application starts with incorrect DPI.

**Solution**: Get DPI after SDK initialization, not before window creation.

```cpp
SDK::Initialize();
// Now get DPI
auto dpi = SDK::DPIManager::GetInstance().GetSystemDPI();
// Create window with scaled dimensions
```

---

## API Reference Summary

### DPIManager

```cpp
class DPIManager {
    static DPIManager& GetInstance();
    
    bool Initialize(DPIAwareness awareness = DPIAwareness::PER_MONITOR_V2);
    DPIScaleInfo GetDPIForWindow(HWND hwnd) const;
    DPIScaleInfo GetDPIForMonitor(HMONITOR hMonitor) const;
    DPIScaleInfo GetSystemDPI() const;
    
    int ScaleValueX(int value, const DPIScaleInfo& dpi) const;
    int ScaleValueY(int value, const DPIScaleInfo& dpi) const;
    RECT ScaleRect(const RECT& rect, const DPIScaleInfo& dpi) const;
    
    void RegisterDPIChangeCallback(HWND hwnd, DPIChangeCallback callback);
    void HandleDPIChange(HWND hwnd, WPARAM wParam, LPARAM lParam);
};
```

### MonitorManager

```cpp
class MonitorManager {
    static MonitorManager& GetInstance();
    
    bool Initialize();
    void RefreshMonitors();
    const std::vector<MonitorInfo>& GetMonitors() const;
    
    const MonitorInfo* GetMonitorForWindow(HWND hwnd) const;
    const MonitorInfo* GetPrimaryMonitor() const;
    
    void TrackWindow(HWND hwnd);
    bool HasWindowChangedMonitor(HWND hwnd);
    
    void SetMonitorTheme(HMONITOR hMonitor, std::shared_ptr<Theme> theme);
    void MigrateWindowToMonitor(HWND hwnd, HMONITOR hMonitor, bool center);
    
    void RegisterMonitorChangeCallback(HWND hwnd, MonitorChangeCallback callback);
    void Update();
};
```

---

For more examples, see the `dpi_multimonitor_demo.cpp` example in the SDK.
