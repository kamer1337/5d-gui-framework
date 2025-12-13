#pragma once

#include "Platform.h"
#include <functional>
#include <unordered_map>

namespace SDK {

/**
 * DPI Awareness Level
 * Controls how the application responds to DPI settings
 */
enum class DPIAwareness {
    UNAWARE = 0,           // System scaled (blurry on high DPI)
    SYSTEM_AWARE = 1,      // Single DPI for all monitors
    PER_MONITOR_V1 = 2,    // Per-monitor DPI awareness
    PER_MONITOR_V2 = 3     // Enhanced per-monitor DPI v2 (Windows 10 1703+)
};

/**
 * DPI Scale Info
 * Contains DPI information for a specific context
 */
struct DPIScaleInfo {
    float dpiX;
    float dpiY;
    float scaleX;
    float scaleY;
    UINT dpi;
    
    DPIScaleInfo() : dpiX(96.0f), dpiY(96.0f), scaleX(1.0f), scaleY(1.0f), dpi(96) {}
};

/**
 * DPIManager - Centralized DPI management for the SDK
 * Handles per-monitor DPI awareness and automatic scaling
 */
class DPIManager {
public:
    static DPIManager& GetInstance();
    
    // Initialize DPI management
    bool Initialize(DPIAwareness awareness = DPIAwareness::PER_MONITOR_V2);
    void Shutdown();
    
    // DPI awareness control
    bool SetDPIAwareness(DPIAwareness awareness);
    DPIAwareness GetDPIAwareness() const { return m_awareness; }
    
    // Get DPI for a window
    DPIScaleInfo GetDPIForWindow(HWND hwnd) const;
    
    // Get DPI for a monitor
    DPIScaleInfo GetDPIForMonitor(HMONITOR hMonitor) const;
    
    // Get system DPI (primary monitor)
    DPIScaleInfo GetSystemDPI() const;
    
    // Scale conversion helpers
    int ScaleValueX(int value, float scale) const;
    int ScaleValueY(int value, float scale) const;
    int ScaleValueX(int value, const DPIScaleInfo& dpi) const;
    int ScaleValueY(int value, const DPIScaleInfo& dpi) const;
    
    // Unscale conversion helpers
    int UnscaleValueX(int value, float scale) const;
    int UnscaleValueY(int value, float scale) const;
    int UnscaleValueX(int value, const DPIScaleInfo& dpi) const;
    int UnscaleValueY(int value, const DPIScaleInfo& dpi) const;
    
    // RECT scaling
    RECT ScaleRect(const RECT& rect, const DPIScaleInfo& dpi) const;
    RECT UnscaleRect(const RECT& rect, const DPIScaleInfo& dpi) const;
    
    // SIZE scaling
    SIZE ScaleSize(const SIZE& size, const DPIScaleInfo& dpi) const;
    SIZE UnscaleSize(const SIZE& size, const DPIScaleInfo& dpi) const;
    
    // POINT scaling
    POINT ScalePoint(const POINT& pt, const DPIScaleInfo& dpi) const;
    POINT UnscalePoint(const POINT& pt, const DPIScaleInfo& dpi) const;
    
    // DPI change notifications
    using DPIChangeCallback = std::function<void(HWND, const DPIScaleInfo&, const DPIScaleInfo&)>;
    void RegisterDPIChangeCallback(HWND hwnd, DPIChangeCallback callback);
    void UnregisterDPIChangeCallback(HWND hwnd);
    
    // Handle DPI change message (call from WndProc)
    void HandleDPIChange(HWND hwnd, WPARAM wParam, LPARAM lParam);
    
    // Check if DPI awareness is supported
    bool IsDPIAwarenessSupported() const { return m_dpiAwarenessSupported; }
    
private:
    DPIManager();
    ~DPIManager();
    
    DPIManager(const DPIManager&) = delete;
    DPIManager& operator=(const DPIManager&) = delete;
    
    void LoadDPIFunctions();
    bool SetProcessDPIAwareness();
    DPIScaleInfo CalculateDPIScaleInfo(UINT dpi) const;
    
    DPIAwareness m_awareness;
    bool m_initialized;
    bool m_dpiAwarenessSupported;
    DPIScaleInfo m_systemDPI;
    
    // Cache DPI info per window
    mutable std::unordered_map<HWND, DPIScaleInfo> m_windowDPICache;
    
    // DPI change callbacks
    std::unordered_map<HWND, DPIChangeCallback> m_dpiChangeCallbacks;
    
    // Function pointers for DPI APIs (dynamically loaded for compatibility)
    typedef BOOL (WINAPI *SetProcessDpiAwarenessContextFunc)(DPI_AWARENESS_CONTEXT);
    typedef BOOL (WINAPI *SetProcessDpiAwarenessFunc)(int);
    typedef UINT (WINAPI *GetDpiForWindowFunc)(HWND);
    typedef UINT (WINAPI *GetDpiForSystemFunc)(void);
    typedef int (WINAPI *GetSystemMetricsForDpiFunc)(int, UINT);
    typedef BOOL (WINAPI *AdjustWindowRectExForDpiFunc)(LPRECT, DWORD, BOOL, DWORD, UINT);
    
    SetProcessDpiAwarenessContextFunc m_setProcessDpiAwarenessContext;
    SetProcessDpiAwarenessFunc m_setProcessDpiAwareness;
    GetDpiForWindowFunc m_getDpiForWindow;
    GetDpiForSystemFunc m_getDpiForSystem;
    GetSystemMetricsForDpiFunc m_getSystemMetricsForDpi;
    AdjustWindowRectExForDpiFunc m_adjustWindowRectExForDpi;
};

} // namespace SDK
