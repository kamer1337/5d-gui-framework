# Neural Network GUI Generation Guide

This guide explains how to use the neural network-powered GUI creation system in the 5D GUI SDK.

## Overview

The SDK includes a lightweight neural network implementation (zero external dependencies) that can interpret natural language prompts to create GUI windows, add widgets, and set up callbacks. The network is pre-trained on the project documentation and understands common GUI creation patterns.

## Recent Updates (v2.0)

### Enhanced Polymorphism Support
- **12 New Widget Types**: Added support for Slider, ComboBox, ListBox, ListView, RadioButton, SpinBox, Image, Separator, Panel, TabControl, and Toolbar
- **Multi-Widget Parsing**: Single prompts can now specify multiple widgets (e.g., "window with button, textbox, and slider")
- **Layout Intelligence**: Automatic detection of layout intent (vertical, horizontal, grid)
- **Complex Callbacks**: Support for more event types including double-click, value-changed, and key-press

### Advanced Features
- **Layout Types**: VERTICAL, HORIZONTAL, GRID, and ABSOLUTE positioning
- **Widget Properties**: Extract and apply widget-specific properties (items, ranges, values)
- **Hierarchical Structures**: Support for nested containers and panels

## Features

- **Natural Language Understanding**: Parse English prompts to understand GUI creation intent
- **Intent Classification**: Recognizes 22+ different intents (create window, add various widgets, set callbacks, layouts)
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

// Add multiple widgets in one prompt
builder.BuildFromPrompt(L"Create window with label, textbox, and button", hInstance);

// Add specific widget types
builder.BuildFromPrompt(L"Add a progress bar", hInstance);
builder.BuildFromPrompt(L"Add a checkbox for agree to terms", hInstance);
builder.BuildFromPrompt(L"Add a tooltip that says 'Help'", hInstance);

// Advanced widgets (NEW!)
builder.BuildFromPrompt(L"Add a slider from 0 to 100", hInstance);
builder.BuildFromPrompt(L"Add a combobox with items: Red, Green, Blue", hInstance);
builder.BuildFromPrompt(L"Add a listbox with multiple items", hInstance);
builder.BuildFromPrompt(L"Add a panel with title 'Settings'", hInstance);

// Enhanced with colors and styles (v1.2.2!)
builder.BuildFromPrompt(L"Add a large red button with bold text", hInstance);
builder.BuildFromPrompt(L"Add a small light blue label at top", hInstance);
builder.BuildFromPrompt(L"Add a disabled textbox with placeholder 'Enter name'", hInstance);
builder.BuildFromPrompt(L"Add 3 buttons side by side", hInstance);
```

### Layout Control (NEW!)

The neural network now understands layout specifications:

```cpp
// Vertical layout
builder.BuildFromPrompt(
    L"Create window with button, textbox, and checkbox arranged vertically",
    hInstance
);

// Horizontal layout
builder.BuildFromPrompt(
    L"Create window with three buttons side by side",
    hInstance
);

// Grid layout
builder.BuildFromPrompt(
    L"Create window with 6 buttons in a grid",
    hInstance
);

// Position-based layout
builder.BuildFromPrompt(
    L"Create window with label at top, textbox below, and button at bottom",
    hInstance
);
```

### Complex Multi-Widget Prompts (NEW!)

Create sophisticated interfaces with a single prompt:

```cpp
// Login form with layout
builder.BuildFromPrompt(
    L"Create dialog 400x300 'Login' with label 'Username', textbox, "
    L"label 'Password', textbox, and button 'Sign In' arranged vertically",
    hInstance
);

// Settings panel with multiple controls
builder.BuildFromPrompt(
    L"Create window 600x500 'Settings' with checkbox 'Enable notifications', "
    L"slider for volume, combobox for theme, and button 'Apply'",
    hInstance
);

// Data entry form
builder.BuildFromPrompt(
    L"Create form 700x600 with label 'Name', textbox for name, combobox for category, "
    L"slider for priority, checkbox for urgent, and button 'Submit'",
    hInstance
);

// Enhanced contextual forms (v1.2.2!)
builder.BuildFromPrompt(
    L"Create a login form",  // Automatically creates: window with username, password, and login button
    hInstance
);

builder.BuildFromPrompt(
    L"Create a signup page with email, password, and confirm password",
    hInstance
);

builder.BuildFromPrompt(
    L"Create settings dialog with dark theme dropdown and large save button",
    hInstance
);

// Color and style-aware prompts (v1.2.2!)
builder.BuildFromPrompt(
    L"Create window 800x600 with red submit button, green cancel button, "
    L"and blue reset button arranged horizontally",
    hInstance
);

builder.BuildFromPrompt(
    L"Create form with required textbox for email, optional textbox for phone, "
    L"and bold submit button at bottom",
    hInstance
);
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

// Access extracted entities (standard getters)
int width = parsed.GetWidth();
int height = parsed.GetHeight();
std::wstring title = parsed.GetTitle();
std::wstring text = parsed.GetWidgetText();

// Access enhanced entities (v1.2.2)
std::wstring color = parsed.GetColor();            // e.g., "red", "blue", "green"
std::wstring colorMod = parsed.GetColorModifier(); // e.g., "dark", "light", "bright"
std::wstring size = parsed.GetSize();              // e.g., "small", "medium", "large"
std::wstring state = parsed.GetState();            // e.g., "enabled", "disabled", "readonly"
std::wstring action = parsed.GetAction();          // e.g., "submit", "cancel", "save"
std::wstring fontStyle = parsed.GetFontStyle();    // e.g., "bold", "italic", "underline"
std::wstring pattern = parsed.GetPattern();        // e.g., "login", "signup", "settings"
std::wstring validation = parsed.GetValidation();  // e.g., "required", "optional"
std::wstring placeholder = parsed.GetPlaceholder();// Placeholder text
int minValue = parsed.GetMinValue();               // For sliders/spinboxes
int maxValue = parsed.GetMaxValue();               // For sliders/spinboxes
int count = parsed.GetCount();                     // Number of widgets to create
```

#### Intent Types

The network recognizes the following intents:

**Window Creation:**
- `CREATE_WINDOW` - Creating a new window or dialog

**Basic Widgets:**
- `ADD_BUTTON` - Adding a button widget
- `ADD_LABEL` - Adding a label widget
- `ADD_TEXTBOX` - Adding a text input widget
- `ADD_CHECKBOX` - Adding a checkbox widget

**Advanced Widgets (NEW!):**
- `ADD_PROGRESSBAR` - Adding a progress bar widget
- `ADD_TOOLTIP` - Adding a tooltip widget
- `ADD_SLIDER` - Adding a slider/trackbar widget
- `ADD_COMBOBOX` - Adding a combo box/dropdown widget
- `ADD_LISTBOX` - Adding a list box widget
- `ADD_LISTVIEW` - Adding a list view with columns
- `ADD_RADIOBUTTON` - Adding a radio button widget
- `ADD_SPINBOX` - Adding a spin box/numeric up-down
- `ADD_IMAGE` - Adding an image/picture widget
- `ADD_SEPARATOR` - Adding a separator/divider line
- `ADD_PANEL` - Adding a panel/container widget
- `ADD_TABCONTROL` - Adding a tab control
- `ADD_TOOLBAR` - Adding a toolbar

**Actions:**
- `SET_CALLBACK` - Setting up event handlers
- `SET_THEME` - Applying themes
- `SET_LAYOUT` - Arranging widget layout (NEW!)
- `UNKNOWN` - Unrecognized intent

#### Callback Types

For event handlers, the network understands:

- `ON_CLICK` - Click events
- `ON_DOUBLE_CLICK` - Double-click events (NEW!)
- `ON_HOVER` - Hover/mouse enter events
- `ON_FOCUS` - Focus events
- `ON_BLUR` - Blur/focus lost events
- `ON_CHANGE` - Change events (NEW!)
- `ON_VALUE_CHANGED` - Value change events for sliders, spinboxes (NEW!)
- `ON_KEY_PRESS` - Keyboard press events (NEW!)
- `CUSTOM` - Custom event types

#### Layout Types (NEW!)

The network can determine layout preferences:

- `NONE` - No specific layout (default positioning)
- `VERTICAL` - Stack widgets vertically
- `HORIZONTAL` - Arrange widgets horizontally
- `GRID` - Arrange widgets in a grid pattern
- `ABSOLUTE` - Absolute positioning specified

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

### Understanding the Neural Network

### Architecture

The neural network uses a simple feedforward architecture:

1. **Input Layer**: Word embeddings (32 dimensions)
2. **Hidden Layer**: 64 neurons with ReLU activation
3. **Output Layer**: 22 neurons (one per intent) with sigmoid activation (EXPANDED!)

### Training Process

The network is pre-initialized with:

- **Vocabulary**: 2400+ words related to GUI development (EXPANDED from 2100+!)
- **Embeddings**: Random initialization with uniform distribution
- **Patterns**: Keyword-based intent recognition for 22 intents with 60+ synonyms
- **Entity Extractors**: Enhanced regex and pattern-based dimension, title, type, color, and property extraction

The hybrid approach combines:
- **Neural network inference** for complex natural language understanding
- **Pattern matching** for high-confidence simple cases with extensive synonym support
- **Entity extraction** for structured information (dimensions, colors, styles, ranges)
- **Layout detection** for spatial arrangement with contextual inference (NEW!)
- **Multi-widget parsing** for complex prompts with pattern-aware creation (NEW!)
- **Contextual understanding** for common UI patterns (login forms, settings dialogs)

### Performance

- **Initialization**: < 1ms
- **Single prompt parsing**: < 5ms
- **Multi-widget parsing**: < 10ms (NEW!)
- **Window creation**: Depends on window complexity
- **Memory footprint**: ~1.4MB for network weights and expanded vocabulary (2400+ words, up from 2100+)

### Limitations

1. **Fixed vocabulary**: Unknown words are mapped to `<UNK>` token (2400+ words supported, up from 2100+)
2. **Simple architecture**: Not suitable for extremely complex semantic understanding
3. **Limited context**: No conversation history or multi-turn dialogues
4. **English only**: Currently trained on English GUI terminology
5. **Layout constraints**: Complex nested layouts may require manual adjustment

### Recent Enhancements (v1.2.2)

**Significantly Expanded Vocabulary** (400+ terms, up from 200+):
- Advanced widget types with multiple synonyms (slider/trackbar/range, combobox/dropdown/picker)
- Comprehensive layout and positioning keywords (centered, anchor, dock, pin, absolute, relative)
- Extended color vocabulary (all basic colors plus modifiers: dark, light, bright, pale)
- Rich state and visibility terms (readonly, editable, valid, invalid, loading, active)
- Expanded GUI action words (undo, redo, delete, remove, edit, modify, copy, paste)
- UI pattern terms (navigation, header, footer, sidebar, modal, wizard, carousel)
- Form-specific vocabulary (required, optional, mandatory, placeholder, validation)
- Neural network and AI terms (neural, network, ai, intelligent, smart, suggest, recommend)

**Enhanced Entity Extraction**:
- **Color detection**: Recognizes all basic colors and modifiers (e.g., "dark blue", "light green")
- **Size keywords**: small, medium, large, tiny, huge
- **Font styles**: bold, italic, underline detection
- **Item counting**: "3 buttons", "5 items" pattern recognition
- **Range values**: "from 0 to 100" pattern extraction for sliders/spinboxes
- **Form patterns**: Auto-detects login, signup, settings forms and applies appropriate defaults
- **Validation modifiers**: required, optional, mandatory field detection
- **Placeholder text**: Extracts placeholder values from prompts
- **Double-click events**: Enhanced callback detection including double-click, keypress
- **Contextual defaults**: Automatically infers window titles for common patterns (Login, Sign Up, Settings)

**Improved Layout Intelligence**:
- **Priority-based detection**: Explicit layout keywords take precedence over contextual hints
- **Weighted indicators**: Counts positional words to determine likely layout direction
- **Contextual inference**: Forms default to vertical, toolbars to horizontal
- **Grid detection**: Smart detection for multiple items suggesting grid layout (6, 9, 12 items)
- **Absolute positioning**: Support for fixed/absolute positioning keywords
- **Pattern-aware layouts**: Login forms automatically use vertical layout

**Better Pattern Matching**:
- 60+ new widget synonyms (btn, entry, caption, heading, stepper, counter, etc.)
- Improved form/dialog detection with automatic title generation
- Enhanced field/input recognition with more variations
- Context-aware widget creation based on detected patterns

## Best Practices

### 1. Clear and Specific Prompts

```cpp
// Good: Specific dimensions and widget types
builder.BuildFromPrompt(L"Create window 800x600 with button and textbox", hInstance);

// Better: Include position information
builder.BuildFromPrompt(L"Create window 800x600 with button at top and textbox below", hInstance);

// Best: Specify layout and multiple widgets (NEW!)
builder.BuildFromPrompt(
    L"Create window 800x600 with button, textbox, and slider arranged vertically", 
    hInstance
);

// Enhanced: Include colors and styles (v1.2.2!)
builder.BuildFromPrompt(
    L"Create window 800x600 with large red button, small textbox, and dark blue slider stacked vertically",
    hInstance
);

// Avoid: Vague or ambiguous
builder.BuildFromPrompt(L"Make something", hInstance);
```

### 2. Use Standard Terminology

```cpp
// Good: Uses standard widget names
builder.BuildFromPrompt(L"Add a progress bar", hInstance);

// Also good: Use synonyms now supported
builder.BuildFromPrompt(L"Add a slider", hInstance);
builder.BuildFromPrompt(L"Add a dropdown menu", hInstance);  // Maps to combobox
builder.BuildFromPrompt(L"Add a stepper", hInstance);        // Maps to spinbox (v1.2.2)
builder.BuildFromPrompt(L"Add a picker", hInstance);         // Maps to combobox (v1.2.2)

// Works but less accurate: Non-standard terms
builder.BuildFromPrompt(L"Add a loading indicator", hInstance);
```

### 3. Leverage Multi-Widget Support (NEW!)

```cpp
// Single widget per prompt (old way)
auto hwnd = builder.BuildFromPrompt(L"Create window 600x400", hInstance);
// Then manually add widgets...

// Multiple widgets in one prompt (new way - better!)
auto hwnd = builder.BuildFromPrompt(
    L"Create window 600x400 with label, textbox, button, and checkbox",
    hInstance
);
```

### 4. Use Layout Keywords (NEW!)

```cpp
// Vertical layout
builder.BuildFromPrompt(
    L"Create window with 5 buttons stacked vertically", 
    hInstance
);

// Horizontal layout  
builder.BuildFromPrompt(
    L"Create window with 3 buttons side by side",
    hInstance
);

// Grid layout
builder.BuildFromPrompt(
    L"Create window with 9 buttons in a grid",
    hInstance
);
```

### 5. Combine with Manual Configuration

```cpp
// Use neural network for structure
HWND hwnd = builder.BuildFromPrompt(
    L"Create window 600x400 with slider, combobox, and button arranged vertically",
    hInstance
);

// Manually configure details
auto window = SDK::WindowManager::GetInstance().RegisterWindow(hwnd);
window->SetTheme(myCustomTheme);
window->SetRoundedCorners(true, 20);

// Access widgets and customize
auto widgetMgr = builder.GetLastWidgetManager();
// Customize individual widgets as needed...
```

### 6. Check Confidence Scores

```cpp
auto parsed = nn->ParsePrompt(prompt);

if (parsed.confidence > 0.8f) {
    // High confidence - proceed
    CreateFromParsed(parsed);
} else {
    // Low confidence - use fallback or ask for clarification
    CreateDefault();
}
```

## Examples

### Login Window

```cpp
// Simple login form
HWND loginWindow = builder.BuildFromPrompt(
    L"Create a window 400x300 'Login' with label 'Username', textbox, "
    L"label 'Password', textbox, and button 'Login'",
    hInstance
);

// With layout specification (NEW!)
HWND betterLogin = builder.BuildFromPrompt(
    L"Create dialog 400x300 'Login' with label, textbox, label, textbox, "
    L"and button arranged vertically",
    hInstance
);
```

### Settings Dialog

```cpp
// Basic settings
HWND settingsDialog = builder.BuildFromPrompt(
    L"Create dialog 500x400 'Settings' with checkbox 'Enable notifications', "
    L"checkbox 'Auto-save', and button 'Apply'",
    hInstance
);

// Advanced settings with multiple widget types (NEW!)
HWND advancedSettings = builder.BuildFromPrompt(
    L"Create window 600x500 'Settings' with checkbox 'Enable notifications', "
    L"slider for volume, combobox for theme selection, separator, "
    L"and two buttons arranged vertically",
    hInstance
);
```

### Data Entry Form (NEW!)

```cpp
// Complex form with various widgets
HWND dataEntry = builder.BuildFromPrompt(
    L"Create form 700x600 'Data Entry' with label 'Name', textbox, "
    L"label 'Category', combobox, label 'Priority', slider, "
    L"checkbox 'Urgent', and button 'Submit' in vertical layout",
    hInstance
);
```

### Control Panel (NEW!)

```cpp
// Control panel with grid layout
HWND controlPanel = builder.BuildFromPrompt(
    L"Create window 800x600 'Control Panel' with 9 buttons in a grid",
    hInstance
);
```

### Horizontal Toolbar (NEW!)

```cpp
// Toolbar with buttons side by side
HWND toolbar = builder.BuildFromPrompt(
    L"Create window 800x100 with 5 buttons arranged horizontally",
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

## Enhanced Examples (v1.2.2)

### Color and Style-Aware Windows

```cpp
// Window with color-coded buttons
HWND colorWindow = builder.BuildFromPrompt(
    L"Create window 600x400 with large red button 'Danger', "
    L"medium green button 'Success', and small blue button 'Info' horizontally",
    hInstance
);

// Form with styled inputs
HWND styledForm = builder.BuildFromPrompt(
    L"Create form 500x600 with bold label 'Name', textbox with placeholder 'Enter name', "
    L"bold label 'Email', textbox with placeholder 'your@email.com', "
    L"and large dark blue submit button at bottom",
    hInstance
);
```

### Context-Aware Form Generation

```cpp
// Simple login form - auto-generates username, password fields and login button
HWND quickLogin = builder.BuildFromPrompt(
    L"Create a login form",
    hInstance
);

// Enhanced signup form
HWND signupForm = builder.BuildFromPrompt(
    L"Create signup page with email, password, confirm password, and register button",
    hInstance
);

// Settings dialog with validation
HWND settingsDialog = builder.BuildFromPrompt(
    L"Create settings dialog with required textbox for API key, "
    L"optional textbox for webhook URL, checkbox for notifications, "
    L"and save button",
    hInstance
);
```

### Range-Based Widgets

```cpp
// Slider with explicit range
HWND sliderWindow = builder.BuildFromPrompt(
    L"Create window with slider from 0 to 100 for volume control",
    hInstance
);

// Spinbox with range
HWND spinboxWindow = builder.BuildFromPrompt(
    L"Create window with counter from 1 to 10",
    hInstance
);
```

### Multi-Widget Counting

```cpp
// Create multiple identical widgets
HWND buttonGrid = builder.BuildFromPrompt(
    L"Create window with 9 buttons in a grid",
    hInstance
);

HWND horizontalButtons = builder.BuildFromPrompt(
    L"Create window with 5 small buttons side by side",
    hInstance
);
```

### State-Aware Widgets

```cpp
// Disabled input
HWND disabledInput = builder.BuildFromPrompt(
    L"Create window with disabled textbox showing 'Read Only'",
    hInstance
);

// Readonly with placeholder
HWND readonlyField = builder.BuildFromPrompt(
    L"Create window with readonly textbox with placeholder 'Cannot edit'",
    hInstance
);
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

The neural network system is designed to be extensible. Completed and planned improvements:

### Completed (v2.0)
1. ✅ **Enhanced widget support**: Added 12+ new widget types
2. ✅ **Layout intelligence**: Automatic vertical, horizontal, and grid layouts
3. ✅ **Multi-widget parsing**: Single prompts with multiple widgets
4. ✅ **Expanded vocabulary**: 2100+ GUI-related terms
5. ✅ **Complex callbacks**: More event types and handlers

### Planned
1. **Enhanced training**: Add more training examples from real applications
2. **Context awareness**: Remember previous prompts in session
3. **Multi-language**: Support for non-English prompts
4. **Advanced callbacks**: Generate more complex event handlers with logic
5. **Nested layouts**: Complex hierarchical widget arrangements
6. **Style inference**: Infer theme and styling from prompt tone
7. **Widget relationships**: Parent-child and sibling relationships
8. **Data binding**: Connect widgets to data models

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
