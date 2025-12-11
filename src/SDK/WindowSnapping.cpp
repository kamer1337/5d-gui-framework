#include "../../include/SDK/WindowSnapping.h"
#include <algorithm>
#include <cmath>

namespace SDK {

WindowSnapping::WindowSnapping()
    : m_edgeSnapEnabled(true)
    , m_gridSnapEnabled(false)
    , m_magneticEnabled(true)
    , m_edgeSnapThreshold(20)
    , m_windowSnapThreshold(15)
    , m_gridWidth(100)
    , m_gridHeight(100)
{
}

WindowSnapping::~WindowSnapping() {
}

void WindowSnapping::SetGridSize(int width, int height) {
    m_gridWidth = (width > 0) ? width : 100;
    m_gridHeight = (height > 0) ? height : 100;
}

void WindowSnapping::GetGridSize(int& width, int& height) const {
    width = m_gridWidth;
    height = m_gridHeight;
}

RECT WindowSnapping::GetScreenWorkArea(HWND hwnd) {
    RECT workArea = {0};
    
    // Get monitor info for the window
    HMONITOR hMonitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
    if (hMonitor) {
        MONITORINFO mi = {0};
        mi.cbSize = sizeof(MONITORINFO);
        if (GetMonitorInfo(hMonitor, &mi)) {
            workArea = mi.rcWork;
        }
    }
    
    // Fallback to system metrics
    if (workArea.right == 0 && workArea.bottom == 0) {
        workArea.left = 0;
        workArea.top = 0;
        workArea.right = GetSystemMetrics(SM_CXSCREEN);
        workArea.bottom = GetSystemMetrics(SM_CYSCREEN);
    }
    
    return workArea;
}

bool WindowSnapping::IsNearEdge(int value, int edge, int threshold) const {
    return std::abs(value - edge) <= threshold;
}

int WindowSnapping::SnapToValue(int value, int target, int threshold) const {
    if (IsNearEdge(value, target, threshold)) {
        return target;
    }
    return value;
}

RECT WindowSnapping::SnapToEdges(HWND hwnd, const RECT& proposed) {
    if (!m_edgeSnapEnabled) return proposed;
    
    RECT result = proposed;
    RECT workArea = GetScreenWorkArea(hwnd);
    
    int width = proposed.right - proposed.left;
    int height = proposed.bottom - proposed.top;
    
    // Snap left edge
    result.left = SnapToValue(proposed.left, workArea.left, m_edgeSnapThreshold);
    result.right = result.left + width;
    
    // Snap right edge
    if (IsNearEdge(proposed.right, workArea.right, m_edgeSnapThreshold)) {
        result.right = workArea.right;
        result.left = result.right - width;
    }
    
    // Snap top edge
    result.top = SnapToValue(proposed.top, workArea.top, m_edgeSnapThreshold);
    result.bottom = result.top + height;
    
    // Snap bottom edge
    if (IsNearEdge(proposed.bottom, workArea.bottom, m_edgeSnapThreshold)) {
        result.bottom = workArea.bottom;
        result.top = result.bottom - height;
    }
    
    return result;
}

RECT WindowSnapping::SnapToGrid(const RECT& proposed) {
    if (!m_gridSnapEnabled) return proposed;
    
    RECT result = proposed;
    int width = proposed.right - proposed.left;
    int height = proposed.bottom - proposed.top;
    
    // Snap to grid
    result.left = (proposed.left / m_gridWidth) * m_gridWidth;
    result.top = (proposed.top / m_gridHeight) * m_gridHeight;
    result.right = result.left + width;
    result.bottom = result.top + height;
    
    return result;
}

RECT WindowSnapping::SnapToWindows(HWND hwnd, const RECT& proposed,
                                   const std::vector<std::shared_ptr<Window>>& windows)
{
    if (!m_magneticEnabled) return proposed;
    
    RECT result = proposed;
    int width = proposed.right - proposed.left;
    int height = proposed.bottom - proposed.top;
    
    for (const auto& window : windows) {
        if (!window || !window->IsValid()) continue;
        
        HWND otherHwnd = window->GetHandle();
        if (otherHwnd == hwnd) continue;
        
        RECT otherRect;
        if (!GetWindowRect(otherHwnd, &otherRect)) continue;
        
        // Snap left edge to other window's right edge
        if (IsNearEdge(proposed.left, otherRect.right, m_windowSnapThreshold)) {
            result.left = otherRect.right;
            result.right = result.left + width;
        }
        
        // Snap right edge to other window's left edge
        if (IsNearEdge(proposed.right, otherRect.left, m_windowSnapThreshold)) {
            result.right = otherRect.left;
            result.left = result.right - width;
        }
        
        // Snap top edge to other window's bottom edge
        if (IsNearEdge(proposed.top, otherRect.bottom, m_windowSnapThreshold)) {
            result.top = otherRect.bottom;
            result.bottom = result.top + height;
        }
        
        // Snap bottom edge to other window's top edge
        if (IsNearEdge(proposed.bottom, otherRect.top, m_windowSnapThreshold)) {
            result.bottom = otherRect.top;
            result.top = result.bottom - height;
        }
    }
    
    return result;
}

RECT WindowSnapping::ApplySnapping(HWND hwnd, const RECT& proposed,
                                   const std::vector<std::shared_ptr<Window>>& windows)
{
    RECT result = proposed;
    
    // Apply snapping in order of priority
    if (m_edgeSnapEnabled) {
        result = SnapToEdges(hwnd, result);
    }
    
    if (m_magneticEnabled) {
        result = SnapToWindows(hwnd, result, windows);
    }
    
    if (m_gridSnapEnabled) {
        result = SnapToGrid(result);
    }
    
    return result;
}

} // namespace SDK
