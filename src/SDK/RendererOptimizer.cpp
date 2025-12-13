#include "SDK/RendererOptimizer.h"
#include <cmath>
#include <algorithm>

namespace SDK {

// Performance thresholds
namespace {
    constexpr float GOOD_PERFORMANCE_THRESHOLD_MS = 8.0f;  // Target render time for good performance (60fps = 16ms frame, aim for half)
    constexpr float MAX_CONFIDENCE = 0.95f;  // Maximum confidence to prevent overconfidence
}

// OptimizationModel Implementation
RendererOptimizer::OptimizationModel::OptimizationModel()
    : learningRate_(0.01f)
    , confidence_(0.5f)
    , trainingCount_(0)
    , bias_(0.0f)
{
    // Initialize weights for features
    // Features: renderTime, changeFreq, pixelArea, screenCoverage, isAnimated, cacheHitRate
    // Weight rationale:
    //   renderTime (0.3): Important for identifying slow elements
    //   changeFreq (0.4): Most important - frequently changing elements need full render
    //   pixelArea (0.15): Larger elements benefit more from optimization
    //   screenCoverage (0.1): On-screen elements prioritized
    //   isAnimated (0.25): Animated elements need frequent updates
    //   cacheHitRate (-0.2): Negative weight - low cache hit suggests changing content
    weights_ = {0.3f, 0.4f, 0.15f, 0.1f, 0.25f, -0.2f};
}

std::vector<float> RendererOptimizer::OptimizationModel::ExtractFeatures(const ElementMetrics& metrics) const {
    std::vector<float> features;
    
    // Normalize render time (0-1, assuming max 16ms for 60fps)
    features.push_back(std::min(metrics.avgRenderTime / 16.0f, 1.0f));
    
    // Change frequency (already 0-1)
    features.push_back(metrics.changeFrequency);
    
    // Pixel area (normalize by typical screen size 1920x1080)
    features.push_back(std::min(metrics.pixelArea / 2073600.0f, 1.0f));
    
    // Screen coverage (already 0-1)
    features.push_back(metrics.screenCoverage);
    
    // Is animated (0 or 1)
    features.push_back(metrics.isAnimated ? 1.0f : 0.0f);
    
    // Cache hit rate (proper float division)
    float totalCacheAccess = static_cast<float>(metrics.cacheHits + metrics.cacheMisses);
    float cacheHitRate = totalCacheAccess > 0 ? 
        static_cast<float>(metrics.cacheHits) / totalCacheAccess : 0.5f;
    features.push_back(cacheHitRate);
    
    return features;
}

float RendererOptimizer::OptimizationModel::Sigmoid(float x) const {
    return 1.0f / (1.0f + std::exp(-x));
}

RendererOptimizer::RenderStrategy RendererOptimizer::OptimizationModel::Predict(const ElementMetrics& metrics) const {
    auto features = ExtractFeatures(metrics);
    
    // Compute weighted sum
    float sum = bias_;
    for (size_t i = 0; i < features.size() && i < weights_.size(); ++i) {
        sum += features[i] * weights_[i];
    }
    
    // Apply activation
    float output = Sigmoid(sum);
    
    // Decision logic based on output and metrics
    if (metrics.isAnimated || metrics.changeFrequency > 0.8f) {
        return RenderStrategy::FULL_RENDER;
    }
    
    if (output < 0.3f) {
        // Low priority - consider caching or LOD
        if (metrics.screenCoverage < 0.1f) {
            return RenderStrategy::LOD_LOW;
        }
        return RenderStrategy::CACHED_RENDER;
    } else if (output < 0.5f) {
        // Medium priority
        if (metrics.screenCoverage < 0.2f) {
            return RenderStrategy::LOD_MEDIUM;
        }
        return RenderStrategy::CACHED_RENDER;
    } else if (output < 0.7f) {
        // High priority
        if (metrics.changeFrequency < 0.2f) {
            return RenderStrategy::CACHED_RENDER;
        }
        return RenderStrategy::FULL_RENDER;
    } else {
        // Very high priority - always render
        return metrics.screenCoverage > 0.5f ? RenderStrategy::FULL_RENDER : RenderStrategy::LOD_HIGH;
    }
}

void RendererOptimizer::OptimizationModel::UpdateWeights(const std::vector<float>& features, float error) {
    // Gradient descent update
    for (size_t i = 0; i < features.size() && i < weights_.size(); ++i) {
        weights_[i] += learningRate_ * error * features[i];
    }
    bias_ += learningRate_ * error;
}

void RendererOptimizer::OptimizationModel::Learn(const ElementMetrics& metrics, RenderStrategy actual, float performance) {
    auto features = ExtractFeatures(metrics);
    
    // Calculate current prediction
    float sum = bias_;
    for (size_t i = 0; i < features.size() && i < weights_.size(); ++i) {
        sum += features[i] * weights_[i];
    }
    float predicted = Sigmoid(sum);
    
    // Calculate error based on performance feedback
    // Good performance (low render time) = correct prediction
    float target = performance < GOOD_PERFORMANCE_THRESHOLD_MS ? 1.0f : 0.0f;
    float error = target - predicted;
    
    // Update weights
    UpdateWeights(features, error);
    
    // Update confidence
    trainingCount_++;
    float accuracy = 1.0f - std::abs(error);
    confidence_ = (confidence_ * (trainingCount_ - 1) + accuracy) / trainingCount_;
    confidence_ = std::min(confidence_, MAX_CONFIDENCE);
}

// RendererOptimizer Implementation
RendererOptimizer::RendererOptimizer()
    : enabled_(true)
    , model_(std::make_unique<OptimizationModel>())
    , totalDecisions_(0)
    , correctDecisions_(0)
    , totalRenderTime_(0.0f)
    , cacheHits_(0)
    , cacheMisses_(0)
{
}

void RendererOptimizer::RegisterElement(const std::string& elementId, const RECT& bounds) {
    if (elementMetrics_.find(elementId) != elementMetrics_.end()) {
        return; // Already registered
    }
    
    ElementMetrics metrics = {};
    metrics.renderCount = 0;
    metrics.avgRenderTime = 0.0f;
    metrics.lastRenderTime = 0.0f;
    metrics.cacheHits = 0;
    metrics.cacheMisses = 0;
    metrics.isAnimated = false;
    metrics.changeFrequency = 0.5f; // Start with medium frequency
    metrics.pixelArea = (bounds.right - bounds.left) * (bounds.bottom - bounds.top);
    metrics.screenCoverage = 0.0f; // Will be updated
    metrics.lastUpdateTime = 0.0f;
    
    elementMetrics_[elementId] = metrics;
}

RendererOptimizer::RenderStrategy RendererOptimizer::GetOptimalStrategy(const std::string& elementId) {
    if (!enabled_) {
        return RenderStrategy::FULL_RENDER;
    }
    
    auto it = elementMetrics_.find(elementId);
    if (it == elementMetrics_.end()) {
        return RenderStrategy::FULL_RENDER; // Unknown element, render fully
    }
    
    totalDecisions_++;
    return model_->Predict(it->second);
}

void RendererOptimizer::RecordRenderMetrics(const std::string& elementId, float renderTime, bool wasVisible) {
    auto it = elementMetrics_.find(elementId);
    if (it == elementMetrics_.end()) {
        return;
    }
    
    ElementMetrics& metrics = it->second;
    
    // Update render count
    metrics.renderCount++;
    
    // Update average render time (exponential moving average)
    if (metrics.renderCount == 1) {
        metrics.avgRenderTime = renderTime;
    } else {
        metrics.avgRenderTime = metrics.avgRenderTime * 0.9f + renderTime * 0.1f;
    }
    metrics.lastRenderTime = renderTime;
    
    totalRenderTime_ += renderTime;
    
    // Learn from this rendering decision
    if (enabled_ && wasVisible) {
        RenderStrategy strategy = model_->Predict(metrics);
        model_->Learn(metrics, strategy, renderTime);
        
        // Update accuracy tracking (using same threshold as learning)
        if (renderTime < GOOD_PERFORMANCE_THRESHOLD_MS) {
            correctDecisions_++;
        }
    }
}

void RendererOptimizer::RecordCacheAccess(const std::string& elementId, bool hit) {
    auto it = elementMetrics_.find(elementId);
    if (it == elementMetrics_.end()) {
        return;
    }
    
    ElementMetrics& metrics = it->second;
    if (hit) {
        metrics.cacheHits++;
        cacheHits_++;
    } else {
        metrics.cacheMisses++;
        cacheMisses_++;
    }
}

void RendererOptimizer::MarkElementChanged(const std::string& elementId) {
    auto it = elementMetrics_.find(elementId);
    if (it == elementMetrics_.end()) {
        return;
    }
    
    UpdateChangeFrequency(it->second, true);
}

void RendererOptimizer::UpdateChangeFrequency(ElementMetrics& metrics, bool changed) {
    // Exponential moving average of change frequency
    float changeValue = changed ? 1.0f : 0.0f;
    metrics.changeFrequency = metrics.changeFrequency * 0.95f + changeValue * 0.05f;
}

const RendererOptimizer::ElementMetrics* RendererOptimizer::GetMetrics(const std::string& elementId) const {
    auto it = elementMetrics_.find(elementId);
    if (it == elementMetrics_.end()) {
        return nullptr;
    }
    return &it->second;
}

int RendererOptimizer::CalculateLOD(const std::string& elementId, float screenCoverage) {
    auto it = elementMetrics_.find(elementId);
    if (it == elementMetrics_.end()) {
        return 0; // Highest detail for unknown elements
    }
    
    ElementMetrics& metrics = it->second;
    metrics.screenCoverage = screenCoverage;
    
    // Calculate LOD based on screen coverage and other factors
    if (screenCoverage > 0.3f || metrics.isAnimated) {
        return 0; // High detail
    } else if (screenCoverage > 0.1f) {
        return 1; // Medium detail
    } else {
        return 2; // Low detail
    }
}

RendererOptimizer::PerformanceStats RendererOptimizer::GetStats() const {
    PerformanceStats stats = {};
    stats.totalElements = static_cast<int>(elementMetrics_.size());
    
    // Count strategy usage and total renders
    stats.fullRenders = 0;
    stats.cachedRenders = 0;
    stats.skippedRenders = 0;
    int totalRenderCount = 0;
    
    for (const auto& pair : elementMetrics_) {
        stats.fullRenders += pair.second.renderCount;
        totalRenderCount += pair.second.renderCount;
    }
    
    stats.cachedRenders = cacheHits_;
    
    // Calculate averages (use total render count, not element count)
    if (totalRenderCount > 0) {
        stats.avgRenderTime = totalRenderTime_ / static_cast<float>(totalRenderCount);
    } else {
        stats.avgRenderTime = 0.0f;
    }
    
    int totalCacheAccess = cacheHits_ + cacheMisses_;
    if (totalCacheAccess > 0) {
        stats.cacheHitRate = static_cast<float>(cacheHits_) / totalCacheAccess;
    } else {
        stats.cacheHitRate = 0.0f;
    }
    
    if (totalDecisions_ > 0) {
        stats.mlAccuracy = static_cast<float>(correctDecisions_) / totalDecisions_;
    } else {
        stats.mlAccuracy = 0.0f;
    }
    
    return stats;
}

void RendererOptimizer::ResetStats() {
    totalDecisions_ = 0;
    correctDecisions_ = 0;
    totalRenderTime_ = 0.0f;
    cacheHits_ = 0;
    cacheMisses_ = 0;
    
    for (auto& pair : elementMetrics_) {
        pair.second.renderCount = 0;
        pair.second.cacheHits = 0;
        pair.second.cacheMisses = 0;
    }
}

} // namespace SDK
