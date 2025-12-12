/**
 * Hardware Acceleration Demo
 * 
 * This demo showcases the new hardware acceleration features with Direct2D backend.
 * Compares performance between GDI (software) and Direct2D (GPU) rendering.
 */

#include "../include/SDK/SDK.h"
#include "../include/SDK/RenderBackend.h"
#include "../include/SDK/GDIRenderBackend.h"
#include "../include/SDK/D2DRenderBackend.h"
#include <chrono>
#include <string>

class HardwareAccelDemo {
private:
    HWND m_hwnd;
    std::unique_ptr<SDK::RenderBackend> m_backend;
    bool m_useGPU;
    int m_frameCount;
    double m_avgFrameTime;
    std::chrono::high_resolution_clock::time_point m_lastTime;
    
public:
    HardwareAccelDemo() 
        : m_hwnd(nullptr)
        , m_useGPU(true)
        , m_frameCount(0)
        , m_avgFrameTime(0.0)
    {
    }
    
    bool Initialize(HINSTANCE hInstance) {
        // Initialize SDK
        SDK::Initialize();
        
        // Register window class
        WNDCLASSEXW wc = {};
        wc.cbSize = sizeof(wc);
        wc.style = CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc = WindowProc;
        wc.hInstance = hInstance;
        wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wc.lpszClassName = L"HardwareAccelDemoClass";
        
        if (!RegisterClassExW(&wc)) {
            return false;
        }
        
        // Create window
        m_hwnd = CreateWindowExW(
            0,
            L"HardwareAccelDemoClass",
            L"Hardware Acceleration Demo - Press SPACE to toggle GPU/CPU",
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT, 1024, 768,
            nullptr, nullptr, hInstance, this
        );
        
        if (!m_hwnd) {
            return false;
        }
        
        // Create initial backend (GPU)
        CreateBackend(true);
        
        ShowWindow(m_hwnd, SW_SHOW);
        UpdateWindow(m_hwnd);
        
        m_lastTime = std::chrono::high_resolution_clock::now();
        
        return true;
    }
    
    void CreateBackend(bool useGPU) {
        m_useGPU = useGPU;
        
        // Shutdown old backend if exists
        if (m_backend) {
            m_backend->Shutdown();
        }
        
        // Create new backend
        if (useGPU) {
            m_backend = SDK::RenderBackend::Create(SDK::RenderBackend::BackendType::DIRECT2D);
        } else {
            m_backend = SDK::RenderBackend::Create(SDK::RenderBackend::BackendType::GDI);
        }
        
        if (m_backend) {
            if (!m_backend->Initialize(m_hwnd)) {
                // Fallback to GDI if Direct2D fails
                m_backend = SDK::RenderBackend::Create(SDK::RenderBackend::BackendType::GDI);
                m_backend->Initialize(m_hwnd);
                m_useGPU = false;
            }
        }
        
        // Update window title
        UpdateTitle();
    }
    
    void UpdateTitle() {
        auto caps = m_backend->GetCapabilities();
        std::wstring title = L"Hardware Acceleration Demo - ";
        title += m_useGPU ? L"GPU (Direct2D)" : L"CPU (GDI)";
        title += L" - FPS: ";
        
        if (m_avgFrameTime > 0) {
            int fps = (int)(1000.0 / m_avgFrameTime);
            title += std::to_wstring(fps);
        } else {
            title += L"--";
        }
        
        title += L" - Press SPACE to toggle";
        
        SetWindowTextW(m_hwnd, title.c_str());
    }
    
    void Render() {
        if (!m_backend) return;
        
        // Measure frame time
        auto currentTime = std::chrono::high_resolution_clock::now();
        auto deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(
            currentTime - m_lastTime
        ).count() / 1000.0;
        
        m_lastTime = currentTime;
        
        // Update average frame time
        m_avgFrameTime = (m_avgFrameTime * m_frameCount + deltaTime) / (m_frameCount + 1);
        m_frameCount++;
        
        if (m_frameCount % 30 == 0) {
            UpdateTitle();
        }
        
        // Begin drawing
        m_backend->BeginDraw();
        
        // Clear background
        m_backend->Clear(SDK::Color{240, 240, 245, 255});
        
        // Draw title
        RECT titleRect = {50, 20, 950, 70};
        m_backend->DrawText(
            L"Hardware Acceleration Showcase",
            titleRect,
            SDK::Color{40, 40, 60, 255},
            L"Segoe UI",
            32.0f,
            FW_BOLD
        );
        
        // Draw backend info
        RECT infoRect = {50, 70, 950, 100};
        std::wstring info = m_useGPU ? L"Rendering with GPU (Direct2D)" : L"Rendering with CPU (GDI)";
        m_backend->DrawText(
            info,
            infoRect,
            SDK::Color{100, 100, 120, 255},
            L"Segoe UI",
            16.0f,
            FW_NORMAL
        );
        
        // Draw performance comparison boxes
        int boxY = 120;
        int boxHeight = 150;
        int boxSpacing = 20;
        
        // Box 1: Gradients
        RECT box1 = {50, boxY, 320, boxY + boxHeight};
        m_backend->DrawRoundedRectangle(
            box1, 15.0f,
            SDK::Color{255, 255, 255, 255},
            SDK::Color{200, 200, 210, 255},
            2.0f
        );
        
        RECT gradRect = {70, boxY + 40, 300, boxY + 90};
        m_backend->DrawLinearGradient(
            gradRect,
            SDK::Color{255, 100, 100, 255},
            SDK::Color{100, 100, 255, 255},
            true
        );
        
        RECT label1 = {60, boxY + 10, 310, boxY + 35};
        m_backend->DrawText(
            L"Linear Gradient",
            label1,
            SDK::Color{40, 40, 60, 255},
            L"Segoe UI",
            14.0f,
            FW_SEMIBOLD
        );
        
        // Box 2: Radial Gradient
        RECT box2 = {340, boxY, 610, boxY + boxHeight};
        m_backend->DrawRoundedRectangle(
            box2, 15.0f,
            SDK::Color{255, 255, 255, 255},
            SDK::Color{200, 200, 210, 255},
            2.0f
        );
        
        RECT radialRect = {360, boxY + 40, 590, boxY + 130};
        m_backend->DrawRadialGradient(
            radialRect,
            SDK::Color{255, 220, 100, 255},
            SDK::Color{255, 100, 100, 255},
            475, boxY + 85
        );
        
        RECT label2 = {350, boxY + 10, 600, boxY + 35};
        m_backend->DrawText(
            L"Radial Gradient",
            label2,
            SDK::Color{40, 40, 60, 255},
            L"Segoe UI",
            14.0f,
            FW_SEMIBOLD
        );
        
        // Box 3: Shadow & Glow
        RECT box3 = {630, boxY, 900, boxY + boxHeight};
        m_backend->DrawRoundedRectangle(
            box3, 15.0f,
            SDK::Color{255, 255, 255, 255},
            SDK::Color{200, 200, 210, 255},
            2.0f
        );
        
        RECT shadowRect = {680, boxY + 60, 850, boxY + 110};
        m_backend->DrawShadow(
            shadowRect, 5, 5, 10,
            SDK::Color{0, 0, 0, 80}
        );
        m_backend->DrawRoundedRectangle(
            shadowRect, 8.0f,
            SDK::Color{100, 200, 255, 255},
            SDK::Color{50, 150, 200, 255},
            2.0f
        );
        
        RECT label3 = {640, boxY + 10, 890, boxY + 35};
        m_backend->DrawText(
            L"Shadow Effect",
            label3,
            SDK::Color{40, 40, 60, 255},
            L"Segoe UI",
            14.0f,
            FW_SEMIBOLD
        );
        
        // Second row - Complex shapes
        boxY += boxHeight + boxSpacing;
        
        // Box 4: Multiple shapes
        RECT box4 = {50, boxY, 470, boxY + boxHeight};
        m_backend->DrawRoundedRectangle(
            box4, 15.0f,
            SDK::Color{255, 255, 255, 255},
            SDK::Color{200, 200, 210, 255},
            2.0f
        );
        
        // Draw multiple overlapping shapes
        for (int i = 0; i < 5; i++) {
            RECT shapeRect = {70 + i * 70, boxY + 50, 140 + i * 70, boxY + 120};
            SDK::Color color;
            color.r = 255 - i * 40;
            color.g = 100 + i * 30;
            color.b = 100 + i * 30;
            color.a = 200;
            
            m_backend->DrawRoundedRectangle(
                shapeRect, 10.0f,
                color,
                SDK::Color{color.r - 50, color.g - 50, color.b - 50, 255},
                2.0f
            );
        }
        
        RECT label4 = {60, boxY + 10, 460, boxY + 35};
        m_backend->DrawText(
            L"Overlapping Shapes",
            label4,
            SDK::Color{40, 40, 60, 255},
            L"Segoe UI",
            14.0f,
            FW_SEMIBOLD
        );
        
        // Box 5: Text rendering
        RECT box5 = {490, boxY, 900, boxY + boxHeight};
        m_backend->DrawRoundedRectangle(
            box5, 15.0f,
            SDK::Color{255, 255, 255, 255},
            SDK::Color{200, 200, 210, 255},
            2.0f
        );
        
        RECT label5 = {500, boxY + 10, 890, boxY + 35};
        m_backend->DrawText(
            L"Text Rendering",
            label5,
            SDK::Color{40, 40, 60, 255},
            L"Segoe UI",
            14.0f,
            FW_SEMIBOLD
        );
        
        RECT textRect1 = {510, boxY + 45, 880, boxY + 70};
        m_backend->DrawText(
            L"Smooth antialiased text",
            textRect1,
            SDK::Color{60, 60, 80, 255},
            L"Segoe UI",
            16.0f,
            FW_NORMAL
        );
        
        RECT textRect2 = {510, boxY + 75, 880, boxY + 100};
        m_backend->DrawText(
            L"with full Unicode support",
            textRect2,
            SDK::Color{80, 80, 100, 255},
            L"Segoe UI",
            14.0f,
            FW_LIGHT
        );
        
        // Performance info at bottom
        boxY += boxHeight + boxSpacing;
        
        RECT perfBox = {50, boxY, 900, boxY + 100};
        m_backend->DrawRoundedRectangle(
            perfBox, 15.0f,
            SDK::Color{250, 250, 255, 255},
            SDK::Color{200, 200, 220, 255},
            2.0f
        );
        
        auto caps = m_backend->GetCapabilities();
        
        RECT perfTitle = {70, boxY + 10, 880, boxY + 35};
        m_backend->DrawText(
            L"Backend Capabilities",
            perfTitle,
            SDK::Color{40, 40, 60, 255},
            L"Segoe UI",
            16.0f,
            FW_SEMIBOLD
        );
        
        std::wstring capsText = L"GPU Acceleration: ";
        capsText += caps.supportsGPUAcceleration ? L"Yes" : L"No";
        capsText += L" | Advanced Effects: ";
        capsText += caps.supportsAdvancedEffects ? L"Yes" : L"No";
        capsText += L" | Antialiasing: ";
        capsText += caps.supportsAntialiasing ? L"Yes" : L"No";
        
        RECT capsRect = {70, boxY + 40, 880, boxY + 65};
        m_backend->DrawText(
            capsText,
            capsRect,
            SDK::Color{80, 80, 100, 255},
            L"Segoe UI",
            12.0f,
            FW_NORMAL
        );
        
        std::wstring perfText = L"Average Frame Time: ";
        perfText += std::to_wstring((int)m_avgFrameTime);
        perfText += L" ms | FPS: ";
        perfText += std::to_wstring((int)(1000.0 / m_avgFrameTime));
        
        RECT perfTextRect = {70, boxY + 65, 880, boxY + 90};
        m_backend->DrawText(
            perfText,
            perfTextRect,
            SDK::Color{100, 100, 120, 255},
            L"Segoe UI",
            12.0f,
            FW_NORMAL
        );
        
        // End drawing
        m_backend->EndDraw();
    }
    
    void ToggleBackend() {
        CreateBackend(!m_useGPU);
        m_frameCount = 0;
        m_avgFrameTime = 0.0;
        InvalidateRect(m_hwnd, nullptr, FALSE);
    }
    
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
        HardwareAccelDemo* pThis = nullptr;
        
        if (msg == WM_CREATE) {
            CREATESTRUCT* pCreate = (CREATESTRUCT*)lParam;
            pThis = (HardwareAccelDemo*)pCreate->lpCreateParams;
            SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pThis);
        } else {
            pThis = (HardwareAccelDemo*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
        }
        
        if (pThis) {
            switch (msg) {
                case WM_PAINT:
                    pThis->Render();
                    ValidateRect(hwnd, nullptr);
                    return 0;
                    
                case WM_KEYDOWN:
                    if (wParam == VK_SPACE) {
                        pThis->ToggleBackend();
                    }
                    return 0;
                    
                case WM_SIZE:
                    // Re-create backend on resize
                    if (pThis->m_backend) {
                        pThis->m_backend->Shutdown();
                        pThis->m_backend->Initialize(hwnd);
                    }
                    InvalidateRect(hwnd, nullptr, FALSE);
                    return 0;
                    
                case WM_DESTROY:
                    PostQuitMessage(0);
                    return 0;
            }
        }
        
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    
    void Run() {
        MSG msg;
        while (GetMessage(&msg, nullptr, 0, 0)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            
            // Continuous rendering
            InvalidateRect(m_hwnd, nullptr, FALSE);
        }
    }
    
    void Shutdown() {
        if (m_backend) {
            m_backend->Shutdown();
        }
        SDK::Shutdown();
    }
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int) {
    HardwareAccelDemo demo;
    
    if (!demo.Initialize(hInstance)) {
        MessageBoxW(nullptr, L"Failed to initialize demo", L"Error", MB_ICONERROR);
        return 1;
    }
    
    demo.Run();
    demo.Shutdown();
    
    return 0;
}
