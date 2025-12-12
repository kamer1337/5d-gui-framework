/**
 * Advanced Neural Network Demo
 * 
 * Demonstrates the enhanced neural network-powered GUI creation system (v2.0)
 * Shows polymorphism, complex prompts, multi-widget parsing, and layout intelligence.
 */

#include "SDK/SDK.h"
#include <string>

// Window procedure for the demo windows
LRESULT CALLBACK AdvancedNeuralDemoWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
            
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            
            // Get the SDK window
            auto window = SDK::WindowManager::GetInstance().GetWindow(hwnd);
            if (window) {
                window->Render(hdc);
            }
            
            EndPaint(hwnd, &ps);
            return 0;
        }
        
        case WM_ERASEBKGND:
            return 1;  // Prevent flicker
    }
    
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // Initialize SDK
    if (!SDK::Initialize()) {
        MessageBoxW(nullptr, L"Failed to initialize SDK", L"Error", MB_OK | MB_ICONERROR);
        return -1;
    }
    
    // Register window class
    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = AdvancedNeuralDemoWndProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.lpszClassName = L"AdvancedNeuralDemoClass";
    
    if (!RegisterClassExW(&wc)) {
        MessageBoxW(nullptr, L"Failed to register window class", L"Error", MB_OK | MB_ICONERROR);
        SDK::Shutdown();
        return -1;
    }
    
    // Create a neural prompt builder
    SDK::NeuralPromptBuilder builder;
    
    // Demo 1: Multi-widget prompt with advanced widgets
    MessageBoxW(nullptr, 
        L"Demo 1: Multi-Widget Complex Prompt (v2.0 Feature)\n\n"
        L"Prompt: \"Create window 800x600 'Advanced Demo' with slider, combobox, "
        L"checkbox, and button\"",
        L"Advanced Neural Network Demo", MB_OK | MB_ICONINFORMATION);
    
    HWND window1 = builder.BuildFromPrompt(
        L"Create window 800x600 'Advanced Demo' with slider, combobox, checkbox, and button",
        hInstance
    );
    
    if (window1) {
        auto sdkWindow1 = SDK::WindowManager::GetInstance().RegisterWindow(window1);
        if (sdkWindow1) {
            auto theme = std::make_shared<SDK::Theme>(SDK::Theme::CreateModernTheme());
            sdkWindow1->SetTheme(theme);
            sdkWindow1->SetDepth(SDK::WindowDepth::FOREGROUND);
            sdkWindow1->UpdateAppearance();
        }
        
        ShowWindow(window1, nCmdShow);
        UpdateWindow(window1);
    }
    
    Sleep(2000);
    
    // Demo 2: Vertical layout
    MessageBoxW(nullptr,
        L"Demo 2: Vertical Layout Intelligence (v2.0 Feature)\n\n"
        L"Prompt: \"Create dialog 400x500 'Settings' with label, textbox, "
        L"checkbox, slider, and button arranged vertically\"",
        L"Advanced Neural Network Demo", MB_OK | MB_ICONINFORMATION);
    
    HWND window2 = builder.BuildFromPrompt(
        L"Create dialog 400x500 'Settings' with label, textbox, checkbox, slider, and button arranged vertically",
        hInstance
    );
    
    if (window2) {
        auto sdkWindow2 = SDK::WindowManager::GetInstance().RegisterWindow(window2);
        if (sdkWindow2) {
            auto theme = std::make_shared<SDK::Theme>(SDK::Theme::CreateAuroraTheme());
            sdkWindow2->SetTheme(theme);
            sdkWindow2->SetDepth(SDK::WindowDepth::FOREGROUND);
            sdkWindow2->SetRoundedCorners(true, 16);
            sdkWindow2->UpdateAppearance();
        }
        
        ShowWindow(window2, SW_SHOW);
        UpdateWindow(window2);
    }
    
    Sleep(2000);
    
    // Demo 3: Horizontal layout
    MessageBoxW(nullptr,
        L"Demo 3: Horizontal Layout (v2.0 Feature)\n\n"
        L"Prompt: \"Create window 800x150 'Toolbar' with 5 buttons side by side\"",
        L"Advanced Neural Network Demo", MB_OK | MB_ICONINFORMATION);
    
    HWND window3 = builder.BuildFromPrompt(
        L"Create window 800x150 'Toolbar' with 5 buttons side by side",
        hInstance
    );
    
    if (window3) {
        auto sdkWindow3 = SDK::WindowManager::GetInstance().RegisterWindow(window3);
        if (sdkWindow3) {
            auto theme = std::make_shared<SDK::Theme>(SDK::Theme::CreateDarkTheme());
            sdkWindow3->SetTheme(theme);
            sdkWindow3->SetDepth(SDK::WindowDepth::FOREGROUND);
            sdkWindow3->UpdateAppearance();
        }
        
        ShowWindow(window3, SW_SHOW);
        UpdateWindow(window3);
    }
    
    Sleep(2000);
    
    // Demo 4: Grid layout
    MessageBoxW(nullptr,
        L"Demo 4: Grid Layout (v2.0 Feature)\n\n"
        L"Prompt: \"Create window 600x600 'Control Panel' with 9 buttons in a grid\"",
        L"Advanced Neural Network Demo", MB_OK | MB_ICONINFORMATION);
    
    HWND window4 = builder.BuildFromPrompt(
        L"Create window 600x600 'Control Panel' with 9 buttons in a grid",
        hInstance
    );
    
    if (window4) {
        auto sdkWindow4 = SDK::WindowManager::GetInstance().RegisterWindow(window4);
        if (sdkWindow4) {
            auto theme = std::make_shared<SDK::Theme>(SDK::Theme::CreateLightTheme());
            sdkWindow4->SetTheme(theme);
            sdkWindow4->SetDepth(SDK::WindowDepth::FOREGROUND);
            sdkWindow4->UpdateAppearance();
        }
        
        ShowWindow(window4, SW_SHOW);
        UpdateWindow(window4);
    }
    
    Sleep(2000);
    
    // Demo 5: Test polymorphic widget support
    MessageBoxW(nullptr,
        L"Demo 5: Polymorphic Widget Support (v2.0 Feature)\n\n"
        L"Testing neural network recognition of various widget types...",
        L"Advanced Neural Network Demo", MB_OK | MB_ICONINFORMATION);
    
    auto nn = builder.GetNeuralNetwork();
    
    std::vector<std::wstring> testPrompts = {
        L"Create window with slider",
        L"Add a combobox with options",
        L"Create form with listbox",
        L"Add a panel with title Settings",
        L"Create window with separator",
        L"Add a spinbox for numeric input",
        L"Create window with radiobutton"
    };
    
    std::wstring results = L"Neural Network Widget Recognition Test:\n\n";
    
    for (const auto& prompt : testPrompts) {
        auto parsed = nn->ParsePrompt(prompt);
        
        results += L"Prompt: \"" + prompt + L"\"\n";
        results += L"Intent: ";
        
        switch (parsed.intent) {
            case SDK::NeuralNetwork::Intent::ADD_SLIDER:
                results += L"Add Slider";
                break;
            case SDK::NeuralNetwork::Intent::ADD_COMBOBOX:
                results += L"Add ComboBox";
                break;
            case SDK::NeuralNetwork::Intent::ADD_LISTBOX:
                results += L"Add ListBox";
                break;
            case SDK::NeuralNetwork::Intent::ADD_PANEL:
                results += L"Add Panel";
                break;
            case SDK::NeuralNetwork::Intent::ADD_SEPARATOR:
                results += L"Add Separator";
                break;
            case SDK::NeuralNetwork::Intent::ADD_SPINBOX:
                results += L"Add SpinBox";
                break;
            case SDK::NeuralNetwork::Intent::ADD_RADIOBUTTON:
                results += L"Add RadioButton";
                break;
            default:
                results += L"Other";
                break;
        }
        
        results += L"\nConfidence: " + std::to_wstring(static_cast<int>(parsed.confidence * 100)) + L"%\n\n";
    }
    
    MessageBoxW(nullptr, results.c_str(), L"Widget Recognition Results", MB_OK | MB_ICONINFORMATION);
    
    // Demo 6: Complex form with layout
    MessageBoxW(nullptr,
        L"Demo 6: Complex Form (v2.0 Feature)\n\n"
        L"Prompt: \"Create form 700x600 'Data Entry' with label, textbox, "
        L"label, combobox, slider, and button in vertical layout\"",
        L"Advanced Neural Network Demo", MB_OK | MB_ICONINFORMATION);
    
    HWND window5 = builder.BuildFromPrompt(
        L"Create form 700x600 'Data Entry' with label, textbox, label, combobox, slider, and button in vertical layout",
        hInstance
    );
    
    if (window5) {
        auto sdkWindow5 = SDK::WindowManager::GetInstance().RegisterWindow(window5);
        if (sdkWindow5) {
            auto theme = std::make_shared<SDK::Theme>(SDK::Theme::CreateModernTheme());
            sdkWindow5->SetTheme(theme);
            sdkWindow5->SetDepth(SDK::WindowDepth::FOREGROUND);
            sdkWindow5->SetRoundedCorners(true, 12);
            sdkWindow5->UpdateAppearance();
        }
        
        ShowWindow(window5, SW_SHOW);
        UpdateWindow(window5);
    }
    
    MessageBoxW(nullptr,
        L"Advanced Neural Network Demo Complete!\n\n"
        L"Features Demonstrated:\n"
        L"✓ Multi-widget parsing from single prompt\n"
        L"✓ Vertical layout intelligence\n"
        L"✓ Horizontal layout intelligence\n"
        L"✓ Grid layout intelligence\n"
        L"✓ Polymorphic widget support (12+ types)\n"
        L"✓ Complex form generation\n\n"
        L"All windows are now open and interactive.",
        L"Demo Complete", MB_OK | MB_ICONINFORMATION);
    
    // Message loop
    MSG msg = {};
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    // Cleanup
    SDK::Shutdown();
    
    return static_cast<int>(msg.wParam);
}
