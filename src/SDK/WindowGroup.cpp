#include "../../include/SDK/WindowGroup.h"
#include <algorithm>

namespace SDK {

WindowGroup::WindowGroup(const std::wstring& name)
    : m_name(name)
    , m_linkedMovement(true)
    , m_synchronizedDepth(true)
{
}

WindowGroup::~WindowGroup() {
    ClearWindows();
}

void WindowGroup::AddWindow(std::shared_ptr<Window> window) {
    if (!window) return;
    
    // Check if already in group
    auto it = std::find(m_windows.begin(), m_windows.end(), window);
    if (it != m_windows.end()) {
        return;
    }
    
    m_windows.push_back(window);
}

void WindowGroup::RemoveWindow(std::shared_ptr<Window> window) {
    if (!window) return;
    
    auto it = std::find(m_windows.begin(), m_windows.end(), window);
    if (it != m_windows.end()) {
        m_windows.erase(it);
    }
}

void WindowGroup::ClearWindows() {
    m_windows.clear();
}

void WindowGroup::MoveGroup(int deltaX, int deltaY) {
    if (!m_linkedMovement) return;
    
    for (auto& window : m_windows) {
        if (!window || !window->IsValid()) continue;
        
        HWND hwnd = window->GetHandle();
        RECT rect;
        if (GetWindowRect(hwnd, &rect)) {
            SetWindowPos(hwnd, nullptr,
                rect.left + deltaX, rect.top + deltaY,
                0, 0,
                SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
        }
    }
}

void WindowGroup::SetGroupDepth(WindowDepth depth) {
    if (!m_synchronizedDepth) return;
    
    for (auto& window : m_windows) {
        if (window && window->IsValid()) {
            window->SetDepth(depth);
        }
    }
}

void WindowGroup::OffsetGroupDepth(int offset) {
    if (!m_synchronizedDepth) return;
    
    for (auto& window : m_windows) {
        if (!window || !window->IsValid()) continue;
        
        int currentDepth = static_cast<int>(window->GetDepth());
        int newDepth = currentDepth + offset;
        
        // Clamp to valid range
        if (newDepth < 0) newDepth = 0;
        if (newDepth > 4) newDepth = 4;
        
        window->SetDepth(static_cast<WindowDepth>(newDepth));
    }
}

void WindowGroup::SetGroupAlpha(BYTE alpha) {
    for (auto& window : m_windows) {
        if (window && window->IsValid()) {
            window->SetAlpha(alpha);
        }
    }
}

void WindowGroup::SetGroupScale(float scale) {
    for (auto& window : m_windows) {
        if (window && window->IsValid()) {
            window->SetScale(scale);
        }
    }
}

void WindowGroup::SetGroupOffset(int x, int y) {
    for (auto& window : m_windows) {
        if (window && window->IsValid()) {
            window->SetDepthOffset(x, y);
        }
    }
}

void WindowGroup::ApplyTransformation(
    std::function<void(std::shared_ptr<Window>)> transformation)
{
    if (!transformation) return;
    
    for (auto& window : m_windows) {
        if (window && window->IsValid()) {
            transformation(window);
        }
    }
}

} // namespace SDK
