# Neural Network GUI Generation Guide

This guide explains how to use the neural network-powered GUI creation system in the 5D GUI SDK.

## Overview

The SDK includes a lightweight neural network implementation (zero external dependencies) that can interpret natural language prompts to create GUI windows, add widgets, and set up callbacks. The network is pre-trained on the project documentation and understands common GUI creation patterns.

## Features

- **Natural Language Understanding**: Parse English prompts to understand GUI creation intent
- **Intent Classification**: Recognizes 9+ different intents (create window, add button, set callback, etc.)
- **Entity Extraction**: Automatically extracts dimensions, titles, widget types, and callback types
- **Callback Generation**: Creates event handler functions from prompt descriptions
- **Zero Dependencies**: Pure C++ implementation with no external ML libraries
- **Extensible**: Can be retrained with custom data for domain-specific applications

## Quick Start

### Basic Usage

```cpp
#include "SDK/SDK.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    // Initialize SDK
    SDK::Initialize();
    
    // Create neural prompt builder
    SDK::NeuralPromptBuilder builder;
    
    // Create a window from natural language prompt
    HWND hwnd = builder.BuildFromPrompt(
        L"Create a window 800x600 called 'My App' with a button",
        hInstance
    );
    
    // Show window
    ShowWindow(hwnd, nCmdShow);
    
    // Message loop...
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    SDK::Shutdown();
    return 0;
}
```

## Natural Language Prompt Examples

### Window Creation

```cpp
// Simple window
builder.BuildFromPrompt(L"Create a window 800x600", hInstance);

// Window with title
builder.BuildFromPrompt(L"Make a window called 'Settings'", hInstance);

// Window with dimensions and title
builder.BuildFromPrompt(L"Create window 1024x768 'My Application'", hInstance);

// Dialog window
builder.BuildFromPrompt(L"Create a dialog 400x300", hInstance);
```

### Adding Widgets

```cpp
// Add a button
builder.BuildFromPrompt(L"Add a button with text 'Submit'", hInstance);

// Add multiple widgets
builder.BuildFromPrompt(L"Create window with label, textbox, and button", hInstance);

// Add specific widget types
builder.BuildFromPrompt(L"Add a progress bar", hInstance);
builder.BuildFromPrompt(L"Add a checkbox for agree to terms", hInstance);
builder.BuildFromPrompt(L"Add a tooltip that says 'Help'", hInstance);
```

### Callbacks and Event Handlers

```cpp
// Button with click callback
auto callback = builder.GenerateCallback(
    L"Show a message when button is clicked"
);

// Get parsed callback information
auto callbackSpec = builder.ParseCallbackPrompt(
    L"Create a button 'Close' that closes the window on click"
);

// The neural network understands:
// - Click events: "on click", "when clicked"
// - Hover events: "on hover", "when mouse over"
// - Focus events: "on focus", "when focused"
```

## Neural Network API

### NeuralNetwork Class

The core neural network for natural language understanding.

#### Initialization

```cpp
auto nn = std::make_shared<SDK::NeuralNetwork>();
nn->Initialize();
```

#### Parsing Prompts

```cpp
auto parsed = nn->ParsePrompt(L"Create a window with a button");

// Access results
switch (parsed.intent) {
    case SDK::NeuralNetwork::Intent::CREATE_WINDOW:
        // Handle window creation
        break;
    case SDK::NeuralNetwork::Intent::ADD_BUTTON:
        // Handle button addition
        break;
    // ... other intents
}

// Get confidence score (0.0 to 1.0)
float confidence = parsed.confidence;

// Access extracted entities
int width = parsed.GetWidth();
int height = parsed.GetHeight();
std::wstring title = parsed.GetTitle();
std::wstring text = parsed.GetWidgetText();
```

#### Intent Types

The network recognizes the following intents:

- `CREATE_WINDOW` - Creating a new window or dialog
- `ADD_BUTTON` - Adding a button widget
- `ADD_LABEL` - Adding a label widget
- `ADD_TEXTBOX` - Adding a text input widget
- `ADD_CHECKBOX` - Adding a checkbox widget
- `ADD_PROGRESSBAR` - Adding a progress bar widget
- `ADD_TOOLTIP` - Adding a tooltip widget
- `SET_CALLBACK` - Setting up event handlers
- `SET_THEME` - Applying themes
- `UNKNOWN` - Unrecognized intent

#### Callback Types

For event handlers, the network understands:

- `ON_CLICK` - Click events
- `ON_HOVER` - Hover/mouse enter events
- `ON_FOCUS` - Focus events
- `ON_BLUR` - Blur/focus lost events
- `CUSTOM` - Custom event types

### NeuralPromptBuilder Class

High-level interface that extends PromptWindowBuilder with neural network capabilities.

#### Creating Windows

```cpp
SDK::NeuralPromptBuilder builder;

// Build complete window with widgets
HWND hwnd = builder.BuildFromPrompt(
    L"Create a login window with username textbox, password textbox, and login button",
    hInstance
);
```

#### Parsing Prompts

```cpp
// Parse without creating window
auto spec = builder.ParsePrompt(L"Window 640x480 'Settings'");

// Access window specification
std::wstring title = spec.title;
int width = spec.width;
int height = spec.height;
std::vector<std::wstring> widgets = spec.widgets;
```

#### Generating Callbacks

```cpp
// Generate callback from description
auto callback = builder.GenerateCallback(
    L"Show a tooltip on hover"
);

// Apply to widget
button->SetEventCallback(callback);

// Parse callback specifications
auto callbackSpec = builder.ParseCallbackPrompt(
    L"Button 'Submit' that validates form on click"
);

std::wstring widgetText = callbackSpec.widgetText;  // "Submit"
auto type = callbackSpec.type;  // ON_CLICK
std::wstring action = callbackSpec.action;  // "validates form"
```

#### Training Custom Data

```cpp
// Prepare training data
std::vector<std::pair<std::wstring, SDK::NeuralNetwork::ParsedPrompt>> trainingData;

SDK::NeuralNetwork::ParsedPrompt sample;
sample.intent = SDK::NeuralNetwork::Intent::ADD_BUTTON;
sample.confidence = 1.0f;
sample.entities[L"text"] = L"Custom Button";

trainingData.push_back({L"Add custom button", sample});

// Train the network
builder.TrainOnData(trainingData);
```

## Advanced Usage

### Direct Neural Network Access

```cpp
SDK::NeuralPromptBuilder builder;
auto nn = builder.GetNeuralNetwork();

// Get vocabulary size
size_t vocabSize = nn->GetVocabularySize();

// Parse multiple prompts
std::vector<std::wstring> prompts = {
    L"Create window with progress bar",
    L"Add checkbox",
    L"Set dark theme"
};

for (const auto& prompt : prompts) {
    auto parsed = nn->ParsePrompt(prompt);
    // Process results...
}
```

### Custom Widget Factories

```cpp
SDK::NeuralPromptBuilder builder;

// Register custom widget factory
builder.RegisterWidgetFactory(L"customwidget", [](const std::wstring& params) {
    return std::make_shared<MyCustomWidget>();
});

// Now the neural network can create custom widgets
builder.BuildFromPrompt(L"Add a customwidget", hInstance);
```

### Combining with Existing SDK Features

```cpp
SDK::NeuralPromptBuilder builder;

// Create window with neural network
HWND hwnd = builder.BuildFromPrompt(
    L"Create window 800x600 'Neural Demo'",
    hInstance
);

// Register with SDK and apply theme
auto window = SDK::WindowManager::GetInstance().RegisterWindow(hwnd);
auto theme = std::make_shared<SDK::Theme>(SDK::Theme::CreateModernTheme());
window->SetTheme(theme);
window->SetDepth(SDK::WindowDepth::FOREGROUND);
window->SetRoundedCorners(true, 16);
window->UpdateAppearance();

// Get widgets from builder
auto widgetManager = builder.GetLastWidgetManager();
```

## Understanding the Neural Network

### Architecture

The neural network uses a simple feedforward architecture:

1. **Input Layer**: Word embeddings (32 dimensions)
2. **Hidden Layer**: 64 neurons with ReLU activation
3. **Output Layer**: 10 neurons (one per intent) with sigmoid activation

### Training Process

The network is pre-initialized with:

- **Vocabulary**: 1000+ words related to GUI development
- **Embeddings**: Random initialization with uniform distribution
- **Patterns**: Keyword-based intent recognition
- **Entity Extractors**: Regex-based dimension, title, and type extraction

The hybrid approach combines:
- **Neural network inference** for complex natural language understanding
- **Pattern matching** for high-confidence simple cases
- **Entity extraction** for structured information

### Performance

- **Initialization**: < 1ms
- **Single prompt parsing**: < 5ms
- **Window creation**: Depends on window complexity
- **Memory footprint**: ~800KB for network weights and expanded vocabulary (2100+ words)

### Limitations

1. **Fixed vocabulary**: Unknown words are mapped to `<UNK>` token (2100+ words supported)
2. **Simple architecture**: Not suitable for complex semantic understanding
3. **Limited context**: No conversation history or multi-turn dialogues
4. **English only**: Currently trained on English GUI terminology

### Recent Enhancements (v1.2.1)

**Expanded Vocabulary**: Added 200+ new GUI-related terms including:
- Advanced widget types (slider, combobox, listbox, toolbar, etc.)
- Layout and positioning keywords (left, right, top, bottom, center, horizontal, vertical)
- State and visibility terms (enabled, disabled, hidden, visible)
- Common GUI actions (submit, cancel, apply, save, load, search)
- Neural network-specific terms (neural, network, ai, intelligent, smart)

**Improved Entity Extraction**:
- Position detection (left, right, top, bottom, center)
- Alignment recognition (horizontal, vertical)
- State extraction (visible, hidden, enabled, disabled)
- Action keywords (submit, cancel, apply, save, load)
- Support for double quotes in addition to single quotes
- Enhanced callback type detection (added onChange)

**Better Pattern Matching**:
- More synonym support for common widgets
- Improved form/dialog detection
- Enhanced field/input recognition

## Best Practices

### 1. Clear and Specific Prompts

```cpp
// Good: Specific dimensions and widget types
builder.BuildFromPrompt(L"Create window 800x600 with button and textbox", hInstance);

// Better: Include position information
builder.BuildFromPrompt(L"Create window 800x600 with button at top and textbox below", hInstance);

// Avoid: Vague or ambiguous
builder.BuildFromPrompt(L"Make something", hInstance);
```

### 2. Use Standard Terminology

```cpp
// Good: Uses standard widget names
builder.BuildFromPrompt(L"Add a progress bar", hInstance);

// Also good: Use synonyms now supported
builder.BuildFromPrompt(L"Add a slider", hInstance);
builder.BuildFromPrompt(L"Add a dropdown menu", hInstance);

// Works but less accurate: Non-standard terms
builder.BuildFromPrompt(L"Add a loading indicator", hInstance);
```

### 3. Combine with Manual Configuration

```cpp
// Use neural network for structure
HWND hwnd = builder.BuildFromPrompt(L"Create window 600x400", hInstance);

// Manually configure details
auto window = SDK::WindowManager::GetInstance().RegisterWindow(hwnd);
window->SetTheme(myCustomTheme);
window->SetRoundedCorners(true, 20);
```

### 4. Check Confidence Scores

```cpp
auto parsed = nn->ParsePrompt(prompt);

if (parsed.confidence > 0.8f) {
    // High confidence - proceed
    CreateFromParsed(parsed);
} else {
    // Low confidence - use fallback
    CreateDefault();
}
```

## Examples

### Login Window

```cpp
HWND loginWindow = builder.BuildFromPrompt(
    L"Create a window 400x300 'Login' with label 'Username', textbox, "
    L"label 'Password', textbox, and button 'Login'",
    hInstance
);
```

### Settings Dialog

```cpp
HWND settingsDialog = builder.BuildFromPrompt(
    L"Create dialog 500x400 'Settings' with checkbox 'Enable notifications', "
    L"checkbox 'Auto-save', and button 'Apply'",
    hInstance
);
```

### Progress Window

```cpp
HWND progressWindow = builder.BuildFromPrompt(
    L"Create window 600x200 'Download' with label 'Downloading...', "
    L"progress bar, and label 'Status'",
    hInstance
);
```

### Custom Callback Example

```cpp
// Create button with custom callback
auto button = std::make_shared<SDK::Button>(L"Calculate");

auto callback = builder.GenerateCallback(
    L"Show calculation result on click"
);

button->SetEventCallback(callback);
button->SetPosition(50, 50);
button->SetSize(120, 40);
```

## Troubleshooting

### Neural Network Returns UNKNOWN Intent

**Cause**: Prompt doesn't match known patterns or vocabulary

**Solution**:
- Use more specific GUI-related terms
- Check spelling and grammar
- Try different phrasing
- Train network with custom data if needed

### Extracted Entities Are Wrong

**Cause**: Entity extraction patterns don't match prompt format

**Solution**:
- Use standard formats: "800x600" for dimensions
- Use quotes for titles: 'My Window'
- Put numbers before widget types: "3 buttons"

### Generated Callback Doesn't Work

**Cause**: Callback generator uses simple pattern matching

**Solution**:
- Use explicit action verbs: "show", "close", "open"
- Specify event type: "on click", "on hover"
- For complex callbacks, implement manually

## Future Enhancements

The neural network system is designed to be extensible:

1. **Enhanced training**: Add more training examples from real applications
2. **Context awareness**: Remember previous prompts in session
3. **Multi-language**: Support for non-English prompts
4. **Advanced callbacks**: Generate more complex event handlers
5. **UI layout**: Understand spatial relationships and positioning (✅ Partially implemented)
6. **Style inference**: Infer theme and styling from prompt tone

## Cross-Platform Support

The neural network implementation is **fully cross-platform** and works identically on:

- ✅ **Windows** (7, 8, 10, 11) - Full support
- ✅ **Linux** (X11/Wayland) - Full support
- ✅ **macOS** - Experimental support

The neural network uses only standard C++ libraries with no platform-specific dependencies:
- Pure C++17 implementation
- No external ML frameworks (TensorFlow, PyTorch, etc.)
- Platform-independent text processing
- Works with any windowing system

**Benefits for Linux/Unix Users**:
- Same natural language GUI creation capabilities
- Full vocabulary and entity extraction
- Identical API and usage patterns
- No need for platform-specific adjustments

**Example Cross-Platform Usage**:
```cpp
#include "SDK/SDK.h"

// This code works identically on Windows, Linux, and macOS
SDK::NeuralPromptBuilder builder;
auto parsed = builder.ParsePrompt(
    L"Create window 800x600 with button and textbox"
);

// The neural network processing is platform-independent
// Platform-specific rendering happens elsewhere
```

## See Also

- [PromptWindowBuilder Guide](WIDGET_GUIDE.md#prompt-window-builder) - Template-based window generation
- [Widget Guide](WIDGET_GUIDE.md) - Complete widget system documentation
- [API Reference](API.md) - Full SDK API documentation
- [Examples](examples/) - Sample applications using neural network
- [BUILD.md](BUILD.md) - Cross-platform build instructions
