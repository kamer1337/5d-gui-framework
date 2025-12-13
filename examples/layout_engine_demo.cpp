#include "SDK/SDK.h"
#include "SDK/Window.h"
#include "SDK/Widget.h"
#include "SDK/Layout.h"
#include <memory>
#include <vector>

using namespace SDK;

// Simple button widget for demonstration
class DemoButton : public Widget {
public:
    DemoButton(const std::wstring& text) : m_text(text) {
        SetSize(100, 40);
    }
    
    void Render(HDC hdc) override {
        RECT bounds;
        GetBounds(bounds);
        
        // Draw button background
        HBRUSH brush = CreateSolidBrush(IsHovered() ? RGB(100, 150, 200) : RGB(80, 120, 180));
        FillRect(hdc, &bounds, brush);
        DeleteObject(brush);
        
        // Draw text
        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, RGB(255, 255, 255));
        DrawTextW(hdc, m_text.c_str(), -1, &bounds, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    }
    
private:
    std::wstring m_text;
};

// Demo window
std::shared_ptr<Window> g_demoWindow;
std::vector<std::shared_ptr<Widget>> g_widgets;
std::shared_ptr<LayoutEngine> g_layoutEngine;
int g_layoutMode = 0; // 0: Auto, 1: Grid, 2: Flow, 3: Stack, 4: Constraints

void CreateDemoWidgets() {
    g_widgets.clear();
    
    // Create sample buttons
    for (int i = 0; i < 8; i++) {
        auto button = std::make_shared<DemoButton>(L"Button " + std::to_wstring(i + 1));
        g_widgets.push_back(button);
    }
}

void ApplyLayout() {
    if (!g_demoWindow || g_widgets.empty()) return;
    
    RECT clientRect;
    GetClientRect(g_demoWindow->GetHandle(), &clientRect);
    
    g_layoutEngine->ClearConstraints();
    
    switch (g_layoutMode) {
        case 0: // Auto layout
            g_layoutEngine->SetAutoLayoutMode(true);
            g_layoutEngine->SetBaseLayout(nullptr);
            break;
            
        case 1: // Grid layout
            g_layoutEngine->SetAutoLayoutMode(false);
            g_layoutEngine->SetBaseLayout(std::make_shared<GridLayout>(3, 0));
            break;
            
        case 2: // Flow layout
            g_layoutEngine->SetAutoLayoutMode(false);
            g_layoutEngine->SetBaseLayout(std::make_shared<FlowLayout>(FlowLayout::Direction::LEFT_TO_RIGHT));
            break;
            
        case 3: // Stack layout
            g_layoutEngine->SetAutoLayoutMode(false);
            {
                auto stackLayout = std::make_shared<StackLayout>(StackLayout::Orientation::VERTICAL);
                stackLayout->SetDistribution(StackLayout::Distribution::SPACE_EVENLY);
                g_layoutEngine->SetBaseLayout(stackLayout);
            }
            break;
            
        case 4: // Constraint-based layout
            g_layoutEngine->SetAutoLayoutMode(false);
            g_layoutEngine->SetBaseLayout(nullptr);
            
            // Example: Create a constraint-based layout
            if (g_widgets.size() >= 4) {
                // Position first widget in top-left
                g_layoutEngine->AddConstraint(LayoutConstraint(
                    g_widgets[0], LayoutConstraint::Attribute::LEFT,
                    LayoutConstraint::Type::EQUAL, 20));
                g_layoutEngine->AddConstraint(LayoutConstraint(
                    g_widgets[0], LayoutConstraint::Attribute::TOP,
                    LayoutConstraint::Type::EQUAL, 20));
                
                // Position second widget to the right of first
                g_layoutEngine->AddConstraint(LayoutConstraint(
                    g_widgets[1], LayoutConstraint::Attribute::LEFT,
                    LayoutConstraint::Type::EQUAL,
                    g_widgets[0], LayoutConstraint::Attribute::RIGHT, 10));
                g_layoutEngine->AddConstraint(LayoutConstraint(
                    g_widgets[1], LayoutConstraint::Attribute::TOP,
                    LayoutConstraint::Type::EQUAL,
                    g_widgets[0], LayoutConstraint::Attribute::TOP, 0));
                
                // Position third widget below first
                g_layoutEngine->AddConstraint(LayoutConstraint(
                    g_widgets[2], LayoutConstraint::Attribute::LEFT,
                    LayoutConstraint::Type::EQUAL,
                    g_widgets[0], LayoutConstraint::Attribute::LEFT, 0));
                g_layoutEngine->AddConstraint(LayoutConstraint(
                    g_widgets[2], LayoutConstraint::Attribute::TOP,
                    LayoutConstraint::Type::EQUAL,
                    g_widgets[0], LayoutConstraint::Attribute::BOTTOM, 10));
                
                // Position fourth widget to the right of third
                g_layoutEngine->AddConstraint(LayoutConstraint(
                    g_widgets[3], LayoutConstraint::Attribute::LEFT,
                    LayoutConstraint::Type::EQUAL,
                    g_widgets[2], LayoutConstraint::Attribute::RIGHT, 10));
                g_layoutEngine->AddConstraint(LayoutConstraint(
                    g_widgets[3], LayoutConstraint::Attribute::TOP,
                    LayoutConstraint::Type::EQUAL,
                    g_widgets[2], LayoutConstraint::Attribute::TOP, 0));
                
                // Center remaining widgets vertically
                for (size_t i = 4; i < g_widgets.size(); i++) {
                    g_layoutEngine->AddConstraint(LayoutConstraint(
                        g_widgets[i], LayoutConstraint::Attribute::CENTER_X,
                        LayoutConstraint::Type::EQUAL,
                        clientRect.right / 2));
                    g_layoutEngine->AddConstraint(LayoutConstraint(
                        g_widgets[i], LayoutConstraint::Attribute::TOP,
                        LayoutConstraint::Type::EQUAL,
                        static_cast<int>(200 + (i - 4) * 50)));
                }
            }
            break;
    }
    
    g_layoutEngine->Apply(clientRect, g_widgets);
}

void RenderDemo(HDC hdc) {
    // Clear background
    RECT clientRect;
    GetClientRect(g_demoWindow->GetHandle(), &clientRect);
    HBRUSH bgBrush = CreateSolidBrush(RGB(240, 240, 240));
    FillRect(hdc, &clientRect, bgBrush);
    DeleteObject(bgBrush);
    
    // Draw title
    RECT titleRect = { 10, 10, clientRect.right - 10, 40 };
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(0, 0, 0));
    HFONT hFont = CreateFontW(24, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                              DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                              CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");
    HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);
    
    const wchar_t* layoutNames[] = {
        L"Automatic Layout Engine",
        L"Grid Layout (3 columns)",
        L"Flow Layout (Left to Right)",
        L"Stack Layout (Vertical, Space Evenly)",
        L"Constraint-Based Layout"
    };
    DrawTextW(hdc, layoutNames[g_layoutMode], -1, &titleRect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    
    SelectObject(hdc, hOldFont);
    DeleteObject(hFont);
    
    // Draw instructions
    RECT instrRect = { 10, 40, clientRect.right - 10, 70 };
    hFont = CreateFontW(14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                        CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");
    hOldFont = (HFONT)SelectObject(hdc, hFont);
    DrawTextW(hdc, L"Press SPACE to cycle through layout modes", -1, &instrRect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    SelectObject(hdc, hOldFont);
    DeleteObject(hFont);
    
    // Render widgets
    for (auto& widget : g_widgets) {
        if (widget->IsVisible()) {
            widget->Render(hdc);
        }
    }
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            RenderDemo(hdc);
            EndPaint(hwnd, &ps);
            return 0;
        }
        
        case WM_KEYDOWN:
            if (wParam == VK_SPACE) {
                g_layoutMode = (g_layoutMode + 1) % 5;
                ApplyLayout();
                InvalidateRect(hwnd, NULL, TRUE);
            }
            return 0;
        
        case WM_SIZE:
            ApplyLayout();
            InvalidateRect(hwnd, NULL, TRUE);
            return 0;
        
        case WM_MOUSEMOVE: {
            int x = LOWORD(lParam);
            int y = HIWORD(lParam);
            for (auto& widget : g_widgets) {
                if (widget->HitTest(x, y)) {
                    widget->SetHovered(true);
                } else {
                    widget->SetHovered(false);
                }
            }
            InvalidateRect(hwnd, NULL, TRUE);
            return 0;
        }
        
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // Register window class
    WNDCLASSEXW wc = { 0 };
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = L"LayoutEngineDemo";
    RegisterClassExW(&wc);
    
    // Create window
    HWND hwnd = CreateWindowExW(
        0, L"LayoutEngineDemo", L"Advanced Layout Engine Demo",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
        NULL, NULL, hInstance, NULL);
    
    if (!hwnd) return 0;
    
    // Initialize demo
    g_demoWindow = std::make_shared<Window>(hwnd);
    
    g_layoutEngine = std::make_shared<LayoutEngine>();
    CreateDemoWidgets();
    ApplyLayout();
    
    // Message loop
    MSG msg = { 0 };
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    return static_cast<int>(msg.wParam);
}
