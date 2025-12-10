/**
 * 3D Camera Controller Demo
 * Demonstrates camera controls and 3D widget placement in the scene
 */

#include "../include/SDK/SDK.h"
#include <vector>
#include <cmath>

// Global state
HWND g_mainWindow = nullptr;
std::shared_ptr<SDK::Window> g_sdkWindow;
std::shared_ptr<SDK::CameraController> g_camera;
std::shared_ptr<SDK::Widget3DManager> g_widget3DManager;
float g_time = 0.0f;
LARGE_INTEGER g_frequency, g_lastTime;

// 3D scene objects
std::vector<SDK::Renderer::Vector3D> g_cubePositions;

// Forward declarations
void InitializeScene();
void UpdateScene(float deltaTime);
void RenderScene(HDC hdc, int width, int height);

// Window procedure
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
            
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            
            RECT clientRect;
            GetClientRect(hwnd, &clientRect);
            int width = clientRect.right - clientRect.left;
            int height = clientRect.bottom - clientRect.top;
            
            // Render the scene
            RenderScene(hdc, width, height);
            
            EndPaint(hwnd, &ps);
            return 0;
        }
        
        case WM_MOUSEMOVE: {
            if (g_camera && g_widget3DManager) {
                int x = LOWORD(lParam);
                int y = HIWORD(lParam);
                
                RECT clientRect;
                GetClientRect(hwnd, &clientRect);
                int width = clientRect.right - clientRect.left;
                int height = clientRect.bottom - clientRect.top;
                
                // Handle widget interaction
                g_widget3DManager->HandleMouseMove(x, y, width, height, g_camera.get());
                
                // Forward to camera
                g_camera->HandleMouseMove(x, y);
                
                InvalidateRect(hwnd, nullptr, FALSE);
            }
            return 0;
        }
        
        case WM_LBUTTONDOWN: {
            if (g_camera && g_widget3DManager) {
                int x = LOWORD(lParam);
                int y = HIWORD(lParam);
                
                RECT clientRect;
                GetClientRect(hwnd, &clientRect);
                int width = clientRect.right - clientRect.left;
                int height = clientRect.bottom - clientRect.top;
                
                // Handle widget click first
                if (!g_widget3DManager->HandleMouseDown(x, y, width, height, 0, g_camera.get())) {
                    // If no widget was clicked, handle camera
                    g_camera->HandleMouseDown(x, y, 0);
                }
                
                InvalidateRect(hwnd, nullptr, FALSE);
            }
            return 0;
        }
        
        case WM_LBUTTONUP: {
            if (g_camera && g_widget3DManager) {
                int x = LOWORD(lParam);
                int y = HIWORD(lParam);
                
                RECT clientRect;
                GetClientRect(hwnd, &clientRect);
                int width = clientRect.right - clientRect.left;
                int height = clientRect.bottom - clientRect.top;
                
                g_widget3DManager->HandleMouseUp(x, y, width, height, 0, g_camera.get());
                g_camera->HandleMouseUp(x, y, 0);
                
                InvalidateRect(hwnd, nullptr, FALSE);
            }
            return 0;
        }
        
        case WM_MBUTTONDOWN: {
            if (g_camera) {
                int x = LOWORD(lParam);
                int y = HIWORD(lParam);
                g_camera->HandleMouseDown(x, y, 1);
            }
            return 0;
        }
        
        case WM_MBUTTONUP: {
            if (g_camera) {
                int x = LOWORD(lParam);
                int y = HIWORD(lParam);
                g_camera->HandleMouseUp(x, y, 1);
            }
            return 0;
        }
        
        case WM_RBUTTONDOWN: {
            if (g_camera) {
                int x = LOWORD(lParam);
                int y = HIWORD(lParam);
                g_camera->HandleMouseDown(x, y, 2);
            }
            return 0;
        }
        
        case WM_RBUTTONUP: {
            if (g_camera) {
                int x = LOWORD(lParam);
                int y = HIWORD(lParam);
                g_camera->HandleMouseUp(x, y, 2);
            }
            return 0;
        }
        
        case WM_MOUSEWHEEL: {
            if (g_camera) {
                int delta = GET_WHEEL_DELTA_WPARAM(wParam);
                g_camera->HandleMouseWheel(delta);
                InvalidateRect(hwnd, nullptr, FALSE);
            }
            return 0;
        }
        
        case WM_KEYDOWN: {
            if (g_camera) {
                g_camera->HandleKeyDown((int)wParam);
            }
            return 0;
        }
        
        case WM_KEYUP: {
            if (g_camera) {
                g_camera->HandleKeyUp((int)wParam);
            }
            return 0;
        }
        
        case WM_SIZE: {
            InvalidateRect(hwnd, nullptr, FALSE);
            return 0;
        }
    }
    
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void InitializeScene() {
    // Initialize camera
    g_camera = std::make_shared<SDK::CameraController>();
    g_camera->SetPosition(0.0f, 0.0f, -300.0f);
    g_camera->SetTarget(0.0f, 0.0f, 0.0f);
    g_camera->SetCameraMode(SDK::CameraController::CameraMode::MODE_3D);
    
    // Initialize widget manager
    g_widget3DManager = std::make_shared<SDK::Widget3DManager>();
    
    // Create 3D cubes for reference
    g_cubePositions.push_back(SDK::Renderer::Vector3D(-100.0f, 0.0f, 0.0f));
    g_cubePositions.push_back(SDK::Renderer::Vector3D(100.0f, 0.0f, 0.0f));
    g_cubePositions.push_back(SDK::Renderer::Vector3D(0.0f, -100.0f, 0.0f));
    g_cubePositions.push_back(SDK::Renderer::Vector3D(0.0f, 100.0f, 0.0f));
    g_cubePositions.push_back(SDK::Renderer::Vector3D(0.0f, 0.0f, 100.0f));
    
    // Add 3D widgets to the scene
    
    // Add buttons around the origin
    auto button1 = std::make_shared<SDK::Button3D>(L"Click Me!");
    button1->SetPosition3D(-150.0f, 50.0f, 0.0f);
    button1->SetEventCallback([](SDK::Widget* w, SDK::WidgetEvent e, void* data) {
        if (e == SDK::WidgetEvent::CLICK) {
            MessageBoxW(nullptr, L"Button 1 clicked in 3D space!", L"3D Widget", MB_OK);
        }
    });
    g_widget3DManager->AddWidget(button1);
    
    auto button2 = std::make_shared<SDK::Button3D>(L"Press Me!");
    button2->SetPosition3D(150.0f, 50.0f, 0.0f);
    button2->SetEventCallback([](SDK::Widget* w, SDK::WidgetEvent e, void* data) {
        if (e == SDK::WidgetEvent::CLICK) {
            MessageBoxW(nullptr, L"Button 2 clicked in 3D space!", L"3D Widget", MB_OK);
        }
    });
    g_widget3DManager->AddWidget(button2);
    
    // Add labels
    auto label1 = std::make_shared<SDK::Label3D>(L"3D Scene Camera Demo");
    label1->SetPosition3D(0.0f, 150.0f, 0.0f);
    label1->SetTransparent(false);
    g_widget3DManager->AddWidget(label1);
    
    auto label2 = std::make_shared<SDK::Label3D>(L"Controls:");
    label2->SetPosition3D(-200.0f, -50.0f, 0.0f);
    g_widget3DManager->AddWidget(label2);
    
    auto label3 = std::make_shared<SDK::Label3D>(L"Left Mouse: Orbit");
    label3->SetPosition3D(-200.0f, -80.0f, 0.0f);
    g_widget3DManager->AddWidget(label3);
    
    auto label4 = std::make_shared<SDK::Label3D>(L"Middle Mouse: Pan");
    label4->SetPosition3D(-200.0f, -110.0f, 0.0f);
    g_widget3DManager->AddWidget(label4);
    
    auto label5 = std::make_shared<SDK::Label3D>(L"Right Mouse: Zoom");
    label5->SetPosition3D(-200.0f, -140.0f, 0.0f);
    g_widget3DManager->AddWidget(label5);
    
    auto label6 = std::make_shared<SDK::Label3D>(L"WASD/Arrows: Move");
    label6->SetPosition3D(150.0f, -80.0f, 0.0f);
    g_widget3DManager->AddWidget(label6);
    
    auto label7 = std::make_shared<SDK::Label3D>(L"Q/E: Up/Down");
    label7->SetPosition3D(150.0f, -110.0f, 0.0f);
    g_widget3DManager->AddWidget(label7);
    
    auto label8 = std::make_shared<SDK::Label3D>(L"R: Reset Camera");
    label8->SetPosition3D(150.0f, -140.0f, 0.0f);
    g_widget3DManager->AddWidget(label8);
    
    // Add a panel
    auto panel = std::make_shared<SDK::Panel3D>();
    panel->SetPosition3D(0.0f, -50.0f, 50.0f);
    panel->SetSize(180, 120);
    g_widget3DManager->AddWidget(panel);
    
    // Add label on panel
    auto panelLabel = std::make_shared<SDK::Label3D>(L"3D Panel");
    panelLabel->SetPosition3D(0.0f, -50.0f, 51.0f);
    panelLabel->SetTransparent(true);
    g_widget3DManager->AddWidget(panelLabel);
}

void UpdateScene(float deltaTime) {
    if (g_camera) {
        g_camera->Update(deltaTime);
    }
    
    if (g_widget3DManager && g_camera) {
        RECT clientRect;
        GetClientRect(g_mainWindow, &clientRect);
        int width = clientRect.right - clientRect.left;
        int height = clientRect.bottom - clientRect.top;
        int originX = width / 2;
        int originY = height / 2;
        
        g_widget3DManager->UpdateAll(deltaTime, g_camera.get(), originX, originY);
    }
    
    g_time += deltaTime;
}

void RenderScene(HDC hdc, int width, int height) {
    // Clear background
    RECT rect = {0, 0, width, height};
    HBRUSH bgBrush = CreateSolidBrush(RGB(20, 20, 30));
    FillRect(hdc, &rect, bgBrush);
    DeleteObject(bgBrush);
    
    int originX = width / 2;
    int originY = height / 2;
    
    // Draw grid for reference
    SetBkMode(hdc, TRANSPARENT);
    HPEN gridPen = CreatePen(PS_SOLID, 1, RGB(50, 50, 60));
    HPEN oldPen = (HPEN)SelectObject(hdc, gridPen);
    
    for (int i = -5; i <= 5; i++) {
        SDK::Renderer::Vector3D start1(-250.0f, 0.0f, i * 50.0f);
        SDK::Renderer::Vector3D end1(250.0f, 0.0f, i * 50.0f);
        SDK::Renderer::Vector3D start2(i * 50.0f, 0.0f, -250.0f);
        SDK::Renderer::Vector3D end2(i * 50.0f, 0.0f, 250.0f);
        
        if (g_camera) {
            SDK::Renderer::Vector3D vs1, ve1, vs2, ve2;
            g_camera->TransformPoint3D(start1, vs1);
            g_camera->TransformPoint3D(end1, ve1);
            g_camera->TransformPoint3D(start2, vs2);
            g_camera->TransformPoint3D(end2, ve2);
            
            int x1, y1, x2, y2, x3, y3, x4, y4;
            g_camera->ApplyToProjection(start1, x1, y1, originX, originY, 1.0f);
            g_camera->ApplyToProjection(end1, x2, y2, originX, originY, 1.0f);
            g_camera->ApplyToProjection(start2, x3, y3, originX, originY, 1.0f);
            g_camera->ApplyToProjection(end2, x4, y4, originX, originY, 1.0f);
            
            MoveToEx(hdc, x1, y1, nullptr);
            LineTo(hdc, x2, y2);
            MoveToEx(hdc, x3, y3, nullptr);
            LineTo(hdc, x4, y4);
        }
    }
    
    SelectObject(hdc, oldPen);
    DeleteObject(gridPen);
    
    // Draw 3D cubes
    for (const auto& pos : g_cubePositions) {
        if (g_camera) {
            // Draw cube with camera transformation
            float size = 30.0f;
            SDK::Renderer::Render3DCube(hdc, pos, size, originX, originY, 
                                       SDK::Color(100, 150, 255, 255), 
                                       g_time * 20.0f, g_time * 30.0f, 0.0f);
        }
    }
    
    // Draw axes
    HPEN axisPen = CreatePen(PS_SOLID, 2, RGB(255, 0, 0));
    SelectObject(hdc, axisPen);
    SDK::Renderer::Vector3D xAxis(100.0f, 0.0f, 0.0f);
    if (g_camera) {
        int x1, y1, x2, y2;
        g_camera->ApplyToProjection(SDK::Renderer::Vector3D(0, 0, 0), x1, y1, originX, originY, 1.0f);
        g_camera->ApplyToProjection(xAxis, x2, y2, originX, originY, 1.0f);
        MoveToEx(hdc, x1, y1, nullptr);
        LineTo(hdc, x2, y2);
    }
    DeleteObject(axisPen);
    
    axisPen = CreatePen(PS_SOLID, 2, RGB(0, 255, 0));
    SelectObject(hdc, axisPen);
    SDK::Renderer::Vector3D yAxis(0.0f, 100.0f, 0.0f);
    if (g_camera) {
        int x1, y1, x2, y2;
        g_camera->ApplyToProjection(SDK::Renderer::Vector3D(0, 0, 0), x1, y1, originX, originY, 1.0f);
        g_camera->ApplyToProjection(yAxis, x2, y2, originX, originY, 1.0f);
        MoveToEx(hdc, x1, y1, nullptr);
        LineTo(hdc, x2, y2);
    }
    DeleteObject(axisPen);
    
    axisPen = CreatePen(PS_SOLID, 2, RGB(0, 0, 255));
    SelectObject(hdc, axisPen);
    SDK::Renderer::Vector3D zAxis(0.0f, 0.0f, 100.0f);
    if (g_camera) {
        int x1, y1, x2, y2;
        g_camera->ApplyToProjection(SDK::Renderer::Vector3D(0, 0, 0), x1, y1, originX, originY, 1.0f);
        g_camera->ApplyToProjection(zAxis, x2, y2, originX, originY, 1.0f);
        MoveToEx(hdc, x1, y1, nullptr);
        LineTo(hdc, x2, y2);
    }
    DeleteObject(axisPen);
    
    // Render 3D widgets
    if (g_widget3DManager && g_camera) {
        g_widget3DManager->RenderAll(hdc, g_camera.get(), originX, originY);
    }
    
    // Draw instructions
    SetTextColor(hdc, RGB(200, 200, 200));
    HFONT font = CreateFontW(14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                            DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Arial");
    HFONT oldFont = (HFONT)SelectObject(hdc, font);
    
    RECT textRect = {10, 10, width - 10, 100};
    DrawTextW(hdc, L"3D Camera Controller Demo - Use mouse and keyboard to navigate", -1, &textRect, DT_LEFT | DT_TOP);
    
    SelectObject(hdc, oldFont);
    DeleteObject(font);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // Initialize SDK
    SDK::Initialize();
    
    // Register window class
    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = L"CameraDemo3D";
    
    RegisterClassExW(&wc);
    
    // Create window using universal function
    SDK::PromptWindowBuilder::WindowConfig config;
    config.className = L"CameraDemo3D";
    config.title = L"3D Camera Controller Demo - 5D GUI SDK";
    config.width = 1024;
    config.height = 768;
    config.exStyle = 0;  // No layered window for this demo
    
    g_mainWindow = SDK::PromptWindowBuilder::CreateWidgetsWindow(config, hInstance);
    
    if (!g_mainWindow) {
        MessageBoxW(nullptr, L"Failed to create window!", L"Error", MB_ICONERROR);
        return 1;
    }
    
    // Initialize scene
    InitializeScene();
    
    // Show window
    ShowWindow(g_mainWindow, nCmdShow);
    UpdateWindow(g_mainWindow);
    
    // Initialize timer
    QueryPerformanceFrequency(&g_frequency);
    QueryPerformanceCounter(&g_lastTime);
    
    // Message loop with animation
    MSG msg = {};
    bool running = true;
    
    while (running) {
        // Process messages
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                running = false;
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        
        if (running) {
            // Calculate delta time
            LARGE_INTEGER currentTime;
            QueryPerformanceCounter(&currentTime);
            float deltaTime = (float)(currentTime.QuadPart - g_lastTime.QuadPart) / (float)g_frequency.QuadPart;
            g_lastTime = currentTime;
            
            // Update scene
            UpdateScene(deltaTime);
            
            // Redraw
            InvalidateRect(g_mainWindow, nullptr, FALSE);
            
            // Sleep to maintain frame rate
            Sleep(16); // ~60 FPS
        }
    }
    
    // Cleanup
    g_widget3DManager.reset();
    g_camera.reset();
    g_sdkWindow.reset();
    
    SDK::Shutdown();
    
    return (int)msg.wParam;
}
