#pragma once

#include "Platform.h"
#include <vector>
#include <memory>
#include <unordered_map>
#include <string>

namespace SDK {

/**
 * RendererOptimizer - Machine Learning-based renderer optimization
 * Uses lightweight ML algorithms to predict optimal rendering strategies
 */
class RendererOptimizer {
public:
    // Rendering decision types
    enum class RenderStrategy {
        FULL_RENDER,        // Full render of element
        CACHED_RENDER,      // Use cached version
        SKIP_RENDER,        // Skip (occluded)
        LOD_HIGH,           // High level of detail
        LOD_MEDIUM,         // Medium level of detail
        LOD_LOW             // Low level of detail
    };
    
    // Performance metrics for an element
    struct ElementMetrics {
        int renderCount;            // How many times rendered
        float avgRenderTime;        // Average render time (ms)
        float lastRenderTime;       // Last render time (ms)
        int cacheHits;              // Cache hit count
        int cacheMisses;            // Cache miss count
        bool isAnimated;            // Is element animated
        float changeFrequency;      // How often element changes (0-1)
        int pixelArea;              // Area in pixels
        float screenCoverage;       // % of screen covered
        float lastUpdateTime;       // Time since last update
    };
    
    // ML model for optimization decisions
    class OptimizationModel {
    public:
        OptimizationModel();
        
        // Predict best rendering strategy
        RenderStrategy Predict(const ElementMetrics& metrics) const;
        
        // Update model with feedback (online learning)
        void Learn(const ElementMetrics& metrics, RenderStrategy actual, float performance);
        
        // Get model confidence (0-1)
        float GetConfidence() const { return confidence_; }
        
    private:
        // Simple neural network weights (lightweight)
        std::vector<float> weights_;
        float bias_;
        float learningRate_;
        float confidence_;
        int trainingCount_;
        
        // Feature extraction from metrics
        std::vector<float> ExtractFeatures(const ElementMetrics& metrics) const;
        
        // Activation function
        float Sigmoid(float x) const;
        
        // Update weights based on feedback
        void UpdateWeights(const std::vector<float>& features, float error);
    };
    
    RendererOptimizer();
    ~RendererOptimizer() = default;
    
    // Register an element for optimization
    void RegisterElement(const std::string& elementId, const RECT& bounds);
    
    // Get recommended rendering strategy for element
    RenderStrategy GetOptimalStrategy(const std::string& elementId);
    
    // Update metrics after rendering
    void RecordRenderMetrics(const std::string& elementId, float renderTime, bool wasVisible);
    
    // Record cache hit/miss
    void RecordCacheAccess(const std::string& elementId, bool hit);
    
    // Mark element as changed
    void MarkElementChanged(const std::string& elementId);
    
    // Get current metrics for element
    const ElementMetrics* GetMetrics(const std::string& elementId) const;
    
    // Calculate appropriate LOD level based on screen coverage and distance
    int CalculateLOD(const std::string& elementId, float screenCoverage);
    
    // Enable/disable ML optimization
    void SetEnabled(bool enabled) { enabled_ = enabled; }
    bool IsEnabled() const { return enabled_; }
    
    // Get performance statistics
    struct PerformanceStats {
        int totalElements;
        int fullRenders;
        int cachedRenders;
        int skippedRenders;
        float avgRenderTime;
        float cacheHitRate;
        float mlAccuracy;
    };
    
    PerformanceStats GetStats() const;
    
    // Reset statistics
    void ResetStats();
    
private:
    bool enabled_;
    std::unordered_map<std::string, ElementMetrics> elementMetrics_;
    std::unique_ptr<OptimizationModel> model_;
    
    // Statistics
    mutable int totalDecisions_;
    mutable int correctDecisions_;
    mutable float totalRenderTime_;
    mutable int cacheHits_;
    mutable int cacheMisses_;
    
    // Update change frequency based on activity
    void UpdateChangeFrequency(ElementMetrics& metrics, bool changed);
    
    // Decay metrics over time
    void DecayMetrics(ElementMetrics& metrics, float deltaTime);
};

} // namespace SDK
