#include "SDK/MonitorManager.h"
#include "SDK/DPIManager.h"
#include <algorithm>

namespace SDK {

MonitorManager::MonitorManager()
    : m_initialized(false)
{
}

MonitorManager::~MonitorManager() {
    Shutdown();
}

MonitorManager& MonitorManager::GetInstance() {
    static MonitorManager instance;
    return instance;
}

bool MonitorManager::Initialize() {
    if (m_initialized) {
        return true;
    }
    
    RefreshMonitors();
    m_initialized = true;
    return true;
}

void MonitorManager::Shutdown() {
    m_monitors.clear();
    m_monitorThemes.clear();
    m_windowTracking.clear();
    m_monitorChangeCallbacks.clear();
    m_initialized = false;
}

void MonitorManager::RefreshMonitors() {
    m_monitors.clear();
    EnumerateMonitors();
}

void MonitorManager::EnumerateMonitors() {
    // Enumerate all monitors
    EnumDisplayMonitors(nullptr, nullptr, MonitorEnumProc, reinterpret_cast<LPARAM>(this));
}

BOOL CALLBACK MonitorManager::MonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, 
                                               LPRECT lprcMonitor, LPARAM dwData) {
    MonitorManager* pThis = reinterpret_cast<MonitorManager*>(dwData);
    
    MonitorInfo info;
    info.hMonitor = hMonitor;
    
    // Get monitor info
    MONITORINFOEXW miex = {0};
    miex.cbSize = sizeof(MONITORINFOEXW);
    if (GetMonitorInfoW(hMonitor, &miex)) {
        info.bounds = miex.rcMonitor;
        info.workArea = miex.rcWork;
        info.isPrimary = (miex.dwFlags & MONITORINFOF_PRIMARY) != 0;
        info.deviceName = miex.szDevice;
    }
    
    // Gather additional details
    pThis->GatherMonitorDetails(info);
    
    // Get DPI info for this monitor
    info.dpiInfo = DPIManager::GetInstance().GetDPIForMonitor(hMonitor);
    
    pThis->m_monitors.push_back(info);
    return TRUE;
}

void MonitorManager::GatherMonitorDetails(MonitorInfo& info) {
    // Get device mode for additional information
    DEVMODEW dm = {0};
    dm.dmSize = sizeof(DEVMODEW);
    
    if (EnumDisplaySettingsW(info.deviceName.c_str(), ENUM_CURRENT_SETTINGS, &dm)) {
        info.refreshRate = dm.dmDisplayFrequency;
        info.bitDepth = dm.dmBitsPerPel;
        
        // Try to get physical size if available
        // Note: Not all drivers provide accurate physical size
        if (dm.dmPelsWidth > 0 && dm.dmPelsHeight > 0) {
            // Estimate based on common DPI
            // This is approximate - actual physical size requires EDID data
            float estimatedDPI = info.dpiInfo.GetPhysicalDPIX();
            if (estimatedDPI > 0) {
                info.widthMM = static_cast<int>((dm.dmPelsWidth / estimatedDPI) * 25.4f);
                info.heightMM = static_cast<int>((dm.dmPelsHeight / estimatedDPI) * 25.4f);
            }
        }
    }
    
    // Try to get friendly name from display config
    DISPLAYCONFIG_PATH_INFO pathInfo[128];
    DISPLAYCONFIG_MODE_INFO modeInfo[128];
    UINT32 pathCount = 128;
    UINT32 modeCount = 128;
    
    if (QueryDisplayConfig(QDC_ONLY_ACTIVE_PATHS, &pathCount, pathInfo, &modeCount, modeInfo, nullptr) == ERROR_SUCCESS) {
        for (UINT32 i = 0; i < pathCount; ++i) {
            DISPLAYCONFIG_SOURCE_DEVICE_NAME sourceName = {0};
            sourceName.header.type = DISPLAYCONFIG_DEVICE_INFO_GET_SOURCE_NAME;
            sourceName.header.size = sizeof(sourceName);
            sourceName.header.adapterId = pathInfo[i].sourceInfo.adapterId;
            sourceName.header.id = pathInfo[i].sourceInfo.id;
            
            if (DisplayConfigGetDeviceInfo(&sourceName.header) == ERROR_SUCCESS) {
                if (info.deviceName == sourceName.viewGdiDeviceName) {
                    DISPLAYCONFIG_TARGET_DEVICE_NAME targetName = {0};
                    targetName.header.type = DISPLAYCONFIG_DEVICE_INFO_GET_TARGET_NAME;
                    targetName.header.size = sizeof(targetName);
                    targetName.header.adapterId = pathInfo[i].targetInfo.adapterId;
                    targetName.header.id = pathInfo[i].targetInfo.id;
                    
                    if (DisplayConfigGetDeviceInfo(&targetName.header) == ERROR_SUCCESS) {
                        info.name = targetName.monitorFriendlyDeviceName;
                        
                        // Get more accurate physical size from EDID if available
                        if (targetName.edidManufactureId != 0) {
                            // Physical size is available in output technology info
                            DISPLAYCONFIG_TARGET_PREFERRED_MODE prefMode = {0};
                            prefMode.header.type = DISPLAYCONFIG_DEVICE_INFO_GET_TARGET_PREFERRED_MODE;
                            prefMode.header.size = sizeof(prefMode);
                            prefMode.header.adapterId = pathInfo[i].targetInfo.adapterId;
                            prefMode.header.id = pathInfo[i].targetInfo.id;
                            
                            if (DisplayConfigGetDeviceInfo(&prefMode.header) == ERROR_SUCCESS) {
                                // Width and height are in 0.1mm units
                                // Note: Not all displays provide this info
                            }
                        }
                    }
                    break;
                }
            }
        }
    }
    
    // If no friendly name found, use device name
    if (info.name.empty()) {
        info.name = info.deviceName;
    }
}

const MonitorInfo* MonitorManager::GetMonitor(HMONITOR hMonitor) const {
    for (const auto& monitor : m_monitors) {
        if (monitor.hMonitor == hMonitor) {
            return &monitor;
        }
    }
    return nullptr;
}

const MonitorInfo* MonitorManager::GetMonitorForWindow(HWND hwnd) const {
    if (!hwnd || !IsWindow(hwnd)) {
        return nullptr;
    }
    
    HMONITOR hMonitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
    return GetMonitor(hMonitor);
}

const MonitorInfo* MonitorManager::GetMonitorAtPoint(const POINT& pt) const {
    HMONITOR hMonitor = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
    return GetMonitor(hMonitor);
}

const MonitorInfo* MonitorManager::GetPrimaryMonitor() const {
    for (const auto& monitor : m_monitors) {
        if (monitor.isPrimary) {
            return &monitor;
        }
    }
    return m_monitors.empty() ? nullptr : &m_monitors[0];
}

HMONITOR MonitorManager::GetMonitorHandleForWindow(HWND hwnd) const {
    if (!hwnd || !IsWindow(hwnd)) {
        return nullptr;
    }
    return MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
}

bool MonitorManager::IsPointOnMonitor(const POINT& pt, HMONITOR hMonitor) const {
    const MonitorInfo* info = GetMonitor(hMonitor);
    if (!info) {
        return false;
    }
    
    return pt.x >= info->bounds.left && pt.x < info->bounds.right &&
           pt.y >= info->bounds.top && pt.y < info->bounds.bottom;
}

bool MonitorManager::IsRectOnMonitor(const RECT& rect, HMONITOR hMonitor) const {
    const MonitorInfo* info = GetMonitor(hMonitor);
    if (!info) {
        return false;
    }
    
    // Check if rectangles intersect
    return !(rect.right <= info->bounds.left || 
             rect.left >= info->bounds.right ||
             rect.bottom <= info->bounds.top || 
             rect.top >= info->bounds.bottom);
}

RECT MonitorManager::GetMonitorBounds(HMONITOR hMonitor) const {
    const MonitorInfo* info = GetMonitor(hMonitor);
    if (info) {
        return info->bounds;
    }
    return {0, 0, 0, 0};
}

RECT MonitorManager::GetMonitorWorkArea(HMONITOR hMonitor) const {
    const MonitorInfo* info = GetMonitor(hMonitor);
    if (info) {
        return info->workArea;
    }
    return {0, 0, 0, 0};
}

void MonitorManager::SetMonitorTheme(HMONITOR hMonitor, std::shared_ptr<Theme> theme) {
    if (hMonitor && theme) {
        m_monitorThemes[hMonitor] = theme;
    }
}

std::shared_ptr<Theme> MonitorManager::GetMonitorTheme(HMONITOR hMonitor) const {
    auto it = m_monitorThemes.find(hMonitor);
    if (it != m_monitorThemes.end()) {
        return it->second;
    }
    return nullptr;
}

void MonitorManager::ClearMonitorTheme(HMONITOR hMonitor) {
    m_monitorThemes.erase(hMonitor);
}

void MonitorManager::ClearAllMonitorThemes() {
    m_monitorThemes.clear();
}

std::shared_ptr<Theme> MonitorManager::GetEffectiveThemeForWindow(HWND hwnd) const {
    if (!hwnd || !IsWindow(hwnd)) {
        return nullptr;
    }
    
    HMONITOR hMonitor = GetMonitorHandleForWindow(hwnd);
    return GetMonitorTheme(hMonitor);
}

void MonitorManager::TrackWindow(HWND hwnd) {
    if (!hwnd || !IsWindow(hwnd)) {
        return;
    }
    
    HMONITOR hMonitor = GetMonitorHandleForWindow(hwnd);
    
    WindowMonitorTrack& track = m_windowTracking[hwnd];
    track.currentMonitor = hMonitor;
    track.previousMonitor = hMonitor;
}

void MonitorManager::UntrackWindow(HWND hwnd) {
    m_windowTracking.erase(hwnd);
}

HMONITOR MonitorManager::GetTrackedMonitorForWindow(HWND hwnd) const {
    auto it = m_windowTracking.find(hwnd);
    if (it != m_windowTracking.end()) {
        return it->second.currentMonitor;
    }
    return nullptr;
}

bool MonitorManager::HasWindowChangedMonitor(HWND hwnd) {
    if (!hwnd || !IsWindow(hwnd)) {
        return false;
    }
    
    auto it = m_windowTracking.find(hwnd);
    if (it == m_windowTracking.end()) {
        // Not tracking this window, start tracking
        TrackWindow(hwnd);
        return false;
    }
    
    HMONITOR currentMonitor = GetMonitorHandleForWindow(hwnd);
    bool changed = (currentMonitor != it->second.currentMonitor);
    
    if (changed) {
        HMONITOR oldMonitor = it->second.currentMonitor;
        it->second.previousMonitor = it->second.currentMonitor;
        it->second.currentMonitor = currentMonitor;
        
        // Notify callback
        NotifyMonitorChange(hwnd, oldMonitor, currentMonitor);
    }
    
    return changed;
}

void MonitorManager::RegisterMonitorChangeCallback(HWND hwnd, MonitorChangeCallback callback) {
    m_monitorChangeCallbacks[hwnd] = callback;
}

void MonitorManager::UnregisterMonitorChangeCallback(HWND hwnd) {
    m_monitorChangeCallbacks.erase(hwnd);
}

void MonitorManager::NotifyMonitorChange(HWND hwnd, HMONITOR oldMonitor, HMONITOR newMonitor) {
    auto it = m_monitorChangeCallbacks.find(hwnd);
    if (it != m_monitorChangeCallbacks.end()) {
        it->second(hwnd, oldMonitor, newMonitor);
    }
}

void MonitorManager::MigrateWindowToMonitor(HWND hwnd, HMONITOR hTargetMonitor, bool centerOnMonitor) {
    if (!hwnd || !IsWindow(hwnd) || !hTargetMonitor) {
        return;
    }
    
    RECT windowRect;
    GetWindowRect(hwnd, &windowRect);
    int width = windowRect.right - windowRect.left;
    int height = windowRect.bottom - windowRect.top;
    
    const MonitorInfo* targetInfo = GetMonitor(hTargetMonitor);
    if (!targetInfo) {
        return;
    }
    
    if (centerOnMonitor) {
        CenterWindowOnMonitor(hwnd, hTargetMonitor);
    } else {
        // Move to same relative position on target monitor
        HMONITOR currentMonitor = GetMonitorHandleForWindow(hwnd);
        const MonitorInfo* currentInfo = GetMonitor(currentMonitor);
        
        if (currentInfo) {
            // Calculate relative position in current monitor
            int relX = windowRect.left - currentInfo->bounds.left;
            int relY = windowRect.top - currentInfo->bounds.top;
            
            // Apply to target monitor
            int newX = targetInfo->bounds.left + relX;
            int newY = targetInfo->bounds.top + relY;
            
            // Ensure window stays within target monitor bounds
            if (newX + width > targetInfo->bounds.right) {
                newX = targetInfo->bounds.right - width;
            }
            if (newY + height > targetInfo->bounds.bottom) {
                newY = targetInfo->bounds.bottom - height;
            }
            if (newX < targetInfo->bounds.left) {
                newX = targetInfo->bounds.left;
            }
            if (newY < targetInfo->bounds.top) {
                newY = targetInfo->bounds.top;
            }
            
            SetWindowPos(hwnd, nullptr, newX, newY, 0, 0, 
                        SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
        }
    }
    
    // Update tracking
    auto it = m_windowTracking.find(hwnd);
    if (it != m_windowTracking.end()) {
        HMONITOR oldMonitor = it->second.currentMonitor;
        it->second.previousMonitor = oldMonitor;
        it->second.currentMonitor = hTargetMonitor;
        NotifyMonitorChange(hwnd, oldMonitor, hTargetMonitor);
    }
}

void MonitorManager::CenterWindowOnMonitor(HWND hwnd, HMONITOR hMonitor) {
    if (!hwnd || !IsWindow(hwnd) || !hMonitor) {
        return;
    }
    
    const MonitorInfo* info = GetMonitor(hMonitor);
    if (!info) {
        return;
    }
    
    RECT windowRect;
    GetWindowRect(hwnd, &windowRect);
    int width = windowRect.right - windowRect.left;
    int height = windowRect.bottom - windowRect.top;
    
    // Center in work area (excludes taskbar)
    int centerX = info->workArea.left + (info->workArea.right - info->workArea.left - width) / 2;
    int centerY = info->workArea.top + (info->workArea.bottom - info->workArea.top - height) / 2;
    
    SetWindowPos(hwnd, nullptr, centerX, centerY, 0, 0, 
                SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
}

RECT MonitorManager::GetVirtualScreenBounds() const {
    RECT virtualScreen;
    virtualScreen.left = GetSystemMetrics(SM_XVIRTUALSCREEN);
    virtualScreen.top = GetSystemMetrics(SM_YVIRTUALSCREEN);
    virtualScreen.right = virtualScreen.left + GetSystemMetrics(SM_CXVIRTUALSCREEN);
    virtualScreen.bottom = virtualScreen.top + GetSystemMetrics(SM_CYVIRTUALSCREEN);
    return virtualScreen;
}

void MonitorManager::Update() {
    // Check tracked windows for monitor changes
    std::vector<HWND> toCheck;
    for (const auto& pair : m_windowTracking) {
        toCheck.push_back(pair.first);
    }
    
    for (HWND hwnd : toCheck) {
        HasWindowChangedMonitor(hwnd);
    }
}

} // namespace SDK
