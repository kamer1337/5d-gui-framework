#pragma once


#include "Platform.h"
#include <vector>
#include <memory>
#include <unordered_map>
#include <string>
#include <thread>
#include <mutex>
#include "Theme.h"

namespace SDK {

/**
 * Renderer - Advanced rendering utilities for 5D GUI
 * Implements gradients, shadows, rounded corners, and effects
 * Now with optimizations and advanced visual effects
 */
class Renderer {
public:
    // Gradient rendering
    static void DrawGradient(HDC hdc, const RECT& rect, const Gradient& gradient);
    static void DrawVerticalGradient(HDC hdc, const RECT& rect, Color startColor, Color endColor);
    static void DrawHorizontalGradient(HDC hdc, const RECT& rect, Color startColor, Color endColor);
    static void DrawRadialGradient(HDC hdc, const RECT& rect, Color centerColor, Color edgeColor, int cx, int cy);
    
    // Advanced gradient types
    struct GradientStop {
        float position;  // 0.0 to 1.0
        Color color;
    };
    
    static void DrawMultiStopGradient(HDC hdc, const RECT& rect, const std::vector<GradientStop>& stops, bool horizontal = true);
    static void DrawConicalGradient(HDC hdc, const RECT& rect, const std::vector<GradientStop>& stops, int cx, int cy, float startAngle = 0.0f);
    
    // Rounded rectangle with alpha
    static void DrawRoundedRect(HDC hdc, const RECT& rect, int radius, Color fillColor, Color borderColor, int borderWidth);
    
    // Shadow rendering
    static void DrawShadow(HDC hdc, const RECT& rect, int offsetX, int offsetY, int blur, Color shadowColor);
    
    // Glow effect
    static void DrawGlow(HDC hdc, const RECT& rect, int radius, Color glowColor);
    
    // Advanced visual effects
    static void ApplyBlur(HDC hdc, const RECT& rect, int blurRadius);
    static void ApplyBloom(HDC hdc, const RECT& rect, float threshold, float intensity);
    static void ApplyColorCorrection(HDC hdc, const RECT& rect, float brightness, float contrast, float saturation);
    static void ApplyNoiseOverlay(HDC hdc, const RECT& rect, float intensity, int seed = 0);
    
    // Particle system
    struct Particle {
        float x, y;
        float vx, vy;
        float life;
        Color color;
        bool active;  // For object pooling
    };
    
    // Particle pool for memory optimization
    class ParticlePool {
    public:
        ParticlePool(size_t initialSize = 1000);
        ~ParticlePool() = default;
        
        Particle* Acquire();
        void Release(Particle* particle);
        void ReleaseAll();
        size_t GetActiveCount() const;
        size_t GetTotalCount() const;
        
    private:
        std::vector<Particle> particles_;
        std::vector<Particle*> available_;
        size_t activeCount_;
    };
    
    static void DrawParticles(HDC hdc, const std::vector<Particle>& particles);
    static void UpdateParticles(std::vector<Particle>& particles, float deltaTime);
    static std::vector<Particle> CreateParticleEmission(int x, int y, int count, Color color);
    static void DrawParticlesFromPool(HDC hdc, ParticlePool& pool);
    static void UpdateParticlesInPool(ParticlePool& pool, float deltaTime);
    static void UpdateParticlesInPoolMultiThreaded(ParticlePool& pool, float deltaTime, int numThreads = 0);
    
    // Icon rendering for 5D icon system
    enum class IconType {
        CIRCLE,
        SQUARE,
        DIAMOND,
        FOLDER,
        STAR
    };
    
    static void DrawIcon(HDC hdc, IconType type, int x, int y, int size, Color color, float alpha = 1.0f);
    
    // Texture atlas for icon optimization
    class TextureAtlas {
    public:
        struct AtlasEntry {
            int x, y, width, height;
            HBITMAP bitmap;
        };
        
        TextureAtlas(int width, int height);
        ~TextureAtlas();
        
        bool AddTexture(const std::string& name, HBITMAP bitmap, int width, int height);
        const AtlasEntry* GetTexture(const std::string& name) const;
        void Clear();
        
    private:
        int atlasWidth_;
        int atlasHeight_;
        int currentX_;
        int currentY_;
        int rowHeight_;
        std::unordered_map<std::string, AtlasEntry> textures_;
    };
    
    // Animation system
    enum class EasingType {
        LINEAR,
        EASE_IN_QUAD,
        EASE_OUT_QUAD,
        EASE_IN_OUT_QUAD,
        EASE_IN_CUBIC,
        EASE_OUT_CUBIC,
        EASE_IN_OUT_CUBIC,
        EASE_IN_QUART,
        EASE_OUT_QUART,
        EASE_IN_OUT_QUART,
        EASE_IN_ELASTIC,
        EASE_OUT_ELASTIC,
        EASE_IN_OUT_ELASTIC,
        EASE_IN_BOUNCE,
        EASE_OUT_BOUNCE,
        EASE_IN_OUT_BOUNCE
    };
    
    struct Keyframe {
        float time;      // Time in seconds
        float value;     // Value at this time
        EasingType easing;
    };
    
    class Animation {
    public:
        Animation() : duration_(1.0f), currentTime_(0.0f), playing_(false), looping_(false) {}
        
        void AddKeyframe(float time, float value, EasingType easing = EasingType::LINEAR);
        void SetDuration(float duration) { duration_ = duration; }
        void SetLooping(bool loop) { looping_ = loop; }
        
        void Play() { playing_ = true; currentTime_ = 0.0f; }
        void Stop() { playing_ = false; currentTime_ = 0.0f; }
        void Pause() { playing_ = false; }
        void Resume() { playing_ = true; }
        
        void Update(float deltaTime);
        float GetValue() const;
        bool IsPlaying() const { return playing_; }
        bool IsFinished() const { return currentTime_ >= duration_ && !looping_; }
        
    private:
        std::vector<Keyframe> keyframes_;
        float duration_;
        float currentTime_;
        bool playing_;
        bool looping_;
    };
    
    static float ApplyEasing(float t, EasingType type);
    
    // Rendering optimization
    struct DirtyRect {
        RECT rect;
        bool dirty;
    };
    
    class RenderCache {
    public:
        RenderCache(int width, int height);
        ~RenderCache();
        
        void MarkDirty(const RECT& rect);
        void MarkClean();
        bool IsDirty(const RECT& rect) const;
        
        HDC GetCacheDC() { return cacheDC_; }
        void CopyToTarget(HDC targetDC, const RECT& srcRect, int destX, int destY);
        
    private:
        HDC cacheDC_;
        HBITMAP cacheBitmap_;
        int width_;
        int height_;
        std::vector<DirtyRect> dirtyRegions_;
    };
    
    // Occlusion culling helper
    static bool IsRectOccluded(const RECT& rect, const std::vector<RECT>& occluders);
    static bool RectsIntersect(const RECT& a, const RECT& b);
    
    // Utility functions
    static Color InterpolateColor(Color c1, Color c2, float t);
    static BYTE BlendAlpha(BYTE src, BYTE dst, BYTE alpha);
    
    // Create memory DC for offscreen rendering
    static HDC CreateMemoryDC(int width, int height, HBITMAP* outBitmap);
    static void DeleteMemoryDC(HDC hdc, HBITMAP bitmap);
    
    // Multi-dimensional rendering (3D/4D/5D/6D)
    struct Vector3D {
        float x, y, z;
        Vector3D(float _x = 0, float _y = 0, float _z = 0) : x(_x), y(_y), z(_z) {}
    };
    
    struct Vector4D {
        float x, y, z, w;
        Vector4D(float _x = 0, float _y = 0, float _z = 0, float _w = 0) 
            : x(_x), y(_y), z(_z), w(_w) {}
    };
    
    struct Vector5D {
        float x, y, z, w, d; // d = depth layer
        Vector5D(float _x = 0, float _y = 0, float _z = 0, float _w = 0, float _d = 0) 
            : x(_x), y(_y), z(_z), w(_w), d(_d) {}
    };
    
    struct Vector6D {
        float x, y, z, w, d, t; // t = time dimension
        Vector6D(float _x = 0, float _y = 0, float _z = 0, float _w = 0, float _d = 0, float _t = 0) 
            : x(_x), y(_y), z(_z), w(_w), d(_d), t(_t) {}
    };
    
    // 3D Rendering (basic perspective projection)
    static void Render3DPoint(HDC hdc, const Vector3D& point, int originX, int originY, Color color, float scale = 1.0f);
    static void Render3DLine(HDC hdc, const Vector3D& start, const Vector3D& end, int originX, int originY, Color color, float scale = 1.0f);
    static void Render3DCube(HDC hdc, const Vector3D& center, float size, int originX, int originY, Color color, float rotationX = 0, float rotationY = 0, float rotationZ = 0);
    
    // 4D Rendering (time-animated 3D)
    static void Render4DPoint(HDC hdc, const Vector4D& point, float time, int originX, int originY, Color color, float scale = 1.0f);
    static void Render4DHypercube(HDC hdc, const Vector4D& center, float size, float time, int originX, int originY, Color color);
    
    // 5D Rendering (depth layers + 3D)
    static void Render5DPoint(HDC hdc, const Vector5D& point, float time, int originX, int originY, Color color, float scale = 1.0f);
    static void Render5DScene(HDC hdc, const std::vector<Vector5D>& points, float time, int originX, int originY, const std::vector<Color>& colors);
    
    // 6D Rendering (multi-timeline rendering)
    static void Render6DPoint(HDC hdc, const Vector6D& point, int originX, int originY, Color color, float scale = 1.0f);
    static void Render6DPath(HDC hdc, const std::vector<Vector6D>& path, int originX, int originY, Color color);
    
    // Projection helpers
    static void Project3Dto2D(const Vector3D& point3D, int& x2D, int& y2D, int originX, int originY, float scale);
    static void Project4Dto3D(const Vector4D& point4D, Vector3D& point3D, float time);
    static void Project5Dto4D(const Vector5D& point5D, Vector4D& point4D, float depthScale);
    static void Project6Dto5D(const Vector6D& point6D, Vector5D& point5D);
    
private:
    Renderer() = delete;
};

} // namespace SDK
