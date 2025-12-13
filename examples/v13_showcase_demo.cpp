/**
 * V1.3 Feature Showcase Demo
 * Demonstrates new visual effects, animation enhancements, and UI components
 */

#include "../include/SDK/SDK.h"
#include "../include/SDK/RenderBackend.h"
#include "../include/SDK/Layout.h"
#include "../include/SDK/Menu.h"
#include "../include/SDK/RichText.h"
#include <windowsx.h>

// Global variables
SDK::RenderBackend* g_renderBackend = nullptr;
std::shared_ptr<SDK::WindowAnimation> g_animation;
std::shared_ptr<SDK::AnimationGroup> g_animGroup;
std::vector<std::shared_ptr<SDK::Widget>> g_widgets;
std::shared_ptr<SDK::Layout> g_layout;
std::shared_ptr<SDK::MenuBar> g_menuBar;
std::shared_ptr<SDK::RichTextBox> g_richText;
SDK::RenderBackend::EffectPreset g_currentPreset = SDK::RenderBackend::EffectPreset::NONE;

// Window procedure
LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE: {
            // Initialize render backend
            g_renderBackend = SDK::RenderBackend::Create(SDK::RenderBackend::BackendType::AUTO).release();
            if (g_renderBackend) {
                g_renderBackend->Initialize(hwnd);
            }
            
            // Create animation
            g_animation = std::make_shared<SDK::WindowAnimation>(hwnd);
            g_animation->SetAnimationDuration(500);
            g_animation->SetEasingType(SDK::WindowAnimation::EasingType::EASE_IN_OUT);
            
            // Create animation group
            g_animGroup = std::make_shared<SDK::AnimationGroup>(SDK::AnimationGroup::PlayMode::PARALLEL);
            
            // Create layout
            g_layout = std::make_shared<SDK::GridLayout>(3, 2);
            g_layout->SetSpacing(10);
            g_layout->SetPadding(20, 60, 20, 20);
            
            // Create menu bar
            g_menuBar = std::make_shared<SDK::MenuBar>();
            g_menuBar->SetPosition(0, 0);
            g_menuBar->SetSize(800, 30);
            
            // Add File menu
            auto fileMenu = std::make_shared<SDK::Menu>();
            fileMenu->AddItem(std::make_shared<SDK::MenuItem>(L"New"));
            fileMenu->AddItem(std::make_shared<SDK::MenuItem>(L"Open"));
            fileMenu->AddItem(std::make_shared<SDK::MenuItem>(L"Save"));
            fileMenu->AddItem(std::make_shared<SDK::MenuItem>(L"-", SDK::MenuItem::Type::SEPARATOR));
            fileMenu->AddItem(std::make_shared<SDK::MenuItem>(L"Exit"));
            g_menuBar->AddMenu(L"File", fileMenu);
            
            // Add Effects menu
            auto effectsMenu = std::make_shared<SDK::Menu>();
            auto noneItem = std::make_shared<SDK::MenuItem>(L"None");
            noneItem->SetOnClick([]() { g_currentPreset = SDK::RenderBackend::EffectPreset::NONE; });
            effectsMenu->AddItem(noneItem);
            
            auto cinematicItem = std::make_shared<SDK::MenuItem>(L"Cinematic");
            cinematicItem->SetOnClick([]() { g_currentPreset = SDK::RenderBackend::EffectPreset::CINEMATIC; });
            effectsMenu->AddItem(cinematicItem);
            
            auto gameUIItem = std::make_shared<SDK::MenuItem>(L"Game UI");
            gameUIItem->SetOnClick([]() { g_currentPreset = SDK::RenderBackend::EffectPreset::GAME_UI; });
            effectsMenu->AddItem(gameUIItem);
            
            auto retroItem = std::make_shared<SDK::MenuItem>(L"Retro");
            retroItem->SetOnClick([]() { g_currentPreset = SDK::RenderBackend::EffectPreset::RETRO; });
            effectsMenu->AddItem(retroItem);
            
            auto dreamyItem = std::make_shared<SDK::MenuItem>(L"Dreamy");
            dreamyItem->SetOnClick([]() { g_currentPreset = SDK::RenderBackend::EffectPreset::DREAMY; });
            effectsMenu->AddItem(dreamyItem);
            
            auto motionItem = std::make_shared<SDK::MenuItem>(L"Motion");
            motionItem->SetOnClick([]() { g_currentPreset = SDK::RenderBackend::EffectPreset::MOTION; });
            effectsMenu->AddItem(motionItem);
            
            g_menuBar->AddMenu(L"Effects", effectsMenu);
            
            // Add Animation menu
            auto animMenu = std::make_shared<SDK::Menu>();
            auto playItem = std::make_shared<SDK::MenuItem>(L"Play");
            playItem->SetOnClick([]() { 
                if (g_animation) g_animation->AnimateMaximize(); 
            });
            animMenu->AddItem(playItem);
            
            auto pauseItem = std::make_shared<SDK::MenuItem>(L"Pause");
            pauseItem->SetOnClick([]() { 
                if (g_animation) g_animation->PauseAnimation(); 
            });
            animMenu->AddItem(pauseItem);
            
            auto resumeItem = std::make_shared<SDK::MenuItem>(L"Resume");
            resumeItem->SetOnClick([]() { 
                if (g_animation) g_animation->ResumeAnimation(); 
            });
            animMenu->AddItem(resumeItem);
            
            auto reverseItem = std::make_shared<SDK::MenuItem>(L"Reverse");
            reverseItem->SetOnClick([]() { 
                if (g_animation) g_animation->ReverseAnimation(); 
            });
            animMenu->AddItem(reverseItem);
            
            g_menuBar->AddMenu(L"Animation", animMenu);
            
            // Create widgets for layout demo
            for (int i = 0; i < 6; i++) {
                auto button = std::make_shared<SDK::Button>(L"Button " + std::to_wstring(i + 1));
                button->SetSize(150, 40);
                g_widgets.push_back(button);
            }
            
            // Apply layout
            RECT layoutBounds = { 0, 30, 800, 400 };
            g_layout->Apply(layoutBounds, g_widgets);
            
            // Create rich text widget
            g_richText = std::make_shared<SDK::RichTextBox>();
            g_richText->SetPosition(20, 420);
            g_richText->SetSize(760, 150);
            
            auto doc = std::make_shared<SDK::RichTextDocument>();
            doc->AddHeading(L"5D GUI Framework v1.3", 1);
            doc->AddParagraph(L"Welcome to version 1.3!");
            doc->AddText(L"New features include:", true, false);
            doc->AddParagraph(L"• Advanced visual effects (depth-of-field, motion blur, chromatic aberration)");
            doc->AddParagraph(L"• Effect presets for quick styling");
            doc->AddParagraph(L"• Enhanced animations with Bezier curves");
            doc->AddParagraph(L"• Animation grouping and control");
            doc->AddParagraph(L"• Layout system (Grid, Flow, Stack)");
            doc->AddParagraph(L"• Menu system with context menus");
            doc->AddParagraph(L"• Rich text rendering");
            
            g_richText->SetDocument(doc);
            g_richText->SetReadOnly(true);
            
            return 0;
        }
        
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            
            if (g_renderBackend) {
                g_renderBackend->BeginDraw();
                
                // Clear background
                g_renderBackend->Clear(SDK::Color(240, 240, 240, 255));
                
                // Render menu bar
                if (g_menuBar) {
                    g_menuBar->Render(hdc);
                }
                
                // Render widgets
                for (auto& widget : g_widgets) {
                    widget->Render(hdc);
                }
                
                // Render rich text
                if (g_richText) {
                    g_richText->Render(hdc);
                }
                
                // Apply effect preset if selected
                if (g_currentPreset != SDK::RenderBackend::EffectPreset::NONE) {
                    RECT effectRect = { 0, 30, 800, 400 };
                    g_renderBackend->ApplyEffectPreset(effectRect, g_currentPreset);
                }
                
                g_renderBackend->EndDraw();
            }
            
            EndPaint(hwnd, &ps);
            return 0;
        }
        
        case WM_LBUTTONDOWN: {
            int x = GET_X_LPARAM(lParam);
            int y = GET_Y_LPARAM(lParam);
            
            // Check menu bar
            if (g_menuBar) {
                g_menuBar->HandleEvent(SDK::WidgetEvent::CLICK, &lParam);
            }
            
            // Check widgets
            for (auto& widget : g_widgets) {
                RECT bounds = widget->GetBounds();
                if (x >= bounds.left && x <= bounds.right && y >= bounds.top && y <= bounds.bottom) {
                    widget->HandleEvent(SDK::WidgetEvent::CLICK, nullptr);
                }
            }
            
            return 0;
        }
        
        case WM_MOUSEMOVE: {
            // Update animation
            if (g_animation) {
                g_animation->Update();
            }
            if (g_animGroup) {
                g_animGroup->Update();
            }
            
            InvalidateRect(hwnd, nullptr, FALSE);
            return 0;
        }
        
        case WM_DESTROY:
            if (g_renderBackend) {
                g_renderBackend->Shutdown();
                delete g_renderBackend;
                g_renderBackend = nullptr;
            }
            PostQuitMessage(0);
            return 0;
    }
    
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    // Register window class
    WNDCLASSEXW wc = { sizeof(WNDCLASSEXW) };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = L"V13ShowcaseWindow";
    RegisterClassExW(&wc);
    
    // Create window
    HWND hwnd = CreateWindowExW(
        WS_EX_LAYERED,
        L"V13ShowcaseWindow",
        L"5D GUI Framework v1.3 - Feature Showcase",
        WS_OVERLAPPEDWINDOW,
        100, 100, 800, 600,
        nullptr, nullptr, hInstance, nullptr
    );
    
    if (!hwnd) {
        return 1;
    }
    
    // Set layered window attributes
    SetLayeredWindowAttributes(hwnd, 0, 255, LWA_ALPHA);
    
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);
    
    // Message loop
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    return (int)msg.wParam;
}
