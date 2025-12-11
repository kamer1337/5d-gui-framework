/**
 * Optimization and Enhancement Demo
 * Demonstrates the new rendering optimizations, memory optimizations,
 * visual enhancements, and advanced effects
 */

#include "../include/SDK/SDK.h"
#include "../include/SDK/Window.h"
#include "../include/SDK/WindowManager.h"
#include "../include/SDK/Renderer.h"
#include "../include/SDK/Theme.h"
#include <chrono>
#include <sstream>

using namespace SDK;

// Demo state
struct DemoState {
    // Particle pool for memory optimization
    Renderer::ParticlePool particlePool;
    
    // Animation system
    Renderer::Animation fadeAnimation;
    Renderer::Animation bounceAnimation;
    Renderer::Animation colorAnimation;
    
    // Render cache for optimization
    std::unique_ptr<Renderer::RenderCache> renderCache;
    
    // Texture atlas for icon optimization
    std::unique_ptr<Renderer::TextureAtlas> textureAtlas;
    
    // Demo controls
    bool showBlur = false;
    bool showBloom = false;
    bool showNoise = false;
    bool showParticles = true;
    bool showAnimations = true;
    bool useMultiThreaded = true;  // Use multi-threaded particle updates by default
    
    // Timing
    std::chrono::steady_clock::time_point lastTime;
    float deltaTime = 0.016f;
    
    DemoState() : particlePool(5000) {  // Increased pool size to benefit from multi-threading
        lastTime = std::chrono::steady_clock::now();
        
        // Initialize animations
        fadeAnimation.AddKeyframe(0.0f, 0.0f, Renderer::EasingType::LINEAR);
        fadeAnimation.AddKeyframe(1.0f, 1.0f, Renderer::EasingType::EASE_IN_OUT_CUBIC);
        fadeAnimation.SetDuration(2.0f);
        fadeAnimation.SetLooping(true);
        fadeAnimation.Play();
        
        bounceAnimation.AddKeyframe(0.0f, 0.0f, Renderer::EasingType::EASE_OUT_BOUNCE);
        bounceAnimation.AddKeyframe(1.0f, 100.0f, Renderer::EasingType::LINEAR);
        bounceAnimation.SetDuration(1.5f);
        bounceAnimation.SetLooping(true);
        bounceAnimation.Play();
        
        colorAnimation.AddKeyframe(0.0f, 0.0f, Renderer::EasingType::LINEAR);
        colorAnimation.AddKeyframe(0.5f, 1.0f, Renderer::EasingType::EASE_IN_OUT_QUAD);
        colorAnimation.AddKeyframe(1.0f, 0.0f, Renderer::EasingType::LINEAR);
        colorAnimation.SetDuration(3.0f);
        colorAnimation.SetLooping(true);
        colorAnimation.Play();
    }
    
    void Update() {
        auto now = std::chrono::steady_clock::now();
        deltaTime = std::chrono::duration<float>(now - lastTime).count();
        lastTime = now;
        
        if (showAnimations) {
            fadeAnimation.Update(deltaTime);
            bounceAnimation.Update(deltaTime);
            colorAnimation.Update(deltaTime);
        }
        
        if (showParticles) {
            if (useMultiThreaded) {
                Renderer::UpdateParticlesInPoolMultiThreaded(particlePool, deltaTime);
            } else {
                Renderer::UpdateParticlesInPool(particlePool, deltaTime);
            }
        }
    }
};

DemoState* g_demoState = nullptr;

void RenderMainWindow(HDC hdc, Window* window) {
    if (!g_demoState) return;
    
    RECT clientRect;
    GetClientRect(window->GetHandle(), &clientRect);
    int width = clientRect.right - clientRect.left;
    int height = clientRect.bottom - clientRect.top;
    
    // Clear background
    HBRUSH bgBrush = CreateSolidBrush(RGB(20, 20, 30));
    FillRect(hdc, &clientRect, bgBrush);
    DeleteObject(bgBrush);
    
    // Title
    SetTextColor(hdc, RGB(255, 255, 255));
    SetBkMode(hdc, TRANSPARENT);
    HFONT hFont = CreateFont(24, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                             DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                             CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");
    HFONT oldFont = (HFONT)SelectObject(hdc, hFont);
    
    TextOut(hdc, 20, 20, L"5D GUI Framework - Optimization & Enhancement Demo", 50);
    SelectObject(hdc, oldFont);
    DeleteObject(hFont);
    
    // Section 1: Multi-stop gradient
    RECT gradientRect = {20, 70, width / 2 - 10, 170};
    std::vector<Renderer::GradientStop> stops;
    stops.push_back({0.0f, Color(255, 0, 0, 255)});      // Red
    stops.push_back({0.25f, Color(255, 255, 0, 255)});   // Yellow
    stops.push_back({0.5f, Color(0, 255, 0, 255)});      // Green
    stops.push_back({0.75f, Color(0, 255, 255, 255)});   // Cyan
    stops.push_back({1.0f, Color(0, 0, 255, 255)});      // Blue
    
    Renderer::DrawMultiStopGradient(hdc, gradientRect, stops, true);
    
    // Label
    hFont = CreateFont(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                       DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                       CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");
    oldFont = (HFONT)SelectObject(hdc, hFont);
    TextOut(hdc, 20, 175, L"Multi-Stop Gradient", 19);
    SelectObject(hdc, oldFont);
    DeleteObject(hFont);
    
    // Section 2: Conical gradient
    RECT conicalRect = {width / 2 + 10, 70, width - 20, 170};
    std::vector<Renderer::GradientStop> conicalStops;
    conicalStops.push_back({0.0f, Color(255, 0, 0, 255)});
    conicalStops.push_back({0.33f, Color(0, 255, 0, 255)});
    conicalStops.push_back({0.66f, Color(0, 0, 255, 255)});
    conicalStops.push_back({1.0f, Color(255, 0, 0, 255)});
    
    Renderer::DrawConicalGradient(hdc, conicalRect, conicalStops, 0, 0, 0.0f);
    
    hFont = CreateFont(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                       DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                       CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");
    oldFont = (HFONT)SelectObject(hdc, hFont);
    TextOut(hdc, width / 2 + 10, 175, L"Conical Gradient", 16);
    SelectObject(hdc, oldFont);
    DeleteObject(hFont);
    
    // Section 3: Animation demo with easing
    if (g_demoState->showAnimations) {
        int animX = 20;
        int animY = 220;
        int animWidth = width / 3 - 20;
        int animHeight = 150;
        
        // Background
        RECT animRect = {animX, animY, animX + animWidth, animY + animHeight};
        HBRUSH animBrush = CreateSolidBrush(RGB(40, 40, 50));
        FillRect(hdc, &animRect, animBrush);
        DeleteObject(animBrush);
        
        // Animated element with bounce
        float bounceValue = g_demoState->bounceAnimation.GetValue();
        int ballY = animY + animHeight - 30 - (int)bounceValue;
        
        HBRUSH ballBrush = CreateSolidBrush(RGB(255, 100, 100));
        HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, ballBrush);
        Ellipse(hdc, animX + animWidth / 2 - 15, ballY - 15, 
                animX + animWidth / 2 + 15, ballY + 15);
        SelectObject(hdc, oldBrush);
        DeleteObject(ballBrush);
        
        hFont = CreateFont(14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                           DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                           CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");
        oldFont = (HFONT)SelectObject(hdc, hFont);
        TextOut(hdc, animX + 5, animY + 5, L"Bounce Easing", 13);
        SelectObject(hdc, oldFont);
        DeleteObject(hFont);
    }
    
    // Section 4: Particle system with object pooling
    if (g_demoState->showParticles) {
        int particleX = width / 3 + 20;
        int particleY = 220;
        int particleWidth = width / 3 - 20;
        int particleHeight = 150;
        
        RECT particleRect = {particleX, particleY, particleX + particleWidth, particleY + particleHeight};
        HBRUSH particleBrush = CreateSolidBrush(RGB(40, 40, 50));
        FillRect(hdc, &particleRect, particleBrush);
        DeleteObject(particleBrush);
        
        // Draw particles
        Renderer::DrawParticlesFromPool(hdc, g_demoState->particlePool);
        
        // Spawn new particles occasionally - more particles to test multi-threading
        static int frameCount = 0;
        if (++frameCount % 5 == 0) {  // Spawn more frequently
            for (int i = 0; i < 10; i++) {  // Spawn more particles per frame
                Renderer::Particle* p = g_demoState->particlePool.Acquire();
                if (p) {
                    p->x = (float)(particleX + particleWidth / 2);
                    p->y = (float)(particleY + particleHeight - 20);
                    
                    float angle = (float)(rand() % 360) * 3.14159f / 180.0f;
                    float speed = 50.0f + (float)(rand() % 100);
                    p->vx = cos(angle) * speed;
                    p->vy = sin(angle) * speed - 100.0f;
                    p->life = 1.0f + (float)(rand() % 100) / 100.0f;
                    
                    float colorT = g_demoState->colorAnimation.GetValue();
                    p->color = Renderer::InterpolateColor(
                        Color(255, 100, 100, 255), 
                        Color(100, 100, 255, 255), 
                        colorT);
                }
            }
        }
        
        hFont = CreateFont(14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                           DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                           CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");
        oldFont = (HFONT)SelectObject(hdc, hFont);
        TextOut(hdc, particleX + 5, particleY + 5, L"Particle Pool", 13);
        
        // Show pool stats
        std::wstringstream ss;
        ss << L"Active: " << g_demoState->particlePool.GetActiveCount() 
           << L" / " << g_demoState->particlePool.GetTotalCount();
        std::wstring statsStr = ss.str();
        TextOut(hdc, particleX + 5, particleY + 25, statsStr.c_str(), (int)statsStr.length());
        
        // Show threading mode
        std::wstring threadMode = g_demoState->useMultiThreaded ? 
            L"Mode: Multi-threaded" : L"Mode: Single-threaded";
        TextOut(hdc, particleX + 5, particleY + 45, threadMode.c_str(), (int)threadMode.length());
        
        SelectObject(hdc, oldFont);
        DeleteObject(hFont);
    }
    
    // Section 5: Visual effects showcase
    int effectX = 2 * width / 3 + 20;
    int effectY = 220;
    int effectWidth = width - effectX - 20;
    int effectHeight = 150;
    
    RECT effectRect = {effectX, effectY, effectX + effectWidth, effectY + effectHeight};
    
    // Draw gradient background
    Renderer::DrawVerticalGradient(hdc, effectRect, 
                                   Color(80, 50, 120, 255), 
                                   Color(120, 80, 160, 255));
    
    // Apply effects based on state
    if (g_demoState->showBlur) {
        Renderer::ApplyBlur(hdc, effectRect, 3);
    }
    
    if (g_demoState->showBloom) {
        Renderer::ApplyBloom(hdc, effectRect, 0.6f, 1.5f);
    }
    
    if (g_demoState->showNoise) {
        Renderer::ApplyNoiseOverlay(hdc, effectRect, 0.1f, (int)GetTickCount());
    }
    
    hFont = CreateFont(14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                       DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                       CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");
    oldFont = (HFONT)SelectObject(hdc, hFont);
    TextOut(hdc, effectX + 5, effectY + 5, L"Visual Effects", 14);
    SelectObject(hdc, oldFont);
    DeleteObject(hFont);
    
    // Instructions
    hFont = CreateFont(14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                       DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                       CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");
    oldFont = (HFONT)SelectObject(hdc, hFont);
    
    int instructY = height - 120;
    TextOut(hdc, 20, instructY, L"Controls:", 9);
    TextOut(hdc, 20, instructY + 20, L"B - Toggle Blur Effect", 22);
    TextOut(hdc, 20, instructY + 40, L"L - Toggle Bloom Effect", 23);
    TextOut(hdc, 20, instructY + 60, L"N - Toggle Noise Overlay", 24);
    TextOut(hdc, 20, instructY + 80, L"T - Toggle Multi-threaded Particles", 35);
    
    SelectObject(hdc, oldFont);
    DeleteObject(hFont);
    
    // Performance info
    std::wstringstream perfStream;
    perfStream << L"FPS: " << (int)(1.0f / g_demoState->deltaTime) 
               << L"  Frame Time: " << (int)(g_demoState->deltaTime * 1000) << L"ms";
    std::wstring perfStr = perfStream.str();
    
    hFont = CreateFont(12, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                       DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                       CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Consolas");
    oldFont = (HFONT)SelectObject(hdc, hFont);
    SetTextColor(hdc, RGB(0, 255, 0));
    TextOut(hdc, width - 300, height - 25, perfStr.c_str(), (int)perfStr.length());
    SelectObject(hdc, oldFont);
    DeleteObject(hFont);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            
            Window* window = (Window*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
            if (window) {
                RenderMainWindow(hdc, window);
            }
            
            EndPaint(hwnd, &ps);
            return 0;
        }
        
        case WM_KEYDOWN: {
            if (!g_demoState) break;
            
            switch (wParam) {
                case 'B':
                    g_demoState->showBlur = !g_demoState->showBlur;
                    InvalidateRect(hwnd, nullptr, FALSE);
                    break;
                case 'L':
                    g_demoState->showBloom = !g_demoState->showBloom;
                    InvalidateRect(hwnd, nullptr, FALSE);
                    break;
                case 'N':
                    g_demoState->showNoise = !g_demoState->showNoise;
                    InvalidateRect(hwnd, nullptr, FALSE);
                    break;
                case 'T':
                    g_demoState->useMultiThreaded = !g_demoState->useMultiThreaded;
                    InvalidateRect(hwnd, nullptr, FALSE);
                    break;
            }
            break;
        }
        
        case WM_CLOSE:
            PostQuitMessage(0);
            return 0;
        
        case WM_DESTROY:
            return 0;
    }
    
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // Initialize demo state
    g_demoState = new DemoState();
    
    // Initialize SDK
    SDK::Initialize();
    
    // Create window
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"OptimizationDemo";
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    
    RegisterClass(&wc);
    
    HWND hwnd = CreateWindowEx(
        0,
        L"OptimizationDemo",
        L"5D GUI - Optimization & Enhancement Demo",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 1200, 700,
        nullptr, nullptr, hInstance, nullptr
    );
    
    if (!hwnd) {
        return 0;
    }
    
    Window* window = new Window(hwnd);
    SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)window);
    
    ShowWindow(hwnd, nCmdShow);
    
    // Main loop
    MSG msg = {};
    while (true) {
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                goto cleanup;
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        
        // Update demo state
        g_demoState->Update();
        
        // Redraw
        InvalidateRect(hwnd, nullptr, FALSE);
        
        // Small delay to prevent 100% CPU usage
        Sleep(16);  // ~60 FPS
    }
    
cleanup:
    delete window;
    delete g_demoState;
    SDK::Shutdown();
    
    return 0;
}
