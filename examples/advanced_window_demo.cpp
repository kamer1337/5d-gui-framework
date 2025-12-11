/**
 * Advanced Window Features Demo
 * Demonstrates window grouping, snapping, and animations
 */

#include "SDK/SDK.h"

// Global variables
HINSTANCE g_hInstance;
std::shared_ptr<SDK::WindowGroup> g_windowGroup;
std::shared_ptr<SDK::WindowAnimation> g_mainWindowAnim;
std::shared_ptr<SDK::WindowAnimation> g_child1Anim;
std::shared_ptr<SDK::WindowAnimation> g_child2Anim;

HWND g_mainWindow = nullptr;
HWND g_childWindow1 = nullptr;
HWND g_childWindow2 = nullptr;

// Window procedure
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
            
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            
            // Get SDK window
            auto window = SDK::WindowManager::GetInstance().GetWindow(hwnd);
            if (window) {
                window->Render(hdc);
            }
            
            EndPaint(hwnd, &ps);
            return 0;
        }
        
        case WM_KEYDOWN:
            if (wParam == VK_ESCAPE) {
                PostQuitMessage(0);
            } else if (wParam == 'M') {
                // Minimize with animation
                if (hwnd == g_mainWindow && g_mainWindowAnim) {
                    g_mainWindowAnim->AnimateMinimize();
                }
            } else if (wParam == 'X') {
                // Maximize with animation
                if (hwnd == g_mainWindow && g_mainWindowAnim) {
                    g_mainWindowAnim->AnimateMaximize();
                }
            } else if (wParam == 'R') {
                // Restore with animation
                if (hwnd == g_mainWindow && g_mainWindowAnim) {
                    g_mainWindowAnim->AnimateRestore();
                }
            } else if (wParam == 'G') {
                // Toggle window grouping
                if (g_windowGroup) {
                    bool linked = g_windowGroup->IsLinkedMovementEnabled();
                    g_windowGroup->SetLinkedMovement(!linked);
                    
                    std::wstring msg = linked ? L"Group movement disabled" : L"Group movement enabled";
                    MessageBoxW(hwnd, msg.c_str(), L"Window Group", MB_OK);
                }
            } else if (wParam == 'S') {
                // Toggle snapping
                auto& snapping = SDK::WindowManager::GetInstance().GetSnapping();
                bool enabled = snapping.IsEdgeSnapEnabled();
                snapping.SetEdgeSnapEnabled(!enabled);
                snapping.SetMagneticWindowsEnabled(!enabled);
                
                std::wstring msg = enabled ? L"Snapping disabled" : L"Snapping enabled";
                MessageBoxW(hwnd, msg.c_str(), L"Window Snapping", MB_OK);
            }
            return 0;
            
        case WM_MOVING: {
            // Apply snapping during window move
            RECT* pRect = reinterpret_cast<RECT*>(lParam);
            if (pRect) {
                auto& snapping = SDK::WindowManager::GetInstance().GetSnapping();
                auto windows = SDK::WindowManager::GetInstance().GetWindowsByDepth();
                *pRect = snapping.ApplySnapping(hwnd, *pRect, windows);
            }
            return TRUE;
        }
    }
    
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

// Create a window with SDK enhancements
HWND CreateEnhancedWindow(const wchar_t* title, int x, int y, int width, int height,
                          SDK::WindowDepth depth, HWND parent = nullptr)
{
    // Create window
    HWND hwnd = CreateWindowExW(
        WS_EX_LAYERED,
        L"AdvancedWindowDemo",
        title,
        WS_OVERLAPPEDWINDOW,
        x, y, width, height,
        parent,
        nullptr,
        g_hInstance,
        nullptr
    );
    
    if (!hwnd) return nullptr;
    
    // Register with SDK
    auto window = SDK::WindowManager::GetInstance().RegisterWindow(hwnd);
    if (window) {
        // Apply theme
        auto theme = std::make_shared<SDK::Theme>(SDK::Theme::CreateDarkTheme());
        window->SetTheme(theme);
        window->SetDepth(depth);
        window->SetRoundedCorners(true, 12);
        
        // Add informational label
        auto label = std::make_shared<SDK::Label>(std::wstring(L"Window: ") + title);
        label->SetPosition(20, 20);
        label->SetSize(300, 30);
        window->AddWidget(label);
        
        // Add instruction text
        auto instructions = std::make_shared<SDK::Label>(
            L"Press: M=Minimize, X=Maximize, R=Restore\n"
            L"       G=Toggle Group, S=Toggle Snap\n"
            L"       ESC=Exit\n"
            L"Drag windows to see snapping!"
        );
        instructions->SetPosition(20, 60);
        instructions->SetSize(400, 100);
        window->AddWidget(instructions);
    }
    
    return hwnd;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow)
{
    g_hInstance = hInstance;
    
    // Initialize SDK
    SDK::Initialize();
    
    // Register window class
    WNDCLASSEXW wc = {0};
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = L"AdvancedWindowDemo";
    
    if (!RegisterClassExW(&wc)) {
        MessageBoxW(nullptr, L"Failed to register window class", L"Error", MB_OK);
        return 1;
    }
    
    // Create main window
    g_mainWindow = CreateEnhancedWindow(
        L"Main Window (Advanced Features Demo)",
        100, 100, 600, 400,
        SDK::WindowDepth::FOREGROUND
    );
    
    // Create child windows
    g_childWindow1 = CreateEnhancedWindow(
        L"Child Window 1",
        750, 100, 400, 300,
        SDK::WindowDepth::MID_FRONT
    );
    
    g_childWindow2 = CreateEnhancedWindow(
        L"Child Window 2",
        100, 550, 400, 300,
        SDK::WindowDepth::MID_FRONT
    );
    
    if (!g_mainWindow || !g_childWindow1 || !g_childWindow2) {
        MessageBoxW(nullptr, L"Failed to create windows", L"Error", MB_OK);
        return 1;
    }
    
    // Create window group
    g_windowGroup = std::make_shared<SDK::WindowGroup>(L"Demo Group");
    auto mainWin = SDK::WindowManager::GetInstance().GetWindow(g_mainWindow);
    auto child1Win = SDK::WindowManager::GetInstance().GetWindow(g_childWindow1);
    auto child2Win = SDK::WindowManager::GetInstance().GetWindow(g_childWindow2);
    
    g_windowGroup->AddWindow(mainWin);
    g_windowGroup->AddWindow(child1Win);
    g_windowGroup->AddWindow(child2Win);
    g_windowGroup->SetLinkedMovement(true);
    g_windowGroup->SetSynchronizedDepth(true);
    
    // Create window animations
    g_mainWindowAnim = std::make_shared<SDK::WindowAnimation>(g_mainWindow);
    g_mainWindowAnim->SetAnimationDuration(300);
    g_mainWindowAnim->SetEasingType(SDK::WindowAnimation::EasingType::EASE_OUT);
    
    g_child1Anim = std::make_shared<SDK::WindowAnimation>(g_childWindow1);
    g_child1Anim->SetAnimationDuration(300);
    
    g_child2Anim = std::make_shared<SDK::WindowAnimation>(g_childWindow2);
    g_child2Anim->SetAnimationDuration(300);
    
    // Configure snapping
    auto& snapping = SDK::WindowManager::GetInstance().GetSnapping();
    snapping.SetEdgeSnapEnabled(true);
    snapping.SetMagneticWindowsEnabled(true);
    snapping.SetEdgeSnapThreshold(20);
    snapping.SetWindowSnapThreshold(15);
    
    // Show windows
    ShowWindow(g_mainWindow, nCmdShow);
    ShowWindow(g_childWindow1, SW_SHOW);
    ShowWindow(g_childWindow2, SW_SHOW);
    
    UpdateWindow(g_mainWindow);
    UpdateWindow(g_childWindow1);
    UpdateWindow(g_childWindow2);
    
    // Message loop
    MSG msg = {0};
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
        
        // Update animations
        if (g_mainWindowAnim) g_mainWindowAnim->Update();
        if (g_child1Anim) g_child1Anim->Update();
        if (g_child2Anim) g_child2Anim->Update();
        
        // Update window manager
        SDK::WindowManager::GetInstance().Update(0.016f);
    }
    
    // Cleanup
    SDK::Shutdown();
    
    return static_cast<int>(msg.wParam);
}
