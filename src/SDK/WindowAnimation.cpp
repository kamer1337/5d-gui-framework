#include "../../include/SDK/WindowAnimation.h"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace SDK {

WindowAnimation::WindowAnimation(HWND hwnd)
    : m_hwnd(hwnd)
    , m_minimizeType(AnimationType::SCALE)
    , m_maximizeType(AnimationType::ZOOM)
    , m_restoreType(AnimationType::FADE)
    , m_state(AnimationState::IDLE)
    , m_duration(250)
    , m_easing(EasingType::EASE_OUT)
    , m_startAlpha(255)
    , m_targetAlpha(255)
{
    m_startRect = {0};
    m_targetRect = {0};
}

WindowAnimation::~WindowAnimation() {
    StopAnimation();
}

float WindowAnimation::GetProgress() const {
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
        now - m_startTime).count();
    
    float progress = static_cast<float>(elapsed) / static_cast<float>(m_duration);
    return (progress > 1.0f) ? 1.0f : progress;
}

float WindowAnimation::ApplyEasing(float t) const {
    switch (m_easing) {
        case EasingType::LINEAR:
            return t;
            
        case EasingType::EASE_IN:
            return t * t;
            
        case EasingType::EASE_OUT:
            return t * (2.0f - t);
            
        case EasingType::EASE_IN_OUT:
            return (t < 0.5f) ? (2.0f * t * t) : (-1.0f + (4.0f - 2.0f * t) * t);
            
        case EasingType::BOUNCE: {
            // Bounce out effect - simulates a ball bouncing with decreasing amplitude
            if (t < (1.0f / 2.75f)) {
                return 7.5625f * t * t;
            } else if (t < (2.0f / 2.75f)) {
                float t2 = t - (1.5f / 2.75f);
                return 7.5625f * t2 * t2 + 0.75f;
            } else if (t < (2.5f / 2.75f)) {
                float t2 = t - (2.25f / 2.75f);
                return 7.5625f * t2 * t2 + 0.9375f;
            } else {
                float t2 = t - (2.625f / 2.75f);
                return 7.5625f * t2 * t2 + 0.984375f;
            }
        }
        
        default:
            return t;
    }
}

void WindowAnimation::AnimateMinimize() {
    if (!m_hwnd || !IsWindow(m_hwnd)) return;
    
    m_state = AnimationState::MINIMIZING;
    m_startTime = std::chrono::steady_clock::now();
    
    // Get current window rect
    GetWindowRect(m_hwnd, &m_startRect);
    
    // Target is taskbar position
    RECT taskbarRect;
    HWND taskbar = FindWindow(L"Shell_TrayWnd", nullptr);
    if (taskbar) {
        GetWindowRect(taskbar, &taskbarRect);
        m_targetRect = taskbarRect;
    } else {
        // Default to bottom of screen
        m_targetRect.left = 0;
        m_targetRect.top = GetSystemMetrics(SM_CYSCREEN) - 50;
        m_targetRect.right = 200;
        m_targetRect.bottom = GetSystemMetrics(SM_CYSCREEN);
    }
    
    m_startAlpha = 255;
    m_targetAlpha = 0;
}

void WindowAnimation::AnimateMaximize() {
    if (!m_hwnd || !IsWindow(m_hwnd)) return;
    
    m_state = AnimationState::MAXIMIZING;
    m_startTime = std::chrono::steady_clock::now();
    
    // Get current window rect
    GetWindowRect(m_hwnd, &m_startRect);
    
    // Target is maximized position
    HMONITOR hMonitor = MonitorFromWindow(m_hwnd, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = {0};
    mi.cbSize = sizeof(MONITORINFO);
    if (GetMonitorInfo(hMonitor, &mi)) {
        m_targetRect = mi.rcWork;
    } else {
        m_targetRect.left = 0;
        m_targetRect.top = 0;
        m_targetRect.right = GetSystemMetrics(SM_CXSCREEN);
        m_targetRect.bottom = GetSystemMetrics(SM_CYSCREEN);
    }
    
    m_startAlpha = 255;
    m_targetAlpha = 255;
}

void WindowAnimation::AnimateRestore() {
    if (!m_hwnd || !IsWindow(m_hwnd)) return;
    
    m_state = AnimationState::RESTORING;
    m_startTime = std::chrono::steady_clock::now();
    
    // Get current state
    if (IsIconic(m_hwnd)) {
        // Restoring from minimized
        RECT taskbarRect;
        HWND taskbar = FindWindow(L"Shell_TrayWnd", nullptr);
        if (taskbar) {
            GetWindowRect(taskbar, &taskbarRect);
            m_startRect = taskbarRect;
        } else {
            m_startRect.left = 0;
            m_startRect.top = GetSystemMetrics(SM_CYSCREEN) - 50;
            m_startRect.right = 200;
            m_startRect.bottom = GetSystemMetrics(SM_CYSCREEN);
        }
        m_startAlpha = 0;
    } else {
        // Restoring from maximized
        GetWindowRect(m_hwnd, &m_startRect);
        m_startAlpha = 255;
    }
    
    // Target is normal position (center of screen for now)
    int width = 800;
    int height = 600;
    m_targetRect.left = (GetSystemMetrics(SM_CXSCREEN) - width) / 2;
    m_targetRect.top = (GetSystemMetrics(SM_CYSCREEN) - height) / 2;
    m_targetRect.right = m_targetRect.left + width;
    m_targetRect.bottom = m_targetRect.top + height;
    
    m_targetAlpha = 255;
}

void WindowAnimation::StopAnimation() {
    if (m_state == AnimationState::MINIMIZING) {
        ShowWindow(m_hwnd, SW_MINIMIZE);
        m_state = AnimationState::MINIMIZED;
    } else if (m_state == AnimationState::MAXIMIZING) {
        ShowWindow(m_hwnd, SW_MAXIMIZE);
        m_state = AnimationState::MAXIMIZED;
    } else if (m_state == AnimationState::RESTORING) {
        ShowWindow(m_hwnd, SW_RESTORE);
        m_state = AnimationState::IDLE;
    } else {
        m_state = AnimationState::IDLE;
    }
}

void WindowAnimation::PerformFadeAnimation(float progress) {
    BYTE alpha = static_cast<BYTE>(
        m_startAlpha + (m_targetAlpha - m_startAlpha) * progress);
    
    SetLayeredWindowAttributes(m_hwnd, 0, alpha, LWA_ALPHA);
}

void WindowAnimation::PerformSlideAnimation(float progress) {
    int left = static_cast<int>(
        m_startRect.left + (m_targetRect.left - m_startRect.left) * progress);
    int top = static_cast<int>(
        m_startRect.top + (m_targetRect.top - m_startRect.top) * progress);
    
    int width = m_startRect.right - m_startRect.left;
    int height = m_startRect.bottom - m_startRect.top;
    
    SetWindowPos(m_hwnd, nullptr, left, top, width, height,
        SWP_NOZORDER | SWP_NOACTIVATE);
    
    PerformFadeAnimation(progress);
}

void WindowAnimation::PerformScaleAnimation(float progress) {
    int startWidth = m_startRect.right - m_startRect.left;
    int startHeight = m_startRect.bottom - m_startRect.top;
    int targetWidth = m_targetRect.right - m_targetRect.left;
    int targetHeight = m_targetRect.bottom - m_targetRect.top;
    
    int width = static_cast<int>(
        startWidth + (targetWidth - startWidth) * progress);
    int height = static_cast<int>(
        startHeight + (targetHeight - startHeight) * progress);
    
    int left = static_cast<int>(
        m_startRect.left + (m_targetRect.left - m_startRect.left) * progress);
    int top = static_cast<int>(
        m_startRect.top + (m_targetRect.top - m_startRect.top) * progress);
    
    SetWindowPos(m_hwnd, nullptr, left, top, width, height,
        SWP_NOZORDER | SWP_NOACTIVATE);
    
    PerformFadeAnimation(progress);
}

void WindowAnimation::PerformZoomAnimation(float progress) {
    // Similar to scale but with center-point scaling
    int startWidth = m_startRect.right - m_startRect.left;
    int startHeight = m_startRect.bottom - m_startRect.top;
    int targetWidth = m_targetRect.right - m_targetRect.left;
    int targetHeight = m_targetRect.bottom - m_targetRect.top;
    
    int width = static_cast<int>(
        startWidth + (targetWidth - startWidth) * progress);
    int height = static_cast<int>(
        startHeight + (targetHeight - startHeight) * progress);
    
    // Center the zoom
    int centerX = (m_startRect.left + m_startRect.right) / 2;
    int centerY = (m_startRect.top + m_startRect.bottom) / 2;
    
    int targetCenterX = (m_targetRect.left + m_targetRect.right) / 2;
    int targetCenterY = (m_targetRect.top + m_targetRect.bottom) / 2;
    
    int finalCenterX = static_cast<int>(
        centerX + (targetCenterX - centerX) * progress);
    int finalCenterY = static_cast<int>(
        centerY + (targetCenterY - centerY) * progress);
    
    int left = finalCenterX - width / 2;
    int top = finalCenterY - height / 2;
    
    SetWindowPos(m_hwnd, nullptr, left, top, width, height,
        SWP_NOZORDER | SWP_NOACTIVATE);
    
    PerformFadeAnimation(progress);
}

void WindowAnimation::CompleteAnimation() {
    if (m_state == AnimationState::MINIMIZING) {
        ShowWindow(m_hwnd, SW_MINIMIZE);
        m_state = AnimationState::MINIMIZED;
    } else if (m_state == AnimationState::MAXIMIZING) {
        ShowWindow(m_hwnd, SW_MAXIMIZE);
        m_state = AnimationState::MAXIMIZED;
    } else if (m_state == AnimationState::RESTORING) {
        ShowWindow(m_hwnd, SW_RESTORE);
        m_state = AnimationState::IDLE;
    }
    
    if (m_onComplete) {
        m_onComplete();
    }
}

void WindowAnimation::Update() {
    if (!IsAnimating()) return;
    if (!m_hwnd || !IsWindow(m_hwnd)) {
        StopAnimation();
        return;
    }
    
    float progress = GetProgress();
    float easedProgress = ApplyEasing(progress);
    
    // Apply animation based on type
    AnimationType type = AnimationType::NONE;
    if (m_state == AnimationState::MINIMIZING) {
        type = m_minimizeType;
    } else if (m_state == AnimationState::MAXIMIZING) {
        type = m_maximizeType;
    } else if (m_state == AnimationState::RESTORING) {
        type = m_restoreType;
    }
    
    switch (type) {
        case AnimationType::FADE:
            PerformFadeAnimation(easedProgress);
            break;
            
        case AnimationType::SLIDE:
            PerformSlideAnimation(easedProgress);
            break;
            
        case AnimationType::SCALE:
            PerformScaleAnimation(easedProgress);
            break;
            
        case AnimationType::ZOOM:
            PerformZoomAnimation(easedProgress);
            break;
            
        case AnimationType::NONE:
        default:
            break;
    }
    
    // Check if animation is complete
    if (progress >= 1.0f) {
        CompleteAnimation();
    }
}

} // namespace SDK
