/**
 * Widget Showcase Demo
 * Demonstrates all widgets, themes, and multidimensional rendering
 */

#include "../include/SDK/SDK.h"
#include <vector>
#include <cmath>

// Global state
HWND g_mainWindow = nullptr;
std::shared_ptr<SDK::Window> g_sdkWindow;
bool g_useDarkTheme = false;
float g_time = 0.0f;

// Window procedure
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
            
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            
            auto window = SDK::WindowManager::GetInstance().GetWindow(hwnd);
            if (window) {
                window->Render(hdc);
            }
            
            EndPaint(hwnd, &ps);
            return 0;
        }
        
        case WM_MOUSEMOVE: {
            if (g_sdkWindow) {
                int x = LOWORD(lParam);
                int y = HIWORD(lParam);
                g_sdkWindow->HandleWidgetMouseMove(x, y);
                InvalidateRect(hwnd, nullptr, FALSE);
            }
            return 0;
        }
        
        case WM_LBUTTONDOWN: {
            if (g_sdkWindow) {
                int x = LOWORD(lParam);
                int y = HIWORD(lParam);
                g_sdkWindow->HandleWidgetMouseDown(x, y, 0);
                InvalidateRect(hwnd, nullptr, FALSE);
            }
            return 0;
        }
        
        case WM_LBUTTONUP: {
            if (g_sdkWindow) {
                int x = LOWORD(lParam);
                int y = HIWORD(lParam);
                g_sdkWindow->HandleWidgetMouseUp(x, y, 0);
                InvalidateRect(hwnd, nullptr, FALSE);
            }
            return 0;
        }
        
        case WM_KEYDOWN: {
            if (g_sdkWindow) {
                g_sdkWindow->HandleWidgetKeyDown((int)wParam);
                InvalidateRect(hwnd, nullptr, FALSE);
            }
            return 0;
        }
        
        case WM_CHAR: {
            if (g_sdkWindow) {
                g_sdkWindow->HandleWidgetChar((wchar_t)wParam);
                InvalidateRect(hwnd, nullptr, FALSE);
            }
            return 0;
        }
    }
    
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

// Custom rendering callback
void RenderWidgetShowcase(HDC hdc) {
    RECT rect;
    GetClientRect(WindowFromDC(hdc), &rect);
    
    // Draw title
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(255, 255, 255));
    
    HFONT font = CreateFontW(24, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                             DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                             DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Arial");
    HFONT oldFont = (HFONT)SelectObject(hdc, font);
    
    RECT titleRect = {20, 40, rect.right - 20, 70};
    DrawTextW(hdc, L"Widget & Renderer Showcase", -1, &titleRect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    
    SelectObject(hdc, oldFont);
    DeleteObject(font);
    
    // Draw 3D visualization
    int centerX = rect.right - 150;
    int centerY = 150;
    
    // Render 3D cube
    SDK::Renderer::Vector3D cubeCenter(0, 0, 0);
    SDK::Renderer::Render3DCube(hdc, cubeCenter, 80.0f, centerX, centerY,
                                SDK::Color(100, 149, 237, 255), g_time * 0.5f, g_time * 0.3f, 0);
    
    // Draw multidimensional info
    SetTextColor(hdc, RGB(100, 100, 100));
    RECT infoRect = {rect.right - 200, 250, rect.right - 20, 350};
    const wchar_t* infoText = L"3D/4D/5D/6D\nRendering\nSupported";
    DrawTextW(hdc, infoText, -1, &infoRect, DT_CENTER | DT_WORDBREAK);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // Initialize SDK with inline hooking
    if (!SDK::Initialize()) {
        MessageBoxW(nullptr, L"Failed to initialize SDK", L"Error", MB_ICONERROR);
        return -1;
    }
    
    // Enable inline hooking
    SDK::WindowHook::GetInstance().Initialize(SDK::WindowHook::HookType::INLINE);
    
    // Register window class
    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = L"WidgetShowcase";
    
    if (!RegisterClassExW(&wc)) {
        MessageBoxW(nullptr, L"Window Registration Failed!", L"Error", MB_ICONERROR);
        SDK::Shutdown();
        return -1;
    }
    
    // Create main window using universal function
    SDK::PromptWindowBuilder::WindowConfig config;
    config.className = L"WidgetShowcase";
    config.title = L"Widget & Renderer Showcase";
    config.width = 1000;
    config.height = 700;
    config.theme = std::make_shared<SDK::Theme>(SDK::Theme::CreateModernTheme());
    config.depth = SDK::WindowDepth::FOREGROUND;
    config.roundedCorners = true;
    config.cornerRadius = 12;
    config.renderCallback = RenderWidgetShowcase;
    
    g_mainWindow = SDK::PromptWindowBuilder::CreateWidgetsWindow(config, hInstance);
    
    if (!g_mainWindow) {
        MessageBoxW(nullptr, L"Window Creation Failed!", L"Error", MB_ICONERROR);
        SDK::Shutdown();
        return -1;
    }
    
    // Get the SDK window for widget management
    g_sdkWindow = SDK::WindowManager::GetInstance().GetWindow(g_mainWindow);
    if (g_sdkWindow) {
        // Add widgets
        
        // Buttons section
        auto btnToggleTheme = std::make_shared<SDK::Button>(L"Toggle Theme");
        btnToggleTheme->SetPosition(20, 80);
        btnToggleTheme->SetSize(150, 35);
        btnToggleTheme->SetEventCallback([](SDK::Widget* widget, SDK::WidgetEvent event, void* data) {
            if (event == SDK::WidgetEvent::CLICK) {
                g_useDarkTheme = !g_useDarkTheme;
                auto newTheme = g_useDarkTheme ? 
                    std::make_shared<SDK::Theme>(SDK::Theme::CreateDarkTheme()) :
                    std::make_shared<SDK::Theme>(SDK::Theme::CreateLightTheme());
                g_sdkWindow->SetTheme(newTheme);
                InvalidateRect(g_mainWindow, nullptr, TRUE);
            }
        });
        g_sdkWindow->AddWidget(btnToggleTheme);
        
        auto btnTest = std::make_shared<SDK::Button>(L"Test Button");
        btnTest->SetPosition(180, 80);
        btnTest->SetSize(120, 35);
        g_sdkWindow->AddWidget(btnTest);
        
        // Label
        auto label = std::make_shared<SDK::Label>(L"Enter your name:");
        label->SetPosition(20, 130);
        label->SetSize(150, 25);
        g_sdkWindow->AddWidget(label);
        
        // TextBox
        auto textBox = std::make_shared<SDK::TextBox>();
        textBox->SetPosition(20, 155);
        textBox->SetSize(250, 30);
        textBox->SetPlaceholder(L"Type here...");
        g_sdkWindow->AddWidget(textBox);
        
        // CheckBox
        auto checkbox = std::make_shared<SDK::CheckBox>(L"Enable notifications");
        checkbox->SetPosition(20, 200);
        checkbox->SetSize(200, 20);
        g_sdkWindow->AddWidget(checkbox);
        
        // Separator
        auto separator = std::make_shared<SDK::Separator>(SDK::Separator::Orientation::HORIZONTAL);
        separator->SetPosition(20, 240);
        separator->SetSize(350, 2);
        g_sdkWindow->AddWidget(separator);
        
        // ComboBox
        auto comboBox = std::make_shared<SDK::ComboBox>();
        comboBox->SetPosition(20, 260);
        comboBox->SetSize(200, 30);
        comboBox->AddItem(L"Option 1");
        comboBox->AddItem(L"Option 2");
        comboBox->AddItem(L"Option 3");
        comboBox->SetSelectedIndex(0);
        g_sdkWindow->AddWidget(comboBox);
        
        // ListBox
        auto listBox = std::make_shared<SDK::ListBox>();
        listBox->SetPosition(20, 310);
        listBox->SetSize(200, 120);
        listBox->AddItem(L"Item 1");
        listBox->AddItem(L"Item 2");
        listBox->AddItem(L"Item 3");
        listBox->AddItem(L"Item 4");
        listBox->AddItem(L"Item 5");
        g_sdkWindow->AddWidget(listBox);
        
        // ListView with checkboxes
        auto listView = std::make_shared<SDK::ListView>();
        listView->SetPosition(240, 260);
        listView->SetSize(250, 170);
        listView->SetCheckboxEnabled(true);
        listView->AddItem(L"Task 1", false);
        listView->AddItem(L"Task 2", true);
        listView->AddItem(L"Task 3", false);
        listView->AddItem(L"Task 4", true);
        g_sdkWindow->AddWidget(listView);
        
        // TabControl
        auto tabControl = std::make_shared<SDK::TabControl>();
        tabControl->SetPosition(20, 450);
        tabControl->SetSize(600, 200);
        
        // Tab 1 content
        auto tab1Content = std::make_shared<SDK::Label>(L"This is tab 1 content");
        tab1Content->SetPosition(0, 0);
        
        // Tab 2 content
        auto tab2Content = std::make_shared<SDK::Label>(L"This is tab 2 content");
        tab2Content->SetPosition(0, 0);
        
        tabControl->AddTab(L"General", tab1Content);
        tabControl->AddTab(L"Advanced", tab2Content);
        tabControl->AddTab(L"About", nullptr);
        tabControl->SetActiveTab(0);
        g_sdkWindow->AddWidget(tabControl);
        
        g_sdkWindow->UpdateAppearance();
    }
    
    ShowWindow(g_mainWindow, nCmdShow);
    UpdateWindow(g_mainWindow);
    
    // Message loop with timing
    MSG msg = {};
    DWORD lastTime = GetTickCount();
    
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
        
        // Update time for animations
        DWORD currentTime = GetTickCount();
        float deltaTime = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;
        
        g_time += deltaTime;
        
        // Update widgets
        if (g_sdkWindow) {
            g_sdkWindow->UpdateWidgets(deltaTime);
        }
        
        // Update window manager
        SDK::WindowManager::GetInstance().Update(deltaTime);
        
        // Redraw for animations
        InvalidateRect(g_mainWindow, nullptr, FALSE);
        
        // Limit frame rate
        Sleep(16); // ~60 FPS
    }
    
    SDK::Shutdown();
    return (int)msg.wParam;
}
