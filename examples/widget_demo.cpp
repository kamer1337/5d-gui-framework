/**
 * 5D GUI SDK Widget Demo Application
 * Demonstrates ProgressBar, Tooltip, and Prompt-based window generation
 */

#include "../include/SDK/SDK.h"
#include <vector>
#include <string>

// Global state
HWND g_mainWindow = nullptr;
std::shared_ptr<SDK::WidgetManager> g_widgetManager;
std::shared_ptr<SDK::ProgressBar> g_progressBar1;
std::shared_ptr<SDK::ProgressBar> g_progressBar2;
std::shared_ptr<SDK::ProgressBar> g_progressBar3;
std::shared_ptr<SDK::Tooltip> g_tooltip;
SDK::PromptWindowBuilder g_promptBuilder;
float g_animationTime = 0.0f;

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
            
            RECT titleRect = {20, 20, clientRect.right - 20, 60};
            HFONT titleFont = CreateFontW(24, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
            HFONT oldFont = (HFONT)SelectObject(hdc, titleFont);
            
            DrawTextW(hdc, L"5D GUI SDK - Widget Demo", -1, &titleRect,
                DT_LEFT | DT_VCENTER | DT_SINGLELINE);
            
            SelectObject(hdc, oldFont);
            DeleteObject(titleFont);
            
            // Draw description
            RECT descRect = {20, 60, clientRect.right - 20, 90};
            DrawTextW(hdc, L"ProgressBar, Tooltip, and Prompt-based Window Generation", -1, &descRect,
                DT_LEFT | DT_VCENTER | DT_SINGLELINE);
            
            // Render all widgets
            if (g_widgetManager) {
                g_widgetManager->RenderAll(hdc);
            }
            
            // Draw button labels
            RECT buttonRect1 = {20, 400, 200, 440};
            SDK::Renderer::DrawRoundedRect(hdc, buttonRect1, 6,
                SDK::Color(100, 149, 237, 255),
                SDK::Color(65, 105, 225, 255), 2);
            
            SetTextColor(hdc, RGB(255, 255, 255));
            DrawTextW(hdc, L"Animate Progress", -1, &buttonRect1,
                DT_CENTER | DT_VCENTER | DT_SINGLELINE);
            
            RECT buttonRect2 = {220, 400, 400, 440};
            SDK::Renderer::DrawRoundedRect(hdc, buttonRect2, 6,
                SDK::Color(138, 43, 226, 255),
                SDK::Color(75, 0, 130, 255), 2);
            
            DrawTextW(hdc, L"Show Tooltip", -1, &buttonRect2,
                DT_CENTER | DT_VCENTER | DT_SINGLELINE);
            
            RECT buttonRect3 = {420, 400, 600, 440};
            SDK::Renderer::DrawRoundedRect(hdc, buttonRect3, 6,
                SDK::Color(255, 140, 0, 255),
                SDK::Color(255, 69, 0, 255), 2);
            
            DrawTextW(hdc, L"Generate Window", -1, &buttonRect3,
                DT_CENTER | DT_VCENTER | DT_SINGLELINE);
            
            // Draw info text
            SetTextColor(hdc, RGB(100, 100, 100));
            RECT infoRect = {20, 460, clientRect.right - 20, clientRect.bottom - 20};
            const wchar_t* infoText =
                L"Features demonstrated:\n\n"
                L"• ProgressBar with gradient fill and smooth animations\n"
                L"• Tooltip with fade in/out effects and auto-positioning\n"
                L"• Prompt-based window generation (template system)\n\n"
                L"Hover over widgets to see tooltips!\n"
                L"Click 'Animate Progress' to see animated progress bars.\n"
                L"Click 'Generate Window' to create a window from a text prompt.";
            
            DrawTextW(hdc, infoText, -1, &infoRect, DT_LEFT | DT_WORDBREAK);
            
            EndPaint(hwnd, &ps);
            return 0;
        }
        
        case WM_MOUSEMOVE: {
            int x = LOWORD(lParam);
            int y = HIWORD(lParam);
            
            if (g_widgetManager) {
                g_widgetManager->HandleMouseMove(x, y);
            }
            
            // Show tooltip on progressbar hover
            if (g_progressBar1 && g_progressBar1->IsHovered() && g_tooltip) {
                if (!g_tooltip->IsShowing()) {
                    g_tooltip->SetText(L"Horizontal ProgressBar\nWith gradient animation");
                    g_tooltip->Show(x + 10, y + 10);
                    InvalidateRect(hwnd, nullptr, FALSE);
                }
            } else if (g_progressBar2 && g_progressBar2->IsHovered() && g_tooltip) {
                if (!g_tooltip->IsShowing()) {
                    g_tooltip->SetText(L"Vertical ProgressBar\nRotated orientation");
                    g_tooltip->Show(x + 10, y + 10);
                    InvalidateRect(hwnd, nullptr, FALSE);
                }
            } else if (g_progressBar3 && g_progressBar3->IsHovered() && g_tooltip) {
                if (!g_tooltip->IsShowing()) {
                    wchar_t buffer[128];
                    swprintf_s(buffer, 128, L"Animated ProgressBar\nCurrent: %.1f%%",
                        g_progressBar3->GetPercentage());
                    g_tooltip->SetText(buffer);
                    g_tooltip->Show(x + 10, y + 10);
                    InvalidateRect(hwnd, nullptr, FALSE);
                }
            } else if (g_tooltip && g_tooltip->IsShowing()) {
                // Hide tooltip if not hovering over any progressbar
                bool anyHovered = (g_progressBar1 && g_progressBar1->IsHovered()) ||
                                 (g_progressBar2 && g_progressBar2->IsHovered()) ||
                                 (g_progressBar3 && g_progressBar3->IsHovered());
                if (!anyHovered) {
                    g_tooltip->Hide();
                }
            }
            
            return 0;
        }
        
        case WM_LBUTTONDOWN: {
            int x = LOWORD(lParam);
            int y = HIWORD(lParam);
            
            if (g_widgetManager) {
                g_widgetManager->HandleMouseDown(x, y);
            }
            
            // Check button clicks manually (simple hit test)
            RECT buttonRect1 = {20, 400, 200, 440};
            RECT buttonRect2 = {220, 400, 400, 440};
            RECT buttonRect3 = {420, 400, 600, 440};
            
            POINT pt = {x, y};
            
            if (PtInRect(&buttonRect1, pt)) {
                // Animate Progress button
                if (g_progressBar1 && g_progressBar2 && g_progressBar3) {
                    g_progressBar1->SetAnimated(true);
                    g_progressBar2->SetAnimated(true);
                    g_progressBar3->SetAnimated(true);
                    
                    g_progressBar1->SetValue(75.0f);
                    g_progressBar2->SetValue(50.0f);
                    g_progressBar3->SetValue(90.0f);
                }
            } else if (PtInRect(&buttonRect2, pt)) {
                // Show Tooltip button
                if (g_tooltip) {
                    g_tooltip->SetText(L"This is a sample tooltip!\n\nIt supports:\n• Multi-line text\n• Fade animations\n• Auto-positioning");
                    g_tooltip->Show(x, y + 20);
                    InvalidateRect(hwnd, nullptr, FALSE);
                }
            } else if (PtInRect(&buttonRect3, pt)) {
                // Generate Window button
                std::wstring prompt = L"window 600x400 'Generated Window' with progressbar and tooltip";
                HWND generatedWindow = g_promptBuilder.BuildFromPrompt(
                    prompt, GetModuleHandle(nullptr), hwnd);
                
                if (generatedWindow) {
                    // Set up the generated window
                    auto window = SDK::WindowManager::GetInstance().RegisterWindow(generatedWindow);
                    if (window) {
                        auto theme = std::make_shared<SDK::Theme>(SDK::Theme::CreateModernTheme());
                        window->SetTheme(theme);
                        window->SetDepth(SDK::WindowDepth::FOREGROUND);
                        window->UpdateAppearance();
                        
                        // Set up render callback for the generated window
                        auto widgetMgr = g_promptBuilder.GetLastWidgetManager();
                        window->SetRenderCallback([widgetMgr](HDC hdc) {
                            // Clear background
                            RECT clientRect;
                            GetClientRect(WindowFromDC(hdc), &clientRect);
                            HBRUSH bgBrush = CreateSolidBrush(RGB(245, 245, 250));
                            FillRect(hdc, &clientRect, bgBrush);
                            DeleteObject(bgBrush);
                            
                            // Draw header
                            SetBkMode(hdc, TRANSPARENT);
                            SetTextColor(hdc, RGB(50, 50, 50));
                            RECT headerRect = {20, 20, clientRect.right - 20, 50};
                            DrawTextW(hdc, L"Generated from Prompt!", -1, &headerRect,
                                DT_LEFT | DT_VCENTER | DT_SINGLELINE);
                            
                            // Render widgets
                            if (widgetMgr) {
                                widgetMgr->RenderAll(hdc);
                            }
                        });
                    }
                    
                    ShowWindow(generatedWindow, SW_SHOW);
                }
            }
            
            return 0;
        }
        
        case WM_LBUTTONUP: {
            int x = LOWORD(lParam);
            int y = HIWORD(lParam);
            
            if (g_widgetManager) {
                g_widgetManager->HandleMouseUp(x, y);
            }
            
            return 0;
        }
    }
    
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
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
    wc.lpszClassName = L"5DGUIDemo";
    
    if (!RegisterClassExW(&wc)) {
        MessageBoxW(nullptr, L"Window Registration Failed!", L"Error", MB_ICONERROR);
        SDK::Shutdown();
        return -1;
    }
    
    // Create main window using universal function
    SDK::PromptWindowBuilder::WindowConfig config;
    config.className = L"5DGUIDemo";
    config.title = L"5D GUI SDK - Widget Demo";
    config.width = 800;
    config.height = 600;
    config.theme = std::make_shared<SDK::Theme>(SDK::Theme::CreateModernTheme());
    config.depth = SDK::WindowDepth::FOREGROUND;
    
    g_mainWindow = SDK::PromptWindowBuilder::CreateWidgetsWindow(config, hInstance);
    
    if (!g_mainWindow) {
        MessageBoxW(nullptr, L"Window Creation Failed!", L"Error", MB_ICONERROR);
        SDK::Shutdown();
        return -1;
    }
    
    // Create widget manager
    g_widgetManager = std::make_shared<SDK::WidgetManager>();
    
    // Create ProgressBar widgets
    g_progressBar1 = std::make_shared<SDK::ProgressBar>();
    g_progressBar1->SetBounds(20, 120, 760, 30);
    g_progressBar1->SetValue(33.0f);
    g_progressBar1->SetGradientColors(
        SDK::Color(100, 149, 237, 255),
        SDK::Color(65, 105, 225, 255)
    );
    g_progressBar1->SetId(1);
    g_widgetManager->AddWidget(g_progressBar1);
    
    g_progressBar2 = std::make_shared<SDK::ProgressBar>();
    g_progressBar2->SetBounds(20, 170, 760, 30);
    g_progressBar2->SetValue(66.0f);
    g_progressBar2->SetGradientColors(
        SDK::Color(138, 43, 226, 255),
        SDK::Color(75, 0, 130, 255)
    );
    g_progressBar2->SetId(2);
    g_widgetManager->AddWidget(g_progressBar2);
    
    g_progressBar3 = std::make_shared<SDK::ProgressBar>();
    g_progressBar3->SetBounds(20, 220, 760, 30);
    g_progressBar3->SetValue(0.0f);
    g_progressBar3->SetAnimated(true);
    g_progressBar3->SetGradientColors(
        SDK::Color(255, 140, 0, 255),
        SDK::Color(255, 69, 0, 255)
    );
    g_progressBar3->SetId(3);
    g_widgetManager->AddWidget(g_progressBar3);
    
    // Create Tooltip
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
        
        g_animationTime += deltaTime;
        
        // Animate third progress bar
        float progress = (sinf(g_animationTime * 2.0f) + 1.0f) * 0.5f * 100.0f;
        if (g_progressBar3) {
            g_progressBar3->SetValue(progress);
        }
        
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
