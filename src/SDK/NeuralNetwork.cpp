#include "../../include/SDK/NeuralNetwork.h"
#include <algorithm>
#include <sstream>
#include <cctype>
#include <locale>
#include <codecvt>

namespace SDK {

// ParsedPrompt convenience methods
int NeuralNetwork::ParsedPrompt::GetWidth() const {
    auto it = entities.find(L"width");
    if (it != entities.end()) {
        return std::stoi(it->second);
    }
    return 800;  // Default
}

int NeuralNetwork::ParsedPrompt::GetHeight() const {
    auto it = entities.find(L"height");
    if (it != entities.end()) {
        return std::stoi(it->second);
    }
    return 600;  // Default
}

std::wstring NeuralNetwork::ParsedPrompt::GetTitle() const {
    auto it = entities.find(L"title");
    if (it != entities.end()) {
        return it->second;
    }
    return L"Window";
}

std::wstring NeuralNetwork::ParsedPrompt::GetWidgetText() const {
    auto it = entities.find(L"text");
    if (it != entities.end()) {
        return it->second;
    }
    return L"Button";
}

NeuralNetwork::CallbackType NeuralNetwork::ParsedPrompt::GetCallbackType() const {
    auto it = entities.find(L"callback");
    if (it != entities.end()) {
        if (it->second == L"click" || it->second == L"onclick") {
            return CallbackType::ON_CLICK;
        } else if (it->second == L"hover") {
            return CallbackType::ON_HOVER;
        } else if (it->second == L"focus") {
            return CallbackType::ON_FOCUS;
        }
    }
    return CallbackType::ON_CLICK;  // Default
}

NeuralNetwork::NeuralNetwork() 
    : m_rng(std::random_device{}()) {
}

void NeuralNetwork::Initialize() {
    InitializeVocabulary();
    InitializeWeights();
    InitializePatterns();
}

void NeuralNetwork::InitializeVocabulary() {
    // Build vocabulary from common GUI-related words
    std::vector<std::wstring> words = {
        L"create", L"make", L"add", L"new", L"build",
        L"window", L"dialog", L"frame", L"form",
        L"button", L"label", L"text", L"textbox", L"input",
        L"checkbox", L"check", L"box",
        L"progressbar", L"progress", L"bar",
        L"tooltip", L"tip", L"hint",
        L"click", L"hover", L"focus", L"blur", L"press",
        L"callback", L"handler", L"event", L"action",
        L"width", L"height", L"size", L"dimension",
        L"title", L"name", L"caption", L"heading",
        L"theme", L"style", L"color", L"appearance",
        L"with", L"and", L"at", L"to", L"for",
        L"set", L"apply", L"configure", L"setup",
        L"show", L"display", L"render", L"draw",
        L"x", L"pixels", L"px"
    };
    
    // Add numbers 0-1000 to vocabulary
    for (int i = 0; i <= 1000; i++) {
        words.push_back(std::to_wstring(i));
    }
    
    // Build vocabulary map and embeddings
    m_vocabulary.clear();
    m_embeddings.clear();
    
    std::uniform_real_distribution<float> dist(-0.1f, 0.1f);
    
    for (size_t i = 0; i < words.size(); i++) {
        m_vocabulary[words[i]] = static_cast<int>(i);
        
        // Create random embedding for each word
        std::vector<float> embedding(EMBEDDING_DIM);
        for (int j = 0; j < EMBEDDING_DIM; j++) {
            embedding[j] = dist(m_rng);
        }
        m_embeddings.push_back(embedding);
    }
    
    // Add unknown token
    m_vocabulary[L"<UNK>"] = static_cast<int>(words.size());
    std::vector<float> unkEmbedding(EMBEDDING_DIM, 0.0f);
    m_embeddings.push_back(unkEmbedding);
}

void NeuralNetwork::InitializeWeights() {
    m_layers.clear();
    
    std::uniform_real_distribution<float> dist(-0.5f, 0.5f);
    
    // Input layer to hidden layer
    Layer hidden;
    hidden.weights.resize(HIDDEN_LAYER_SIZE);
    hidden.biases.resize(HIDDEN_LAYER_SIZE);
    hidden.activations.resize(HIDDEN_LAYER_SIZE);
    hidden.deltas.resize(HIDDEN_LAYER_SIZE);
    
    for (int i = 0; i < HIDDEN_LAYER_SIZE; i++) {
        hidden.weights[i].resize(EMBEDDING_DIM);
        for (int j = 0; j < EMBEDDING_DIM; j++) {
            hidden.weights[i][j] = dist(m_rng);
        }
        hidden.biases[i] = dist(m_rng);
    }
    m_layers.push_back(hidden);
    
    // Hidden layer to output layer
    Layer output;
    output.weights.resize(OUTPUT_SIZE);
    output.biases.resize(OUTPUT_SIZE);
    output.activations.resize(OUTPUT_SIZE);
    output.deltas.resize(OUTPUT_SIZE);
    
    for (int i = 0; i < OUTPUT_SIZE; i++) {
        output.weights[i].resize(HIDDEN_LAYER_SIZE);
        for (int j = 0; j < HIDDEN_LAYER_SIZE; j++) {
            output.weights[i][j] = dist(m_rng);
        }
        output.biases[i] = dist(m_rng);
    }
    m_layers.push_back(output);
}

void NeuralNetwork::InitializePatterns() {
    // Initialize intent keywords
    m_intentKeywords[L"window"] = Intent::CREATE_WINDOW;
    m_intentKeywords[L"dialog"] = Intent::CREATE_WINDOW;
    m_intentKeywords[L"frame"] = Intent::CREATE_WINDOW;
    m_intentKeywords[L"button"] = Intent::ADD_BUTTON;
    m_intentKeywords[L"label"] = Intent::ADD_LABEL;
    m_intentKeywords[L"textbox"] = Intent::ADD_TEXTBOX;
    m_intentKeywords[L"input"] = Intent::ADD_TEXTBOX;
    m_intentKeywords[L"checkbox"] = Intent::ADD_CHECKBOX;
    m_intentKeywords[L"progressbar"] = Intent::ADD_PROGRESSBAR;
    m_intentKeywords[L"progress"] = Intent::ADD_PROGRESSBAR;
    m_intentKeywords[L"tooltip"] = Intent::ADD_TOOLTIP;
    m_intentKeywords[L"callback"] = Intent::SET_CALLBACK;
    m_intentKeywords[L"handler"] = Intent::SET_CALLBACK;
    m_intentKeywords[L"theme"] = Intent::SET_THEME;
}

std::vector<std::wstring> NeuralNetwork::Tokenize(const std::wstring& text) {
    std::vector<std::wstring> tokens;
    std::wstring current;
    
    for (wchar_t ch : text) {
        if (std::isspace(ch) || ch == L',' || ch == L'.' || ch == L'!' || ch == L'?') {
            if (!current.empty()) {
                // Convert to lowercase
                std::transform(current.begin(), current.end(), current.begin(), ::towlower);
                tokens.push_back(current);
                current.clear();
            }
        } else if (ch == L'\'') {
            // Skip quotes
            continue;
        } else if (ch == L'x' && !current.empty() && std::all_of(current.begin(), current.end(), ::iswdigit)) {
            // Handle dimension patterns like "800x600"
            tokens.push_back(current);
            current = L"x";
        } else {
            current += ch;
        }
    }
    
    if (!current.empty()) {
        std::transform(current.begin(), current.end(), current.begin(), ::towlower);
        tokens.push_back(current);
    }
    
    return tokens;
}

std::vector<float> NeuralNetwork::TextToEmbedding(const std::wstring& text) {
    auto tokens = Tokenize(text);
    
    // Average word embeddings
    std::vector<float> embedding(EMBEDDING_DIM, 0.0f);
    int count = 0;
    
    for (const auto& token : tokens) {
        auto it = m_vocabulary.find(token);
        int idx = (it != m_vocabulary.end()) ? it->second : m_vocabulary[L"<UNK>"];
        
        for (int i = 0; i < EMBEDDING_DIM; i++) {
            embedding[i] += m_embeddings[idx][i];
        }
        count++;
    }
    
    // Average
    if (count > 0) {
        for (int i = 0; i < EMBEDDING_DIM; i++) {
            embedding[i] /= count;
        }
    }
    
    return embedding;
}

float NeuralNetwork::Sigmoid(float x) {
    return 1.0f / (1.0f + std::exp(-x));
}

float NeuralNetwork::SigmoidDerivative(float x) {
    float s = Sigmoid(x);
    return s * (1.0f - s);
}

float NeuralNetwork::ReLU(float x) {
    return std::max(0.0f, x);
}

float NeuralNetwork::ReLUDerivative(float x) {
    return x > 0.0f ? 1.0f : 0.0f;
}

std::vector<float> NeuralNetwork::Forward(const std::vector<float>& input) {
    // Input to hidden
    for (int i = 0; i < HIDDEN_LAYER_SIZE; i++) {
        float sum = m_layers[0].biases[i];
        for (size_t j = 0; j < input.size(); j++) {
            sum += m_layers[0].weights[i][j] * input[j];
        }
        m_layers[0].activations[i] = ReLU(sum);
    }
    
    // Hidden to output
    for (int i = 0; i < OUTPUT_SIZE; i++) {
        float sum = m_layers[1].biases[i];
        for (int j = 0; j < HIDDEN_LAYER_SIZE; j++) {
            sum += m_layers[1].weights[i][j] * m_layers[0].activations[j];
        }
        m_layers[1].activations[i] = Sigmoid(sum);
    }
    
    return m_layers[1].activations;
}

void NeuralNetwork::Backward(const std::vector<float>& target) {
    // Calculate output layer deltas
    for (int i = 0; i < OUTPUT_SIZE; i++) {
        float error = target[i] - m_layers[1].activations[i];
        m_layers[1].deltas[i] = error * SigmoidDerivative(m_layers[1].activations[i]);
    }
    
    // Calculate hidden layer deltas
    for (int i = 0; i < HIDDEN_LAYER_SIZE; i++) {
        float error = 0.0f;
        for (int j = 0; j < OUTPUT_SIZE; j++) {
            error += m_layers[1].deltas[j] * m_layers[1].weights[j][i];
        }
        m_layers[0].deltas[i] = error * ReLUDerivative(m_layers[0].activations[i]);
    }
    
    // Update output layer weights
    for (int i = 0; i < OUTPUT_SIZE; i++) {
        for (int j = 0; j < HIDDEN_LAYER_SIZE; j++) {
            m_layers[1].weights[i][j] += LEARNING_RATE * m_layers[1].deltas[i] * m_layers[0].activations[j];
        }
        m_layers[1].biases[i] += LEARNING_RATE * m_layers[1].deltas[i];
    }
    
    // Update hidden layer weights (would need input activations - simplified here)
}

NeuralNetwork::Intent NeuralNetwork::OutputToIntent(const std::vector<float>& output, float& confidence) {
    // Find max activation
    int maxIdx = 0;
    float maxVal = output[0];
    
    for (int i = 1; i < OUTPUT_SIZE; i++) {
        if (output[i] > maxVal) {
            maxVal = output[i];
            maxIdx = i;
        }
    }
    
    confidence = maxVal;
    
    // Map output index to intent
    switch (maxIdx) {
        case 0: return Intent::CREATE_WINDOW;
        case 1: return Intent::ADD_BUTTON;
        case 2: return Intent::ADD_LABEL;
        case 3: return Intent::ADD_TEXTBOX;
        case 4: return Intent::ADD_CHECKBOX;
        case 5: return Intent::ADD_PROGRESSBAR;
        case 6: return Intent::ADD_TOOLTIP;
        case 7: return Intent::SET_CALLBACK;
        case 8: return Intent::SET_THEME;
        default: return Intent::UNKNOWN;
    }
}

std::map<std::wstring, std::wstring> NeuralNetwork::ExtractEntities(const std::wstring& prompt) {
    std::map<std::wstring, std::wstring> entities;
    auto tokens = Tokenize(prompt);
    
    // Extract dimensions (e.g., "800x600" or "width 800 height 600")
    for (size_t i = 0; i < tokens.size(); i++) {
        const auto& token = tokens[i];
        
        // Check for "WIDTHxHEIGHT" pattern
        size_t xPos = token.find(L'x');
        if (xPos != std::wstring::npos) {
            std::wstring widthStr = token.substr(0, xPos);
            std::wstring heightStr = token.substr(xPos + 1);
            
            if (!widthStr.empty() && !heightStr.empty() &&
                std::all_of(widthStr.begin(), widthStr.end(), ::iswdigit) &&
                std::all_of(heightStr.begin(), heightStr.end(), ::iswdigit)) {
                entities[L"width"] = widthStr;
                entities[L"height"] = heightStr;
            }
        }
        
        // Check for "width 800" pattern
        if ((token == L"width" || token == L"w") && i + 1 < tokens.size()) {
            const auto& nextToken = tokens[i + 1];
            if (std::all_of(nextToken.begin(), nextToken.end(), ::iswdigit)) {
                entities[L"width"] = nextToken;
            }
        }
        
        // Check for "height 600" pattern
        if ((token == L"height" || token == L"h") && i + 1 < tokens.size()) {
            const auto& nextToken = tokens[i + 1];
            if (std::all_of(nextToken.begin(), nextToken.end(), ::iswdigit)) {
                entities[L"height"] = nextToken;
            }
        }
        
        // Extract text in quotes (titles, labels)
        if (i + 1 < tokens.size() && (token == L"title" || token == L"text" || token == L"label")) {
            // Look for the next non-keyword token as the title
            if (tokens[i + 1] != L"with" && tokens[i + 1] != L"and") {
                entities[L"text"] = tokens[i + 1];
            }
        }
        
        // Extract callback types
        if (token == L"onclick" || (token == L"on" && i + 1 < tokens.size() && tokens[i + 1] == L"click")) {
            entities[L"callback"] = L"click";
        } else if (token == L"onhover" || (token == L"on" && i + 1 < tokens.size() && tokens[i + 1] == L"hover")) {
            entities[L"callback"] = L"hover";
        } else if (token == L"onfocus" || (token == L"on" && i + 1 < tokens.size() && tokens[i + 1] == L"focus")) {
            entities[L"callback"] = L"focus";
        }
    }
    
    // Try to extract title from quoted strings in the original prompt
    size_t quoteStart = prompt.find(L'\'');
    if (quoteStart != std::wstring::npos) {
        size_t quoteEnd = prompt.find(L'\'', quoteStart + 1);
        if (quoteEnd != std::wstring::npos) {
            entities[L"title"] = prompt.substr(quoteStart + 1, quoteEnd - quoteStart - 1);
        }
    }
    
    return entities;
}

NeuralNetwork::ParsedPrompt NeuralNetwork::ParsePrompt(const std::wstring& prompt) {
    ParsedPrompt result;
    
    // Use keyword matching for primary intent detection (simple but effective)
    auto tokens = Tokenize(prompt);
    
    // Determine intent based on keywords
    result.intent = Intent::UNKNOWN;
    result.confidence = 0.5f;
    
    for (const auto& token : tokens) {
        auto it = m_intentKeywords.find(token);
        if (it != m_intentKeywords.end()) {
            result.intent = it->second;
            result.confidence = 0.9f;
            break;
        }
    }
    
    // If still unknown, use neural network
    if (result.intent == Intent::UNKNOWN) {
        auto embedding = TextToEmbedding(prompt);
        auto output = Forward(embedding);
        result.intent = OutputToIntent(output, result.confidence);
    }
    
    // Extract entities
    result.entities = ExtractEntities(prompt);
    
    return result;
}

void NeuralNetwork::Train(const std::vector<std::pair<std::wstring, ParsedPrompt>>& trainingData, int epochs) {
    // Simple training loop
    for (int epoch = 0; epoch < epochs; epoch++) {
        float totalLoss = 0.0f;
        
        for (const auto& sample : trainingData) {
            // Convert prompt to embedding
            auto embedding = TextToEmbedding(sample.first);
            
            // Create target vector (one-hot encoding)
            std::vector<float> target(OUTPUT_SIZE, 0.0f);
            int intentIdx = static_cast<int>(sample.second.intent);
            if (intentIdx >= 0 && intentIdx < OUTPUT_SIZE) {
                target[intentIdx] = 1.0f;
            }
            
            // Forward pass
            auto output = Forward(embedding);
            
            // Calculate loss
            for (int i = 0; i < OUTPUT_SIZE; i++) {
                float error = target[i] - output[i];
                totalLoss += error * error;
            }
            
            // Backward pass
            Backward(target);
        }
        
        // Optional: Print progress every 10 epochs
        // (commented out to avoid console output in production)
        // if (epoch % 10 == 0) {
        //     float avgLoss = totalLoss / trainingData.size();
        //     // printf("Epoch %d, Loss: %f\n", epoch, avgLoss);
        // }
    }
}

} // namespace SDK
