#pragma once


#include "Platform.h"
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
 * WindowHook - Enhanced hooking system with inline hooking support
 * Intercepts window creation to enable custom rendering and theming
 * Supports both IAT hooking and inline code patching
 */
class WindowHook {
public:
    enum class HookType {
        IAT,        // Import Address Table hooking
        INLINE      // Inline code patching (trampoline method)
    };
    
    static WindowHook& GetInstance();
    
    // Initialize hooking system with specified type
    bool Initialize(HookType type = HookType::INLINE);
    
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
    HookType GetHookType() const { return m_hookType; }
    
private:
    WindowHook();
    ~WindowHook();
    
    WindowHook(const WindowHook&) = delete;
    WindowHook& operator=(const WindowHook&) = delete;
    
    // Install IAT hook
    bool InstallIATHook();
    void RemoveIATHook();
    
    // Install inline hook (trampoline method)
    bool InstallInlineHook();
    void RemoveInlineHook();
    
    // Inline hook helpers
    bool CreateTrampoline(void* target, void* hook, void** trampoline);
    void FreeTrampoline();
    bool WriteMemory(void* address, const void* data, size_t size);
    
    // IAT hook helper
    PIMAGE_THUNK_DATA FindIATEntry(HMODULE hModule, const char* dllName, const char* functionName);
    
    CreateWindowExW_t m_pOriginalCreateWindowExW;
    void* m_pTrampoline;
    BYTE m_originalBytes[16];
    size_t m_originalBytesSize;
    bool m_bIsHooked;
    HookType m_hookType;
    CreateWindowCallback m_createCallback;
};

} // namespace SDK
