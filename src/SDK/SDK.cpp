#include "../../include/SDK/SDK.h"
#include "../../include/SDK/WindowHook.h"
#include "../../include/SDK/WindowManager.h"
#include "../../include/SDK/DPIManager.h"
#include "../../include/SDK/MonitorManager.h"
#include <cstdlib>
#include <ctime>
#include <cstdio>

namespace SDK {

static bool g_bInitialized = false;

bool Initialize() {
    if (g_bInitialized) {
        return true;
    }
    
    // Initialize random number generator for particle system
    srand((unsigned int)time(nullptr));
    
    // Initialize DPI manager (v2.0)
    if (!DPIManager::GetInstance().Initialize(DPIAwareness::PER_MONITOR_V2)) {
        return false;
    }
    
    // Initialize monitor manager (v2.0)
    if (!MonitorManager::GetInstance().Initialize()) {
        DPIManager::GetInstance().Shutdown();
        return false;
    }
    
    // Initialize window hook
    if (!WindowHook::GetInstance().Initialize()) {
        MonitorManager::GetInstance().Shutdown();
        DPIManager::GetInstance().Shutdown();
        return false;
    }
    
    // Initialize window manager
    if (!WindowManager::GetInstance().Initialize()) {
        WindowHook::GetInstance().Shutdown();
        MonitorManager::GetInstance().Shutdown();
        DPIManager::GetInstance().Shutdown();
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
    MonitorManager::GetInstance().Shutdown();
    DPIManager::GetInstance().Shutdown();
    
    g_bInitialized = false;
}

const char* GetVersion() {
    static char version[32];
    snprintf(version, sizeof(version), "5D GUI SDK v%d.%d.%d", 
             SDK_VERSION_MAJOR, SDK_VERSION_MINOR, SDK_VERSION_PATCH);
    return version;
}

bool IsInitialized() {
    return g_bInitialized;
}

} // namespace SDK
