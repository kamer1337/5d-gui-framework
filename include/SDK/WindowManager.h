#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <vector>
#include <memory>
#include <unordered_map>
#include "Window.h"
#include "Theme.h"

namespace SDK {

/**
 * WindowManager - Manages multiple windows with multimodal support
 * Handles window creation, depth sorting, and rendering orchestration
 */
class WindowManager {
public:
    static WindowManager& GetInstance();
    
    // Initialize window manager
    bool Initialize();
    void Shutdown();
    
    // Window management
    std::shared_ptr<Window> RegisterWindow(HWND hwnd);
    void UnregisterWindow(HWND hwnd);
    std::shared_ptr<Window> GetWindow(HWND hwnd);
    
    // Get all windows sorted by depth
    std::vector<std::shared_ptr<Window>> GetWindowsByDepth() const;
    
    // Multimodal window support
    void SetActiveModal(HWND hwnd);
    HWND GetActiveModal() const { return m_activeModal; }
    void ClearActiveModal();
    
    // Theme management
    void SetDefaultTheme(std::shared_ptr<Theme> theme);
    std::shared_ptr<Theme> GetDefaultTheme() const { return m_defaultTheme; }
    
    // Rendering
    void RenderAllWindows();
    void UpdateWindowDepths();
    
    // Animation and effects
    void EnableDepthAnimation(bool enabled);
    bool IsDepthAnimationEnabled() const { return m_depthAnimation; }
    
    void Update(float deltaTime);
    
private:
    WindowManager();
    ~WindowManager();
    
    WindowManager(const WindowManager&) = delete;
    WindowManager& operator=(const WindowManager&) = delete;
    
    void SortWindowsByDepth();
    
    std::unordered_map<HWND, std::shared_ptr<Window>> m_windows;
    std::vector<std::shared_ptr<Window>> m_sortedWindows;
    
    HWND m_activeModal;
    std::shared_ptr<Theme> m_defaultTheme;
    
    bool m_depthAnimation;
    float m_animationTime;
};

} // namespace SDK
