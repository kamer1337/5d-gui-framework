#include "SDK/SDK.h"
#include <string>
#include <sstream>

// Global variables
HWND g_hwnd = nullptr;
SDK::DPIScaleInfo g_currentDPI;
const SDK::MonitorInfo* g_currentMonitor = nullptr;

// Forward declarations
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
void UpdateMonitorInfo(HWND hwnd);
void DrawDPIInfo(HDC hdc, const RECT& clientRect);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    // Initialize SDK with DPI and monitor support
    if (!SDK::Initialize()) {
        MessageBoxW(nullptr, L"Failed to initialize SDK", L"Error", MB_OK | MB_ICONERROR);
        return -1;
    }
    
    // Get DPI and Monitor managers
    auto& dpiMgr = SDK::DPIManager::GetInstance();
    auto& monitorMgr = SDK::MonitorManager::GetInstance();
    
    // Register window class
    const wchar_t* className = L"DPIMultiMonitorDemo";
    WNDCLASSEXW wc = {0};
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = className;
    
    if (!RegisterClassExW(&wc)) {
        MessageBoxW(nullptr, L"Failed to register window class", L"Error", MB_OK | MB_ICONERROR);
        SDK::Shutdown();
        return -1;
    }
    
    // Get system DPI
    SDK::DPIScaleInfo systemDPI = dpiMgr.GetSystemDPI();
    
    // Create window with DPI-scaled dimensions
    int windowWidth = dpiMgr.ScaleValueX(800, systemDPI);
    int windowHeight = dpiMgr.ScaleValueY(600, systemDPI);
    
    g_hwnd = CreateWindowExW(
        WS_EX_LAYERED,
        className,
        L"DPI & Multi-Monitor Demo - 5D GUI SDK v2.0.0",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        windowWidth, windowHeight,
        nullptr, nullptr, hInstance, nullptr
    );
    
    if (!g_hwnd) {
        MessageBoxW(nullptr, L"Failed to create window", L"Error", MB_OK | MB_ICONERROR);
        SDK::Shutdown();
        return -1;
    }
    
    // Register with SDK
    auto window = SDK::WindowManager::GetInstance().RegisterWindow(g_hwnd);
    if (!window) {
        MessageBoxW(nullptr, L"Failed to register window with SDK", L"Error", MB_OK | MB_ICONERROR);
        DestroyWindow(g_hwnd);
        SDK::Shutdown();
        return -1;
    }
    
    // Apply modern theme
    auto theme = std::make_shared<SDK::Theme>(SDK::Theme::CreateModernTheme());
    window->SetTheme(theme);
    window->SetDepth(SDK::WindowDepth::FOREGROUND);
    window->UpdateAppearance();
    
    // Track this window for monitor changes
    monitorMgr.TrackWindow(g_hwnd);
    
    // Register DPI change callback
    dpiMgr.RegisterDPIChangeCallback(g_hwnd, [window](HWND hwnd, const SDK::DPIScaleInfo& oldDPI, const SDK::DPIScaleInfo& newDPI) {
        window->HandleDPIChange(oldDPI, newDPI);
        UpdateMonitorInfo(hwnd);
    });
    
    // Register monitor change callback
    monitorMgr.RegisterMonitorChangeCallback(g_hwnd, [window](HWND hwnd, HMONITOR oldMonitor, HMONITOR newMonitor) {
        window->HandleMonitorChange(oldMonitor, newMonitor);
        UpdateMonitorInfo(hwnd);
    });
    
    // Add widgets to demonstrate DPI scaling
    auto label = std::make_shared<SDK::Label>(L"DPI & Multi-Monitor Features:");
    label->SetPosition(20, 80);
    label->SetSize(760, 30);
    label->SetFontSize(18);
    label->SetFontWeight(700);
    window->AddWidget(label);
    
    auto infoLabel = std::make_shared<SDK::Label>(L"Move this window between monitors to see DPI scaling and theme changes!");
    infoLabel->SetPosition(20, 120);
    infoLabel->SetSize(760, 40);
    infoLabel->SetTextColor(SDK::Color(100, 100, 100));
    window->AddWidget(infoLabel);
    
    // Add buttons to test features
    auto migrateBtn = std::make_shared<SDK::Button>(L"Migrate to Primary Monitor");
    migrateBtn->SetPosition(20, 180);
    migrateBtn->SetSize(250, 40);
    migrateBtn->SetEventCallback([](SDK::Widget* w, SDK::WidgetEvent e, void* data) {
        if (e == SDK::WidgetEvent::CLICK) {
            auto& monitorMgr = SDK::MonitorManager::GetInstance();
            const SDK::MonitorInfo* primary = monitorMgr.GetPrimaryMonitor();
            if (primary) {
                monitorMgr.MigrateWindowToMonitor(g_hwnd, primary->hMonitor, true);
            }
        }
    });
    window->AddWidget(migrateBtn);
    
    auto refreshBtn = std::make_shared<SDK::Button>(L"Refresh Monitor List");
    refreshBtn->SetPosition(290, 180);
    refreshBtn->SetSize(250, 40);
    refreshBtn->SetEventCallback([](SDK::Widget* w, SDK::WidgetEvent e, void* data) {
        if (e == SDK::WidgetEvent::CLICK) {
            SDK::MonitorManager::GetInstance().RefreshMonitors();
            UpdateMonitorInfo(g_hwnd);
            InvalidateRect(g_hwnd, nullptr, TRUE);
        }
    });
    window->AddWidget(refreshBtn);
    
    // Initialize monitor info
    UpdateMonitorInfo(g_hwnd);
    
    // Show window
    ShowWindow(g_hwnd, nCmdShow);
    UpdateWindow(g_hwnd);
    
    // Message loop
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
        
        // Update monitor manager to detect window movement
        monitorMgr.Update();
    }
    
    // Cleanup
    dpiMgr.UnregisterDPIChangeCallback(g_hwnd);
    monitorMgr.UnregisterMonitorChangeCallback(g_hwnd);
    monitorMgr.UntrackWindow(g_hwnd);
    SDK::Shutdown();
    
    return static_cast<int>(msg.wParam);
}

void UpdateMonitorInfo(HWND hwnd) {
    auto& dpiMgr = SDK::DPIManager::GetInstance();
    auto& monitorMgr = SDK::MonitorManager::GetInstance();
    
    g_currentDPI = dpiMgr.GetDPIForWindow(hwnd);
    g_currentMonitor = monitorMgr.GetMonitorForWindow(hwnd);
}

void DrawDPIInfo(HDC hdc, const RECT& clientRect) {
    auto& dpiMgr = SDK::DPIManager::GetInstance();
    auto& monitorMgr = SDK::MonitorManager::GetInstance();
    
    // Get current info
    UpdateMonitorInfo(g_hwnd);
    
    // Draw background for info panel
    RECT infoRect = {20, 240, clientRect.right - 20, clientRect.bottom - 20};
    SDK::Renderer::DrawRoundedRect(hdc, infoRect, 8, 
        SDK::Color(245, 245, 245), SDK::Color(200, 200, 200), 1);
    
    // Prepare text
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(0, 0, 0));
    
    HFONT hFont = CreateFontW(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
    HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);
    
    int yPos = 260;
    int lineHeight = 25;
    
    // DPI Information
    std::wstringstream ss;
    ss << L"DPI Information:";
    TextOutW(hdc, 40, yPos, ss.str().c_str(), (int)ss.str().length());
    yPos += lineHeight;
    
    ss.str(L"");
    ss << L"  Current DPI: " << g_currentDPI.dpi << L" (" << (int)(g_currentDPI.scaleX * 100) << L"%)";
    TextOutW(hdc, 40, yPos, ss.str().c_str(), (int)ss.str().length());
    yPos += lineHeight;
    
    ss.str(L"");
    ss << L"  Scale Factor: " << g_currentDPI.scaleX << L"x";
    TextOutW(hdc, 40, yPos, ss.str().c_str(), (int)ss.str().length());
    yPos += lineHeight;
    
    SDK::DPIScaleInfo systemDPI = dpiMgr.GetSystemDPI();
    ss.str(L"");
    ss << L"  System DPI: " << systemDPI.dpi;
    TextOutW(hdc, 40, yPos, ss.str().c_str(), (int)ss.str().length());
    yPos += lineHeight + 10;
    
    // Monitor Information
    ss.str(L"");
    ss << L"Monitor Information:";
    TextOutW(hdc, 40, yPos, ss.str().c_str(), (int)ss.str().length());
    yPos += lineHeight;
    
    int monitorCount = monitorMgr.GetMonitorCount();
    ss.str(L"");
    ss << L"  Total Monitors: " << monitorCount;
    TextOutW(hdc, 40, yPos, ss.str().c_str(), (int)ss.str().length());
    yPos += lineHeight;
    
    if (g_currentMonitor) {
        ss.str(L"");
        ss << L"  Current Monitor: " << g_currentMonitor->name.c_str();
        TextOutW(hdc, 40, yPos, ss.str().c_str(), (int)ss.str().length());
        yPos += lineHeight;
        
        ss.str(L"");
        ss << L"  Resolution: " << (g_currentMonitor->bounds.right - g_currentMonitor->bounds.left) 
           << L" x " << (g_currentMonitor->bounds.bottom - g_currentMonitor->bounds.top);
        TextOutW(hdc, 40, yPos, ss.str().c_str(), (int)ss.str().length());
        yPos += lineHeight;
        
        ss.str(L"");
        ss << L"  Refresh Rate: " << g_currentMonitor->refreshRate << L" Hz";
        TextOutW(hdc, 40, yPos, ss.str().c_str(), (int)ss.str().length());
        yPos += lineHeight;
        
        ss.str(L"");
        ss << L"  Primary: " << (g_currentMonitor->isPrimary ? L"Yes" : L"No");
        TextOutW(hdc, 40, yPos, ss.str().c_str(), (int)ss.str().length());
        yPos += lineHeight;
    }
    
    SelectObject(hdc, hOldFont);
    DeleteObject(hFont);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    auto window = SDK::WindowManager::GetInstance().GetWindow(hwnd);
    
    switch (msg) {
        case WM_DPICHANGED:
            // Handle DPI change
            SDK::DPIManager::GetInstance().HandleDPIChange(hwnd, wParam, lParam);
            return 0;
            
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            
            RECT clientRect;
            GetClientRect(hwnd, &clientRect);
            
            // Let SDK render the window
            if (window) {
                window->Render(hdc);
            }
            
            // Draw DPI and monitor info
            DrawDPIInfo(hdc, clientRect);
            
            EndPaint(hwnd, &ps);
            return 0;
        }
        
        case WM_MOUSEMOVE:
            if (window) {
                int x = LOWORD(lParam);
                int y = HIWORD(lParam);
                window->HandleWidgetMouseMove(x, y);
            }
            return 0;
            
        case WM_LBUTTONDOWN:
            if (window) {
                int x = LOWORD(lParam);
                int y = HIWORD(lParam);
                window->HandleWidgetMouseDown(x, y, 0);
                InvalidateRect(hwnd, nullptr, TRUE);
            }
            return 0;
            
        case WM_LBUTTONUP:
            if (window) {
                int x = LOWORD(lParam);
                int y = HIWORD(lParam);
                window->HandleWidgetMouseUp(x, y, 0);
                InvalidateRect(hwnd, nullptr, TRUE);
            }
            return 0;
            
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
            
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
}
