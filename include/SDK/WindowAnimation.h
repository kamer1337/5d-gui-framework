#pragma once


#include "Platform.h"
#include <functional>
#include <chrono>

namespace SDK {

/**
 * WindowAnimation - Provides window animation effects
 * Supports custom minimize/maximize effects and smooth transitions
 */
class WindowAnimation {
public:
    // Animation types
    enum class AnimationType {
        NONE,
        FADE,
        SLIDE,
        SCALE,
        ZOOM
    };
    
    // Animation state
    enum class AnimationState {
        IDLE,
        MINIMIZING,
        MINIMIZED,
        RESTORING,
        MAXIMIZING,
        MAXIMIZED
    };
    
    // Easing functions
    enum class EasingType {
        LINEAR,
        EASE_IN,
        EASE_OUT,
        EASE_IN_OUT,
        BOUNCE
    };
    
    WindowAnimation(HWND hwnd);
    ~WindowAnimation();
    
    // Configuration
    void SetMinimizeAnimation(AnimationType type) { m_minimizeType = type; }
    AnimationType GetMinimizeAnimation() const { return m_minimizeType; }
    
    void SetMaximizeAnimation(AnimationType type) { m_maximizeType = type; }
    AnimationType GetMaximizeAnimation() const { return m_maximizeType; }
    
    void SetRestoreAnimation(AnimationType type) { m_restoreType = type; }
    AnimationType GetRestoreAnimation() const { return m_restoreType; }
    
    void SetAnimationDuration(int milliseconds) { m_duration = milliseconds; }
    int GetAnimationDuration() const { return m_duration; }
    
    void SetEasingType(EasingType easing) { m_easing = easing; }
    EasingType GetEasingType() const { return m_easing; }
    
    // Animation control
    void AnimateMinimize();
    void AnimateMaximize();
    void AnimateRestore();
    
    void StopAnimation();
    bool IsAnimating() const { return m_state != AnimationState::IDLE && m_state != AnimationState::MINIMIZED && m_state != AnimationState::MAXIMIZED; }
    
    AnimationState GetState() const { return m_state; }
    
    // Update animation (call from message loop)
    void Update();
    
    // Callbacks
    void SetOnAnimationComplete(std::function<void()> callback) {
        m_onComplete = callback;
    }
    
private:
    HWND m_hwnd;
    
    AnimationType m_minimizeType;
    AnimationType m_maximizeType;
    AnimationType m_restoreType;
    
    AnimationState m_state;
    
    int m_duration; // milliseconds
    EasingType m_easing;
    
    std::chrono::steady_clock::time_point m_startTime;
    RECT m_startRect;
    RECT m_targetRect;
    BYTE m_startAlpha;
    BYTE m_targetAlpha;
    
    std::function<void()> m_onComplete;
    
    // Helper methods
    float GetProgress() const;
    float ApplyEasing(float t) const;
    void PerformFadeAnimation(float progress);
    void PerformSlideAnimation(float progress);
    void PerformScaleAnimation(float progress);
    void PerformZoomAnimation(float progress);
    void CompleteAnimation();
};

} // namespace SDK
