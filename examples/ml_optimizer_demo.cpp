/**
 * ML Renderer Optimization Demo
 * Demonstrates machine learning-based renderer optimization
 */

#include "SDK/SDK.h"
#include "SDK/RendererOptimizer.h"
#include <Windows.h>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>

// Global variables
SDK::RendererOptimizer g_optimizer;
HWND g_hwnd = nullptr;
std::vector<RECT> g_elements;
std::vector<std::string> g_elementIds;
bool g_showStats = true;
int g_frameCount = 0;
DWORD g_lastTime = 0;
float g_fps = 0.0f;

// Window procedure
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Render elements with ML optimization
void RenderElements(HDC hdc);

// Render statistics overlay
void RenderStats(HDC hdc);

// Entry point
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // Initialize SDK
    SDK::Initialize();
    
    // Register window class
    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = L"MLOptimizerDemo";
    
    if (!RegisterClassEx(&wc)) {
        MessageBox(nullptr, L"Window registration failed!", L"Error", MB_ICONERROR);
        return 1;
    }
    
    // Create window
    g_hwnd = CreateWindowEx(
        WS_EX_LAYERED,
        L"MLOptimizerDemo",
        L"ML Renderer Optimization Demo - 5D GUI Framework",
        WS_OVERLAPPEDWINDOW,
        100, 100, 1200, 800,
        nullptr, nullptr, hInstance, nullptr
    );
    
    if (!g_hwnd) {
        MessageBox(nullptr, L"Window creation failed!", L"Error", MB_ICONERROR);
        return 1;
    }
    
    // Initialize elements to render
    int numElements = 50;
    for (int i = 0; i < numElements; ++i) {
        RECT rect;
        rect.left = 50 + (i % 10) * 100;
        rect.top = 100 + (i / 10) * 120;
        rect.right = rect.left + 80;
        rect.bottom = rect.top + 80;
        g_elements.push_back(rect);
        
        std::string elementId = "element_" + std::to_string(i);
        g_elementIds.push_back(elementId);
        
        // Register with optimizer
        g_optimizer.RegisterElement(elementId, rect);
    }
    
    // Register window with SDK
    auto window = SDK::WindowManager::GetInstance().RegisterWindow(g_hwnd);
    auto theme = std::make_shared<SDK::Theme>(SDK::Theme::CreateDarkTheme());
    window->SetTheme(theme);
    window->SetDepth(SDK::WindowDepth::FOREGROUND);
    window->SetRoundedCorners(true, 16);
    window->UpdateAppearance();
    
    ShowWindow(g_hwnd, nCmdShow);
    UpdateWindow(g_hwnd);
    
    g_lastTime = GetTickCount();
    
    // Message loop
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    SDK::Shutdown();
    return static_cast<int>(msg.wParam);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            
            // Create memory DC for double buffering
            RECT clientRect;
            GetClientRect(hwnd, &clientRect);
            HDC memDC = CreateCompatibleDC(hdc);
            HBITMAP memBitmap = CreateCompatibleBitmap(hdc, 
                clientRect.right - clientRect.left, 
                clientRect.bottom - clientRect.top);
            HBITMAP oldBitmap = (HBITMAP)SelectObject(memDC, memBitmap);
            
            // Clear background
            HBRUSH bgBrush = CreateSolidBrush(RGB(30, 30, 40));
            FillRect(memDC, &clientRect, bgBrush);
            DeleteObject(bgBrush);
            
            // Render elements
            RenderElements(memDC);
            
            // Render stats overlay
            if (g_showStats) {
                RenderStats(memDC);
            }
            
            // Copy to screen
            BitBlt(hdc, 0, 0, 
                clientRect.right - clientRect.left, 
                clientRect.bottom - clientRect.top, 
                memDC, 0, 0, SRCCOPY);
            
            // Cleanup
            SelectObject(memDC, oldBitmap);
            DeleteObject(memBitmap);
            DeleteDC(memDC);
            
            EndPaint(hwnd, &ps);
            
            // Calculate FPS
            g_frameCount++;
            DWORD currentTime = GetTickCount();
            if (currentTime - g_lastTime >= 1000) {
                g_fps = g_frameCount * 1000.0f / (currentTime - g_lastTime);
                g_frameCount = 0;
                g_lastTime = currentTime;
            }
            
            // Trigger next frame
            InvalidateRect(hwnd, nullptr, FALSE);
            return 0;
        }
        
        case WM_KEYDOWN: {
            if (wParam == VK_SPACE) {
                g_showStats = !g_showStats;
                InvalidateRect(hwnd, nullptr, FALSE);
            } else if (wParam == 'R') {
                g_optimizer.ResetStats();
                InvalidateRect(hwnd, nullptr, FALSE);
            } else if (wParam == 'T') {
                g_optimizer.SetEnabled(!g_optimizer.IsEnabled());
                InvalidateRect(hwnd, nullptr, FALSE);
            }
            return 0;
        }
        
        case WM_LBUTTONDOWN: {
            // Mark random elements as changed (simulate interaction)
            for (int i = 0; i < 5; ++i) {
                int idx = rand() % g_elementIds.size();
                g_optimizer.MarkElementChanged(g_elementIds[idx]);
            }
            InvalidateRect(hwnd, nullptr, FALSE);
            return 0;
        }
        
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

void RenderElements(HDC hdc) {
    DWORD startTime = GetTickCount();
    
    for (size_t i = 0; i < g_elements.size(); ++i) {
        const RECT& rect = g_elements[i];
        const std::string& elementId = g_elementIds[i];
        
        // Get optimal rendering strategy from ML optimizer
        auto strategy = g_optimizer.GetOptimalStrategy(elementId);
        
        DWORD elementStartTime = GetTickCount();
        
        // Render based on strategy
        COLORREF color = RGB(80, 120, 200);
        switch (strategy) {
            case SDK::RendererOptimizer::RenderStrategy::FULL_RENDER:
                color = RGB(100, 200, 100); // Green - full render
                break;
            case SDK::RendererOptimizer::RenderStrategy::CACHED_RENDER:
                color = RGB(200, 200, 100); // Yellow - cached
                break;
            case SDK::RendererOptimizer::RenderStrategy::LOD_HIGH:
                color = RGB(150, 150, 250); // Light blue - high LOD
                break;
            case SDK::RendererOptimizer::RenderStrategy::LOD_MEDIUM:
                color = RGB(120, 120, 200); // Medium blue - medium LOD
                break;
            case SDK::RendererOptimizer::RenderStrategy::LOD_LOW:
                color = RGB(80, 80, 150); // Dark blue - low LOD
                break;
            case SDK::RendererOptimizer::RenderStrategy::SKIP_RENDER:
                continue; // Don't render
        }
        
        // Draw element
        HBRUSH brush = CreateSolidBrush(color);
        HPEN pen = CreatePen(PS_SOLID, 2, RGB(255, 255, 255));
        HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, brush);
        HPEN oldPen = (HPEN)SelectObject(hdc, pen);
        
        RoundRect(hdc, rect.left, rect.top, rect.right, rect.bottom, 10, 10);
        
        SelectObject(hdc, oldBrush);
        SelectObject(hdc, oldPen);
        DeleteObject(brush);
        DeleteObject(pen);
        
        // Draw element ID
        std::wstring text = std::to_wstring(i);
        SetTextColor(hdc, RGB(255, 255, 255));
        SetBkMode(hdc, TRANSPARENT);
        DrawTextW(hdc, text.c_str(), -1, const_cast<RECT*>(&rect), 
            DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        
        DWORD elementEndTime = GetTickCount();
        float renderTime = static_cast<float>(elementEndTime - elementStartTime);
        
        // Record metrics
        g_optimizer.RecordRenderMetrics(elementId, renderTime, true);
    }
}

void RenderStats(HDC hdc) {
    auto stats = g_optimizer.GetStats();
    
    // Create semi-transparent background
    RECT statsRect = {10, 10, 400, 250};
    HBRUSH bgBrush = CreateSolidBrush(RGB(20, 20, 30));
    FillRect(hdc, &statsRect, bgBrush);
    DeleteObject(bgBrush);
    
    // Draw border
    HPEN borderPen = CreatePen(PS_SOLID, 2, RGB(100, 150, 200));
    HPEN oldPen = (HPEN)SelectObject(hdc, borderPen);
    MoveToEx(hdc, statsRect.left, statsRect.top, nullptr);
    LineTo(hdc, statsRect.right, statsRect.top);
    LineTo(hdc, statsRect.right, statsRect.bottom);
    LineTo(hdc, statsRect.left, statsRect.bottom);
    LineTo(hdc, statsRect.left, statsRect.top);
    SelectObject(hdc, oldPen);
    DeleteObject(borderPen);
    
    // Render text
    SetTextColor(hdc, RGB(200, 230, 255));
    SetBkMode(hdc, TRANSPARENT);
    
    std::wstringstream ss;
    ss << std::fixed << std::setprecision(2);
    
    int y = 20;
    int lineHeight = 20;
    
    // Title
    std::wstring title = L"ML Renderer Optimization Stats";
    TextOutW(hdc, 20, y, title.c_str(), static_cast<int>(title.length()));
    y += lineHeight + 10;
    
    // Stats
    ss.str(L"");
    ss << L"FPS: " << g_fps;
    std::wstring fpsText = ss.str();
    TextOutW(hdc, 20, y, fpsText.c_str(), static_cast<int>(fpsText.length()));
    y += lineHeight;
    
    ss.str(L"");
    ss << L"Total Elements: " << stats.totalElements;
    std::wstring elemText = ss.str();
    TextOutW(hdc, 20, y, elemText.c_str(), static_cast<int>(elemText.length()));
    y += lineHeight;
    
    ss.str(L"");
    ss << L"Full Renders: " << stats.fullRenders;
    std::wstring fullText = ss.str();
    TextOutW(hdc, 20, y, fullText.c_str(), static_cast<int>(fullText.length()));
    y += lineHeight;
    
    ss.str(L"");
    ss << L"Cached Renders: " << stats.cachedRenders;
    std::wstring cacheText = ss.str();
    TextOutW(hdc, 20, y, cacheText.c_str(), static_cast<int>(cacheText.length()));
    y += lineHeight;
    
    ss.str(L"");
    ss << L"Avg Render Time: " << stats.avgRenderTime << L" ms";
    std::wstring timeText = ss.str();
    TextOutW(hdc, 20, y, timeText.c_str(), static_cast<int>(timeText.length()));
    y += lineHeight;
    
    ss.str(L"");
    ss << L"Cache Hit Rate: " << (stats.cacheHitRate * 100.0f) << L"%";
    std::wstring hitText = ss.str();
    TextOutW(hdc, 20, y, hitText.c_str(), static_cast<int>(hitText.length()));
    y += lineHeight;
    
    ss.str(L"");
    ss << L"ML Accuracy: " << (stats.mlAccuracy * 100.0f) << L"%";
    std::wstring mlText = ss.str();
    TextOutW(hdc, 20, y, mlText.c_str(), static_cast<int>(mlText.length()));
    y += lineHeight;
    
    ss.str(L"");
    ss << L"ML Enabled: " << (g_optimizer.IsEnabled() ? L"Yes" : L"No");
    std::wstring enabledText = ss.str();
    TextOutW(hdc, 20, y, enabledText.c_str(), static_cast<int>(enabledText.length()));
    y += lineHeight + 10;
    
    // Controls
    std::wstring controls = L"[SPACE] Toggle Stats  [R] Reset  [T] Toggle ML";
    TextOutW(hdc, 20, y, controls.c_str(), static_cast<int>(controls.length()));
}
