/**
 * 5D GUI SDK Demo Application
 * Demonstrates the enhanced 5D rendering system with multiple windows at different depths
 */

#include "../include/SDK/SDK.h"
#include <vector>
#include <string>
#include <cmath>

// Global state
std::vector<HWND> g_demoWindows;
std::vector<SDK::Renderer::Particle> g_particles;
bool g_particlesEnabled = false;
HWND g_mainWindow = nullptr;
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
            
            // Get our window wrapper
            auto window = SDK::WindowManager::GetInstance().GetWindow(hwnd);
            if (window) {
                window->Render(hdc);
            }
            
            // Draw particles if enabled
            if (g_particlesEnabled && !g_particles.empty()) {
                SDK::Renderer::DrawParticles(hdc, g_particles);
            }
            
            EndPaint(hwnd, &ps);
            return 0;
        }
        
        case WM_MOUSEMOVE: {
            // Enable hover effects
            if (g_particlesEnabled) {
                int x = LOWORD(lParam);
                int y = HIWORD(lParam);
                
                // Generate particles on hover
                if (rand() % 10 == 0) {
                    auto newParticles = SDK::Renderer::CreateParticleEmission(
                        x, y, 3, SDK::Color(100, 150, 255, 200)
                    );
                    g_particles.insert(g_particles.end(), newParticles.begin(), newParticles.end());
                }
            }
            return 0;
        }
        
        case WM_LBUTTONDOWN: {
            // Generate particle burst on click
            int x = LOWORD(lParam);
            int y = HIWORD(lParam);
            
            auto newParticles = SDK::Renderer::CreateParticleEmission(
                x, y, 20, SDK::Color(255, 215, 0, 255)
            );
            g_particles.insert(g_particles.end(), newParticles.begin(), newParticles.end());
            g_particlesEnabled = true;
            
            InvalidateRect(hwnd, nullptr, FALSE);
            return 0;
        }
        
        case WM_COMMAND: {
            int wmId = LOWORD(wParam);
            switch (wmId) {
                case 1: // Layered Windows button
                    // Create multiple windows at different depths
                    for (int i = 0; i < 5; i++) {
                        HWND hwnd = CreateWindowExW(
                            WS_EX_LAYERED | WS_EX_TOPMOST,
                            L"5DGUIDemo",
                            (L"Window Depth " + std::to_wstring(i)).c_str(),
                            WS_OVERLAPPEDWINDOW,
                            100 + i * 20, 100 + i * 20, 300, 200,
                            nullptr, nullptr, GetModuleHandle(nullptr), nullptr
                        );
                        
                        auto window = SDK::WindowManager::GetInstance().RegisterWindow(hwnd);
                        if (window) {
                            window->SetDepth((SDK::WindowDepth)i);
                            window->UpdateAppearance();
                        }
                        
                        ShowWindow(hwnd, SW_SHOW);
                        g_demoWindows.push_back(hwnd);
                    }
                    break;
                    
                case 2: // 5D Icons button
                    g_particlesEnabled = !g_particlesEnabled;
                    InvalidateRect(hwnd, nullptr, TRUE);
                    break;
                    
                case 3: // Animation button
                    SDK::WindowManager::GetInstance().EnableDepthAnimation(
                        !SDK::WindowManager::GetInstance().IsDepthAnimationEnabled()
                    );
                    break;
            }
            return 0;
        }
    }
    
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

// Custom rendering callback for main window
void RenderMainWindow(HDC hdc) {
    RECT rect;
    GetClientRect(WindowFromDC(hdc), &rect);
    
    // Draw buttons
    const int buttonY = 80;
    const int buttonHeight = 40;
    const int buttonSpacing = 10;
    
    RECT buttonRect1 = {20, buttonY, 150, buttonY + buttonHeight};
    SDK::Renderer::DrawRoundedRect(hdc, buttonRect1, 8,
        SDK::Color(100, 149, 237, 255),
        SDK::Color(65, 105, 225, 255), 2);
    
    RECT buttonRect2 = {160, buttonY, 290, buttonY + buttonHeight};
    SDK::Renderer::DrawRoundedRect(hdc, buttonRect2, 8,
        SDK::Color(138, 43, 226, 255),
        SDK::Color(75, 0, 130, 255), 2);
    
    RECT buttonRect3 = {300, buttonY, 430, buttonY + buttonHeight};
    SDK::Renderer::DrawRoundedRect(hdc, buttonRect3, 8,
        SDK::Color(255, 140, 0, 255),
        SDK::Color(255, 69, 0, 255), 2);
    
    // Draw button labels
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(255, 255, 255));
    
    DrawTextW(hdc, L"Layered Windows", -1, &buttonRect1, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    DrawTextW(hdc, L"5D Icons", -1, &buttonRect2, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    DrawTextW(hdc, L"Animation", -1, &buttonRect3, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    
    // Draw demo icons at different depths
    const int iconY = 180;
    const int iconSize = 30;
    const int iconSpacing = 80;
    
    for (int i = 0; i < 5; i++) {
        int x = 50 + i * iconSpacing;
        float scale = 0.7f + i * 0.075f;  // Scale from 70% to 100%
        float alpha = 0.67f + i * 0.083f;  // Alpha from 170/255 to 255/255
        
        SDK::Renderer::IconType iconType = (SDK::Renderer::IconType)(i % 5);
        SDK::Color iconColor(100 + i * 30, 100 + i * 20, 200 + i * 10, 255);
        
        SDK::Renderer::DrawIcon(hdc, iconType, x, iconY, (int)(iconSize * scale), iconColor, alpha);
    }
    
    // Draw gradient showcase
    const int gradientY = 260;
    
    // Horizontal gradient
    RECT hGradRect = {20, gradientY, 200, gradientY + 30};
    SDK::Renderer::DrawHorizontalGradient(hdc, hGradRect,
        SDK::Color(255, 0, 0, 255), SDK::Color(0, 0, 255, 255));
    
    // Vertical gradient
    RECT vGradRect = {220, gradientY, 280, gradientY + 60};
    SDK::Renderer::DrawVerticalGradient(hdc, vGradRect,
        SDK::Color(255, 215, 0, 255), SDK::Color(139, 69, 19, 255));
    
    // Radial gradient
    RECT rGradRect = {300, gradientY, 380, gradientY + 60};
    SDK::Renderer::DrawRadialGradient(hdc, rGradRect,
        SDK::Color(255, 255, 0, 255), SDK::Color(255, 0, 0, 255), 40, 30);
    
    // Info text
    SetTextColor(hdc, RGB(50, 50, 50));
    RECT textRect = {20, 340, rect.right - 20, rect.bottom - 20};
    const wchar_t* infoText = 
        L"5D GUI SDK Demo\n\n"
        L"Features:\n"
        L"• Multi-layer shadows with depth awareness\n"
        L"• Alpha-blended rounded corners\n"
        L"• Layered window rendering (book effect)\n"
        L"• 5D icon system with hover & particles\n"
        L"• Horizontal & radial gradients\n"
        L"• Interactive particle emissions\n\n"
        L"Click anywhere for particle burst!\n"
        L"Move mouse for hover effects!";
    
    DrawTextW(hdc, infoText, -1, &textRect, DT_LEFT | DT_WORDBREAK);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // Initialize SDK (includes random seed)
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
    
    // Create main window
    g_mainWindow = CreateWindowExW(
        WS_EX_LAYERED,
        L"5DGUIDemo",
        L"Enhanced 5D Rendering Demo",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
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
        // Set Aurora theme for main window
        auto theme = std::make_shared<SDK::Theme>(SDK::Theme::CreateModernTheme());
        mainWindow->SetTheme(theme);
        mainWindow->SetDepth(SDK::WindowDepth::FOREGROUND);
        mainWindow->SetRenderCallback(RenderMainWindow);
        mainWindow->UpdateAppearance();
    }
    
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
        
        // Update particles
        if (!g_particles.empty()) {
            SDK::Renderer::UpdateParticles(g_particles, deltaTime);
            InvalidateRect(g_mainWindow, nullptr, FALSE);
        }
        
        // Update window manager
        SDK::WindowManager::GetInstance().Update(deltaTime);
        
        // Sleep to prevent 100% CPU usage
        Sleep(16); // ~60 FPS
    }
    
cleanup:
    // Cleanup
    SDK::Shutdown();
    
    return (int)msg.wParam;
}
