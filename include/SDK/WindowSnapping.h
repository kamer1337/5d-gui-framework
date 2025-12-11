#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <memory>
#include <vector>
#include "Window.h"

namespace SDK {

/**
 * WindowSnapping - Provides window snapping functionality
 * Supports edge snapping, grid snapping, and magnetic windows
 */
class WindowSnapping {
public:
    WindowSnapping();
    ~WindowSnapping();
    
    // Snapping configuration
    void SetEdgeSnapEnabled(bool enabled) { m_edgeSnapEnabled = enabled; }
    bool IsEdgeSnapEnabled() const { return m_edgeSnapEnabled; }
    
    void SetGridSnapEnabled(bool enabled) { m_gridSnapEnabled = enabled; }
    bool IsGridSnapEnabled() const { return m_gridSnapEnabled; }
    
    void SetMagneticWindowsEnabled(bool enabled) { m_magneticEnabled = enabled; }
    bool IsMagneticWindowsEnabled() const { return m_magneticEnabled; }
    
    // Snap thresholds (in pixels)
    void SetEdgeSnapThreshold(int threshold) { m_edgeSnapThreshold = threshold; }
    int GetEdgeSnapThreshold() const { return m_edgeSnapThreshold; }
    
    void SetWindowSnapThreshold(int threshold) { m_windowSnapThreshold = threshold; }
    int GetWindowSnapThreshold() const { return m_windowSnapThreshold; }
    
    // Grid configuration
    void SetGridSize(int width, int height);
    void GetGridSize(int& width, int& height) const;
    
    // Snap operations
    RECT SnapToEdges(HWND hwnd, const RECT& proposed);
    RECT SnapToGrid(const RECT& proposed);
    RECT SnapToWindows(HWND hwnd, const RECT& proposed, 
                       const std::vector<std::shared_ptr<Window>>& windows);
    
    // Combined snap operation
    RECT ApplySnapping(HWND hwnd, const RECT& proposed,
                       const std::vector<std::shared_ptr<Window>>& windows);
    
private:
    bool m_edgeSnapEnabled;
    bool m_gridSnapEnabled;
    bool m_magneticEnabled;
    
    int m_edgeSnapThreshold;
    int m_windowSnapThreshold;
    
    int m_gridWidth;
    int m_gridHeight;
    
    // Helper methods
    bool IsNearEdge(int value, int edge, int threshold) const;
    int SnapToValue(int value, int target, int threshold) const;
    RECT GetScreenWorkArea(HWND hwnd);
};

} // namespace SDK
