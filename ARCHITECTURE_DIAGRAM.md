# Widget System Architecture Diagram

## Component Overview

```
┌─────────────────────────────────────────────────────────────────────┐
│                        5D GUI SDK - Widget System                    │
└─────────────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────────────┐
│                          Core Components                             │
├─────────────────────────────────────────────────────────────────────┤
│                                                                       │
│  ┌──────────────┐      ┌──────────────┐      ┌──────────────┐      │
│  │   Widget     │      │WidgetManager │      │PromptWindow  │      │
│  │  (Base)      │◄─────┤              │      │   Builder    │      │
│  │              │      │  - Rendering  │      │              │      │
│  │ - Position   │      │  - Events     │      │ - Parser     │      │
│  │ - Size       │      │  - Lookup     │      │ - Factory    │      │
│  │ - Visibility │      │  - Updates    │      │ - Layout     │      │
│  │ - Events     │      └──────────────┘      └──────────────┘      │
│  └──────────────┘                                                    │
│         △                                                            │
│         │                                                            │
│         └─────────────┬──────────────┬──────────────┐               │
│                       │              │              │               │
│         ┌─────────────▼────┐  ┌─────▼────────┐  ┌──▼─────────┐    │
│         │  ProgressBar     │  │   Tooltip    │  │   Future   │    │
│         │                  │  │              │  │  Widgets   │    │
│         │ - Value/Max     │  │ - Text       │  │            │    │
│         │ - Orientation   │  │ - Position   │  │ - Button   │    │
│         │ - Gradient      │  │ - Fade       │  │ - Slider   │    │
│         │ - Animation     │  │ - Multi-line │  │ - ...      │    │
│         └──────────────────┘  └──────────────┘  └────────────┘    │
│                                                                       │
└─────────────────────────────────────────────────────────────────────┘
```

## Widget Hierarchy

```
SDK::Widget (Base Class)
│
├── Position & Size Management
│   ├── SetBounds(x, y, width, height)
│   ├── GetBounds(rect)
│   └── HitTest(x, y)
│
├── State Management
│   ├── SetVisible(bool)
│   ├── SetEnabled(bool)
│   └── SetHovered(bool)
│
├── Event System
│   ├── OnMouseEnter()
│   ├── OnMouseLeave()
│   ├── OnMouseMove(x, y)
│   ├── OnMouseDown(x, y)
│   ├── OnMouseUp(x, y)
│   └── OnClick()
│
└── Core Methods
    ├── Render(HDC) - Pure Virtual
    └── Update(deltaTime)

    ├─► SDK::ProgressBar
    │   │
    │   ├── Value Management
    │   │   ├── SetValue(float)
    │   │   ├── SetMaxValue(float)
    │   │   └── SetPercentage(float)
    │   │
    │   ├── Visual Properties
    │   │   ├── SetOrientation(Horizontal/Vertical)
    │   │   ├── SetGradientColors(start, end)
    │   │   └── SetCornerRadius(int)
    │   │
    │   └── Animation
    │       ├── SetAnimated(bool)
    │       └── SetAnimationSpeed(float)
    │
    └─► SDK::Tooltip
        │
        ├── Text Content
        │   ├── SetText(wstring)
        │   └── SetLines(vector<wstring>)
        │
        ├── Positioning
        │   ├── Show(x, y)
        │   ├── ShowNearWidget(widget)
        │   └── ShowAtCursor()
        │
        └── Animation
            ├── SetFadeEnabled(bool)
            ├── SetFadeSpeed(float)
            └── SetShowDelay(float)
```

## Event Flow

```
┌─────────────────────────────────────────────────────────────────────┐
│                         User Interaction                             │
└─────────────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────────────┐
│                      Win32 Window Messages                           │
│  WM_MOUSEMOVE │ WM_LBUTTONDOWN │ WM_LBUTTONUP │ WM_PAINT            │
└─────────────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────────────┐
│                        WidgetManager                                 │
│                                                                       │
│  HandleMouseMove(x, y)                                               │
│  ├─► GetWidgetAt(x, y)         [Hit Testing]                        │
│  ├─► Update hover state                                              │
│  └─► widget->OnMouseMove(x, y)                                       │
│                                                                       │
│  HandleMouseDown(x, y)                                               │
│  ├─► GetWidgetAt(x, y)                                               │
│  └─► widget->OnMouseDown(x, y)                                       │
│                                                                       │
│  HandleMouseUp(x, y)                                                 │
│  ├─► widget->OnMouseUp(x, y)                                         │
│  └─► widget->OnClick()          [If still over widget]              │
│                                                                       │
│  RenderAll(HDC)                                                      │
│  └─► For each widget: widget->Render(hdc)                            │
│                                                                       │
│  UpdateAll(deltaTime)                                                │
│  └─► For each widget: widget->Update(deltaTime)                      │
│                                                                       │
└─────────────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────────────┐
│                        Individual Widgets                            │
│                                                                       │
│  ProgressBar::Update(deltaTime)                                      │
│  └─► Smooth animation: displayValue → targetValue                   │
│                                                                       │
│  Tooltip::Update(deltaTime)                                          │
│  └─► Fade animation: opacity → target (0.0 or 1.0)                  │
│                                                                       │
└─────────────────────────────────────────────────────────────────────┘
```

## Prompt Window Builder Flow

```
┌─────────────────────────────────────────────────────────────────────┐
│                      Text Prompt Input                               │
│  "window 800x600 'My App' with progressbar and tooltip"             │
└─────────────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────────────┐
│                    PromptWindowBuilder                               │
│                                                                       │
│  1. Tokenize(prompt)                                                 │
│     ├─► ["window", "800x600", "'My App'", "with",                   │
│     │    "progressbar", "and", "tooltip"]                            │
│     └─► Identify: WORD, STRING, WITH, AND tokens                    │
│                                                                       │
│  2. ParsePrompt(tokens)                                              │
│     ├─► Extract window dimensions: 800x600                           │
│     ├─► Extract title: "My App"                                      │
│     └─► Extract widgets: [progressbar, tooltip]                     │
│                                                                       │
│  3. Create WindowSpec                                                │
│     ├─► spec.width = 800                                             │
│     ├─► spec.height = 600                                            │
│     ├─► spec.title = "My App"                                        │
│     └─► spec.widgets = ["progressbar", "tooltip"]                   │
│                                                                       │
│  4. CreateWindowFromSpec()                                           │
│     └─► CreateWindowExW(...) → HWND                                  │
│                                                                       │
│  5. CreateWidgetsFromSpec()                                          │
│     ├─► For each widget type:                                        │
│     │   └─► m_widgetFactories[type]() → shared_ptr<Widget>          │
│     └─► LayoutWidgets() - Auto-position                             │
│                                                                       │
│  6. Create WidgetManager                                             │
│     └─► Add all widgets to manager                                   │
│                                                                       │
└─────────────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────────────┐
│                      Ready-to-Use Window                             │
│  ┌───────────────────────────────────────────────────────┐          │
│  │ My App                                          [_][□][X]│         │
│  ├───────────────────────────────────────────────────────┤          │
│  │                                                         │          │
│  │  ▓▓▓▓▓▓▓▓▓▓▓░░░░░░░░░░░░░░░░░░░░░ 33%               │          │
│  │                                                         │          │
│  │  [Hover shows tooltip]                                 │          │
│  │                                                         │          │
│  └───────────────────────────────────────────────────────┘          │
└─────────────────────────────────────────────────────────────────────┘
```

## Neural Network Integration (Future)

```
┌─────────────────────────────────────────────────────────────────────┐
│                   Natural Language Prompt                            │
│  "Create a download manager with progress bars for each file"       │
└─────────────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────────────┐
│              Neural Network Layer (Future Integration)               │
│                                                                       │
│  1. Natural Language Understanding                                   │
│     ├─► Tokenization & Embedding                                     │
│     ├─► Intent Classification: "download_manager"                    │
│     └─► Entity Extraction: "progress_bars", "multiple", "files"     │
│                                                                       │
│  2. UI Generation Model                                              │
│     ├─► Layout Generation Network                                    │
│     ├─► Widget Type Prediction                                       │
│     └─► Constraint Satisfaction                                      │
│                                                                       │
│  3. Convert to WindowSpec                                            │
│     └─► Map neural output to WindowSpec structure                    │
│                                                                       │
└─────────────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────────────┐
│              PromptWindowBuilder (Existing System)                   │
│  Creates window using WindowSpec                                     │
└─────────────────────────────────────────────────────────────────────┘
```

## Integration Example

```cpp
// Current Usage (Template-Based)
SDK::PromptWindowBuilder builder;
HWND hwnd = builder.BuildFromPrompt(
    L"window 600x400 'My App' with progressbar and tooltip",
    hInstance);

// Future Usage (Neural Network)
class AIWindowBuilder : public SDK::PromptWindowBuilder {
    NeuralNetwork* m_model;
    
public:
    WindowSpec ParsePrompt(const std::wstring& prompt) override {
        // Neural network interprets natural language
        auto features = m_model->ExtractFeatures(prompt);
        auto layout = m_model->GenerateLayout(features);
        
        // Convert to WindowSpec
        WindowSpec spec;
        spec.title = features.windowTitle;
        spec.width = layout.recommendedWidth;
        spec.height = layout.recommendedHeight;
        
        for (auto& widget : layout.widgets) {
            spec.widgets.push_back(widget.type);
        }
        
        return spec;
    }
};

// Use AI builder (optional, only if user wants AI features)
AIWindowBuilder aiBuilder(neuralNetwork);
HWND aiWindow = aiBuilder.BuildFromPrompt(
    L"Make me a settings dialog with sliders for volume and brightness",
    hInstance);
```

## Design Principles

1. **Zero Dependencies**: Uses only Win32 API
2. **Extensibility**: Factory pattern for custom widgets
3. **Event-Driven**: Clean separation of input handling
4. **Animation**: Delta-time based for smooth transitions
5. **Future-Ready**: Architecture supports neural network integration
6. **Modularity**: Each widget is independent and reusable

## File Structure

```
include/SDK/
├── Widget.h              (Base class)
├── ProgressBar.h         (Progress indicator)
├── Tooltip.h             (Hover tooltips)
├── WidgetManager.h       (Widget management)
└── PromptWindowBuilder.h (Template-based generation)

src/SDK/
├── Widget.cpp
├── ProgressBar.cpp
├── Tooltip.cpp
├── WidgetManager.cpp
└── PromptWindowBuilder.cpp

examples/
└── widget_demo.cpp       (Comprehensive demo)
```

## Summary

This architecture provides:
- ✓ Immediate value through template parsing
- ✓ Clean, maintainable codebase
- ✓ Extensible widget system
- ✓ Foundation for AI integration
- ✓ Zero external dependencies
- ✓ Pure Win32 API implementation
