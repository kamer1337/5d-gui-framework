#include "../../include/SDK/NeuralNetwork.h"
#include <algorithm>
#include <sstream>
#include <cctype>
#include <locale>
#include <codecvt>

namespace SDK {

// Helper function to check if a wide string consists only of digits
static bool IsAllDigits(const std::wstring& str) {
    return !str.empty() && std::all_of(str.begin(), str.end(), [](wchar_t c) { return std::iswdigit(c); });
}

// ParsedPrompt convenience methods
int NeuralNetwork::ParsedPrompt::GetWidth() const {
    auto it = entities.find(L"width");
    if (it != entities.end()) {
        try {
            return static_cast<int>(std::stol(it->second));
        } catch (const std::exception&) {
            // If parsing fails, return default
        }
    }
    return 800;  // Default
}

int NeuralNetwork::ParsedPrompt::GetHeight() const {
    auto it = entities.find(L"height");
    if (it != entities.end()) {
        try {
            return static_cast<int>(std::stol(it->second));
        } catch (const std::exception&) {
            // If parsing fails, return default
        }
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
        } else if (it->second == L"doubleclick" || it->second == L"ondoubleclick") {
            return CallbackType::ON_DOUBLE_CLICK;
        } else if (it->second == L"hover") {
            return CallbackType::ON_HOVER;
        } else if (it->second == L"focus") {
            return CallbackType::ON_FOCUS;
        } else if (it->second == L"change" || it->second == L"onchange") {
            return CallbackType::ON_CHANGE;
        } else if (it->second == L"valuechanged") {
            return CallbackType::ON_VALUE_CHANGED;
        } else if (it->second == L"keypress") {
            return CallbackType::ON_KEY_PRESS;
        }
    }
    return CallbackType::ON_CLICK;  // Default
}

std::wstring NeuralNetwork::ParsedPrompt::GetWidgetType() const {
    auto it = entities.find(L"widget_type");
    if (it != entities.end()) {
        return it->second;
    }
    return L"";
}

int NeuralNetwork::ParsedPrompt::GetItemCount() const {
    auto it = entities.find(L"item_count");
    if (it != entities.end()) {
        try {
            return static_cast<int>(std::stol(it->second));
        } catch (const std::exception&) {
            // If parsing fails, return default
        }
    }
    return 0;
}

std::vector<std::wstring> NeuralNetwork::ParsedPrompt::GetItems() const {
    std::vector<std::wstring> items;
    auto it = entities.find(L"items");
    if (it != entities.end()) {
        // Parse comma-separated items
        std::wstring itemsStr = it->second;
        size_t start = 0;
        size_t end = itemsStr.find(L',');
        
        while (end != std::wstring::npos) {
            std::wstring item = itemsStr.substr(start, end - start);
            // Trim whitespace
            item.erase(0, item.find_first_not_of(L" \t"));
            item.erase(item.find_last_not_of(L" \t") + 1);
            if (!item.empty()) {
                items.push_back(item);
            }
            start = end + 1;
            end = itemsStr.find(L',', start);
        }
        // Add last item
        std::wstring lastItem = itemsStr.substr(start);
        lastItem.erase(0, lastItem.find_first_not_of(L" \t"));
        lastItem.erase(lastItem.find_last_not_of(L" \t") + 1);
        if (!lastItem.empty()) {
            items.push_back(lastItem);
        }
    }
    return items;
}

// Enhanced getters for v1.2.2
std::wstring NeuralNetwork::ParsedPrompt::GetColor() const {
    auto it = entities.find(L"color");
    if (it != entities.end()) {
        return it->second;
    }
    return L"";
}

std::wstring NeuralNetwork::ParsedPrompt::GetColorModifier() const {
    auto it = entities.find(L"color_modifier");
    if (it != entities.end()) {
        return it->second;
    }
    return L"";
}

std::wstring NeuralNetwork::ParsedPrompt::GetSize() const {
    auto it = entities.find(L"size");
    if (it != entities.end()) {
        return it->second;
    }
    return L"medium";  // Default
}

std::wstring NeuralNetwork::ParsedPrompt::GetState() const {
    auto it = entities.find(L"state");
    if (it != entities.end()) {
        return it->second;
    }
    return L"enabled";  // Default
}

std::wstring NeuralNetwork::ParsedPrompt::GetAction() const {
    auto it = entities.find(L"action");
    if (it != entities.end()) {
        return it->second;
    }
    return L"";
}

std::wstring NeuralNetwork::ParsedPrompt::GetFontStyle() const {
    auto it = entities.find(L"font_style");
    if (it != entities.end()) {
        return it->second;
    }
    return L"";
}

std::wstring NeuralNetwork::ParsedPrompt::GetPattern() const {
    auto it = entities.find(L"pattern");
    if (it != entities.end()) {
        return it->second;
    }
    return L"";
}

std::wstring NeuralNetwork::ParsedPrompt::GetValidation() const {
    auto it = entities.find(L"validation");
    if (it != entities.end()) {
        return it->second;
    }
    return L"";
}

std::wstring NeuralNetwork::ParsedPrompt::GetPlaceholder() const {
    auto it = entities.find(L"placeholder");
    if (it != entities.end()) {
        return it->second;
    }
    return L"";
}

int NeuralNetwork::ParsedPrompt::GetMinValue() const {
    auto it = entities.find(L"min_value");
    if (it != entities.end()) {
        try {
            return static_cast<int>(std::stol(it->second));
        } catch (const std::exception&) {
            // If parsing fails, return default
        }
    }
    return 0;  // Default
}

int NeuralNetwork::ParsedPrompt::GetMaxValue() const {
    auto it = entities.find(L"max_value");
    if (it != entities.end()) {
        try {
            return static_cast<int>(std::stol(it->second));
        } catch (const std::exception&) {
            // If parsing fails, return default
        }
    }
    return 100;  // Default
}

int NeuralNetwork::ParsedPrompt::GetCount() const {
    auto it = entities.find(L"count");
    if (it != entities.end()) {
        try {
            return static_cast<int>(std::stol(it->second));
        } catch (const std::exception&) {
            // If parsing fails, return default
        }
    }
    return 1;  // Default
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
        // Window/container creation
        L"create", L"make", L"add", L"new", L"build", L"open", L"spawn", L"generate", L"construct",
        L"window", L"dialog", L"frame", L"form", L"panel", L"container", L"screen", L"page", L"view",
        
        // Basic widgets
        L"button", L"label", L"text", L"textbox", L"input", L"field", L"entry",
        L"checkbox", L"check", L"box", L"radio", L"radiobutton", L"option",
        
        // Advanced widgets
        L"progressbar", L"progress", L"bar", L"indicator", L"gauge",
        L"slider", L"scroll", L"scrollbar", L"trackbar", L"range",
        L"tooltip", L"tip", L"hint", L"popup", L"balloon", L"helptext",
        L"combobox", L"combo", L"dropdown", L"select", L"menu", L"picker",
        L"listbox", L"list", L"listview", L"tree", L"treeview", L"items",
        L"tabcontrol", L"tab", L"tabs", L"notebook", L"pages",
        L"toolbar", L"statusbar", L"menubar", L"ribbon",
        L"spinbox", L"spin", L"numeric", L"updown", L"stepper", L"counter",
        L"image", L"picture", L"icon", L"bitmap", L"graphic", L"photo",
        L"separator", L"divider", L"line", L"spacer", L"hr",
        
        // Layout and positioning
        L"left", L"right", L"top", L"bottom", L"center", L"middle", L"centered",
        L"above", L"below", L"next", L"beside", L"adjacent", L"near",
        L"horizontal", L"vertical", L"align", L"position", L"place", L"put",
        L"layout", L"grid", L"flow", L"stack", L"arrange", L"organized",
        L"row", L"column", L"table", L"matrix",
        L"anchor", L"dock", L"pin", L"attach", L"fixed", L"absolute", L"relative",
        
        // Events and callbacks
        L"click", L"hover", L"focus", L"blur", L"press", L"release", L"tap",
        L"doubleclick", L"rightclick", L"contextmenu",
        L"callback", L"handler", L"event", L"action", L"trigger", L"listener",
        L"on", L"when", L"if", L"then", L"after", L"before", L"during",
        L"change", L"update", L"modify", L"select", L"choose", L"pick",
        L"keypress", L"keydown", L"keyup", L"keyboard",
        L"mousedown", L"mouseup", L"mousemove", L"mouse",
        L"drag", L"drop", L"dragdrop", L"move",
        
        // Properties
        L"width", L"height", L"size", L"dimension", L"bounds", L"rect",
        L"title", L"name", L"caption", L"heading", L"label", L"description",
        L"theme", L"style", L"color", L"appearance", L"look", L"design",
        L"font", L"bold", L"italic", L"underline", L"typeface", L"family",
        L"visible", L"hidden", L"enabled", L"disabled", L"readonly", L"editable",
        L"border", L"padding", L"margin", L"spacing", L"gap",
        L"background", L"foreground", L"opacity", L"alpha", L"transparent",
        L"radius", L"rounded", L"corner", L"edge", L"smooth",
        
        // Colors
        L"red", L"green", L"blue", L"yellow", L"orange", L"purple", L"pink",
        L"black", L"white", L"gray", L"grey", L"brown",
        L"dark", L"light", L"bright", L"pale", L"vivid",
        
        // Actions
        L"set", L"apply", L"configure", L"setup", L"initialize", L"init",
        L"show", L"display", L"render", L"draw", L"paint", L"present",
        L"hide", L"close", L"minimize", L"maximize", L"restore", L"collapse", L"expand",
        L"update", L"refresh", L"redraw", L"repaint", L"reload",
        L"validate", L"verify", L"check", L"confirm",
        
        // Common terms
        L"with", L"and", L"at", L"to", L"for", L"in", L"from", L"by", L"of",
        L"the", L"a", L"an", L"is", L"are", L"be", L"have", L"has", L"was", L"were",
        L"called", L"named", L"titled", L"labeled", L"containing", L"having",
        L"multiple", L"several", L"many", L"few", L"some", L"all", L"each", L"every",
        
        // Values and units
        L"x", L"pixels", L"px", L"percent", L"em", L"pt", L"dp",
        L"true", L"false", L"yes", L"no", L"on", L"off",
        L"small", L"medium", L"large", L"tiny", L"huge",
        
        // GUI-specific terms
        L"file", L"edit", L"view", L"help", L"tools", L"options", L"window",
        L"ok", L"cancel", L"apply", L"save", L"load", L"open", L"close",
        L"submit", L"reset", L"clear", L"search", L"find", L"filter",
        L"username", L"password", L"email", L"login", L"logout", L"signin", L"signup",
        L"settings", L"preferences", L"configuration", L"properties", L"options",
        L"home", L"back", L"forward", L"next", L"previous", L"first", L"last",
        L"add", L"remove", L"delete", L"edit", L"modify", L"copy", L"paste", L"cut",
        L"undo", L"redo", L"refresh", L"reload",
        
        // Form and data entry
        L"form", L"data", L"entry", L"record", L"item", L"value",
        L"name", L"email", L"address", L"phone", L"number", L"date", L"time",
        L"required", L"optional", L"mandatory", L"placeholder",
        
        // UI patterns
        L"navigation", L"nav", L"header", L"footer", L"sidebar", L"main",
        L"content", L"body", L"section", L"article", L"card", L"modal",
        L"wizard", L"stepper", L"breadcrumb", L"pagination",
        L"accordion", L"carousel", L"slider", L"gallery",
        
        // State and status
        L"loading", L"loaded", L"saving", L"saved", L"error", L"success",
        L"warning", L"info", L"active", L"inactive", L"selected", L"unselected",
        L"checked", L"unchecked", L"expanded", L"collapsed",
        L"valid", L"invalid", L"empty", L"filled",
        
        // Neural network enhancements
        L"neural", L"network", L"ai", L"intelligent", L"smart",
        L"learn", L"train", L"predict", L"generate", L"auto", L"automatic",
        L"suggest", L"recommend", L"assist", L"guide"
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
    // Initialize intent keywords with enhanced widget support
    m_intentKeywords[L"window"] = Intent::CREATE_WINDOW;
    m_intentKeywords[L"dialog"] = Intent::CREATE_WINDOW;
    m_intentKeywords[L"frame"] = Intent::CREATE_WINDOW;
    m_intentKeywords[L"form"] = Intent::CREATE_WINDOW;
    m_intentKeywords[L"screen"] = Intent::CREATE_WINDOW;
    m_intentKeywords[L"page"] = Intent::CREATE_WINDOW;
    
    m_intentKeywords[L"button"] = Intent::ADD_BUTTON;
    m_intentKeywords[L"btn"] = Intent::ADD_BUTTON;
    
    m_intentKeywords[L"label"] = Intent::ADD_LABEL;
    m_intentKeywords[L"caption"] = Intent::ADD_LABEL;
    m_intentKeywords[L"heading"] = Intent::ADD_LABEL;
    
    m_intentKeywords[L"textbox"] = Intent::ADD_TEXTBOX;
    m_intentKeywords[L"input"] = Intent::ADD_TEXTBOX;
    m_intentKeywords[L"field"] = Intent::ADD_TEXTBOX;
    m_intentKeywords[L"text"] = Intent::ADD_TEXTBOX;
    m_intentKeywords[L"entry"] = Intent::ADD_TEXTBOX;
    
    m_intentKeywords[L"checkbox"] = Intent::ADD_CHECKBOX;
    m_intentKeywords[L"check"] = Intent::ADD_CHECKBOX;
    
    m_intentKeywords[L"progressbar"] = Intent::ADD_PROGRESSBAR;
    m_intentKeywords[L"progress"] = Intent::ADD_PROGRESSBAR;
    m_intentKeywords[L"indicator"] = Intent::ADD_PROGRESSBAR;
    m_intentKeywords[L"gauge"] = Intent::ADD_PROGRESSBAR;
    
    m_intentKeywords[L"tooltip"] = Intent::ADD_TOOLTIP;
    m_intentKeywords[L"tip"] = Intent::ADD_TOOLTIP;
    m_intentKeywords[L"hint"] = Intent::ADD_TOOLTIP;
    m_intentKeywords[L"helptext"] = Intent::ADD_TOOLTIP;
    
    // Advanced widgets
    m_intentKeywords[L"slider"] = Intent::ADD_SLIDER;
    m_intentKeywords[L"trackbar"] = Intent::ADD_SLIDER;
    m_intentKeywords[L"range"] = Intent::ADD_SLIDER;
    
    m_intentKeywords[L"combobox"] = Intent::ADD_COMBOBOX;
    m_intentKeywords[L"combo"] = Intent::ADD_COMBOBOX;
    m_intentKeywords[L"dropdown"] = Intent::ADD_COMBOBOX;
    m_intentKeywords[L"picker"] = Intent::ADD_COMBOBOX;
    
    m_intentKeywords[L"listbox"] = Intent::ADD_LISTBOX;
    m_intentKeywords[L"list"] = Intent::ADD_LISTBOX;
    
    m_intentKeywords[L"listview"] = Intent::ADD_LISTVIEW;
    
    m_intentKeywords[L"radiobutton"] = Intent::ADD_RADIOBUTTON;
    m_intentKeywords[L"radio"] = Intent::ADD_RADIOBUTTON;
    m_intentKeywords[L"option"] = Intent::ADD_RADIOBUTTON;
    
    m_intentKeywords[L"spinbox"] = Intent::ADD_SPINBOX;
    m_intentKeywords[L"spin"] = Intent::ADD_SPINBOX;
    m_intentKeywords[L"stepper"] = Intent::ADD_SPINBOX;
    m_intentKeywords[L"counter"] = Intent::ADD_SPINBOX;
    
    m_intentKeywords[L"image"] = Intent::ADD_IMAGE;
    m_intentKeywords[L"picture"] = Intent::ADD_IMAGE;
    m_intentKeywords[L"photo"] = Intent::ADD_IMAGE;
    m_intentKeywords[L"graphic"] = Intent::ADD_IMAGE;
    
    m_intentKeywords[L"separator"] = Intent::ADD_SEPARATOR;
    m_intentKeywords[L"divider"] = Intent::ADD_SEPARATOR;
    m_intentKeywords[L"hr"] = Intent::ADD_SEPARATOR;
    
    m_intentKeywords[L"panel"] = Intent::ADD_PANEL;
    m_intentKeywords[L"container"] = Intent::ADD_PANEL;
    m_intentKeywords[L"section"] = Intent::ADD_PANEL;
    
    m_intentKeywords[L"tabcontrol"] = Intent::ADD_TABCONTROL;
    m_intentKeywords[L"tabs"] = Intent::ADD_TABCONTROL;
    m_intentKeywords[L"notebook"] = Intent::ADD_TABCONTROL;
    
    m_intentKeywords[L"toolbar"] = Intent::ADD_TOOLBAR;
    m_intentKeywords[L"menubar"] = Intent::ADD_TOOLBAR;
    
    m_intentKeywords[L"callback"] = Intent::SET_CALLBACK;
    m_intentKeywords[L"handler"] = Intent::SET_CALLBACK;
    m_intentKeywords[L"event"] = Intent::SET_CALLBACK;
    m_intentKeywords[L"listener"] = Intent::SET_CALLBACK;
    
    m_intentKeywords[L"theme"] = Intent::SET_THEME;
    m_intentKeywords[L"style"] = Intent::SET_THEME;
    m_intentKeywords[L"appearance"] = Intent::SET_THEME;
    
    // Layout keywords
    m_intentKeywords[L"layout"] = Intent::SET_LAYOUT;
    m_intentKeywords[L"arrange"] = Intent::SET_LAYOUT;
    m_intentKeywords[L"organize"] = Intent::SET_LAYOUT;
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
        } else if (ch == L'x' && !current.empty() && IsAllDigits(current)) {
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
    
    for (size_t i = 1; i < output.size() && i < OUTPUT_SIZE; i++) {
        if (output[i] > maxVal) {
            maxVal = output[i];
            maxIdx = static_cast<int>(i);
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
        case 7: return Intent::ADD_SLIDER;
        case 8: return Intent::ADD_COMBOBOX;
        case 9: return Intent::ADD_LISTBOX;
        case 10: return Intent::ADD_LISTVIEW;
        case 11: return Intent::ADD_RADIOBUTTON;
        case 12: return Intent::ADD_SPINBOX;
        case 13: return Intent::ADD_IMAGE;
        case 14: return Intent::ADD_SEPARATOR;
        case 15: return Intent::ADD_PANEL;
        case 16: return Intent::ADD_TABCONTROL;
        case 17: return Intent::ADD_TOOLBAR;
        case 18: return Intent::SET_CALLBACK;
        case 19: return Intent::SET_THEME;
        case 20: return Intent::SET_LAYOUT;
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
                IsAllDigits(widthStr) &&
                IsAllDigits(heightStr)) {
                entities[L"width"] = widthStr;
                entities[L"height"] = heightStr;
            }
        }
        
        // Check for "width 800" pattern
        if ((token == L"width" || token == L"w") && i + 1 < tokens.size()) {
            const auto& nextToken = tokens[i + 1];
            if (IsAllDigits(nextToken)) {
                entities[L"width"] = nextToken;
            }
        }
        
        // Check for "height 600" pattern
        if ((token == L"height" || token == L"h") && i + 1 < tokens.size()) {
            const auto& nextToken = tokens[i + 1];
            if (IsAllDigits(nextToken)) {
                entities[L"height"] = nextToken;
            }
        }
        
        // Extract position keywords (left, right, top, bottom, center)
        if (token == L"left" || token == L"right" || token == L"top" || 
            token == L"bottom" || token == L"center" || token == L"middle" || token == L"centered") {
            entities[L"position"] = token;
        }
        
        // Extract alignment (horizontal, vertical)
        if (token == L"horizontal" || token == L"vertical") {
            entities[L"alignment"] = token;
        }
        
        // Extract text in quotes (titles, labels)
        if (i + 1 < tokens.size() && (token == L"title" || token == L"text" || 
            token == L"label" || token == L"named" || token == L"called")) {
            // Look for the next non-keyword token as the title
            if (tokens[i + 1] != L"with" && tokens[i + 1] != L"and") {
                entities[L"text"] = tokens[i + 1];
            }
        }
        
        // Extract visibility states
        if (token == L"visible" || token == L"hidden" || token == L"enabled" || token == L"disabled" ||
            token == L"readonly" || token == L"editable") {
            entities[L"state"] = token;
        }
        
        // Extract size keywords
        if (token == L"small" || token == L"medium" || token == L"large" || 
            token == L"tiny" || token == L"huge") {
            entities[L"size"] = token;
        }
        
        // Extract color keywords
        if (token == L"red" || token == L"green" || token == L"blue" || token == L"yellow" ||
            token == L"orange" || token == L"purple" || token == L"pink" || token == L"black" ||
            token == L"white" || token == L"gray" || token == L"grey" || token == L"brown") {
            entities[L"color"] = token;
        }
        
        // Extract color modifiers
        if (token == L"dark" || token == L"light" || token == L"bright" || token == L"pale") {
            entities[L"color_modifier"] = token;
        }
        
        // Extract callback types
        if (token == L"onclick" || (token == L"on" && i + 1 < tokens.size() && tokens[i + 1] == L"click")) {
            entities[L"callback"] = L"click";
        } else if (token == L"ondoubleclick" || token == L"doubleclick" ||
                   (token == L"on" && i + 2 < tokens.size() && tokens[i + 1] == L"double" && tokens[i + 2] == L"click")) {
            entities[L"callback"] = L"doubleclick";
        } else if (token == L"onhover" || (token == L"on" && i + 1 < tokens.size() && tokens[i + 1] == L"hover")) {
            entities[L"callback"] = L"hover";
        } else if (token == L"onfocus" || (token == L"on" && i + 1 < tokens.size() && tokens[i + 1] == L"focus")) {
            entities[L"callback"] = L"focus";
        } else if (token == L"onchange" || (token == L"on" && i + 1 < tokens.size() && tokens[i + 1] == L"change")) {
            entities[L"callback"] = L"change";
        } else if (token == L"onkeypress" || token == L"keypress" ||
                   (token == L"on" && i + 1 < tokens.size() && tokens[i + 1] == L"keypress")) {
            entities[L"callback"] = L"keypress";
        }
        
        // Extract common actions
        if (token == L"submit" || token == L"cancel" || token == L"apply" || 
            token == L"save" || token == L"load" || token == L"close" ||
            token == L"open" || token == L"delete" || token == L"remove" ||
            token == L"add" || token == L"edit" || token == L"search" || token == L"find") {
            entities[L"action"] = token;
        }
        
        // Extract font styles
        if (token == L"bold" || token == L"italic" || token == L"underline") {
            entities[L"font_style"] = token;
        }
        
        // Extract item count (e.g., "3 buttons", "5 items")
        if (IsAllDigits(token) && i + 1 < tokens.size()) {
            const auto& nextToken = tokens[i + 1];
            if (nextToken == L"buttons" || nextToken == L"labels" || nextToken == L"textboxes" ||
                nextToken == L"items" || nextToken == L"widgets" || nextToken == L"controls") {
                entities[L"count"] = token;
            }
        }
        
        // Extract form-related patterns
        if (token == L"form" || token == L"login" || token == L"signup" || token == L"registration") {
            entities[L"pattern"] = token;
        }
        
        // Extract required/optional modifiers
        if (token == L"required" || token == L"optional" || token == L"mandatory") {
            entities[L"validation"] = token;
        }
        
        // Extract placeholder detection
        if (token == L"placeholder" && i + 1 < tokens.size()) {
            entities[L"placeholder"] = tokens[i + 1];
        }
        
        // Extract range values (e.g., "from 0 to 100")
        if ((token == L"from" || token == L"min") && i + 1 < tokens.size() &&
            IsAllDigits(tokens[i + 1])) {
            entities[L"min_value"] = tokens[i + 1];
        }
        if ((token == L"to" || token == L"max") && i + 1 < tokens.size() &&
            IsAllDigits(tokens[i + 1])) {
            entities[L"max_value"] = tokens[i + 1];
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
    
    // Also try double quotes
    if (entities.find(L"title") == entities.end()) {
        quoteStart = prompt.find(L'"');
        if (quoteStart != std::wstring::npos) {
            size_t quoteEnd = prompt.find(L'"', quoteStart + 1);
            if (quoteEnd != std::wstring::npos) {
                entities[L"title"] = prompt.substr(quoteStart + 1, quoteEnd - quoteStart - 1);
            }
        }
    }
    
    // Detect contextual patterns that imply certain widgets
    std::wstring lowerPrompt = prompt;
    std::transform(lowerPrompt.begin(), lowerPrompt.end(), lowerPrompt.begin(), ::towlower);
    
    // Login form pattern detection
    if (lowerPrompt.find(L"login") != std::wstring::npos || 
        lowerPrompt.find(L"signin") != std::wstring::npos) {
        entities[L"pattern"] = L"login";
        if (entities.find(L"title") == entities.end()) {
            entities[L"title"] = L"Login";
        }
    }
    
    // Registration/signup form pattern
    if (lowerPrompt.find(L"signup") != std::wstring::npos || 
        lowerPrompt.find(L"registration") != std::wstring::npos ||
        lowerPrompt.find(L"register") != std::wstring::npos) {
        entities[L"pattern"] = L"signup";
        if (entities.find(L"title") == entities.end()) {
            entities[L"title"] = L"Sign Up";
        }
    }
    
    // Settings/preferences pattern
    if (lowerPrompt.find(L"settings") != std::wstring::npos || 
        lowerPrompt.find(L"preferences") != std::wstring::npos) {
        entities[L"pattern"] = L"settings";
        if (entities.find(L"title") == entities.end()) {
            entities[L"title"] = L"Settings";
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
    result.layoutType = LayoutType::NONE;
    
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
    
    // Extract multiple widgets from prompt
    result.additionalWidgets = ExtractMultipleWidgets(prompt);
    
    // Determine layout type
    result.layoutType = DetermineLayout(prompt);
    
    return result;
}

std::vector<NeuralNetwork::Intent> NeuralNetwork::ExtractMultipleWidgets(const std::wstring& prompt) {
    std::vector<Intent> widgets;
    auto tokens = Tokenize(prompt);
    
    // Look for multiple widget mentions in the prompt
    for (const auto& token : tokens) {
        auto it = m_intentKeywords.find(token);
        if (it != m_intentKeywords.end()) {
            // Only add widget-related intents
            if (it->second != Intent::CREATE_WINDOW && 
                it->second != Intent::SET_CALLBACK && 
                it->second != Intent::SET_THEME &&
                it->second != Intent::SET_LAYOUT) {
                widgets.push_back(it->second);
            }
        }
    }
    
    return widgets;
}

NeuralNetwork::LayoutType NeuralNetwork::DetermineLayout(const std::wstring& prompt) {
    auto lowerPrompt = prompt;
    std::transform(lowerPrompt.begin(), lowerPrompt.end(), lowerPrompt.begin(), ::towlower);
    
    // Explicit layout keywords take highest priority
    if (lowerPrompt.find(L"vertical layout") != std::wstring::npos ||
        lowerPrompt.find(L"vertically") != std::wstring::npos ||
        lowerPrompt.find(L"stacked") != std::wstring::npos ||
        lowerPrompt.find(L"stack") != std::wstring::npos ||
        lowerPrompt.find(L"column layout") != std::wstring::npos) {
        return LayoutType::VERTICAL;
    }
    
    if (lowerPrompt.find(L"horizontal layout") != std::wstring::npos ||
        lowerPrompt.find(L"horizontally") != std::wstring::npos ||
        lowerPrompt.find(L"row layout") != std::wstring::npos ||
        lowerPrompt.find(L"side by side") != std::wstring::npos ||
        lowerPrompt.find(L"in a row") != std::wstring::npos) {
        return LayoutType::HORIZONTAL;
    }
    
    if (lowerPrompt.find(L"grid layout") != std::wstring::npos ||
        lowerPrompt.find(L"in a grid") != std::wstring::npos ||
        lowerPrompt.find(L"table layout") != std::wstring::npos ||
        lowerPrompt.find(L"matrix") != std::wstring::npos) {
        return LayoutType::GRID;
    }
    
    if (lowerPrompt.find(L"absolute") != std::wstring::npos ||
        lowerPrompt.find(L"fixed position") != std::wstring::npos ||
        lowerPrompt.find(L"precise position") != std::wstring::npos) {
        return LayoutType::ABSOLUTE;
    }
    
    // Contextual layout detection based on positional words
    int verticalIndicators = 0;
    int horizontalIndicators = 0;
    
    // Count vertical indicators
    if (lowerPrompt.find(L"above") != std::wstring::npos) verticalIndicators++;
    if (lowerPrompt.find(L"below") != std::wstring::npos) verticalIndicators++;
    if (lowerPrompt.find(L"beneath") != std::wstring::npos) verticalIndicators++;
    if (lowerPrompt.find(L"under") != std::wstring::npos) verticalIndicators++;
    if (lowerPrompt.find(L"over") != std::wstring::npos) verticalIndicators++;
    if (lowerPrompt.find(L"top") != std::wstring::npos) verticalIndicators++;
    if (lowerPrompt.find(L"bottom") != std::wstring::npos) verticalIndicators++;
    
    // Count horizontal indicators
    if (lowerPrompt.find(L"beside") != std::wstring::npos) horizontalIndicators++;
    if (lowerPrompt.find(L"next to") != std::wstring::npos) horizontalIndicators++;
    if (lowerPrompt.find(L"adjacent") != std::wstring::npos) horizontalIndicators++;
    if (lowerPrompt.find(L"left") != std::wstring::npos) horizontalIndicators++;
    if (lowerPrompt.find(L"right") != std::wstring::npos) horizontalIndicators++;
    
    // Determine layout based on indicator counts
    if (verticalIndicators > horizontalIndicators && verticalIndicators >= 2) {
        return LayoutType::VERTICAL;
    }
    
    if (horizontalIndicators > verticalIndicators && horizontalIndicators >= 2) {
        return LayoutType::HORIZONTAL;
    }
    
    // Form-specific layout inference
    if (lowerPrompt.find(L"form") != std::wstring::npos ||
        lowerPrompt.find(L"login") != std::wstring::npos ||
        lowerPrompt.find(L"signup") != std::wstring::npos ||
        lowerPrompt.find(L"registration") != std::wstring::npos) {
        // Forms typically use vertical layout
        return LayoutType::VERTICAL;
    }
    
    // Toolbar-specific layout inference
    if (lowerPrompt.find(L"toolbar") != std::wstring::npos ||
        lowerPrompt.find(L"navigation") != std::wstring::npos ||
        lowerPrompt.find(L"navbar") != std::wstring::npos) {
        // Toolbars typically use horizontal layout
        return LayoutType::HORIZONTAL;
    }
    
    // Multiple items of same type might suggest grid
    if (lowerPrompt.find(L"buttons") != std::wstring::npos ||
        lowerPrompt.find(L"cards") != std::wstring::npos ||
        lowerPrompt.find(L"items") != std::wstring::npos) {
        // Check for number indicators suggesting grid
        auto tokens = Tokenize(prompt);
        for (const auto& token : tokens) {
            if (IsAllDigits(token)) {
                try {
                    int count = static_cast<int>(std::stol(token));
                    // If count is 6, 9, 12, or other square-ish numbers, suggest grid
                    if (count >= 6 && (count % 3 == 0 || count % 4 == 0)) {
                        return LayoutType::GRID;
                    }
                } catch (const std::exception&) {
                    // If conversion fails, skip this token
                    continue;
                }
            }
        }
    }
    
    return LayoutType::NONE;
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
