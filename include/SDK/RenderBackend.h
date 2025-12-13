#pragma once

#include "Platform.h"
#include "Theme.h"
#include <memory>
#include <vector>

namespace SDK {

/**
 * RenderBackend - Abstract interface for rendering backends
 * Allows switching between GDI, Direct2D, or other rendering systems
 */
class RenderBackend {
public:
    enum class BackendType {
        GDI,        // Software rendering (default)
        DIRECT2D,   // Hardware accelerated (Windows 7+)
        AUTO        // Automatic selection
    };
    
    virtual ~RenderBackend() = default;
    
    // Factory method to create appropriate backend
    static std::unique_ptr<RenderBackend> Create(BackendType type = BackendType::AUTO);
    
    // Initialization
    virtual bool Initialize(HWND hwnd) = 0;
    virtual void Shutdown() = 0;
    
    // Rendering context
    virtual bool BeginDraw() = 0;
    virtual void EndDraw() = 0;
    virtual void Clear(Color color) = 0;
    
    // Get native handles
    virtual HDC GetDC() const = 0;
    virtual void* GetNativeContext() const = 0;
    
    // Basic drawing
    virtual void DrawRectangle(const RECT& rect, Color fillColor, Color borderColor, float borderWidth) = 0;
    virtual void DrawRoundedRectangle(const RECT& rect, float radius, Color fillColor, Color borderColor, float borderWidth) = 0;
    virtual void DrawLine(int x1, int y1, int x2, int y2, Color color, float width) = 0;
    virtual void DrawEllipse(int cx, int cy, int rx, int ry, Color fillColor, Color borderColor, float borderWidth) = 0;
    
    // Text rendering
    virtual void DrawText(const std::wstring& text, const RECT& rect, Color color, const std::wstring& fontFamily, float fontSize, int fontWeight) = 0;
    
    // Gradients
    virtual void DrawLinearGradient(const RECT& rect, Color startColor, Color endColor, bool horizontal) = 0;
    virtual void DrawRadialGradient(const RECT& rect, Color centerColor, Color edgeColor, int cx, int cy) = 0;
    
    // Effects
    virtual void DrawShadow(const RECT& rect, int offsetX, int offsetY, int blur, Color shadowColor) = 0;
    virtual void DrawGlow(const RECT& rect, int radius, Color glowColor) = 0;
    
    // Advanced effects (GPU accelerated when available)
    virtual bool SupportsGPUEffects() const = 0;
    virtual void ApplyBlur(const RECT& rect, int blurRadius) = 0;
    virtual void ApplyBloom(const RECT& rect, float threshold, float intensity) = 0;
    virtual void ApplyDepthOfField(const RECT& rect, int focalDepth, int blurAmount, float focalRange) = 0;
    virtual void ApplyMotionBlur(const RECT& rect, int directionX, int directionY, float intensity) = 0;
    virtual void ApplyChromaticAberration(const RECT& rect, float strength, int offsetX, int offsetY) = 0;
    
    // Performance
    virtual BackendType GetType() const = 0;
    virtual bool IsHardwareAccelerated() const = 0;
    
    // Capabilities
    struct Capabilities {
        bool supportsGPUAcceleration;
        bool supportsAdvancedEffects;
        bool supportsAntialiasing;
        bool supportsTransparency;
        int maxTextureSize;
    };
    
    virtual Capabilities GetCapabilities() const = 0;
    
    // Effect Presets - Convenient combinations of effects
    enum class EffectPreset {
        NONE,
        CINEMATIC,      // Depth of field + subtle blur + bloom
        GAME_UI,        // Sharp edges + glow + chromatic aberration
        RETRO,          // Chromatic aberration + bloom
        DREAMY,         // Soft blur + bloom
        MOTION,         // Motion blur + slight chromatic aberration
        CUSTOM          // User-defined combination
    };
    
    struct EffectSettings {
        bool enableBlur = false;
        int blurRadius = 5;
        
        bool enableBloom = false;
        float bloomThreshold = 0.8f;
        float bloomIntensity = 1.0f;
        
        bool enableDepthOfField = false;
        int focalDepth = 0;
        int dofBlurAmount = 10;
        float focalRange = 100.0f;
        
        bool enableMotionBlur = false;
        int motionDirX = 5;
        int motionDirY = 0;
        float motionIntensity = 0.5f;
        
        bool enableChromaticAberration = false;
        float chromaticStrength = 0.01f;
        int chromaticOffsetX = 2;
        int chromaticOffsetY = 2;
    };
    
    virtual void ApplyEffectPreset(const RECT& rect, EffectPreset preset);
    virtual void ApplyCustomEffects(const RECT& rect, const EffectSettings& settings);
    
protected:
    RenderBackend() = default;
};

} // namespace SDK
