# Version 1.3 Features Guide

This guide covers all the new features introduced in version 1.3 of the 5D GUI SDK.

## Overview

Version 1.3 brings significant enhancements to visual effects, animation capabilities, and UI components. This release focuses on giving developers more control over the look, feel, and behavior of their applications.

## Table of Contents

1. [Advanced Visual Effects](#advanced-visual-effects)
2. [Effect Presets](#effect-presets)
3. [Enhanced Animations](#enhanced-animations)
4. [Layout System](#layout-system)
5. [Menu System](#menu-system)
6. [Rich Text Rendering](#rich-text-rendering)
7. [Examples](#examples)

---

## Advanced Visual Effects

Version 1.3 introduces three new advanced visual effects that can dramatically enhance the appearance of your UI.

### Depth of Field

Creates a cinematic focus effect by blurring areas outside a focal range.

```cpp
#include "SDK/SDK.h"

// Apply depth of field effect
RECT effectRect = { 0, 0, 800, 600 };
renderBackend->ApplyDepthOfField(effectRect, 
    300,      // focalDepth - distance to focus point
    10,       // blurAmount - strength of blur
    150.0f    // focalRange - size of in-focus area
);
```

**Parameters:**
- `focalDepth`: Vertical position of the focal point
- `blurAmount`: Blur intensity (0-20 recommended)
- `focalRange`: Distance from focal point before blur starts (pixels)

**Use Cases:**
- Cinematic presentations
- Focus attention on specific UI elements
- Create depth perception in flat UIs

### Motion Blur

Simulates motion by directionally blurring content, creating a sense of speed or movement.

```cpp
// Apply motion blur effect
renderBackend->ApplyMotionBlur(effectRect,
    10,      // directionX - horizontal blur direction
    0,       // directionY - vertical blur direction
    0.7f     // intensity - blur strength (0.0-1.0)
);
```

**Parameters:**
- `directionX`: Horizontal blur direction and distance
- `directionY`: Vertical blur direction and distance
- `intensity`: Blur strength multiplier

**Use Cases:**
- Animated transitions
- Scrolling effects
- Action-oriented UIs
- Speed indicators

### Chromatic Aberration

Simulates lens distortion by splitting and offsetting RGB color channels.

```cpp
// Apply chromatic aberration effect
renderBackend->ApplyChromaticAberration(effectRect,
    0.02f,   // strength - aberration intensity
    3,       // offsetX - horizontal channel separation
    3        // offsetY - vertical channel separation
);
```

**Parameters:**
- `strength`: Overall effect intensity (0.0-0.1 typical)
- `offsetX`: Horizontal pixel offset for channel separation
- `offsetY`: Vertical pixel offset for channel separation

**Use Cases:**
- Retro/vintage aesthetics
- Glitch effects
- Sci-fi interfaces
- Error states or alerts

---

## Effect Presets

Effect presets allow you to apply professionally-designed combinations of effects with a single function call.

### Available Presets

```cpp
enum class EffectPreset {
    NONE,       // No effects
    CINEMATIC,  // Depth of field + subtle blur + bloom
    GAME_UI,    // Sharp edges + glow + chromatic aberration
    RETRO,      // Chromatic aberration + bloom
    DREAMY,     // Soft blur + bloom
    MOTION,     // Motion blur + slight chromatic aberration
    CUSTOM      // User-defined combination
};
```

### Using Presets

```cpp
// Quick and easy
RECT effectRect = { 0, 0, 800, 600 };
renderBackend->ApplyEffectPreset(effectRect, RenderBackend::EffectPreset::CINEMATIC);

// Or cycle through presets
EffectPreset presets[] = {
    EffectPreset::CINEMATIC,
    EffectPreset::GAME_UI,
    EffectPreset::RETRO,
    EffectPreset::DREAMY,
    EffectPreset::MOTION
};

for (auto preset : presets) {
    renderBackend->ApplyEffectPreset(effectRect, preset);
    // Display for a moment, then switch
}
```

### Custom Effect Combinations

For full control, use `ApplyCustomEffects` with an `EffectSettings` structure:

```cpp
RenderBackend::EffectSettings settings;

// Enable desired effects
settings.enableBlur = true;
settings.blurRadius = 5;

settings.enableBloom = true;
settings.bloomThreshold = 0.8f;
settings.bloomIntensity = 1.0f;

settings.enableDepthOfField = true;
settings.focalDepth = 200;
settings.dofBlurAmount = 8;
settings.focalRange = 100.0f;

settings.enableMotionBlur = false;
settings.enableChromaticAberration = false;

// Apply custom combination
renderBackend->ApplyCustomEffects(effectRect, settings);
```

---

## Enhanced Animations

Version 1.3 significantly expands animation capabilities with new easing functions, control features, and grouping.

### Bezier Curve Easing

Define custom animation curves using Bezier control points:

```cpp
WindowAnimation anim(hwnd);

// Create custom Bezier curve
WindowAnimation::BezierCurve curve(0.68f, -0.55f, 0.27f, 1.55f);
anim.SetEasingType(WindowAnimation::EasingType::CUBIC_BEZIER);
anim.SetBezierCurve(curve);

anim.AnimateMaximize();
```

**Common Bezier Curves:**
- `(0.25, 0.1, 0.25, 1.0)` - Ease
- `(0.42, 0.0, 1.0, 1.0)` - Ease-in
- `(0.0, 0.0, 0.58, 1.0)` - Ease-out
- `(0.42, 0.0, 0.58, 1.0)` - Ease-in-out (default)
- `(0.68, -0.55, 0.27, 1.55)` - Back effect

### New Easing Functions

```cpp
enum class EasingType {
    LINEAR,
    EASE_IN,
    EASE_OUT,
    EASE_IN_OUT,
    BOUNCE,
    ELASTIC,        // NEW in v1.3
    BACK,           // NEW in v1.3
    CUBIC_BEZIER    // NEW in v1.3
};

// Use elastic easing for springy animations
anim.SetEasingType(WindowAnimation::EasingType::ELASTIC);

// Use back easing for overshoot effect
anim.SetEasingType(WindowAnimation::EasingType::BACK);
```

### Animation Control

```cpp
WindowAnimation anim(hwnd);

// Start animation
anim.AnimateMinimize();

// Pause animation (freeze in current state)
anim.PauseAnimation();

// Resume from paused state
anim.ResumeAnimation();

// Reverse animation direction
anim.ReverseAnimation();

// Stop animation immediately
anim.StopAnimation();

// Check state
if (anim.IsPaused()) {
    anim.ResumeAnimation();
}
```

### Animation Groups

Coordinate multiple animations to play in sequence or parallel:

```cpp
// Create animation group
AnimationGroup group(AnimationGroup::PlayMode::SEQUENCE);

// Add animations
WindowAnimation* anim1 = new WindowAnimation(hwnd1);
WindowAnimation* anim2 = new WindowAnimation(hwnd2);
WindowAnimation* anim3 = new WindowAnimation(hwnd3);

group.AddAnimation(anim1);
group.AddAnimation(anim2);
group.AddAnimation(anim3);

// Set callback for when all complete
group.SetOnComplete([]() {
    std::cout << "All animations finished!" << std::endl;
});

// Play all animations in sequence
group.Play();

// Or switch to parallel mode
group.SetPlayMode(AnimationGroup::PlayMode::PARALLEL);
group.Play();  // All play simultaneously

// Control the group
group.Pause();
group.Resume();
group.Reverse();
group.Stop();
```

---

## Layout System

Automatic widget arrangement with three layout managers.

### Grid Layout

Arranges widgets in a grid with configurable columns and rows:

```cpp
#include "SDK/Layout.h"

// Create grid layout (3 columns, auto rows)
auto gridLayout = std::make_shared<GridLayout>(3, 0);
gridLayout->SetSpacing(10);
gridLayout->SetPadding(20, 20, 20, 20);

// Create widgets
std::vector<std::shared_ptr<Widget>> widgets;
for (int i = 0; i < 9; i++) {
    auto button = std::make_shared<Button>(L"Button " + std::to_wstring(i+1));
    widgets.push_back(button);
}

// Apply layout
RECT bounds = { 0, 0, 800, 600 };
gridLayout->Apply(bounds, widgets);
```

**Configuration:**
- `SetColumns(int)` - Number of columns
- `SetRows(int)` - Number of rows (0 = auto)
- `SetUniformCellSize(bool)` - Equal-sized cells
- `SetSpacing(int)` - Gap between cells
- `SetPadding(l, t, r, b)` - Layout margins

### Flow Layout

Left-to-right, top-to-bottom with automatic wrapping:

```cpp
auto flowLayout = std::make_shared<FlowLayout>(FlowLayout::Direction::LEFT_TO_RIGHT);
flowLayout->SetWrap(true);
flowLayout->SetAlignment(FlowLayout::Alignment::CENTER);
flowLayout->SetSpacing(5);

flowLayout->Apply(bounds, widgets);
```

**Configuration:**
- Direction: `LEFT_TO_RIGHT`, `RIGHT_TO_LEFT`, `TOP_TO_BOTTOM`, `BOTTOM_TO_TOP`
- Alignment: `START`, `CENTER`, `END`, `STRETCH`
- `SetWrap(bool)` - Enable/disable wrapping

### Stack Layout

Vertical or horizontal stacking with distribution options:

```cpp
auto stackLayout = std::make_shared<StackLayout>(StackLayout::Orientation::VERTICAL);
stackLayout->SetDistribution(StackLayout::Distribution::SPACE_BETWEEN);
stackLayout->SetSpacing(10);

stackLayout->Apply(bounds, widgets);
```

**Distribution Modes:**
- `START` - Pack widgets to start
- `END` - Pack widgets to end
- `CENTER` - Center all widgets
- `SPACE_BETWEEN` - Equal space between widgets
- `SPACE_AROUND` - Equal space around widgets
- `SPACE_EVENLY` - Equal space including edges

---

## Menu System

Complete menu implementation with MenuBar, ContextMenu, and customizable MenuItems.

### Creating a Menu Bar

```cpp
#include "SDK/Menu.h"

auto menuBar = std::make_shared<MenuBar>();
menuBar->SetPosition(0, 0);
menuBar->SetSize(800, 30);

// Create File menu
auto fileMenu = std::make_shared<Menu>();

auto newItem = std::make_shared<MenuItem>(L"New");
newItem->SetShortcut(L"Ctrl+N");
newItem->SetOnClick([]() {
    // Handle New action
});
fileMenu->AddItem(newItem);

auto openItem = std::make_shared<MenuItem>(L"Open");
openItem->SetShortcut(L"Ctrl+O");
fileMenu->AddItem(openItem);

fileMenu->AddItem(std::make_shared<MenuItem>(L"-", MenuItem::Type::SEPARATOR));

auto exitItem = std::make_shared<MenuItem>(L"Exit");
exitItem->SetShortcut(L"Alt+F4");
fileMenu->AddItem(exitItem);

menuBar->AddMenu(L"File", fileMenu);
```

### Context Menus

Popup menus that appear at cursor position:

```cpp
auto contextMenu = std::make_shared<ContextMenu>();

auto cutItem = std::make_shared<MenuItem>(L"Cut");
cutItem->SetShortcut(L"Ctrl+X");
contextMenu->AddItem(cutItem);

auto copyItem = std::make_shared<MenuItem>(L"Copy");
copyItem->SetShortcut(L"Ctrl+C");
contextMenu->AddItem(copyItem);

auto pasteItem = std::make_shared<MenuItem>(L"Paste");
pasteItem->SetShortcut(L"Ctrl+V");
contextMenu->AddItem(pasteItem);

// Show at cursor position
contextMenu->Show(mouseX, mouseY);

// Auto-hide when clicking outside
contextMenu->SetAutoHide(true);
```

### Menu Item Types

```cpp
// Normal item
auto item1 = std::make_shared<MenuItem>(L"Command", MenuItem::Type::NORMAL);

// Separator
auto item2 = std::make_shared<MenuItem>(L"-", MenuItem::Type::SEPARATOR);

// Checkbox item
auto item3 = std::make_shared<MenuItem>(L"Show Toolbar", MenuItem::Type::CHECKBOX);
item3->SetChecked(true);

// Radio item (mutually exclusive in group)
auto item4 = std::make_shared<MenuItem>(L"View Mode", MenuItem::Type::RADIO);

// Submenu
auto submenu = std::make_shared<MenuItem>(L"Recent Files", MenuItem::Type::SUBMENU);
submenu->AddSubMenuItem(std::make_shared<MenuItem>(L"File1.txt"));
submenu->AddSubMenuItem(std::make_shared<MenuItem>(L"File2.txt"));
```

### Icons and Shortcuts

```cpp
auto item = std::make_shared<MenuItem>(L"Save");
item->SetShortcut(L"Ctrl+S");
item->SetIcon(LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SAVE)));
item->SetEnabled(true);
```

---

## Rich Text Rendering

Display and edit formatted text with the RichTextBox widget.

### Basic Usage

```cpp
#include "SDK/RichText.h"

auto richText = std::make_shared<RichTextBox>();
richText->SetPosition(20, 50);
richText->SetSize(760, 400);

// Simple text
richText->SetText(L"Hello, World!");

// Formatted text
auto doc = std::make_shared<RichTextDocument>();
doc->AddHeading(L"Welcome", 1);
doc->AddParagraph(L"This is a paragraph of text.");
doc->AddText(L"Bold text", true, false);
doc->AddText(L" and ", false, false);
doc->AddText(L"italic text", false, true);

richText->SetDocument(doc);
```

### Text Formatting

```cpp
// Create formatted text spans
TextSpan span;
span.text = L"Formatted Text";
span.bold = true;
span.italic = false;
span.underline = true;
span.strikethrough = false;
span.foregroundColor = Color(255, 0, 0, 255);  // Red
span.backgroundColor = Color(255, 255, 0, 100); // Semi-transparent yellow
span.fontFamily = L"Arial";
span.fontSize = 14;

doc->AddSpan(span);
```

### Hyperlinks

```cpp
doc->AddLink(L"Click here", L"https://example.com");

// Or manually
TextSpan link;
link.text = L"Link text";
link.isLink = true;
link.linkUrl = L"https://example.com";
link.foregroundColor = Color(0, 0, 255, 255);  // Blue
link.underline = true;

doc->AddSpan(link);
```

### Markdown Support

```cpp
std::wstring markdown = LR"(
# Heading 1
## Heading 2

This is a paragraph with **bold** and *italic* text.

* List item 1
* List item 2
* List item 3

[Link](https://example.com)
)";

auto doc = std::make_shared<RichTextDocument>();
doc->ParseMarkdown(markdown);

richText->SetDocument(doc);
```

### Editing Features

```cpp
// Make editable
richText->SetEditable(true);
richText->SetReadOnly(false);

// Selection
richText->SelectAll();
richText->SetSelectionRange(0, 10);

// Format selection
richText->SetSelectionBold(true);
richText->SetSelectionItalic(true);
richText->SetSelectionUnderline(true);
richText->SetSelectionColor(Color(255, 0, 0, 255));
richText->SetSelectionFontSize(16);

// Scrolling
richText->SetScrollable(true);
richText->ScrollToTop();
richText->ScrollToBottom();
richText->ScrollToPosition(100);
```

---

## Examples

### Complete V1.3 Showcase

See `examples/v13_showcase_demo.cpp` for a comprehensive demonstration of all v1.3 features.

### Quick Example: Effect Preset

```cpp
#include "SDK/SDK.h"

void RenderWithEffect(RenderBackend* backend, EffectPreset preset) {
    backend->BeginDraw();
    backend->Clear(Color(240, 240, 240, 255));
    
    // Render your content here
    // ...
    
    // Apply effect preset to entire window
    RECT windowRect = { 0, 0, 800, 600 };
    backend->ApplyEffectPreset(windowRect, preset);
    
    backend->EndDraw();
}
```

### Quick Example: Layout

```cpp
#include "SDK/Layout.h"

void CreateButtonGrid(std::vector<std::shared_ptr<Button>>& buttons) {
    auto layout = std::make_shared<GridLayout>(4, 3);
    layout->SetSpacing(10);
    
    std::vector<std::shared_ptr<Widget>> widgets(buttons.begin(), buttons.end());
    RECT bounds = { 20, 20, 780, 580 };
    layout->Apply(bounds, widgets);
}
```

### Quick Example: Menu

```cpp
#include "SDK/Menu.h"

std::shared_ptr<MenuBar> CreateMainMenu() {
    auto menuBar = std::make_shared<MenuBar>();
    
    auto fileMenu = std::make_shared<Menu>();
    fileMenu->AddItem(std::make_shared<MenuItem>(L"New"));
    fileMenu->AddItem(std::make_shared<MenuItem>(L"Open"));
    fileMenu->AddItem(std::make_shared<MenuItem>(L"Save"));
    
    menuBar->AddMenu(L"File", fileMenu);
    
    return menuBar;
}
```

---

## Migration from v1.2

Version 1.3 is fully backward compatible with v1.2. All existing code will continue to work without modifications.

### New Features to Adopt

1. **Replace manual effect combinations** with effect presets for cleaner code
2. **Use Bezier curves** for more natural animations
3. **Adopt layout managers** instead of manual positioning
4. **Implement menus** using the new MenuBar and ContextMenu widgets
5. **Enhance text displays** with RichTextBox

### Performance Considerations

- Effect presets may impact performance on low-end hardware
- Use GPU-accelerated backend (Direct2D) when available
- Rich text rendering is more expensive than simple text
- Layouts recalculate on resize - cache when possible

---

## Best Practices

### Visual Effects

1. **Use presets** for consistent styling across your app
2. **Apply effects sparingly** - too many effects can be distracting
3. **Test on target hardware** - effects are GPU-intensive
4. **Limit effect regions** - apply only to necessary areas

### Animations

1. **Choose appropriate easing** - natural motion enhances UX
2. **Keep durations reasonable** - 200-500ms for most transitions
3. **Use groups wisely** - parallel for independence, sequence for choreography
4. **Provide user control** - allow disabling animations

### Layouts

1. **Pick the right layout** - Grid for structured, Flow for flexible, Stack for linear
2. **Set appropriate spacing** - consistent spacing improves readability
3. **Handle resize** - reapply layouts on window size changes
4. **Test with content** - layouts behave differently with varying widget sizes

### Menus

1. **Keep menus shallow** - avoid deep nesting (2-3 levels max)
2. **Add shortcuts** - improve power user efficiency
3. **Use icons judiciously** - only when they aid recognition
4. **Group related items** - use separators to organize

### Rich Text

1. **Limit formatting** - excessive formatting reduces readability
2. **Use semantic structure** - headings, paragraphs, lists
3. **Consider performance** - large documents may scroll slowly
4. **Provide plain text export** - for copy/paste operations

---

## Support and Resources

- **API Reference**: See [API.md](API.md)
- **Architecture**: See [ARCHITECTURE.md](ARCHITECTURE.md)
- **Examples**: Browse `/examples` directory
- **Issues**: [GitHub Issues](https://github.com/kamer1337/5d-gui-framework/issues)

---

**Version 1.3.0**  
**Last Updated**: December 2025
