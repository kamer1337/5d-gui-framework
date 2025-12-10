#include "../../include/SDK/SDK.h"
#include "../../include/SDK/WindowHook.h"
#include "../../include/SDK/WindowManager.h"
#include <cstdlib>
#include <ctime>

namespace SDK {

static bool g_bInitialized = false;

bool Initialize() {
    if (g_bInitialized) {
        return true;
    }
    
    // Initialize random number generator for particle system
    srand((unsigned int)time(nullptr));
    
    // Initialize window hook
    if (!WindowHook::GetInstance().Initialize()) {
        return false;
    }
    
    // Initialize window manager
    if (!WindowManager::GetInstance().Initialize()) {
        WindowHook::GetInstance().Shutdown();
        return false;
    }
    
    // Register callback for automatic window registration
    WindowHook::GetInstance().RegisterCreateCallback([](HWND hwnd) {
        WindowManager::GetInstance().RegisterWindow(hwnd);
    });
    
    g_bInitialized = true;
    return true;
}

void Shutdown() {
    if (!g_bInitialized) {
        return;
    }
    
    WindowManager::GetInstance().Shutdown();
    WindowHook::GetInstance().Shutdown();
    
    g_bInitialized = false;
}

const char* GetVersion() {
    return "5D GUI SDK v1.0.0";
}

bool IsInitialized() {
    return g_bInitialized;
}

} // namespace SDK
