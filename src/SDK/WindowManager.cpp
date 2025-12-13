#include "../../include/SDK/WindowManager.h"
#include <algorithm>
#include <cmath>

namespace SDK {

WindowManager& WindowManager::GetInstance() {
    static WindowManager instance;
    return instance;
}

WindowManager::WindowManager()
    : m_activeModal(nullptr)
    , m_defaultTheme(nullptr)
    , m_depthAnimation(false)
    , m_animationTime(0.0f)
{
}

WindowManager::~WindowManager() {
    Shutdown();
}

bool WindowManager::Initialize() {
    // Create default theme
    m_defaultTheme = std::make_shared<Theme>(Theme::CreateDefaultTheme());
    return true;
}

void WindowManager::Shutdown() {
    m_windows.clear();
    m_sortedWindows.clear();
    m_defaultTheme = nullptr;
}

std::shared_ptr<Window> WindowManager::RegisterWindow(HWND hwnd) {
    if (!hwnd || !::IsWindow(hwnd)) {
        return nullptr;
    }
    
    // Check if already registered
    auto it = m_windows.find(hwnd);
    if (it != m_windows.end()) {
        return it->second;
    }
    
    // Create new window
    auto window = std::make_shared<Window>(hwnd);
    
    // Batch initialization to avoid multiple redraws
    window->BeginUpdate();
    
    // Apply default theme
    if (m_defaultTheme) {
        window->SetTheme(m_defaultTheme);
    }
    
    // Enable layered mode for alpha support
    window->EnableLayeredMode();
    
    // End batched updates
    window->EndUpdate();
    
    // Register
    m_windows[hwnd] = window;
    
    // Update sorted list
    SortWindowsByDepth();
    
    return window;
}

void WindowManager::UnregisterWindow(HWND hwnd) {
    auto it = m_windows.find(hwnd);
    if (it != m_windows.end()) {
        m_windows.erase(it);
        SortWindowsByDepth();
    }
    
    if (m_activeModal == hwnd) {
        m_activeModal = nullptr;
    }
}

std::shared_ptr<Window> WindowManager::GetWindow(HWND hwnd) {
    auto it = m_windows.find(hwnd);
    if (it != m_windows.end()) {
        return it->second;
    }
    return nullptr;
}

std::vector<std::shared_ptr<Window>> WindowManager::GetWindowsByDepth() const {
    return m_sortedWindows;
}

void WindowManager::SetActiveModal(HWND hwnd) {
    m_activeModal = hwnd;
}

void WindowManager::ClearActiveModal() {
    m_activeModal = nullptr;
}

void WindowManager::SetDefaultTheme(std::shared_ptr<Theme> theme) {
    m_defaultTheme = theme;
    
    // Apply to all existing windows
    for (auto& pair : m_windows) {
        pair.second->SetTheme(theme);
    }
}

void WindowManager::RenderAllWindows() {
    // Render in depth order (back to front)
    for (auto& window : m_sortedWindows) {
        if (window->IsValid()) {
            HDC hdc = GetDC(window->GetHandle());
            if (hdc) {
                window->Render(hdc);
                ReleaseDC(window->GetHandle(), hdc);
            }
        }
    }
}

void WindowManager::UpdateWindowDepths() {
    SortWindowsByDepth();
}

void WindowManager::EnableDepthAnimation(bool enabled) {
    m_depthAnimation = enabled;
}

void WindowManager::Update(float deltaTime) {
    if (m_depthAnimation) {
        m_animationTime += deltaTime;
        
        // Apply breathing animation to windows
        for (auto& pair : m_windows) {
            auto& window = pair.second;
            if (window->IsValid()) {
                // Subtle scale animation based on depth
                float baseScale = 1.0f;
                switch (window->GetDepth()) {
                    case WindowDepth::FAR_BACKGROUND: baseScale = 0.7f; break;
                    case WindowDepth::BACKGROUND: baseScale = 0.8f; break;
                    case WindowDepth::MIDDLE: baseScale = 0.9f; break;
                    case WindowDepth::MID_FRONT: baseScale = 0.95f; break;
                    case WindowDepth::FOREGROUND: baseScale = 1.0f; break;
                }
                
                // Add sine wave animation
                float animScale = baseScale + 0.02f * sin(m_animationTime * 2.0f);
                window->SetScale(animScale);
            }
        }
    }
}

void WindowManager::SortWindowsByDepth() {
    m_sortedWindows.clear();
    m_sortedWindows.reserve(m_windows.size());
    
    for (auto& pair : m_windows) {
        m_sortedWindows.push_back(pair.second);
    }
    
    // Sort by depth (back to front)
    std::sort(m_sortedWindows.begin(), m_sortedWindows.end(),
        [](const std::shared_ptr<Window>& a, const std::shared_ptr<Window>& b) {
            return static_cast<int>(a->GetDepth()) < static_cast<int>(b->GetDepth());
        });
}

} // namespace SDK
