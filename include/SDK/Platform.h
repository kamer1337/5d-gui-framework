#pragma once

/**
 * Platform Abstraction Layer
 * Provides platform detection and basic abstractions for cross-platform support
 */

// Platform detection
#if defined(_WIN32) || defined(_WIN64) || defined(__WIN32__) || defined(__WINDOWS__)
    #define SDK_PLATFORM_WINDOWS 1
    #define SDK_PLATFORM_LINUX 0
    #define SDK_PLATFORM_MACOS 0
#elif defined(__linux__) || defined(__gnu_linux__)
    #define SDK_PLATFORM_WINDOWS 0
    #define SDK_PLATFORM_LINUX 1
    #define SDK_PLATFORM_MACOS 0
#elif defined(__APPLE__) || defined(__MACH__)
    #define SDK_PLATFORM_WINDOWS 0
    #define SDK_PLATFORM_LINUX 0
    #define SDK_PLATFORM_MACOS 1
#else
    #error "Unsupported platform"
#endif

// Platform-specific includes
#if SDK_PLATFORM_WINDOWS
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif
    #include <windows.h>
#elif SDK_PLATFORM_LINUX
    // Only include X11 headers if available
    #ifdef __has_include
        #if __has_include(<X11/Xlib.h>)
            #include <X11/Xlib.h>
            #include <X11/Xutil.h>
            #define SDK_HAS_X11 1
        #else
            #define SDK_HAS_X11 0
        #endif
    #else
        // Older compilers without __has_include
        #define SDK_HAS_X11 0
    #endif
    
    #include <cstdint>
    // Define Windows-like types for Linux
    typedef void* HWND;
    typedef void* HINSTANCE;
    typedef void* HCURSOR;
    typedef void* HBITMAP;
    typedef void* HMENU;
    typedef void* LPVOID;
    typedef unsigned char BYTE;
    typedef unsigned long DWORD;
    typedef int BOOL;
    typedef void* HDC;
    typedef void* HBRUSH;
    typedef void* HPEN;
    typedef unsigned long COLORREF;  // RGB color value
    typedef const wchar_t* LPCWSTR;
    typedef struct tagRECT {
        long left;
        long top;
        long right;
        long bottom;
    } RECT;
    typedef struct tagPOINT {
        long x;
        long y;
    } POINT;
    typedef unsigned short WORD;
    typedef unsigned int UINT;
    typedef long LONG;
    typedef long long LONGLONG;
    #define TRUE 1
    #define FALSE 0
    
    // RGB macro for Linux
    #define RGB(r,g,b) ((COLORREF)(((BYTE)(r)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(b))<<16)))
#endif

namespace SDK {

/**
 * Platform abstraction utilities
 */
class Platform {
public:
    // Get platform name
    static const char* GetPlatformName() {
#if SDK_PLATFORM_WINDOWS
        return "Windows";
#elif SDK_PLATFORM_LINUX
        return "Linux";
#elif SDK_PLATFORM_MACOS
        return "macOS";
#else
        return "Unknown";
#endif
    }
    
    // Check if platform is Windows
    static constexpr bool IsWindows() {
        return SDK_PLATFORM_WINDOWS;
    }
    
    // Check if platform is Linux
    static constexpr bool IsLinux() {
        return SDK_PLATFORM_LINUX;
    }
    
    // Check if platform is macOS
    static constexpr bool IsMacOS() {
        return SDK_PLATFORM_MACOS;
    }
    
    // Get platform-specific features
    static bool SupportsWindowHooking() {
        return SDK_PLATFORM_WINDOWS; // Only Windows supports inline hooking
    }
    
    static bool SupportsLayeredWindows() {
        return SDK_PLATFORM_WINDOWS; // Windows layered windows
    }
    
    static bool SupportsHardwareAcceleration() {
        return true; // All platforms support some form of HW acceleration
    }
};

} // namespace SDK
