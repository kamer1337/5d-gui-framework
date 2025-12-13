#pragma once

#include "Platform.h"

#if SDK_PLATFORM_LINUX && SDK_HAS_X11

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <string>
#include <memory>
#include <functional>
#include <vector>

namespace SDK {

// Forward declarations
class X11RenderBackend;

/**
 * WindowX11 - X11 window creation and management for Linux
 * Provides cross-platform window interface using X11
 */
class WindowX11 {
public:
    WindowX11();
    ~WindowX11();
    
    // Window creation
    bool Create(const std::wstring& title, int x, int y, int width, int height);
    void Destroy();
    
    // Window properties
    HWND GetHandle() const { return reinterpret_cast<HWND>(m_window); }
    Display* GetDisplay() const { return m_display; }
    Window GetX11Window() const { return m_window; }
    bool IsValid() const { return m_display != nullptr && m_window != 0; }
    
    // Window operations
    void Show();
    void Hide();
    void SetTitle(const std::wstring& title);
    void SetPosition(int x, int y);
    void SetSize(int width, int height);
    void GetPosition(int& x, int& y) const;
    void GetSize(int& width, int& height) const;
    
    // Event handling
    void ProcessEvents();
    bool HasPendingEvents() const;
    
    // Rendering
    void Invalidate();
    void BeginPaint();
    void EndPaint();
    
    // Callbacks
    void SetCloseCallback(std::function<void()> callback) { m_closeCallback = callback; }
    void SetPaintCallback(std::function<void()> callback) { m_paintCallback = callback; }
    void SetMouseMoveCallback(std::function<void(int, int)> callback) { m_mouseMoveCallback = callback; }
    void SetMouseButtonCallback(std::function<void(int, int, int, bool)> callback) { m_mouseButtonCallback = callback; }
    void SetKeyCallback(std::function<void(int, bool)> callback) { m_keyCallback = callback; }
    
    // Get render backend
    std::shared_ptr<X11RenderBackend> GetRenderBackend() const { return m_renderBackend; }
    
private:
    void InitializeX11();
    void ProcessEvent(XEvent& event);
    int XKeyToVirtualKey(KeySym keysym);
    
    Display* m_display;
    Window m_window;
    int m_screen;
    Atom m_wmDeleteWindow;
    bool m_shouldClose;
    
    int m_width;
    int m_height;
    
    std::shared_ptr<X11RenderBackend> m_renderBackend;
    
    // Callbacks
    std::function<void()> m_closeCallback;
    std::function<void()> m_paintCallback;
    std::function<void(int, int)> m_mouseMoveCallback;
    std::function<void(int, int, int, bool)> m_mouseButtonCallback; // x, y, button, pressed
    std::function<void(int, bool)> m_keyCallback; // keycode, pressed
};

/**
 * X11WindowManager - Manages X11 windows and event loop
 */
class X11WindowManager {
public:
    static X11WindowManager& GetInstance();
    
    // Window management
    std::shared_ptr<WindowX11> CreateWindow(const std::wstring& title, int x, int y, int width, int height);
    void DestroyWindow(std::shared_ptr<WindowX11> window);
    
    // Event loop
    void RunEventLoop();
    void ProcessEvents();
    bool ShouldQuit() const { return m_shouldQuit; }
    void Quit() { m_shouldQuit = true; }
    
private:
    X11WindowManager() : m_shouldQuit(false) {}
    X11WindowManager(const X11WindowManager&) = delete;
    X11WindowManager& operator=(const X11WindowManager&) = delete;
    
    std::vector<std::shared_ptr<WindowX11>> m_windows;
    bool m_shouldQuit;
};

} // namespace SDK

#endif // SDK_PLATFORM_LINUX && SDK_HAS_X11
