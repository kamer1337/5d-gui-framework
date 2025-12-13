#pragma once

#include "Platform.h"
#include "Theme.h"
#include "DPIManager.h"
#include <vector>
#include <memory>
#include <unordered_map>
#include <functional>
#include <string>

namespace SDK {

/**
 * Monitor Information
 * Contains detailed information about a display monitor
 */
struct MonitorInfo {
    HMONITOR hMonitor;
    std::wstring name;
    std::wstring deviceName;
    RECT bounds;           // Monitor bounds in virtual screen coordinates
    RECT workArea;         // Work area excluding taskbar
    bool isPrimary;
    DPIScaleInfo dpiInfo;
    
    // Physical properties
    int widthMM;           // Physical width in millimeters
    int heightMM;          // Physical height in millimeters
    int refreshRate;       // Refresh rate in Hz
    int bitDepth;          // Color bit depth
    
    MonitorInfo() 
        : hMonitor(nullptr), isPrimary(false), 
          widthMM(0), heightMM(0), refreshRate(60), bitDepth(32) {
        bounds = {0};
        workArea = {0};
    }
    
    // Calculate physical DPI
    float GetPhysicalDPIX() const {
        if (widthMM > 0) {
            float widthInches = widthMM / 25.4f;
            return (bounds.right - bounds.left) / widthInches;
        }
        return 96.0f;
    }
    
    float GetPhysicalDPIY() const {
        if (heightMM > 0) {
            float heightInches = heightMM / 25.4f;
            return (bounds.bottom - bounds.top) / heightInches;
        }
        return 96.0f;
    }
};

/**
 * MonitorManager - Multi-monitor management for the SDK
 * Handles monitor enumeration, tracking, and monitor-specific settings
 */
class MonitorManager {
public:
    static MonitorManager& GetInstance();
    
    // Initialize monitor management
    bool Initialize();
    void Shutdown();
    
    // Monitor enumeration
    void RefreshMonitors();
    const std::vector<MonitorInfo>& GetMonitors() const { return m_monitors; }
    int GetMonitorCount() const { return static_cast<int>(m_monitors.size()); }
    
    // Get monitor by various criteria
    const MonitorInfo* GetMonitor(HMONITOR hMonitor) const;
    const MonitorInfo* GetMonitorForWindow(HWND hwnd) const;
    const MonitorInfo* GetMonitorAtPoint(const POINT& pt) const;
    const MonitorInfo* GetPrimaryMonitor() const;
    
    // Monitor for window
    HMONITOR GetMonitorHandleForWindow(HWND hwnd) const;
    
    // Check if point/rect is on a specific monitor
    bool IsPointOnMonitor(const POINT& pt, HMONITOR hMonitor) const;
    bool IsRectOnMonitor(const RECT& rect, HMONITOR hMonitor) const;
    
    // Get monitor bounds
    RECT GetMonitorBounds(HMONITOR hMonitor) const;
    RECT GetMonitorWorkArea(HMONITOR hMonitor) const;
    
    // Monitor-specific themes
    void SetMonitorTheme(HMONITOR hMonitor, std::shared_ptr<Theme> theme);
    std::shared_ptr<Theme> GetMonitorTheme(HMONITOR hMonitor) const;
    void ClearMonitorTheme(HMONITOR hMonitor);
    void ClearAllMonitorThemes();
    
    // Get effective theme for a window (monitor theme or default)
    std::shared_ptr<Theme> GetEffectiveThemeForWindow(HWND hwnd) const;
    
    // Window-to-monitor tracking
    void TrackWindow(HWND hwnd);
    void UntrackWindow(HWND hwnd);
    HMONITOR GetTrackedMonitorForWindow(HWND hwnd) const;
    bool HasWindowChangedMonitor(HWND hwnd);
    
    // Monitor change notifications
    using MonitorChangeCallback = std::function<void(HWND, HMONITOR, HMONITOR)>;
    void RegisterMonitorChangeCallback(HWND hwnd, MonitorChangeCallback callback);
    void UnregisterMonitorChangeCallback(HWND hwnd);
    
    // Window migration helpers
    void MigrateWindowToMonitor(HWND hwnd, HMONITOR hTargetMonitor, bool centerOnMonitor = true);
    void CenterWindowOnMonitor(HWND hwnd, HMONITOR hMonitor);
    
    // Virtual screen information
    RECT GetVirtualScreenBounds() const;
    
    // Update and notification
    void Update();
    
private:
    MonitorManager();
    ~MonitorManager();
    
    MonitorManager(const MonitorManager&) = delete;
    MonitorManager& operator=(const MonitorManager&) = delete;
    
    void EnumerateMonitors();
    void GatherMonitorDetails(MonitorInfo& info);
    void NotifyMonitorChange(HWND hwnd, HMONITOR oldMonitor, HMONITOR newMonitor);
    
    static BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData);
    
    bool m_initialized;
    std::vector<MonitorInfo> m_monitors;
    
    // Monitor-specific themes
    std::unordered_map<HMONITOR, std::shared_ptr<Theme>> m_monitorThemes;
    
    // Window tracking
    struct WindowMonitorTrack {
        HMONITOR currentMonitor;
        HMONITOR previousMonitor;
    };
    std::unordered_map<HWND, WindowMonitorTrack> m_windowTracking;
    
    // Monitor change callbacks
    std::unordered_map<HWND, MonitorChangeCallback> m_monitorChangeCallbacks;
};

} // namespace SDK
