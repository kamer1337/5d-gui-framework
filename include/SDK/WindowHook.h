#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <functional>
#include <unordered_map>
#include <memory>

namespace SDK {

// Forward declarations
class Window;
class WindowManager;

// Hook callback type
using CreateWindowCallback = std::function<void(HWND)>;

/**
 * WindowHook - Core hooking system for CreateWindowExW
 * Intercepts window creation to enable custom rendering and theming
 */
class WindowHook {
public:
    static WindowHook& GetInstance();
    
    // Initialize hooking system
    bool Initialize();
    
    // Cleanup hooking system
    void Shutdown();
    
    // Register callback for window creation
    void RegisterCreateCallback(CreateWindowCallback callback);
    
    // Hooked CreateWindowExW function
    static HWND WINAPI HookedCreateWindowExW(
        DWORD dwExStyle,
        LPCWSTR lpClassName,
        LPCWSTR lpWindowName,
        DWORD dwStyle,
        int X,
        int Y,
        int nWidth,
        int nHeight,
        HWND hWndParent,
        HMENU hMenu,
        HINSTANCE hInstance,
        LPVOID lpParam
    );
    
    // Get original CreateWindowExW
    using CreateWindowExW_t = decltype(&CreateWindowExW);
    CreateWindowExW_t GetOriginalCreateWindowExW() const { return m_pOriginalCreateWindowExW; }
    
    bool IsHooked() const { return m_bIsHooked; }
    
private:
    WindowHook();
    ~WindowHook();
    
    WindowHook(const WindowHook&) = delete;
    WindowHook& operator=(const WindowHook&) = delete;
    
    // Install IAT hook
    bool InstallHook();
    void RemoveHook();
    
    CreateWindowExW_t m_pOriginalCreateWindowExW;
    bool m_bIsHooked;
    CreateWindowCallback m_createCallback;
};

} // namespace SDK
