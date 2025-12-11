/**
 * Neural Network Demo
 * 
 * Demonstrates the neural network-powered GUI creation system
 * Shows how to use natural language prompts to create windows,
 * add widgets, and set up callbacks.
 */

#include "SDK/SDK.h"
#include <string>

// Window procedure for the demo windows
LRESULT CALLBACK NeuralDemoWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
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
    wc.lpfnWndProc = NeuralDemoWndProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.lpszClassName = L"NeuralDemoClass";
    
    if (!RegisterClassExW(&wc)) {
        MessageBoxW(nullptr, L"Failed to register window class", L"Error", MB_OK | MB_ICONERROR);
        SDK::Shutdown();
        return -1;
    }
    
    // Create a neural prompt builder
    SDK::NeuralPromptBuilder builder;
    
    // Example 1: Create a simple window with a button
    MessageBoxW(nullptr, 
        L"Demo 1: Creating a window with a button using natural language\n\n"
        L"Prompt: \"Create a window 800x600 called 'My Application' with a button\"",
        L"Neural Network Demo", MB_OK | MB_ICONINFORMATION);
    
    HWND window1 = builder.BuildFromPrompt(
        L"Create a window 800x600 called 'My Application' with a button",
        hInstance
    );
    
    if (window1) {
        // Register with SDK and apply theme
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
    
    // Wait a moment
    Sleep(2000);
    
    // Example 2: Create a window with multiple widgets
    MessageBoxW(nullptr,
        L"Demo 2: Creating a window with multiple widgets\n\n"
        L"Prompt: \"Make a dialog 600x400 with a label, textbox, and button\"",
        L"Neural Network Demo", MB_OK | MB_ICONINFORMATION);
    
    HWND window2 = builder.BuildFromPrompt(
        L"Make a dialog 600x400 with a label, textbox, and button",
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
    
    // Wait a moment
    Sleep(2000);
    
    // Example 3: Create a button with a callback
    MessageBoxW(nullptr,
        L"Demo 3: Understanding callback intent\n\n"
        L"Prompt: \"Add a button called 'Submit' that shows a message on click\"",
        L"Neural Network Demo", MB_OK | MB_ICONINFORMATION);
    
    // Parse callback prompt
    auto callbackSpec = builder.ParseCallbackPrompt(
        L"Add a button called 'Submit' that shows a message on click"
    );
    
    std::wstring callbackInfo = L"Neural Network Analysis:\n\n";
    callbackInfo += L"Widget Text: " + callbackSpec.widgetText + L"\n";
    callbackInfo += L"Callback Type: ";
    
    switch (callbackSpec.type) {
        case SDK::NeuralNetwork::CallbackType::ON_CLICK:
            callbackInfo += L"Click";
            break;
        case SDK::NeuralNetwork::CallbackType::ON_HOVER:
            callbackInfo += L"Hover";
            break;
        case SDK::NeuralNetwork::CallbackType::ON_FOCUS:
            callbackInfo += L"Focus";
            break;
        default:
            callbackInfo += L"Custom";
            break;
    }
    
    callbackInfo += L"\nAction: " + callbackSpec.action;
    
    MessageBoxW(nullptr, callbackInfo.c_str(), L"Callback Analysis", MB_OK | MB_ICONINFORMATION);
    
    // Example 4: Test the neural network directly
    MessageBoxW(nullptr,
        L"Demo 4: Direct neural network inference\n\n"
        L"Testing various prompts...",
        L"Neural Network Demo", MB_OK | MB_ICONINFORMATION);
    
    auto nn = builder.GetNeuralNetwork();
    
    std::vector<std::wstring> testPrompts = {
        L"Create a window with a progress bar",
        L"Add a checkbox for terms and conditions",
        L"Make a tooltip that says hello",
        L"Set up a click handler for the submit button"
    };
    
    std::wstring results = L"Neural Network Test Results:\n\n";
    
    for (const auto& prompt : testPrompts) {
        auto parsed = nn->ParsePrompt(prompt);
        
        results += L"Prompt: \"" + prompt + L"\"\n";
        results += L"Intent: ";
        
        switch (parsed.intent) {
            case SDK::NeuralNetwork::Intent::CREATE_WINDOW:
                results += L"Create Window";
                break;
            case SDK::NeuralNetwork::Intent::ADD_BUTTON:
                results += L"Add Button";
                break;
            case SDK::NeuralNetwork::Intent::ADD_LABEL:
                results += L"Add Label";
                break;
            case SDK::NeuralNetwork::Intent::ADD_TEXTBOX:
                results += L"Add TextBox";
                break;
            case SDK::NeuralNetwork::Intent::ADD_CHECKBOX:
                results += L"Add CheckBox";
                break;
            case SDK::NeuralNetwork::Intent::ADD_PROGRESSBAR:
                results += L"Add ProgressBar";
                break;
            case SDK::NeuralNetwork::Intent::ADD_TOOLTIP:
                results += L"Add Tooltip";
                break;
            case SDK::NeuralNetwork::Intent::SET_CALLBACK:
                results += L"Set Callback";
                break;
            case SDK::NeuralNetwork::Intent::SET_THEME:
                results += L"Set Theme";
                break;
            default:
                results += L"Unknown";
                break;
        }
        
        results += L"\nConfidence: " + std::to_wstring(static_cast<int>(parsed.confidence * 100)) + L"%\n\n";
    }
    
    MessageBoxW(nullptr, results.c_str(), L"Neural Network Results", MB_OK | MB_ICONINFORMATION);
    
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
