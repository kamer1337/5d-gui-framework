#include "SDK/WindowX11.h"

#if SDK_PLATFORM_LINUX && SDK_HAS_X11

#include "SDK/X11RenderBackend.h"
#include <locale>
#include <cwchar>
#include <cstring>
#include <algorithm>

namespace SDK {

// Helper function to convert wstring to UTF-8 (replaces deprecated std::wstring_convert)
static std::string WStringToUTF8(const std::wstring& wstr) {
    if (wstr.empty()) {
        return std::string();
    }
    
    std::string result;
    result.reserve(wstr.size() * 4); // Reserve worst case (4 bytes per UTF-8 char)
    
    for (wchar_t wc : wstr) {
        if (wc < 0x80) {
            // ASCII range (0x00-0x7F)
            result.push_back(static_cast<char>(wc));
        } else if (wc < 0x800) {
            // 2-byte UTF-8 (0x80-0x7FF)
            result.push_back(static_cast<char>(0xC0 | ((wc >> 6) & 0x1F)));
            result.push_back(static_cast<char>(0x80 | (wc & 0x3F)));
        } else if (wc < 0x10000) {
            // 3-byte UTF-8 (0x800-0xFFFF)
            result.push_back(static_cast<char>(0xE0 | ((wc >> 12) & 0x0F)));
            result.push_back(static_cast<char>(0x80 | ((wc >> 6) & 0x3F)));
            result.push_back(static_cast<char>(0x80 | (wc & 0x3F)));
        } else {
            // 4-byte UTF-8 (0x10000-0x10FFFF)
            result.push_back(static_cast<char>(0xF0 | ((wc >> 18) & 0x07)));
            result.push_back(static_cast<char>(0x80 | ((wc >> 12) & 0x3F)));
            result.push_back(static_cast<char>(0x80 | ((wc >> 6) & 0x3F)));
            result.push_back(static_cast<char>(0x80 | (wc & 0x3F)));
        }
    }
    
    return result;
}

WindowX11::WindowX11()
    : m_display(nullptr)
    , m_window(0)
    , m_screen(0)
    , m_wmDeleteWindow(0)
    , m_shouldClose(false)
    , m_width(0)
    , m_height(0)
{
}

WindowX11::~WindowX11()
{
    Destroy();
}

void WindowX11::InitializeX11()
{
    if (m_display) {
        return;
    }
    
    m_display = XOpenDisplay(nullptr);
    if (!m_display) {
        return;
    }
    
    m_screen = DefaultScreen(m_display);
}

bool WindowX11::Create(const std::wstring& title, int x, int y, int width, int height)
{
    InitializeX11();
    
    if (!m_display) {
        return false;
    }
    
    m_width = width;
    m_height = height;
    
    // Create window
    Window root = RootWindow(m_display, m_screen);
    
    XSetWindowAttributes attrs;
    attrs.background_pixel = WhitePixel(m_display, m_screen);
    attrs.border_pixel = BlackPixel(m_display, m_screen);
    attrs.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask | 
                       ButtonPressMask | ButtonReleaseMask | 
                       PointerMotionMask | StructureNotifyMask;
    
    m_window = XCreateWindow(
        m_display, root,
        x, y, width, height,
        0, // border width
        CopyFromParent, // depth
        InputOutput, // class
        CopyFromParent, // visual
        CWBackPixel | CWBorderPixel | CWEventMask,
        &attrs
    );
    
    if (!m_window) {
        return false;
    }
    
    // Set window title
    SetTitle(title);
    
    // Enable window close event
    m_wmDeleteWindow = XInternAtom(m_display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(m_display, m_window, &m_wmDeleteWindow, 1);
    
    // Create render backend
    m_renderBackend = std::make_shared<X11RenderBackend>();
    m_renderBackend->Initialize(GetHandle());
    
    return true;
}

void WindowX11::Destroy()
{
    if (m_renderBackend) {
        m_renderBackend->Shutdown();
        m_renderBackend.reset();
    }
    
    if (m_window && m_display) {
        XDestroyWindow(m_display, m_window);
        m_window = 0;
    }
    
    if (m_display) {
        XCloseDisplay(m_display);
        m_display = nullptr;
    }
}

void WindowX11::Show()
{
    if (m_display && m_window) {
        XMapWindow(m_display, m_window);
        XFlush(m_display);
    }
}

void WindowX11::Hide()
{
    if (m_display && m_window) {
        XUnmapWindow(m_display, m_window);
        XFlush(m_display);
    }
}

void WindowX11::SetTitle(const std::wstring& title)
{
    if (!m_display || !m_window) {
        return;
    }
    
    // Convert wstring to UTF-8
    std::string utf8Title = WStringToUTF8(title);
    
    XStoreName(m_display, m_window, utf8Title.c_str());
    XFlush(m_display);
}

void WindowX11::SetPosition(int x, int y)
{
    if (m_display && m_window) {
        XMoveWindow(m_display, m_window, x, y);
        XFlush(m_display);
    }
}

void WindowX11::SetSize(int width, int height)
{
    if (m_display && m_window) {
        m_width = width;
        m_height = height;
        XResizeWindow(m_display, m_window, width, height);
        XFlush(m_display);
    }
}

void WindowX11::GetPosition(int& x, int& y) const
{
    if (!m_display || !m_window) {
        x = y = 0;
        return;
    }
    
    XWindowAttributes attrs;
    XGetWindowAttributes(m_display, m_window, &attrs);
    x = attrs.x;
    y = attrs.y;
}

void WindowX11::GetSize(int& width, int& height) const
{
    width = m_width;
    height = m_height;
}

void WindowX11::ProcessEvents()
{
    if (!m_display) {
        return;
    }
    
    while (XPending(m_display) > 0) {
        XEvent event;
        XNextEvent(m_display, &event);
        ProcessEvent(event);
    }
}

bool WindowX11::HasPendingEvents() const
{
    if (!m_display) {
        return false;
    }
    
    return XPending(m_display) > 0;
}

void WindowX11::Invalidate()
{
    if (!m_display || !m_window) {
        return;
    }
    
    // Send expose event to trigger repaint
    XEvent event;
    memset(&event, 0, sizeof(event));
    event.type = Expose;
    event.xexpose.window = m_window;
    event.xexpose.x = 0;
    event.xexpose.y = 0;
    event.xexpose.width = m_width;
    event.xexpose.height = m_height;
    event.xexpose.count = 0;
    
    XSendEvent(m_display, m_window, False, ExposureMask, &event);
    XFlush(m_display);
}

void WindowX11::BeginPaint()
{
    if (m_renderBackend) {
        m_renderBackend->BeginDraw();
    }
}

void WindowX11::EndPaint()
{
    if (m_renderBackend) {
        m_renderBackend->EndDraw();
    }
}

void WindowX11::ProcessEvent(XEvent& event)
{
    switch (event.type) {
        case Expose:
            if (event.xexpose.count == 0) {
                if (m_paintCallback) {
                    m_paintCallback();
                }
            }
            break;
            
        case ClientMessage:
            if (event.xclient.data.l[0] == static_cast<long>(m_wmDeleteWindow)) {
                m_shouldClose = true;
                if (m_closeCallback) {
                    m_closeCallback();
                }
            }
            break;
            
        case MotionNotify:
            if (m_mouseMoveCallback) {
                m_mouseMoveCallback(event.xmotion.x, event.xmotion.y);
            }
            break;
            
        case ButtonPress:
        case ButtonRelease: {
            bool pressed = (event.type == ButtonPress);
            int button = event.xbutton.button;
            if (m_mouseButtonCallback) {
                m_mouseButtonCallback(event.xbutton.x, event.xbutton.y, button, pressed);
            }
            break;
        }
        
        case KeyPress:
        case KeyRelease: {
            bool pressed = (event.type == KeyPress);
            KeySym keysym = XLookupKeysym(&event.xkey, 0);
            int virtualKey = XKeyToVirtualKey(keysym);
            if (m_keyCallback) {
                m_keyCallback(virtualKey, pressed);
            }
            break;
        }
        
        case ConfigureNotify:
            m_width = event.xconfigure.width;
            m_height = event.xconfigure.height;
            break;
    }
}

int WindowX11::XKeyToVirtualKey(KeySym keysym)
{
    // Map X11 keysyms to Windows virtual key codes for compatibility
    // This is a simplified mapping for common keys
    if (keysym >= XK_A && keysym <= XK_Z) {
        return 'A' + (keysym - XK_A);
    }
    if (keysym >= XK_a && keysym <= XK_z) {
        return 'A' + (keysym - XK_a);
    }
    if (keysym >= XK_0 && keysym <= XK_9) {
        return '0' + (keysym - XK_0);
    }
    
    switch (keysym) {
        case XK_Return: return 0x0D; // VK_RETURN
        case XK_Escape: return 0x1B; // VK_ESCAPE
        case XK_BackSpace: return 0x08; // VK_BACK
        case XK_Tab: return 0x09; // VK_TAB
        case XK_space: return 0x20; // VK_SPACE
        case XK_Left: return 0x25; // VK_LEFT
        case XK_Up: return 0x26; // VK_UP
        case XK_Right: return 0x27; // VK_RIGHT
        case XK_Down: return 0x28; // VK_DOWN
        default: return 0;
    }
}

// X11WindowManager implementation

X11WindowManager& X11WindowManager::GetInstance()
{
    static X11WindowManager instance;
    return instance;
}

std::shared_ptr<WindowX11> X11WindowManager::CreateWindow(const std::wstring& title, int x, int y, int width, int height)
{
    auto window = std::make_shared<WindowX11>();
    if (window->Create(title, x, y, width, height)) {
        m_windows.push_back(window);
        return window;
    }
    return nullptr;
}

void X11WindowManager::DestroyWindow(std::shared_ptr<WindowX11> window)
{
    if (!window) {
        return;
    }
    
    auto it = std::find(m_windows.begin(), m_windows.end(), window);
    if (it != m_windows.end()) {
        m_windows.erase(it);
    }
    
    window->Destroy();
}

void X11WindowManager::RunEventLoop()
{
    while (!m_shouldQuit && !m_windows.empty()) {
        ProcessEvents();
    }
}

void X11WindowManager::ProcessEvents()
{
    // Process events for all windows
    for (auto& window : m_windows) {
        if (window && window->IsValid()) {
            window->ProcessEvents();
        }
    }
    
    // Remove closed windows
    m_windows.erase(
        std::remove_if(m_windows.begin(), m_windows.end(),
            [](const std::shared_ptr<WindowX11>& window) {
                return !window || !window->IsValid();
            }),
        m_windows.end()
    );
    
    if (m_windows.empty()) {
        m_shouldQuit = true;
    }
}

} // namespace SDK

#endif // SDK_PLATFORM_LINUX && SDK_HAS_X11
