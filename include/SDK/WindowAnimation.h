#pragma once


#include "Platform.h"
#include <functional>
#include <chrono>
#include <vector>

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
        BOUNCE,
        ELASTIC,
        BACK,
        CUBIC_BEZIER  // Custom Bezier curve
    };
    
    // Bezier curve control points for custom easing
    struct BezierCurve {
        float x1, y1;  // First control point
        float x2, y2;  // Second control point
        
        BezierCurve() : x1(0.42f), y1(0.0f), x2(0.58f), y2(1.0f) {}  // Default ease-in-out
        BezierCurve(float _x1, float _y1, float _x2, float _y2) 
            : x1(_x1), y1(_y1), x2(_x2), y2(_y2) {}
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
    
    // Bezier curve control
    void SetBezierCurve(const BezierCurve& curve) { m_bezierCurve = curve; }
    BezierCurve GetBezierCurve() const { return m_bezierCurve; }
    
    // Animation control
    void AnimateMinimize();
    void AnimateMaximize();
    void AnimateRestore();
    
    void StopAnimation();
    void PauseAnimation();
    void ResumeAnimation();
    void ReverseAnimation();
    
    bool IsAnimating() const { return m_state != AnimationState::IDLE && m_state != AnimationState::MINIMIZED && m_state != AnimationState::MAXIMIZED; }
    bool IsPaused() const { return m_paused; }
    bool IsReversed() const { return m_reversed; }
    
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
    BezierCurve m_bezierCurve;
    
    bool m_paused;
    bool m_reversed;
    std::chrono::milliseconds m_pausedDuration;
    
    std::chrono::steady_clock::time_point m_startTime;
    RECT m_startRect;
    RECT m_targetRect;
    BYTE m_startAlpha;
    BYTE m_targetAlpha;
    
    std::function<void()> m_onComplete;
    
    // Helper methods
    float GetProgress() const;
    float ApplyEasing(float t) const;
    float EvaluateBezier(float t) const;
    void PerformFadeAnimation(float progress);
    void PerformSlideAnimation(float progress);
    void PerformScaleAnimation(float progress);
    void PerformZoomAnimation(float progress);
    void CompleteAnimation();
};

/**
 * AnimationGroup - Manages multiple animations as a sequence or parallel group
 */
class AnimationGroup {
public:
    enum class PlayMode {
        SEQUENCE,   // Play animations one after another
        PARALLEL    // Play all animations simultaneously
    };
    
    AnimationGroup(PlayMode mode = PlayMode::SEQUENCE);
    ~AnimationGroup();
    
    // Add animations to the group
    void AddAnimation(WindowAnimation* animation);
    void RemoveAnimation(WindowAnimation* animation);
    void Clear();
    
    // Playback control
    void Play();
    void Stop();
    void Pause();
    void Resume();
    void Reverse();
    
    // State
    bool IsPlaying() const { return m_playing; }
    bool IsPaused() const { return m_paused; }
    
    // Update all animations (call from message loop)
    void Update();
    
    // Configuration
    void SetPlayMode(PlayMode mode) { m_playMode = mode; }
    PlayMode GetPlayMode() const { return m_playMode; }
    
    // Callbacks
    void SetOnComplete(std::function<void()> callback) {
        m_onComplete = callback;
    }
    
private:
    PlayMode m_playMode;
    std::vector<WindowAnimation*> m_animations;
    size_t m_currentIndex;  // For sequence mode
    bool m_playing;
    bool m_paused;
    std::function<void()> m_onComplete;
};

} // namespace SDK
