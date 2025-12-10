/**
 * 5D GUI SDK Toolbar Demo Application
 * Demonstrates Toolbar widget with horizontal, vertical, and auto-hide modes
 */

#include "../include/SDK/SDK.h"
#include "../include/SDK/Toolbar.h"
#include <vector>
#include <string>

// Global state
HWND g_mainWindow = nullptr;
std::shared_ptr<SDK::WidgetManager> g_widgetManager;
std::shared_ptr<SDK::Toolbar> g_horizontalToolbar;
std::shared_ptr<SDK::Toolbar> g_verticalToolbar;
std::shared_ptr<SDK::Toolbar> g_autoHideToolbar;
std::shared_ptr<SDK::Tooltip> g_tooltip;
std::wstring g_statusText = L"Ready";

// Window procedure
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
            
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            
            // Clear background
            RECT clientRect;
            GetClientRect(hwnd, &clientRect);
            HBRUSH bgBrush = CreateSolidBrush(RGB(240, 240, 245));
            FillRect(hdc, &clientRect, bgBrush);
            DeleteObject(bgBrush);
            
            // Draw title
            SetBkMode(hdc, TRANSPARENT);
            SetTextColor(hdc, RGB(50, 50, 50));
            
            RECT titleRect = {200, 100, clientRect.right - 200, 140};
            HFONT titleFont = CreateFontW(24, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
            HFONT oldFont = (HFONT)SelectObject(hdc, titleFont);
            
            DrawTextW(hdc, L"5D GUI SDK - Toolbar Demo", -1, &titleRect,
                DT_CENTER | DT_VCENTER | DT_SINGLELINE);
            
            SelectObject(hdc, oldFont);
            DeleteObject(titleFont);
            
            // Draw instructions
            RECT infoRect = {200, 160, clientRect.right - 200, 300};
            const wchar_t* infoText =
                L"Toolbar Features Demonstrated:\n\n"
                L"• Horizontal Toolbar (Top): Standard horizontal layout\n"
                L"• Vertical Toolbar (Left): Vertical orientation with items stacked\n"
                L"• Auto-Hide Toolbar (Right): Hides automatically, shows on hover\n\n"
                L"Hover near the right edge to see the auto-hide toolbar!\n"
                L"Click toolbar items to see status updates.";
            
            DrawTextW(hdc, infoText, -1, &infoRect, DT_LEFT | DT_WORDBREAK);
            
            // Draw status text
            RECT statusRect = {200, clientRect.bottom - 80, clientRect.right - 200, clientRect.bottom - 40};
            HBRUSH statusBrush = CreateSolidBrush(RGB(255, 255, 255));
            FillRect(hdc, &statusRect, statusBrush);
            DeleteObject(statusBrush);
            
            SetTextColor(hdc, RGB(0, 100, 0));
            DrawTextW(hdc, (L"Status: " + g_statusText).c_str(), -1, &statusRect,
                DT_CENTER | DT_VCENTER | DT_SINGLELINE);
            
            // Render all widgets
            if (g_widgetManager) {
                g_widgetManager->RenderAll(hdc);
            }
            
            EndPaint(hwnd, &ps);
            return 0;
        }
        
        case WM_MOUSEMOVE: {
            int x = LOWORD(lParam);
            int y = HIWORD(lParam);
            
            if (g_widgetManager) {
                g_widgetManager->HandleMouseMove(x, y);
            }
            
            InvalidateRect(hwnd, nullptr, FALSE);
            return 0;
        }
        
        case WM_LBUTTONDOWN: {
            int x = LOWORD(lParam);
            int y = HIWORD(lParam);
            
            if (g_widgetManager) {
                g_widgetManager->HandleMouseDown(x, y);
            }
            
            InvalidateRect(hwnd, nullptr, FALSE);
            return 0;
        }
        
        case WM_LBUTTONUP: {
            int x = LOWORD(lParam);
            int y = HIWORD(lParam);
            
            if (g_widgetManager) {
                g_widgetManager->HandleMouseUp(x, y);
            }
            
            InvalidateRect(hwnd, nullptr, FALSE);
            return 0;
        }
    }
    
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void OnToolbarItemClick(int itemId) {
    switch (itemId) {
        case 1:
            g_statusText = L"File clicked";
            break;
        case 2:
            g_statusText = L"Edit clicked";
            break;
        case 3:
            g_statusText = L"View clicked";
            break;
        case 4:
            g_statusText = L"Tools clicked";
            break;
        case 5:
            g_statusText = L"Help clicked";
            break;
        case 10:
            g_statusText = L"New clicked";
            break;
        case 11:
            g_statusText = L"Open clicked";
            break;
        case 12:
            g_statusText = L"Save clicked";
            break;
        case 13:
            g_statusText = L"Settings clicked";
            break;
        case 20:
            g_statusText = L"Home clicked";
            break;
        case 21:
            g_statusText = L"Search clicked";
            break;
        case 22:
            g_statusText = L"Profile clicked";
            break;
        case 23:
            g_statusText = L"Exit clicked";
            break;
        default:
            g_statusText = L"Unknown item clicked";
            break;
    }
    
    InvalidateRect(g_mainWindow, nullptr, FALSE);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // Initialize SDK
    if (!SDK::Initialize()) {
        MessageBoxW(nullptr, L"Failed to initialize 5D GUI SDK", L"Error", MB_ICONERROR);
        return -1;
    }
    
    // Register window class
    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = L"5DGUIToolbarDemo";
    
    if (!RegisterClassExW(&wc)) {
        MessageBoxW(nullptr, L"Window Registration Failed!", L"Error", MB_ICONERROR);
        SDK::Shutdown();
        return -1;
    }
    
    // Create main window
    g_mainWindow = CreateWindowExW(
        WS_EX_LAYERED,
        L"5DGUIToolbarDemo",
        L"5D GUI SDK - Toolbar Demo",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 1000, 700,
        nullptr, nullptr, hInstance, nullptr
    );
    
    if (!g_mainWindow) {
        MessageBoxW(nullptr, L"Window Creation Failed!", L"Error", MB_ICONERROR);
        SDK::Shutdown();
        return -1;
    }
    
    // Register window with SDK
    auto mainWindow = SDK::WindowManager::GetInstance().RegisterWindow(g_mainWindow);
    if (mainWindow) {
        auto theme = std::make_shared<SDK::Theme>(SDK::Theme::CreateModernTheme());
        mainWindow->SetTheme(theme);
        mainWindow->SetDepth(SDK::WindowDepth::FOREGROUND);
        mainWindow->UpdateAppearance();
    }
    
    // Create widget manager
    g_widgetManager = std::make_shared<SDK::WidgetManager>();
    
    // Create horizontal toolbar at top
    g_horizontalToolbar = std::make_shared<SDK::Toolbar>();
    g_horizontalToolbar->SetBounds(0, 0, 1000, 50);
    g_horizontalToolbar->SetOrientation(SDK::Toolbar::Orientation::HORIZONTAL);
    g_horizontalToolbar->SetBackgroundColor(SDK::Color(45, 45, 48, 255));
    g_horizontalToolbar->SetItemSize(100, 40);
    g_horizontalToolbar->AddItem(1, L"File", L"File operations");
    g_horizontalToolbar->AddItem(2, L"Edit", L"Edit operations");
    g_horizontalToolbar->AddItem(3, L"View", L"View options");
    g_horizontalToolbar->AddSeparator();
    g_horizontalToolbar->AddItem(4, L"Tools", L"Tools menu");
    g_horizontalToolbar->AddItem(5, L"Help", L"Help menu");
    g_horizontalToolbar->SetItemClickCallback(OnToolbarItemClick);
    g_widgetManager->AddWidget(g_horizontalToolbar);
    
    // Create vertical toolbar on left
    g_verticalToolbar = std::make_shared<SDK::Toolbar>();
    g_verticalToolbar->SetBounds(0, 50, 120, 650);
    g_verticalToolbar->SetOrientation(SDK::Toolbar::Orientation::VERTICAL);
    g_verticalToolbar->SetBackgroundColor(SDK::Color(55, 55, 60, 255));
    g_verticalToolbar->SetItemColor(SDK::Color(70, 70, 75, 255));
    g_verticalToolbar->SetItemSize(110, 50);
    g_verticalToolbar->AddItem(10, L"New", L"Create new file");
    g_verticalToolbar->AddItem(11, L"Open", L"Open file");
    g_verticalToolbar->AddItem(12, L"Save", L"Save file");
    g_verticalToolbar->AddSeparator();
    g_verticalToolbar->AddItem(13, L"Settings", L"Application settings");
    g_verticalToolbar->SetItemClickCallback(OnToolbarItemClick);
    g_widgetManager->AddWidget(g_verticalToolbar);
    
    // Create auto-hide toolbar on right edge
    g_autoHideToolbar = std::make_shared<SDK::Toolbar>();
    g_autoHideToolbar->SetBounds(920, 50, 80, 650);
    g_autoHideToolbar->SetOrientation(SDK::Toolbar::Orientation::VERTICAL);
    g_autoHideToolbar->SetAutoHide(true);
    g_autoHideToolbar->SetAutoHideDelay(0.5f);
    g_autoHideToolbar->SetTriggerZoneSize(5);
    g_autoHideToolbar->SetBackgroundColor(SDK::Color(60, 60, 65, 200));
    g_autoHideToolbar->SetItemSize(70, 60);
    g_autoHideToolbar->AddItem(20, L"Home", L"Go home");
    g_autoHideToolbar->AddItem(21, L"Search", L"Search");
    g_autoHideToolbar->AddSeparator();
    g_autoHideToolbar->AddItem(22, L"Profile", L"User profile");
    g_autoHideToolbar->AddItem(23, L"Exit", L"Exit application");
    g_autoHideToolbar->SetItemClickCallback(OnToolbarItemClick);
    g_widgetManager->AddWidget(g_autoHideToolbar);
    
    // Create tooltip
    g_tooltip = std::make_shared<SDK::Tooltip>();
    g_tooltip->SetFadeEnabled(true);
    g_tooltip->SetFadeSpeed(8.0f);
    g_tooltip->SetShowDelay(0.2f);
    g_widgetManager->AddWidget(g_tooltip);
    
    // Show window
    ShowWindow(g_mainWindow, nCmdShow);
    UpdateWindow(g_mainWindow);
    
    // Main message loop
    MSG msg = {};
    DWORD lastTime = GetTickCount();
    
    while (true) {
        // Process messages
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                goto cleanup;
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        
        // Update animation
        DWORD currentTime = GetTickCount();
        float deltaTime = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;
        
        // Update widgets
        if (g_widgetManager) {
            g_widgetManager->UpdateAll(deltaTime);
        }
        
        // Update window manager
        SDK::WindowManager::GetInstance().Update(deltaTime);
        
        // Redraw
        InvalidateRect(g_mainWindow, nullptr, FALSE);
        
        // Sleep to prevent 100% CPU usage
        Sleep(16); // ~60 FPS
    }
    
cleanup:
    // Cleanup
    g_widgetManager.reset();
    SDK::Shutdown();
    
    return (int)msg.wParam;
}
