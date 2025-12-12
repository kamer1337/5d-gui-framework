/**
 * Widget Enhancements Demo
 * Demonstrates new widget features:
 * - Auto-alignment and overlap detection
 * - Panel folding/unfolding
 * - Panel boundary constraints
 * - FileTree orientation (vertical/horizontal)
 */

#ifdef _WIN32
#include "../include/SDK/SDK.h"
#include "../include/SDK/PromptWindowBuilder.h"
#include <iostream>

// Global window handle
HWND g_hwnd = nullptr;
std::shared_ptr<SDK::Window> g_window = nullptr;

// Demo widgets
std::shared_ptr<SDK::Panel> g_panel1 = nullptr;
std::shared_ptr<SDK::Panel> g_panel2 = nullptr;
std::shared_ptr<SDK::Panel> g_panel3 = nullptr;
std::shared_ptr<SDK::Button> g_btn1 = nullptr;
std::shared_ptr<SDK::Button> g_btn2 = nullptr;
std::shared_ptr<SDK::Button> g_btn3 = nullptr;
std::shared_ptr<SDK::FileTree> g_fileTree = nullptr;

// Window procedure
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            
            // Clear background
            HBRUSH bgBrush = CreateSolidBrush(RGB(240, 240, 240));
            FillRect(hdc, &ps.rcPaint, bgBrush);
            DeleteObject(bgBrush);
            
            // Render window widgets if available
            if (g_window) {
                auto widgets = g_window->GetWidgets();
                for (auto& widget : widgets) {
                    widget->Render(hdc);
                }
            }
            
            EndPaint(hwnd, &ps);
            return 0;
        }
        case WM_LBUTTONDOWN: {
            int x = LOWORD(lParam);
            int y = HIWORD(lParam);
            
            if (g_window) {
                auto widgets = g_window->GetWidgets();
                for (auto& widget : widgets) {
                    if (widget->HandleMouseDown(x, y, 0)) {
                        InvalidateRect(hwnd, NULL, TRUE);
                        break;
                    }
                }
            }
            return 0;
        }
    }
    
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
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
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = L"WidgetEnhancementsDemo";
    
    RegisterClassExW(&wc);
    
    // Create window
    g_hwnd = CreateWindowExW(
        0,
        L"WidgetEnhancementsDemo",
        L"Widget Enhancements Demo - Alignment, Panel Folding, FileTree",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 1200, 700,
        NULL, NULL, hInstance, NULL
    );
    
    if (!g_hwnd) {
        return -1;
    }
    
    // Register with SDK
    g_window = SDK::WindowManager::GetInstance().RegisterWindow(g_hwnd);
    auto theme = std::make_shared<SDK::Theme>(SDK::Theme::CreateModernTheme());
    g_window->SetTheme(theme);
    
    // ===== Demo 1: Widget Auto-Alignment =====
    auto label1 = std::make_shared<SDK::Label>(L"Widget Auto-Alignment Demo:");
    label1->SetPosition(20, 20);
    label1->SetSize(300, 25);
    label1->SetFontBold(true);
    g_window->AddWidget(label1);
    
    // Create buttons with different alignments
    g_btn1 = std::make_shared<SDK::Button>(L"Button 1");
    g_btn1->SetPosition(20, 50);
    g_btn1->SetSize(120, 35);
    g_window->AddWidget(g_btn1);
    
    g_btn2 = std::make_shared<SDK::Button>(L"Align Right");
    g_btn2->SetSize(120, 35);
    g_btn2->AlignToWidget(g_btn1.get(), SDK::WidgetAlignment::RIGHT, 10);
    g_window->AddWidget(g_btn2);
    
    g_btn3 = std::make_shared<SDK::Button>(L"Align Bottom");
    g_btn3->SetSize(120, 35);
    g_btn3->AlignToWidget(g_btn1.get(), SDK::WidgetAlignment::BOTTOM, 10);
    g_window->AddWidget(g_btn3);
    
    // Overlap detection demo button
    auto btnOverlap = std::make_shared<SDK::Button>(L"Check Overlap");
    btnOverlap->SetPosition(300, 50);
    btnOverlap->SetSize(140, 35);
    btnOverlap->SetEventCallback([](SDK::Widget* w, SDK::WidgetEvent e, void* data) {
        if (e == SDK::WidgetEvent::CLICK) {
            if (g_btn1 && g_btn2) {
                bool overlaps = g_btn1->CheckOverlap(g_btn2.get());
                std::wstring msg = overlaps ? L"Buttons overlap!" : L"No overlap detected";
                MessageBoxW(g_hwnd, msg.c_str(), L"Overlap Detection", MB_OK);
            }
        }
    });
    g_window->AddWidget(btnOverlap);
    
    // ===== Demo 2: Panel Folding/Unfolding =====
    auto label2 = std::make_shared<SDK::Label>(L"Panel Folding Demo:");
    label2->SetPosition(20, 120);
    label2->SetSize(300, 25);
    label2->SetFontBold(true);
    g_window->AddWidget(label2);
    
    // Vertical collapsible panel
    g_panel1 = std::make_shared<SDK::Panel>();
    g_panel1->SetPosition(20, 150);
    g_panel1->SetSize(300, 200);
    g_panel1->SetTitle(L"Vertical Collapsible Panel");
    g_panel1->SetCollapsible(true);
    g_panel1->SetCollapseOrientation(SDK::Panel::CollapseOrientation::VERTICAL);
    g_window->AddWidget(g_panel1);
    
    // Add some content to the panel
    auto panelLabel1 = std::make_shared<SDK::Label>(L"Panel Content:");
    panelLabel1->SetPosition(30, 185);
    panelLabel1->SetSize(280, 20);
    g_panel1->AddChild(panelLabel1);
    
    auto panelBtn1 = std::make_shared<SDK::Button>(L"Panel Button 1");
    panelBtn1->SetPosition(30, 210);
    panelBtn1->SetSize(120, 30);
    g_panel1->AddChild(panelBtn1);
    
    auto panelBtn2 = std::make_shared<SDK::Button>(L"Panel Button 2");
    panelBtn2->SetPosition(160, 210);
    panelBtn2->SetSize(120, 30);
    g_panel1->AddChild(panelBtn2);
    
    // Horizontal collapsible panel
    g_panel2 = std::make_shared<SDK::Panel>();
    g_panel2->SetPosition(340, 150);
    g_panel2->SetSize(300, 200);
    g_panel2->SetTitle(L"Horizontal Collapsible Panel");
    g_panel2->SetCollapsible(true);
    g_panel2->SetCollapseOrientation(SDK::Panel::CollapseOrientation::HORIZONTAL);
    g_window->AddWidget(g_panel2);
    
    auto panelLabel2 = std::make_shared<SDK::Label>(L"Horizontal collapse");
    panelLabel2->SetPosition(350, 185);
    panelLabel2->SetSize(280, 20);
    g_panel2->AddChild(panelLabel2);
    
    // ===== Demo 3: Panel Boundary Constraints =====
    auto label3 = std::make_shared<SDK::Label>(L"Panel Boundary Constraints Demo:");
    label3->SetPosition(20, 370);
    label3->SetSize(350, 25);
    label3->SetFontBold(true);
    g_window->AddWidget(label3);
    
    g_panel3 = std::make_shared<SDK::Panel>();
    g_panel3->SetPosition(20, 400);
    g_panel3->SetSize(400, 250);
    g_panel3->SetTitle(L"Constrained Panel");
    g_panel3->SetConstrainChildren(true);
    g_panel3->SetPadding(10);
    g_window->AddWidget(g_panel3);
    
    auto constrainedLabel = std::make_shared<SDK::Label>(L"Widgets are constrained within panel bounds");
    constrainedLabel->SetPosition(30, 435);
    constrainedLabel->SetSize(360, 20);
    g_panel3->AddChild(constrainedLabel);
    
    auto movableBtn = std::make_shared<SDK::Button>(L"Try to move me outside!");
    movableBtn->SetPosition(50, 470);
    movableBtn->SetSize(180, 35);
    movableBtn->SetEventCallback([](SDK::Widget* w, SDK::WidgetEvent e, void* data) {
        if (e == SDK::WidgetEvent::CLICK) {
            // Try to move button outside panel (will be clamped)
            w->SetPosition(500, 500);
            if (g_panel3) {
                g_panel3->ClampChildPosition(w);
            }
            InvalidateRect(g_hwnd, NULL, TRUE);
        }
    });
    g_panel3->AddChild(movableBtn);
    
    // ===== Demo 4: FileTree Orientation =====
    auto label4 = std::make_shared<SDK::Label>(L"FileTree Orientation Demo:");
    label4->SetPosition(660, 20);
    label4->SetSize(300, 25);
    label4->SetFontBold(true);
    g_window->AddWidget(label4);
    
    // Vertical FileTree
    g_fileTree = std::make_shared<SDK::FileTree>();
    g_fileTree->SetPosition(660, 50);
    g_fileTree->SetSize(500, 300);
    g_fileTree->SetOrientation(SDK::FileTree::Orientation::VERTICAL);
    
    // Set to current directory or a default path
    wchar_t currentPath[MAX_PATH];
    GetCurrentDirectoryW(MAX_PATH, currentPath);
    g_fileTree->SetRootPath(currentPath);
    g_window->AddWidget(g_fileTree);
    
    // FileTree control buttons
    auto btnExpand = std::make_shared<SDK::Button>(L"Expand All");
    btnExpand->SetPosition(660, 360);
    btnExpand->SetSize(100, 30);
    btnExpand->SetEventCallback([](SDK::Widget* w, SDK::WidgetEvent e, void* data) {
        if (e == SDK::WidgetEvent::CLICK && g_fileTree) {
            g_fileTree->ExpandAll();
            InvalidateRect(g_hwnd, NULL, TRUE);
        }
    });
    g_window->AddWidget(btnExpand);
    
    auto btnCollapse = std::make_shared<SDK::Button>(L"Collapse All");
    btnCollapse->SetPosition(770, 360);
    btnCollapse->SetSize(100, 30);
    btnCollapse->SetEventCallback([](SDK::Widget* w, SDK::WidgetEvent e, void* data) {
        if (e == SDK::WidgetEvent::CLICK && g_fileTree) {
            g_fileTree->CollapseAll();
            InvalidateRect(g_hwnd, NULL, TRUE);
        }
    });
    g_window->AddWidget(btnCollapse);
    
    auto btnToggleOrientation = std::make_shared<SDK::Button>(L"Toggle Orientation");
    btnToggleOrientation->SetPosition(880, 360);
    btnToggleOrientation->SetSize(150, 30);
    btnToggleOrientation->SetEventCallback([](SDK::Widget* w, SDK::WidgetEvent e, void* data) {
        if (e == SDK::WidgetEvent::CLICK && g_fileTree) {
            if (g_fileTree->GetOrientation() == SDK::FileTree::Orientation::VERTICAL) {
                g_fileTree->SetOrientation(SDK::FileTree::Orientation::HORIZONTAL);
                g_fileTree->SetSize(500, 250);
            } else {
                g_fileTree->SetOrientation(SDK::FileTree::Orientation::VERTICAL);
                g_fileTree->SetSize(500, 300);
            }
            InvalidateRect(g_hwnd, NULL, TRUE);
        }
    });
    g_window->AddWidget(btnToggleOrientation);
    
    // Instructions
    auto instructions = std::make_shared<SDK::Label>(
        L"Instructions:\n"
        L"• Click triangle buttons to collapse/expand panels\n"
        L"• Click 'Check Overlap' to test overlap detection\n"
        L"• Click 'Try to move me outside!' to test boundary constraints\n"
        L"• Use FileTree buttons to control tree expansion and orientation"
    );
    instructions->SetPosition(660, 410);
    instructions->SetSize(500, 100);
    instructions->SetAlignment(DT_LEFT | DT_TOP | DT_WORDBREAK);
    g_window->AddWidget(instructions);
    
    // Show window
    ShowWindow(g_hwnd, nCmdShow);
    UpdateWindow(g_hwnd);
    
    // Message loop
    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    // Cleanup
    SDK::Shutdown();
    
    return 0;
}

#else
// Linux/non-Windows stub
#include <iostream>

int main() {
    std::cout << "Widget Enhancements Demo is currently Windows-only." << std::endl;
    std::cout << "The SDK features are implemented but GUI demo requires Win32 API." << std::endl;
    return 0;
}
#endif
