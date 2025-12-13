/**
 * 5D GUI SDK Demo Application for Linux
 * Demonstrates the X11 window creation and rendering system
 */

#include "../include/SDK/Platform.h"

#if SDK_PLATFORM_LINUX && SDK_HAS_X11

#include "../include/SDK/WindowX11.h"
#include "../include/SDK/X11RenderBackend.h"
#include "../include/SDK/Theme.h"
#include <memory>
#include <cmath>

// Global state
std::shared_ptr<SDK::WindowX11> g_mainWindow;
float g_animationTime = 0.0f;

// Paint callback
void OnPaint() {
    if (!g_mainWindow) {
        return;
    }
    
    auto renderBackend = g_mainWindow->GetRenderBackend();
    if (!renderBackend) {
        return;
    }
    
    g_mainWindow->BeginPaint();
    
    // Clear background
    renderBackend->Clear(SDK::Color(240, 240, 245, 255));
    
    // Draw title
    RECT titleRect = {20, 20, 780, 60};
    renderBackend->DrawText(L"5D GUI SDK - Linux X11 Demo", titleRect,
        SDK::Color(50, 50, 50, 255), L"", 24, 700);
    
    // Draw description
    RECT descRect = {20, 60, 780, 90};
    renderBackend->DrawText(L"X11 Backend with Basic Rendering", descRect,
        SDK::Color(100, 100, 100, 255), L"", 14, 400);
    
    // Draw some colored rectangles
    RECT rect1 = {50, 120, 200, 220};
    renderBackend->DrawRectangle(rect1,
        SDK::Color(100, 149, 237, 255),  // Cornflower blue
        SDK::Color(65, 105, 225, 255),   // Royal blue border
        2.0f);
    
    RECT labelRect1 = {50, 230, 200, 250};
    renderBackend->DrawText(L"Rectangle", labelRect1,
        SDK::Color(50, 50, 50, 255), L"", 12, 400);
    
    // Draw rounded rectangle
    RECT rect2 = {230, 120, 380, 220};
    renderBackend->DrawRoundedRectangle(rect2, 12.0f,
        SDK::Color(138, 43, 226, 255),   // Blue violet
        SDK::Color(75, 0, 130, 255),     // Indigo border
        2.0f);
    
    RECT labelRect2 = {230, 230, 380, 250};
    renderBackend->DrawText(L"Rounded Rect", labelRect2,
        SDK::Color(50, 50, 50, 255), L"", 12, 400);
    
    // Draw ellipse
    renderBackend->DrawEllipse(485, 170, 75, 50,
        SDK::Color(255, 140, 0, 255),    // Dark orange
        SDK::Color(255, 69, 0, 255),     // Orange red border
        2.0f);
    
    RECT labelRect3 = {410, 230, 560, 250};
    renderBackend->DrawText(L"Ellipse", labelRect3,
        SDK::Color(50, 50, 50, 255), L"", 12, 400);
    
    // Draw line
    renderBackend->DrawLine(620, 120, 750, 220,
        SDK::Color(34, 139, 34, 255), 3.0f);  // Forest green
    
    RECT labelRect4 = {620, 230, 750, 250};
    renderBackend->DrawText(L"Line", labelRect4,
        SDK::Color(50, 50, 50, 255), L"", 12, 400);
    
    // Draw gradient
    RECT gradRect = {50, 280, 750, 340};
    renderBackend->DrawLinearGradient(gradRect,
        SDK::Color(255, 105, 180, 255),  // Hot pink
        SDK::Color(138, 43, 226, 255),   // Blue violet
        true);
    
    RECT labelRect5 = {50, 350, 750, 370};
    renderBackend->DrawText(L"Linear Gradient", labelRect5,
        SDK::Color(50, 50, 50, 255), L"", 12, 400);
    
    // Draw info text
    RECT infoRect = {50, 400, 750, 550};
    renderBackend->DrawText(
        L"Features:\n"
        L"- X11 window creation\n"
        L"- Basic rendering (rectangles, lines, text)\n"
        L"- Rounded corners\n"
        L"- Gradients\n"
        L"- Event handling",
        infoRect,
        SDK::Color(100, 100, 100, 255), L"", 12, 400);
    
    g_mainWindow->EndPaint();
    
    // Update animation time
    g_animationTime += 0.016f;
}

// Close callback
void OnClose() {
    SDK::X11WindowManager::GetInstance().Quit();
}

// Mouse move callback
void OnMouseMove(int x, int y) {
    // Could add hover effects here
}

// Mouse button callback
void OnMouseButton(int x, int y, int button, bool pressed) {
    if (pressed && button == 1) { // Left button
        // Trigger repaint on click
        if (g_mainWindow) {
            g_mainWindow->Invalidate();
        }
    }
}

int main() {
    // Create main window
    g_mainWindow = SDK::X11WindowManager::GetInstance().CreateWindow(
        L"5D GUI SDK - Linux Demo",
        100, 100,
        800, 600
    );
    
    if (!g_mainWindow) {
        return 1;
    }
    
    // Set up callbacks
    g_mainWindow->SetPaintCallback(OnPaint);
    g_mainWindow->SetCloseCallback(OnClose);
    g_mainWindow->SetMouseMoveCallback(OnMouseMove);
    g_mainWindow->SetMouseButtonCallback(OnMouseButton);
    
    // Show window
    g_mainWindow->Show();
    
    // Trigger initial paint
    g_mainWindow->Invalidate();
    
    // Run event loop
    SDK::X11WindowManager::GetInstance().RunEventLoop();
    
    return 0;
}

#else

#include <iostream>

int main() {
    std::cout << "This demo requires Linux with X11 support." << std::endl;
    return 1;
}

#endif // SDK_PLATFORM_LINUX && SDK_HAS_X11
