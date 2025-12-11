/**
 * New Widgets Demo - Demonstrates the newly added widgets
 * Shows: Slider, RadioButton, Panel, SpinBox
 */

#include "../include/SDK/SDK.h"
#include <sstream>

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

// Global window pointer for message handling
SDK::Window* g_window = nullptr;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    // Initialize SDK
    SDK::Initialize();
    
    // Register window class
    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.lpszClassName = L"NewWidgetsDemoClass";
    RegisterClassExW(&wc);
    
    // Create window
    HWND hwnd = CreateWindowExW(
        WS_EX_LAYERED,
        L"NewWidgetsDemoClass",
        L"New Widgets Demo - Slider, RadioButton, Panel, SpinBox",
        WS_OVERLAPPEDWINDOW,
        100, 100, 800, 600,
        nullptr, nullptr, hInstance, nullptr
    );
    
    if (!hwnd) {
        MessageBoxW(nullptr, L"Failed to create window!", L"Error", MB_OK | MB_ICONERROR);
        return 1;
    }
    
    // Register with SDK
    g_window = SDK::WindowManager::GetInstance().RegisterWindow(hwnd);
    
    // Apply theme
    auto theme = std::make_shared<SDK::Theme>(SDK::Theme::CreateModernTheme());
    g_window->SetTheme(theme);
    g_window->SetDepth(SDK::WindowDepth::FOREGROUND);
    g_window->UpdateAppearance();
    
    // Create Panel for grouping
    auto panel1 = std::make_shared<SDK::Panel>();
    panel1->SetBounds(20, 20, 360, 250);
    panel1->SetTitle(L"Slider Controls");
    panel1->SetBackgroundColor(SDK::Color(245, 245, 245, 255));
    g_window->AddWidget(panel1);
    
    // Label for horizontal slider
    auto labelHSlider = std::make_shared<SDK::Label>(L"Horizontal Slider:");
    labelHSlider->SetPosition(40, 60);
    labelHSlider->SetSize(150, 20);
    g_window->AddWidget(labelHSlider);
    
    // Horizontal Slider
    auto hSlider = std::make_shared<SDK::Slider>(SDK::Slider::Orientation::HORIZONTAL);
    hSlider->SetPosition(40, 90);
    hSlider->SetSize(300, 30);
    hSlider->SetRange(0.0f, 100.0f);
    hSlider->SetValue(50.0f);
    g_window->AddWidget(hSlider);
    
    // Value label for horizontal slider
    auto labelHValue = std::make_shared<SDK::Label>(L"Value: 50");
    labelHValue->SetPosition(40, 130);
    labelHValue->SetSize(150, 20);
    labelHValue->SetId(1001); // ID to find this label later
    g_window->AddWidget(labelHValue);
    
    // Set callback for horizontal slider
    hSlider->SetEventCallback([](SDK::Widget* w, SDK::WidgetEvent e, void* data) {
        if (e == SDK::WidgetEvent::VALUE_CHANGED && data) {
            float value = *(float*)data;
            
            // Find and update the value label
            if (g_window) {
                for (auto& widget : g_window->GetWidgets()) {
                    if (widget->GetId() == 1001) {
                        SDK::Label* label = dynamic_cast<SDK::Label*>(widget.get());
                        if (label) {
                            std::wstringstream ss;
                            ss << L"Value: " << (int)value;
                            label->SetText(ss.str());
                        }
                        break;
                    }
                }
            }
        }
    });
    
    // Label for vertical slider
    auto labelVSlider = std::make_shared<SDK::Label>(L"Vertical Slider:");
    labelVSlider->SetPosition(210, 160);
    labelVSlider->SetSize(150, 20);
    g_window->AddWidget(labelVSlider);
    
    // Vertical Slider
    auto vSlider = std::make_shared<SDK::Slider>(SDK::Slider::Orientation::VERTICAL);
    vSlider->SetPosition(280, 180);
    vSlider->SetSize(30, 80);
    vSlider->SetRange(0.0f, 10.0f);
    vSlider->SetValue(5.0f);
    g_window->AddWidget(vSlider);
    
    // Create Panel for radio buttons
    auto panel2 = std::make_shared<SDK::Panel>();
    panel2->SetBounds(400, 20, 360, 250);
    panel2->SetTitle(L"Radio Button Group");
    panel2->SetBackgroundColor(SDK::Color(245, 245, 245, 255));
    g_window->AddWidget(panel2);
    
    // Radio buttons (same group)
    auto radio1 = std::make_shared<SDK::RadioButton>(L"Option 1", 0);
    radio1->SetPosition(420, 60);
    radio1->SetSize(200, 20);
    radio1->SetChecked(true);
    panel2->AddChild(radio1);
    g_window->AddWidget(radio1);
    
    auto radio2 = std::make_shared<SDK::RadioButton>(L"Option 2", 0);
    radio2->SetPosition(420, 90);
    radio2->SetSize(200, 20);
    panel2->AddChild(radio2);
    g_window->AddWidget(radio2);
    
    auto radio3 = std::make_shared<SDK::RadioButton>(L"Option 3", 0);
    radio3->SetPosition(420, 120);
    radio3->SetSize(200, 20);
    panel2->AddChild(radio3);
    g_window->AddWidget(radio3);
    
    // Label to show selected radio
    auto labelRadio = std::make_shared<SDK::Label>(L"Selected: Option 1");
    labelRadio->SetPosition(420, 160);
    labelRadio->SetSize(300, 20);
    labelRadio->SetId(1002);
    g_window->AddWidget(labelRadio);
    
    // Set callbacks for radio buttons
    auto radioCallback = [](SDK::Widget* w, SDK::WidgetEvent e, void* data) {
        if (e == SDK::WidgetEvent::VALUE_CHANGED && data) {
            bool checked = *(bool*)data;
            if (checked && g_window) {
                SDK::RadioButton* radio = dynamic_cast<SDK::RadioButton*>(w);
                if (radio) {
                    // Find and update the label
                    for (auto& widget : g_window->GetWidgets()) {
                        if (widget->GetId() == 1002) {
                            SDK::Label* label = dynamic_cast<SDK::Label*>(widget.get());
                            if (label) {
                                label->SetText(L"Selected: " + radio->GetText());
                            }
                            break;
                        }
                    }
                }
            }
        }
    };
    
    radio1->SetEventCallback(radioCallback);
    radio2->SetEventCallback(radioCallback);
    radio3->SetEventCallback(radioCallback);
    
    // Create Panel for SpinBox
    auto panel3 = std::make_shared<SDK::Panel>();
    panel3->SetBounds(20, 290, 360, 250);
    panel3->SetTitle(L"SpinBox Controls");
    panel3->SetBackgroundColor(SDK::Color(245, 245, 245, 255));
    g_window->AddWidget(panel3);
    
    // Label for SpinBox
    auto labelSpinBox = std::make_shared<SDK::Label>(L"Numeric Input (0-100):");
    labelSpinBox->SetPosition(40, 330);
    labelSpinBox->SetSize(200, 20);
    g_window->AddWidget(labelSpinBox);
    
    // SpinBox
    auto spinBox = std::make_shared<SDK::SpinBox>();
    spinBox->SetPosition(40, 360);
    spinBox->SetSize(150, 30);
    spinBox->SetRange(0, 100);
    spinBox->SetStep(5);
    spinBox->SetValue(50);
    g_window->AddWidget(spinBox);
    
    // Value label for SpinBox
    auto labelSpinValue = std::make_shared<SDK::Label>(L"Value: 50");
    labelSpinValue->SetPosition(40, 400);
    labelSpinValue->SetSize(150, 20);
    labelSpinValue->SetId(1003);
    g_window->AddWidget(labelSpinValue);
    
    // Set callback for SpinBox
    spinBox->SetEventCallback([](SDK::Widget* w, SDK::WidgetEvent e, void* data) {
        if (e == SDK::WidgetEvent::VALUE_CHANGED && data) {
            int value = *(int*)data;
            
            // Find and update the value label
            if (g_window) {
                for (auto& widget : g_window->GetWidgets()) {
                    if (widget->GetId() == 1003) {
                        SDK::Label* label = dynamic_cast<SDK::Label*>(widget.get());
                        if (label) {
                            std::wstringstream ss;
                            ss << L"Value: " << value;
                            label->SetText(ss.str());
                        }
                        break;
                    }
                }
            }
        }
    });
    
    // Info text
    auto infoLabel = std::make_shared<SDK::Label>(L"Try the new widgets! Sliders, radio buttons, and spin boxes.");
    infoLabel->SetPosition(400, 320);
    infoLabel->SetSize(350, 20);
    infoLabel->SetTextColor(SDK::Color(100, 100, 100, 255));
    g_window->AddWidget(infoLabel);
    
    // Checkbox to show/hide panels
    auto checkBox = std::make_shared<SDK::CheckBox>(L"Enable Advanced Mode");
    checkBox->SetPosition(400, 360);
    checkBox->SetSize(200, 20);
    g_window->AddWidget(checkBox);
    
    // Show window
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);
    
    // Message loop
    MSG msg = {};
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    // Cleanup
    SDK::Shutdown();
    
    return (int)msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_CLOSE:
            DestroyWindow(hwnd);
            return 0;
            
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
            
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            
            if (g_window) {
                g_window->Render(hdc);
            }
            
            EndPaint(hwnd, &ps);
            return 0;
        }
        
        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_MOUSEMOVE: {
            if (g_window) {
                int x = GET_X_LPARAM(lParam);
                int y = GET_Y_LPARAM(lParam);
                
                if (uMsg == WM_LBUTTONDOWN) {
                    for (auto& widget : g_window->GetWidgets()) {
                        if (widget->HandleMouseDown(x, y, 0)) {
                            InvalidateRect(hwnd, nullptr, FALSE);
                            break;
                        }
                    }
                } else if (uMsg == WM_LBUTTONUP) {
                    for (auto& widget : g_window->GetWidgets()) {
                        if (widget->HandleMouseUp(x, y, 0)) {
                            InvalidateRect(hwnd, nullptr, FALSE);
                            break;
                        }
                    }
                } else if (uMsg == WM_MOUSEMOVE) {
                    for (auto& widget : g_window->GetWidgets()) {
                        widget->HandleMouseMove(x, y);
                    }
                    InvalidateRect(hwnd, nullptr, FALSE);
                }
            }
            return 0;
        }
        
        case WM_KEYDOWN: {
            if (g_window) {
                for (auto& widget : g_window->GetWidgets()) {
                    if (widget->HandleKeyDown((int)wParam)) {
                        InvalidateRect(hwnd, nullptr, FALSE);
                        break;
                    }
                }
            }
            return 0;
        }
    }
    
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
