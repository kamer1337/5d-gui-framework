#pragma once

#include <vector>
#include <string>
#include <map>
#include <memory>
#include <functional>
#include <cmath>
#include <random>

namespace SDK {

/**
 * Simple Neural Network implementation with no external dependencies
 * Designed to interpret natural language prompts for GUI creation
 * 
 * Features:
 * - Feedforward architecture with backpropagation
 * - Word embedding for text tokenization
 * - Pre-trained on project documentation
 * - Classifies intents (create window, add button, set callback, etc.)
 * - Extracts entities (dimensions, widget types, event types)
 */
class NeuralNetwork {
public:
    // Intent types that the network can classify
    enum class Intent {
        CREATE_WINDOW,
        ADD_BUTTON,
        ADD_LABEL,
        ADD_TEXTBOX,
        ADD_CHECKBOX,
        ADD_PROGRESSBAR,
        ADD_TOOLTIP,
        ADD_SLIDER,
        ADD_COMBOBOX,
        ADD_LISTBOX,
        ADD_LISTVIEW,
        ADD_RADIOBUTTON,
        ADD_SPINBOX,
        ADD_IMAGE,
        ADD_SEPARATOR,
        ADD_PANEL,
        ADD_TABCONTROL,
        ADD_TOOLBAR,
        SET_CALLBACK,
        SET_THEME,
        SET_LAYOUT,
        UNKNOWN
    };
    
    // Callback types for widgets
    enum class CallbackType {
        ON_CLICK,
        ON_DOUBLE_CLICK,
        ON_HOVER,
        ON_FOCUS,
        ON_BLUR,
        ON_CHANGE,
        ON_VALUE_CHANGED,
        ON_KEY_PRESS,
        CUSTOM
    };
    
    // Layout types for widget organization
    enum class LayoutType {
        NONE,
        VERTICAL,
        HORIZONTAL,
        GRID,
        ABSOLUTE
    };
    
    // Extracted information from prompt
    struct ParsedPrompt {
        Intent intent;
        std::map<std::wstring, std::wstring> entities;  // Key-value pairs
        float confidence;  // 0.0 to 1.0
        std::vector<Intent> additionalWidgets;  // For multi-widget prompts
        LayoutType layoutType;  // Layout specification
        
        // Convenience getters
        int GetWidth() const;
        int GetHeight() const;
        std::wstring GetTitle() const;
        std::wstring GetWidgetText() const;
        CallbackType GetCallbackType() const;
        std::wstring GetWidgetType() const;
        int GetItemCount() const;
        std::vector<std::wstring> GetItems() const;
        
        // Enhanced getters for v1.2.2
        std::wstring GetColor() const;
        std::wstring GetColorModifier() const;
        std::wstring GetSize() const;
        std::wstring GetState() const;
        std::wstring GetAction() const;
        std::wstring GetFontStyle() const;
        std::wstring GetPattern() const;
        std::wstring GetValidation() const;
        std::wstring GetPlaceholder() const;
        int GetMinValue() const;
        int GetMaxValue() const;
        int GetCount() const;
    };
    
    NeuralNetwork();
    ~NeuralNetwork() = default;
    
    // Initialize the network with pre-trained weights
    void Initialize();
    
    // Parse a natural language prompt
    ParsedPrompt ParsePrompt(const std::wstring& prompt);
    
    // Train the network (optional - for extending training data)
    void Train(const std::vector<std::pair<std::wstring, ParsedPrompt>>& trainingData, int epochs = 100);
    
    // Get vocabulary size
    size_t GetVocabularySize() const { return m_vocabulary.size(); }
    
private:
    // Neural network architecture
    struct Layer {
        std::vector<std::vector<float>> weights;  // weights[neuron][input]
        std::vector<float> biases;
        std::vector<float> activations;
        std::vector<float> deltas;  // For backpropagation
    };
    
    // Network layers
    std::vector<Layer> m_layers;
    
    // Vocabulary and word embeddings
    std::map<std::wstring, int> m_vocabulary;
    std::vector<std::vector<float>> m_embeddings;
    
    // Intent and entity patterns
    std::map<std::wstring, Intent> m_intentKeywords;
    std::map<std::wstring, std::wstring> m_entityPatterns;
    
    // Network parameters
    static constexpr int EMBEDDING_DIM = 32;
    static constexpr int HIDDEN_LAYER_SIZE = 64;
    static constexpr int OUTPUT_SIZE = 22;  // Number of intent classes (expanded)
    static constexpr float LEARNING_RATE = 0.01f;
    
    // Helper functions
    void InitializeVocabulary();
    void InitializeWeights();
    void InitializePatterns();
    
    std::vector<std::wstring> Tokenize(const std::wstring& text);
    std::vector<float> TextToEmbedding(const std::wstring& text);
    std::vector<float> Forward(const std::vector<float>& input);
    void Backward(const std::vector<float>& target);
    
    float Sigmoid(float x);
    float SigmoidDerivative(float x);
    float ReLU(float x);
    float ReLUDerivative(float x);
    
    Intent OutputToIntent(const std::vector<float>& output, float& confidence);
    std::map<std::wstring, std::wstring> ExtractEntities(const std::wstring& prompt);
    std::vector<Intent> ExtractMultipleWidgets(const std::wstring& prompt);
    LayoutType DetermineLayout(const std::wstring& prompt);
    
    std::mt19937 m_rng;
};

} // namespace SDK
