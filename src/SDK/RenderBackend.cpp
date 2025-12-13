#include "../../include/SDK/RenderBackend.h"

#if SDK_PLATFORM_WINDOWS
#include "../../include/SDK/GDIRenderBackend.h"
#include "../../include/SDK/D2DRenderBackend.h"
#include <d2d1.h>
#elif SDK_PLATFORM_LINUX && SDK_HAS_X11
#include "../../include/SDK/X11RenderBackend.h"
#endif

namespace SDK {

std::unique_ptr<RenderBackend> RenderBackend::Create(BackendType type) {
#if SDK_PLATFORM_WINDOWS
    if (type == BackendType::AUTO) {
        // Try Direct2D first (hardware accelerated)
        auto d2dBackend = std::make_unique<D2DRenderBackend>();
        // We'll initialize it later, but check if D2D is available
        
        // Check if Direct2D is available by trying to create factory
        ID2D1Factory* pFactory = nullptr;
        HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &pFactory);
        
        if (SUCCEEDED(hr) && pFactory) {
            pFactory->Release();
            // D2D is available, use it
            return d2dBackend;
        }
        
        // Fall back to GDI
        type = BackendType::GDI;
    }
    
    if (type == BackendType::DIRECT2D) {
        return std::make_unique<D2DRenderBackend>();
    } else {
        return std::make_unique<GDIRenderBackend>();
    }
#elif SDK_PLATFORM_LINUX && SDK_HAS_X11
    // On Linux, always use X11 backend
    return std::make_unique<X11RenderBackend>();
#else
    return nullptr;
#endif
}

void RenderBackend::ApplyEffectPreset(const RECT& rect, EffectPreset preset) {
    EffectSettings settings;
    
    switch (preset) {
        case EffectPreset::CINEMATIC:
            // Depth of field + subtle blur + bloom
            settings.enableDepthOfField = true;
            settings.focalDepth = (rect.bottom - rect.top) / 2;
            settings.dofBlurAmount = 8;
            settings.focalRange = 150.0f;
            settings.enableBloom = true;
            settings.bloomThreshold = 0.9f;
            settings.bloomIntensity = 0.8f;
            break;
            
        case EffectPreset::GAME_UI:
            // Sharp edges + glow + chromatic aberration
            settings.enableBloom = true;
            settings.bloomThreshold = 0.95f;
            settings.bloomIntensity = 1.2f;
            settings.enableChromaticAberration = true;
            settings.chromaticStrength = 0.005f;
            settings.chromaticOffsetX = 1;
            settings.chromaticOffsetY = 1;
            break;
            
        case EffectPreset::RETRO:
            // Chromatic aberration + bloom
            settings.enableChromaticAberration = true;
            settings.chromaticStrength = 0.02f;
            settings.chromaticOffsetX = 3;
            settings.chromaticOffsetY = 3;
            settings.enableBloom = true;
            settings.bloomThreshold = 0.7f;
            settings.bloomIntensity = 1.5f;
            break;
            
        case EffectPreset::DREAMY:
            // Soft blur + bloom
            settings.enableBlur = true;
            settings.blurRadius = 3;
            settings.enableBloom = true;
            settings.bloomThreshold = 0.6f;
            settings.bloomIntensity = 1.3f;
            break;
            
        case EffectPreset::MOTION:
            // Motion blur + slight chromatic aberration
            settings.enableMotionBlur = true;
            settings.motionDirX = 10;
            settings.motionDirY = 0;
            settings.motionIntensity = 0.7f;
            settings.enableChromaticAberration = true;
            settings.chromaticStrength = 0.008f;
            settings.chromaticOffsetX = 2;
            settings.chromaticOffsetY = 1;
            break;
            
        case EffectPreset::NONE:
        case EffectPreset::CUSTOM:
        default:
            // No effects
            return;
    }
    
    ApplyCustomEffects(rect, settings);
}

void RenderBackend::ApplyCustomEffects(const RECT& rect, const EffectSettings& settings) {
    // Apply effects in order: blur -> depth of field -> motion blur -> chromatic aberration -> bloom
    
    if (settings.enableBlur) {
        ApplyBlur(rect, settings.blurRadius);
    }
    
    if (settings.enableDepthOfField) {
        ApplyDepthOfField(rect, settings.focalDepth, settings.dofBlurAmount, settings.focalRange);
    }
    
    if (settings.enableMotionBlur) {
        ApplyMotionBlur(rect, settings.motionDirX, settings.motionDirY, settings.motionIntensity);
    }
    
    if (settings.enableChromaticAberration) {
        ApplyChromaticAberration(rect, settings.chromaticStrength, 
                                  settings.chromaticOffsetX, settings.chromaticOffsetY);
    }
    
    if (settings.enableBloom) {
        ApplyBloom(rect, settings.bloomThreshold, settings.bloomIntensity);
    }
}

} // namespace SDK
