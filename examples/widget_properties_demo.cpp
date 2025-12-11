/**
 * Widget Properties Demo
 * 
 * Demonstrates the comprehensive widget property system including:
 * - Padding and Margin
 * - Size constraints (min/max)
 * - Opacity and transparency
 * - Border width and radius
 * - Font properties (family, size, bold, italic)
 * - Tooltips
 * - Custom cursors
 * - Z-index layering
 * - Named widgets
 */

#include "../include/SDK/SDK.h"
#include <string>

HWND g_mainWindow = nullptr;
std::shared_ptr<SDK::WidgetManager> g_widgetManager;

// Create a styled button with all properties
std::shared_ptr<SDK::Button> CreateStyledButton(int x, int y, const std::wstring& text) {
    auto button = std::make_shared<SDK::Button>(text);
    
    // Position and size
    button->SetBounds(x, y, 150, 45);
    
    // Layout properties
    button->SetPadding(12, 8, 12, 8);  // Internal spacing
    button->SetMargin(10);              // External spacing
    button->SetMinSize(100, 35);        // Minimum size
    button->SetMaxSize(200, 60);        // Maximum size
    
    // Visual properties
    button->SetOpacity(0.95f);          // Slightly transparent
    button->SetBorderWidth(2);          // 2px border
    button->SetBorderRadius(8);         // Rounded corners
    
    // Font properties
    button->SetFontFamily(L"Segoe UI");
    button->SetFontSize(14);
    button->SetFontBold(true);
    button->SetFontItalic(false);
    
    // Interaction properties
    button->SetTooltipText(L"Click to see properties in action!");
    button->SetCursor(LoadCursor(nullptr, IDC_HAND));
    
    // Identification
    button->SetName(L"StyledButton");
    button->SetZIndex(10);
    
    return button;
}

// Create a labeled panel with custom properties
std::shared_ptr<SDK::Panel> CreateStyledPanel(int x, int y, int width, int height) {
    auto panel = std::make_shared<SDK::Panel>();
    
    // Position and size
    panel->SetBounds(x, y, width, height);
    panel->SetTitle(L"Property Demo Panel");
    
    // Layout
    panel->SetPadding(15);              // Space inside panel
    panel->SetMargin(5, 5, 5, 5);       // Space outside panel
    
    // Visual
    panel->SetOpacity(0.9f);            // Semi-transparent
    panel->SetBorderWidth(1);
    panel->SetBorderRadius(12);         // Rounded corners
    
    // Identification
    panel->SetName(L"DemoPanel");
    panel->SetZIndex(5);
    panel->SetTooltipText(L"This panel demonstrates property system");
    
    return panel;
}

// Create a text box with custom font
std::shared_ptr<SDK::TextBox> CreateStyledTextBox(int x, int y) {
    auto textBox = std::make_shared<SDK::TextBox>();
    
    // Position and size with constraints
    textBox->SetBounds(x, y, 250, 35);
    textBox->SetMinSize(150, 25);
    textBox->SetMaxSize(400, 50);
    
    // Layout
    textBox->SetPadding(8, 5, 8, 5);
    textBox->SetMargin(10);
    
    // Visual
    textBox->SetOpacity(1.0f);
    textBox->SetBorderWidth(1);
    textBox->SetBorderRadius(4);
    
    // Font - monospace for code input
    textBox->SetFontFamily(L"Consolas");
    textBox->SetFontSize(11);
    textBox->SetFontBold(false);
    textBox->SetFontItalic(false);
    
    // Interaction
    textBox->SetPlaceholder(L"Type here...");
    textBox->SetTooltipText(L"Input field with custom font");
    textBox->SetName(L"InputBox");
    textBox->SetZIndex(8);
    
    return textBox;
}

// Create labels showing property values
std::shared_ptr<SDK::Label> CreatePropertyLabel(int x, int y, const std::wstring& text) {
    auto label = std::make_shared<SDK::Label>(text);
    
    label->SetPosition(x, y);
    label->SetSize(400, 25);
    
    // Font styling
    label->SetFontFamily(L"Segoe UI");
    label->SetFontSize(10);
    label->SetFontBold(false);
    label->SetFontItalic(true);
    
    // Visual
    label->SetOpacity(0.85f);
    label->SetPadding(5);
    
    label->SetName(L"InfoLabel");
    label->SetZIndex(15);
    
    return label;
}

// Create slider with all properties
std::shared_ptr<SDK::Slider> CreateStyledSlider(int x, int y) {
    auto slider = std::make_shared<SDK::Slider>();
    
    slider->SetBounds(x, y, 200, 30);
    slider->SetRange(0.0f, 100.0f);
    slider->SetValue(50.0f);
    
    // Properties
    slider->SetPadding(5);
    slider->SetMargin(10, 5, 10, 5);
    slider->SetOpacity(0.95f);
    slider->SetBorderRadius(15);  // Rounded track
    
    slider->SetTooltipText(L"Drag to adjust value");
    slider->SetName(L"ValueSlider");
    slider->SetZIndex(10);
    
    return slider;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            
            // Clear background
            RECT clientRect;
            GetClientRect(hwnd, &clientRect);
            FillRect(hdc, &clientRect, (HBRUSH)GetStockObject(WHITE_BRUSH));
            
            // Draw title
            SetBkMode(hdc, TRANSPARENT);
            HFONT titleFont = CreateFontW(24, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
            HFONT oldFont = (HFONT)SelectObject(hdc, titleFont);
            
            RECT titleRect = clientRect;
            titleRect.top = 10;
            titleRect.bottom = 50;
            DrawTextW(hdc, L"Widget Properties Demo", -1, &titleRect,
                DT_CENTER | DT_VCENTER | DT_SINGLELINE);
            
            SelectObject(hdc, oldFont);
            DeleteObject(titleFont);
            
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
        
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    // Initialize SDK
    if (!SDK::Initialize()) {
        MessageBoxW(nullptr, L"Failed to initialize SDK", L"Error", MB_OK | MB_ICONERROR);
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
    wc.lpszClassName = L"WidgetPropertiesDemo";
    
    if (!RegisterClassExW(&wc)) {
        MessageBoxW(nullptr, L"Failed to register window class", L"Error", MB_OK | MB_ICONERROR);
        return -1;
    }
    
    // Create main window
    g_mainWindow = CreateWindowExW(
        WS_EX_LAYERED,
        L"WidgetPropertiesDemo",
        L"Widget Properties Demo",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        900, 700,
        nullptr, nullptr, hInstance, nullptr
    );
    
    if (!g_mainWindow) {
        MessageBoxW(nullptr, L"Failed to create window", L"Error", MB_OK | MB_ICONERROR);
        return -1;
    }
    
    // Register with SDK
    auto window = SDK::WindowManager::GetInstance().RegisterWindow(g_mainWindow);
    if (window) {
        window->SetDepth(SDK::WindowDepth::FOREGROUND);
        auto theme = std::make_shared<SDK::Theme>(SDK::Theme::CreateLightTheme());
        window->SetTheme(theme);
    }
    
    // Create widget manager
    g_widgetManager = std::make_shared<SDK::WidgetManager>();
    
    // Create demo widgets
    
    // Styled panel
    auto panel = CreateStyledPanel(50, 60, 800, 580);
    g_widgetManager->AddWidget(panel);
    
    // Styled buttons demonstrating different properties
    auto button1 = CreateStyledButton(70, 100, L"Button 1");
    button1->SetEventCallback([](SDK::Widget* w, SDK::WidgetEvent e, void* d) {
        if (e == SDK::WidgetEvent::CLICK) {
            MessageBoxW(nullptr, L"Button 1 clicked!\n\nProperties:\n- Padding: 12px\n- Border Radius: 8px\n- Font: Bold 14pt", 
                L"Button Clicked", MB_OK);
        }
    });
    g_widgetManager->AddWidget(button1);
    
    auto button2 = CreateStyledButton(250, 100, L"Button 2");
    button2->SetOpacity(0.8f);
    button2->SetBorderRadius(15);
    g_widgetManager->AddWidget(button2);
    
    auto button3 = CreateStyledButton(430, 100, L"Button 3");
    button3->SetFontItalic(true);
    button3->SetBorderRadius(20);
    g_widgetManager->AddWidget(button3);
    
    // Text boxes with different fonts
    auto textBox1 = CreateStyledTextBox(70, 180);
    g_widgetManager->AddWidget(textBox1);
    
    auto textBox2 = CreateStyledTextBox(70, 240);
    textBox2->SetFontFamily(L"Courier New");
    textBox2->SetFontSize(12);
    g_widgetManager->AddWidget(textBox2);
    
    // Sliders
    auto slider1 = CreateStyledSlider(70, 300);
    g_widgetManager->AddWidget(slider1);
    
    auto slider2 = CreateStyledSlider(70, 360);
    slider2->SetOpacity(0.85f);
    g_widgetManager->AddWidget(slider2);
    
    // Info labels showing property details
    auto label1 = CreatePropertyLabel(70, 420, 
        L"All widgets now support: Padding, Margin, Min/Max Size, Opacity, Border Radius");
    g_widgetManager->AddWidget(label1);
    
    auto label2 = CreatePropertyLabel(70, 450,
        L"Font Properties: Family, Size, Bold, Italic");
    g_widgetManager->AddWidget(label2);
    
    auto label3 = CreatePropertyLabel(70, 480,
        L"Interaction: Tooltips, Custom Cursors, Z-Index for layering");
    g_widgetManager->AddWidget(label3);
    
    auto label4 = CreatePropertyLabel(70, 510,
        L"Identification: Named widgets with IDs for easy management");
    g_widgetManager->AddWidget(label4);
    
    auto label5 = CreatePropertyLabel(70, 560,
        L"Hover over widgets to see tooltips demonstrating the tooltip property!");
    label5->SetFontBold(true);
    label5->SetOpacity(1.0f);
    g_widgetManager->AddWidget(label5);
    
    // Show window
    ShowWindow(g_mainWindow, nCmdShow);
    UpdateWindow(g_mainWindow);
    
    // Message loop with animation support
    MSG msg = {};
    DWORD lastTime = GetTickCount();
    bool running = true;
    
    while (running) {
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                running = false;
                break;
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        
        if (!running) {
            break;
        }
        
        // Update animations
        DWORD currentTime = GetTickCount();
        float deltaTime = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;
        
        if (g_widgetManager) {
            g_widgetManager->UpdateAll(deltaTime);
        }
        
        InvalidateRect(g_mainWindow, nullptr, FALSE);
        
        // Sleep a bit to reduce CPU usage
        Sleep(16);  // ~60 FPS
    }
    
    SDK::Shutdown();
    return (int)msg.wParam;
}
