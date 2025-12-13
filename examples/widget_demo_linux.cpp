/**
 * 5D GUI SDK Widget Demo Application for Linux
 * Demonstrates widget rendering with X11 backend
 */

#include "../include/SDK/Platform.h"

#if SDK_PLATFORM_LINUX && SDK_HAS_X11

#include "../include/SDK/WindowX11.h"
#include "../include/SDK/X11RenderBackend.h"
#include "../include/SDK/Theme.h"
#include <memory>
#include <cmath>
#include <cstdio>

// Global state
std::shared_ptr<SDK::WindowX11> g_mainWindow;
float g_progressValue = 0.0f;
bool g_animating = false;

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
    renderBackend->DrawText(L"5D GUI SDK - Widget Demo (Linux)", titleRect,
        SDK::Color(50, 50, 50, 255), L"", 24, 700);
    
    // Draw description
    RECT descRect = {20, 60, 780, 90};
    renderBackend->DrawText(L"Interactive Widget System with X11 Backend", descRect,
        SDK::Color(100, 100, 100, 255), L"", 14, 400);
    
    // Draw progress bar
    RECT progressRect = {50, 120, 550, 160};
    renderBackend->DrawRoundedRectangle(progressRect, 8.0f,
        SDK::Color(220, 220, 220, 255),
        SDK::Color(180, 180, 180, 255),
        1.0f);
    
    // Draw progress fill
    int fillWidth = static_cast<int>((progressRect.right - progressRect.left) * g_progressValue);
    RECT fillRect = {progressRect.left + 2, progressRect.top + 2, progressRect.left + fillWidth - 2, progressRect.bottom - 2};
    if (fillWidth > 4) {
        renderBackend->DrawLinearGradient(fillRect,
            SDK::Color(100, 149, 237, 255),
            SDK::Color(65, 105, 225, 255),
            true);
    }
    
    // Draw progress text
    RECT progressTextRect = {50, 170, 550, 190};
    char progressText[32];
    snprintf(progressText, sizeof(progressText), "Progress: %.0f%%", g_progressValue * 100);
    std::string progressStr(progressText);
    std::wstring progressWStr(progressStr.begin(), progressStr.end());
    renderBackend->DrawText(progressWStr, progressTextRect,
        SDK::Color(50, 50, 50, 255), L"", 12, 400);
    
    // Draw buttons
    RECT button1 = {50, 220, 230, 260};
    renderBackend->DrawRoundedRectangle(button1, 6.0f,
        SDK::Color(100, 149, 237, 255),
        SDK::Color(65, 105, 225, 255),
        2.0f);
    
    RECT button1Text = {50, 230, 230, 250};
    renderBackend->DrawText(L"Animate Progress", button1Text,
        SDK::Color(255, 255, 255, 255), L"", 14, 700);
    
    RECT button2 = {250, 220, 430, 260};
    renderBackend->DrawRoundedRectangle(button2, 6.0f,
        SDK::Color(138, 43, 226, 255),
        SDK::Color(75, 0, 130, 255),
        2.0f);
    
    RECT button2Text = {250, 230, 430, 250};
    renderBackend->DrawText(L"Reset Progress", button2Text,
        SDK::Color(255, 255, 255, 255), L"", 14, 700);
    
    // Draw checkboxes
    RECT checkbox1 = {50, 300, 70, 320};
    if (g_animating) {
        renderBackend->DrawRoundedRectangle(checkbox1, 3.0f,
            SDK::Color(100, 149, 237, 255),
            SDK::Color(65, 105, 225, 255),
            2.0f);
        // Draw checkmark
        renderBackend->DrawLine(52, 310, 58, 316, SDK::Color(255, 255, 255, 255), 2.0f);
        renderBackend->DrawLine(58, 316, 68, 304, SDK::Color(255, 255, 255, 255), 2.0f);
    } else {
        renderBackend->DrawRoundedRectangle(checkbox1, 3.0f,
            SDK::Color(255, 255, 255, 255),
            SDK::Color(180, 180, 180, 255),
            2.0f);
    }
    
    RECT checkbox1Text = {80, 300, 250, 320};
    renderBackend->DrawText(L"Auto-animate", checkbox1Text,
        SDK::Color(50, 50, 50, 255), L"", 12, 400);
    
    // Draw sliders
    RECT sliderTrack = {50, 360, 550, 370};
    renderBackend->DrawRoundedRectangle(sliderTrack, 5.0f,
        SDK::Color(220, 220, 220, 255),
        SDK::Color(180, 180, 180, 255),
        1.0f);
    
    // Draw slider thumb
    int thumbX = static_cast<int>(sliderTrack.left + (sliderTrack.right - sliderTrack.left) * g_progressValue);
    renderBackend->DrawEllipse(thumbX, 365, 12, 12,
        SDK::Color(100, 149, 237, 255),
        SDK::Color(65, 105, 225, 255),
        2.0f);
    
    RECT sliderLabel = {50, 380, 550, 400};
    renderBackend->DrawText(L"Slider Control", sliderLabel,
        SDK::Color(50, 50, 50, 255), L"", 12, 400);
    
    // Draw info text
    RECT infoRect = {50, 430, 750, 560};
    renderBackend->DrawText(
        L"Features demonstrated:\n"
        L"- Progress bars with gradient fills\n"
        L"- Interactive buttons\n"
        L"- Checkboxes with state\n"
        L"- Slider controls\n"
        L"- Click 'Animate Progress' to see animation\n"
        L"- Click 'Reset Progress' to reset to 0%",
        infoRect,
        SDK::Color(100, 100, 100, 255), L"", 12, 400);
    
    g_mainWindow->EndPaint();
    
    // Update animation
    if (g_animating) {
        g_progressValue += 0.005f;
        if (g_progressValue > 1.0f) {
            g_progressValue = 0.0f;
        }
        g_mainWindow->Invalidate();
    }
}

// Close callback
void OnClose() {
    SDK::X11WindowManager::GetInstance().Quit();
}

// Mouse button callback
void OnMouseButton(int x, int y, int button, bool pressed) {
    if (!pressed || button != 1) { // Left button release
        return;
    }
    
    // Check if clicked on "Animate Progress" button
    if (x >= 50 && x <= 230 && y >= 220 && y <= 260) {
        g_animating = !g_animating;
        if (g_mainWindow) {
            g_mainWindow->Invalidate();
        }
    }
    
    // Check if clicked on "Reset Progress" button
    if (x >= 250 && x <= 430 && y >= 220 && y <= 260) {
        g_progressValue = 0.0f;
        g_animating = false;
        if (g_mainWindow) {
            g_mainWindow->Invalidate();
        }
    }
    
    // Check if clicked on checkbox
    if (x >= 50 && x <= 70 && y >= 300 && y <= 320) {
        g_animating = !g_animating;
        if (g_mainWindow) {
            g_mainWindow->Invalidate();
        }
    }
    
    // Check if clicked on slider
    if (x >= 50 && x <= 550 && y >= 353 && y <= 377) {
        g_progressValue = static_cast<float>(x - 50) / 500.0f;
        if (g_progressValue < 0.0f) g_progressValue = 0.0f;
        if (g_progressValue > 1.0f) g_progressValue = 1.0f;
        if (g_mainWindow) {
            g_mainWindow->Invalidate();
        }
    }
}

int main() {
    // Create main window
    g_mainWindow = SDK::X11WindowManager::GetInstance().CreateWindow(
        L"5D GUI SDK - Widget Demo (Linux)",
        100, 100,
        800, 600
    );
    
    if (!g_mainWindow) {
        return 1;
    }
    
    // Set up callbacks
    g_mainWindow->SetPaintCallback(OnPaint);
    g_mainWindow->SetCloseCallback(OnClose);
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
