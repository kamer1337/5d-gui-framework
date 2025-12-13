#include "SDK/DPIManager.h"
#include <ShellScalingApi.h>
#include <algorithm>

#pragma comment(lib, "Shcore.lib")

namespace SDK {

DPIManager::DPIManager()
    : m_awareness(DPIAwareness::UNAWARE)
    , m_initialized(false)
    , m_dpiAwarenessSupported(false)
    , m_setProcessDpiAwarenessContext(nullptr)
    , m_setProcessDpiAwareness(nullptr)
    , m_getDpiForWindow(nullptr)
    , m_getDpiForSystem(nullptr)
    , m_getSystemMetricsForDpi(nullptr)
    , m_adjustWindowRectExForDpi(nullptr)
{
}

DPIManager::~DPIManager() {
    Shutdown();
}

DPIManager& DPIManager::GetInstance() {
    static DPIManager instance;
    return instance;
}

bool DPIManager::Initialize(DPIAwareness awareness) {
    if (m_initialized) {
        return true;
    }
    
    LoadDPIFunctions();
    
    // Try to set DPI awareness
    if (!SetDPIAwareness(awareness)) {
        // Fall back to system awareness
        SetDPIAwareness(DPIAwareness::SYSTEM_AWARE);
    }
    
    // Get system DPI
    m_systemDPI = GetSystemDPI();
    
    m_initialized = true;
    return true;
}

void DPIManager::Shutdown() {
    m_windowDPICache.clear();
    m_dpiChangeCallbacks.clear();
    m_initialized = false;
}

void DPIManager::LoadDPIFunctions() {
    // Load User32.dll functions (Windows 10 1607+)
    HMODULE user32 = GetModuleHandleW(L"User32.dll");
    if (user32) {
        m_setProcessDpiAwarenessContext = 
            reinterpret_cast<SetProcessDpiAwarenessContextFunc>(
                GetProcAddress(user32, "SetProcessDpiAwarenessContext"));
        m_getDpiForWindow = 
            reinterpret_cast<GetDpiForWindowFunc>(
                GetProcAddress(user32, "GetDpiForWindow"));
        m_getDpiForSystem = 
            reinterpret_cast<GetDpiForSystemFunc>(
                GetProcAddress(user32, "GetDpiForSystem"));
        m_getSystemMetricsForDpi = 
            reinterpret_cast<GetSystemMetricsForDpiFunc>(
                GetProcAddress(user32, "GetSystemMetricsForDpi"));
        m_adjustWindowRectExForDpi = 
            reinterpret_cast<AdjustWindowRectExForDpiFunc>(
                GetProcAddress(user32, "AdjustWindowRectExForDpi"));
    }
    
    // Load Shcore.dll functions (Windows 8.1+)
    HMODULE shcore = GetModuleHandleW(L"Shcore.dll");
    if (shcore) {
        m_setProcessDpiAwareness = 
            reinterpret_cast<SetProcessDpiAwarenessFunc>(
                GetProcAddress(shcore, "SetProcessDpiAwareness"));
    }
    
    m_dpiAwarenessSupported = (m_getDpiForWindow != nullptr) || 
                              (m_setProcessDpiAwareness != nullptr);
}

bool DPIManager::SetDPIAwareness(DPIAwareness awareness) {
    // Windows 10 1703+ (Per-Monitor V2)
    if (m_setProcessDpiAwarenessContext && awareness == DPIAwareness::PER_MONITOR_V2) {
        if (m_setProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2)) {
            m_awareness = DPIAwareness::PER_MONITOR_V2;
            return true;
        }
    }
    
    // Windows 8.1+ (Per-Monitor V1)
    if (m_setProcessDpiAwarenessContext && awareness == DPIAwareness::PER_MONITOR_V1) {
        if (m_setProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE)) {
            m_awareness = DPIAwareness::PER_MONITOR_V1;
            return true;
        }
    }
    
    // Alternative API for Windows 8.1+
    if (m_setProcessDpiAwareness) {
        int processAwareness;
        switch (awareness) {
            case DPIAwareness::PER_MONITOR_V2:
            case DPIAwareness::PER_MONITOR_V1:
                processAwareness = PROCESS_PER_MONITOR_DPI_AWARE;
                break;
            case DPIAwareness::SYSTEM_AWARE:
                processAwareness = PROCESS_SYSTEM_DPI_AWARE;
                break;
            default:
                processAwareness = PROCESS_DPI_UNAWARE;
                break;
        }
        
        if (SUCCEEDED(m_setProcessDpiAwareness(processAwareness))) {
            m_awareness = awareness;
            return true;
        }
    }
    
    // Fallback: Windows Vista+ system DPI awareness
    if (awareness == DPIAwareness::SYSTEM_AWARE || awareness == DPIAwareness::PER_MONITOR_V1) {
        if (SetProcessDPIAware()) {
            m_awareness = DPIAwareness::SYSTEM_AWARE;
            return true;
        }
    }
    
    return false;
}

DPIScaleInfo DPIManager::GetDPIForWindow(HWND hwnd) const {
    if (!hwnd || !IsWindow(hwnd)) {
        return m_systemDPI;
    }
    
    // Check cache first
    auto it = m_windowDPICache.find(hwnd);
    if (it != m_windowDPICache.end()) {
        return it->second;
    }
    
    UINT dpi = 96;
    
    // Try Windows 10 1607+ API
    if (m_getDpiForWindow) {
        dpi = m_getDpiForWindow(hwnd);
    } else {
        // Fallback: Get DPI from monitor
        HMONITOR hMonitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
        if (hMonitor) {
            DPIScaleInfo info = GetDPIForMonitor(hMonitor);
            return info;
        }
    }
    
    DPIScaleInfo info = CalculateDPIScaleInfo(dpi);
    m_windowDPICache[hwnd] = info;
    return info;
}

DPIScaleInfo DPIManager::GetDPIForMonitor(HMONITOR hMonitor) const {
    if (!hMonitor) {
        return m_systemDPI;
    }
    
    UINT dpiX = 96;
    UINT dpiY = 96;
    
    // Try Windows 8.1+ API
    typedef HRESULT (WINAPI *GetDpiForMonitorFunc)(HMONITOR, int, UINT*, UINT*);
    HMODULE shcore = GetModuleHandleW(L"Shcore.dll");
    if (shcore) {
        GetDpiForMonitorFunc getDpiForMonitor = 
            reinterpret_cast<GetDpiForMonitorFunc>(
                GetProcAddress(shcore, "GetDpiForMonitor"));
        if (getDpiForMonitor) {
            getDpiForMonitor(hMonitor, 0, &dpiX, &dpiY); // MDT_EFFECTIVE_DPI = 0
        }
    }
    
    DPIScaleInfo info;
    info.dpiX = static_cast<float>(dpiX);
    info.dpiY = static_cast<float>(dpiY);
    info.scaleX = info.dpiX / 96.0f;
    info.scaleY = info.dpiY / 96.0f;
    info.dpi = dpiX; // Use X as primary DPI
    return info;
}

DPIScaleInfo DPIManager::GetSystemDPI() const {
    UINT dpi = 96;
    
    // Try Windows 10 1607+ API
    if (m_getDpiForSystem) {
        dpi = m_getDpiForSystem();
    } else {
        // Fallback: Get DPI from desktop DC
        HDC hdc = GetDC(nullptr);
        if (hdc) {
            dpi = GetDeviceCaps(hdc, LOGPIXELSX);
            ReleaseDC(nullptr, hdc);
        }
    }
    
    return CalculateDPIScaleInfo(dpi);
}

DPIScaleInfo DPIManager::CalculateDPIScaleInfo(UINT dpi) const {
    DPIScaleInfo info;
    info.dpi = dpi;
    info.dpiX = static_cast<float>(dpi);
    info.dpiY = static_cast<float>(dpi);
    info.scaleX = info.dpiX / 96.0f;
    info.scaleY = info.dpiY / 96.0f;
    return info;
}

int DPIManager::ScaleValueX(int value, float scale) const {
    return static_cast<int>(value * scale + 0.5f);
}

int DPIManager::ScaleValueY(int value, float scale) const {
    return static_cast<int>(value * scale + 0.5f);
}

int DPIManager::ScaleValueX(int value, const DPIScaleInfo& dpi) const {
    return ScaleValueX(value, dpi.scaleX);
}

int DPIManager::ScaleValueY(int value, const DPIScaleInfo& dpi) const {
    return ScaleValueY(value, dpi.scaleY);
}

int DPIManager::UnscaleValueX(int value, float scale) const {
    return static_cast<int>(value / scale + 0.5f);
}

int DPIManager::UnscaleValueY(int value, float scale) const {
    return static_cast<int>(value / scale + 0.5f);
}

int DPIManager::UnscaleValueX(int value, const DPIScaleInfo& dpi) const {
    return UnscaleValueX(value, dpi.scaleX);
}

int DPIManager::UnscaleValueY(int value, const DPIScaleInfo& dpi) const {
    return UnscaleValueY(value, dpi.scaleY);
}

RECT DPIManager::ScaleRect(const RECT& rect, const DPIScaleInfo& dpi) const {
    RECT scaled;
    scaled.left = ScaleValueX(rect.left, dpi);
    scaled.top = ScaleValueY(rect.top, dpi);
    scaled.right = ScaleValueX(rect.right, dpi);
    scaled.bottom = ScaleValueY(rect.bottom, dpi);
    return scaled;
}

RECT DPIManager::UnscaleRect(const RECT& rect, const DPIScaleInfo& dpi) const {
    RECT unscaled;
    unscaled.left = UnscaleValueX(rect.left, dpi);
    unscaled.top = UnscaleValueY(rect.top, dpi);
    unscaled.right = UnscaleValueX(rect.right, dpi);
    unscaled.bottom = UnscaleValueY(rect.bottom, dpi);
    return unscaled;
}

SIZE DPIManager::ScaleSize(const SIZE& size, const DPIScaleInfo& dpi) const {
    SIZE scaled;
    scaled.cx = ScaleValueX(size.cx, dpi);
    scaled.cy = ScaleValueY(size.cy, dpi);
    return scaled;
}

SIZE DPIManager::UnscaleSize(const SIZE& size, const DPIScaleInfo& dpi) const {
    SIZE unscaled;
    unscaled.cx = UnscaleValueX(size.cx, dpi);
    unscaled.cy = UnscaleValueY(size.cy, dpi);
    return unscaled;
}

POINT DPIManager::ScalePoint(const POINT& pt, const DPIScaleInfo& dpi) const {
    POINT scaled;
    scaled.x = ScaleValueX(pt.x, dpi);
    scaled.y = ScaleValueY(pt.y, dpi);
    return scaled;
}

POINT DPIManager::UnscalePoint(const POINT& pt, const DPIScaleInfo& dpi) const {
    POINT unscaled;
    unscaled.x = UnscaleValueX(pt.x, dpi);
    unscaled.y = UnscaleValueY(pt.y, dpi);
    return unscaled;
}

void DPIManager::RegisterDPIChangeCallback(HWND hwnd, DPIChangeCallback callback) {
    m_dpiChangeCallbacks[hwnd] = callback;
}

void DPIManager::UnregisterDPIChangeCallback(HWND hwnd) {
    m_dpiChangeCallbacks.erase(hwnd);
}

void DPIManager::HandleDPIChange(HWND hwnd, WPARAM wParam, LPARAM lParam) {
    if (!hwnd) {
        return;
    }
    
    // Get old DPI
    DPIScaleInfo oldDPI = GetDPIForWindow(hwnd);
    
    // Get new DPI from message
    UINT newDpiValue = HIWORD(wParam);
    DPIScaleInfo newDPI = CalculateDPIScaleInfo(newDpiValue);
    
    // Update cache
    m_windowDPICache[hwnd] = newDPI;
    
    // Get suggested rect for new DPI
    RECT* pRect = reinterpret_cast<RECT*>(lParam);
    if (pRect) {
        // Resize and reposition window
        SetWindowPos(hwnd, nullptr,
            pRect->left, pRect->top,
            pRect->right - pRect->left,
            pRect->bottom - pRect->top,
            SWP_NOZORDER | SWP_NOACTIVATE);
    }
    
    // Notify callback
    auto it = m_dpiChangeCallbacks.find(hwnd);
    if (it != m_dpiChangeCallbacks.end()) {
        it->second(hwnd, oldDPI, newDPI);
    }
}

} // namespace SDK
