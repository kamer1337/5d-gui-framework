# Advanced Layout System Guide

This guide explains the advanced layout features in the 5D GUI SDK, including the automatic layout engine and constraint solver.

## Overview

The 5D GUI SDK provides a comprehensive layout system with three levels:

1. **Basic Layouts**: GridLayout, FlowLayout, StackLayout
2. **Layout Engine**: Automatic layout management with heuristics
3. **Constraint Solver**: Fine-grained control with layout constraints

## Basic Layouts

### GridLayout

Arranges widgets in a grid pattern with configurable rows and columns.

```cpp
// Create a 3-column grid with automatic rows
auto gridLayout = std::make_shared<SDK::GridLayout>(3, 0);

// Configure spacing and padding
gridLayout->SetSpacing(10);
gridLayout->SetPadding(20, 20, 20, 20);

// Apply to widgets
RECT bounds = { 0, 0, 800, 600 };
gridLayout->Apply(bounds, widgets);
```

**Features:**
- Automatic row calculation
- Uniform or content-based cell sizing
- Configurable spacing and padding

### FlowLayout

Arranges widgets in a flowing pattern with automatic wrapping.

```cpp
// Create left-to-right flow layout
auto flowLayout = std::make_shared<SDK::FlowLayout>(
    SDK::FlowLayout::Direction::LEFT_TO_RIGHT);

// Configure wrapping and alignment
flowLayout->SetWrap(true);
flowLayout->SetAlignment(SDK::FlowLayout::Alignment::CENTER);

// Apply to widgets
flowLayout->Apply(bounds, widgets);
```

**Features:**
- Four directions: LEFT_TO_RIGHT, RIGHT_TO_LEFT, TOP_TO_BOTTOM, BOTTOM_TO_TOP
- Optional wrapping
- Alignment options: START, CENTER, END, STRETCH

### StackLayout

Stacks widgets vertically or horizontally with distribution control.

```cpp
// Create vertical stack layout
auto stackLayout = std::make_shared<SDK::StackLayout>(
    SDK::StackLayout::Orientation::VERTICAL);

// Configure distribution
stackLayout->SetDistribution(SDK::StackLayout::Distribution::SPACE_EVENLY);

// Apply to widgets
stackLayout->Apply(bounds, widgets);
```

**Features:**
- Vertical or horizontal orientation
- Six distribution modes:
  - START: Pack to start
  - END: Pack to end
  - CENTER: Pack to center
  - SPACE_BETWEEN: Equal space between widgets
  - SPACE_AROUND: Equal space around widgets
  - SPACE_EVENLY: Equal space including edges

## Layout Engine

The LayoutEngine provides automatic layout selection and constraint-based fine-tuning.

### Automatic Layout

The engine can automatically select the best layout based on widget count and container size:

```cpp
// Create layout engine
auto engine = std::make_shared<SDK::LayoutEngine>();

// Enable auto-layout mode
engine->SetAutoLayoutMode(true);

// Apply - engine will choose optimal layout
RECT bounds = { 0, 0, 800, 600 };
engine->Apply(bounds, widgets);
```

**Auto-Layout Heuristics:**
- **Few widgets (≤3)**: Stack layout (horizontal for wide containers, vertical for tall)
- **Many widgets (>10)**: Grid layout with optimal column count
- **Medium widgets (4-10)**: Flow layout with wrapping

### Manual Layout with Engine

You can also specify a base layout and let the engine apply it:

```cpp
auto engine = std::make_shared<SDK::LayoutEngine>();

// Set base layout
engine->SetBaseLayout(std::make_shared<SDK::GridLayout>(4, 0));

// Auto-layout is disabled by default
engine->Apply(bounds, widgets);
```

### Suggested Layout

Get layout suggestions without creating an engine:

```cpp
int widgetCount = 12;
int containerWidth = 800;
int containerHeight = 600;

auto suggestedLayout = SDK::LayoutEngine::SuggestLayout(
    widgetCount, containerWidth, containerHeight);

suggestedLayout->Apply(bounds, widgets);
```

## Constraint Solver

The LayoutConstraintSolver provides precise control over widget positioning using constraints.

### Constraint Types

Three constraint types are supported:

- **EQUAL**: `widget1.attr == widget2.attr + constant`
- **LESS_THAN_OR_EQUAL**: `widget1.attr <= widget2.attr + constant`
- **GREATER_THAN_OR_EQUAL**: `widget1.attr >= widget2.attr + constant`

### Constraint Attributes

Constraints can be applied to these widget attributes:

- **LEFT**: Left edge position
- **RIGHT**: Right edge position
- **TOP**: Top edge position
- **BOTTOM**: Bottom edge position
- **WIDTH**: Widget width
- **HEIGHT**: Widget height
- **CENTER_X**: Horizontal center position
- **CENTER_Y**: Vertical center position

### Creating Constraints

#### Widget-to-Widget Constraints

```cpp
// Position widget2 10 pixels to the right of widget1
SDK::LayoutConstraint constraint(
    widget2, SDK::LayoutConstraint::Attribute::LEFT,
    SDK::LayoutConstraint::Type::EQUAL,
    widget1, SDK::LayoutConstraint::Attribute::RIGHT,
    10  // constant offset
);
```

#### Widget-to-Container Constraints

```cpp
// Fix widget to 20 pixels from container top
SDK::LayoutConstraint constraint(
    widget, SDK::LayoutConstraint::Attribute::TOP,
    SDK::LayoutConstraint::Type::EQUAL,
    20  // absolute value
);
```

### Using the Constraint Solver

#### Direct Solver Usage

```cpp
SDK::LayoutConstraintSolver solver;

// Add constraints
solver.AddConstraint(SDK::LayoutConstraint(
    widget1, SDK::LayoutConstraint::Attribute::LEFT,
    SDK::LayoutConstraint::Type::EQUAL, 20));

solver.AddConstraint(SDK::LayoutConstraint(
    widget2, SDK::LayoutConstraint::Attribute::LEFT,
    SDK::LayoutConstraint::Type::EQUAL,
    widget1, SDK::LayoutConstraint::Attribute::RIGHT, 10));

// Solve constraints
bool converged = solver.Solve(bounds, widgets);

if (converged) {
    // Constraints satisfied
    int iterations = solver.GetIterationCount();
}
```

#### Solver Parameters

```cpp
// Solve with custom parameters
bool converged = solver.Solve(
    bounds,
    widgets,
    100,    // max iterations (default: 100)
    0.1f    // tolerance in pixels (default: 0.1)
);
```

### Layout Engine with Constraints

Combine base layout with constraint-based fine-tuning:

```cpp
auto engine = std::make_shared<SDK::LayoutEngine>();

// Set base layout (optional)
engine->SetBaseLayout(std::make_shared<SDK::FlowLayout>());

// Add constraints for fine-tuning
engine->AddConstraint(SDK::LayoutConstraint(
    importantWidget, SDK::LayoutConstraint::Attribute::TOP,
    SDK::LayoutConstraint::Type::EQUAL, 10));

engine->AddConstraint(SDK::LayoutConstraint(
    importantWidget, SDK::LayoutConstraint::Attribute::WIDTH,
    SDK::LayoutConstraint::Type::GREATER_THAN_OR_EQUAL, 200));

// Apply layout with constraints
engine->Apply(bounds, widgets);
```

## Common Patterns

### Centering a Widget

```cpp
engine->AddConstraint(SDK::LayoutConstraint(
    widget, SDK::LayoutConstraint::Attribute::CENTER_X,
    SDK::LayoutConstraint::Type::EQUAL,
    containerWidth / 2));

engine->AddConstraint(SDK::LayoutConstraint(
    widget, SDK::LayoutConstraint::Attribute::CENTER_Y,
    SDK::LayoutConstraint::Type::EQUAL,
    containerHeight / 2));
```

### Aligning Widgets

```cpp
// Align tops
engine->AddConstraint(SDK::LayoutConstraint(
    widget2, SDK::LayoutConstraint::Attribute::TOP,
    SDK::LayoutConstraint::Type::EQUAL,
    widget1, SDK::LayoutConstraint::Attribute::TOP, 0));

// Align centers vertically
engine->AddConstraint(SDK::LayoutConstraint(
    widget2, SDK::LayoutConstraint::Attribute::CENTER_Y,
    SDK::LayoutConstraint::Type::EQUAL,
    widget1, SDK::LayoutConstraint::Attribute::CENTER_Y, 0));
```

### Maintaining Aspect Ratio

```cpp
// Set initial size
widget->SetSize(400, 300); // 4:3 aspect ratio

// Constrain width to height
// Note: This requires multiple iterations to satisfy
engine->AddConstraint(SDK::LayoutConstraint(
    widget, SDK::LayoutConstraint::Attribute::WIDTH,
    SDK::LayoutConstraint::Type::EQUAL,
    widget, SDK::LayoutConstraint::Attribute::HEIGHT,
    widget->GetWidth() / widget->GetHeight()));
```

### Minimum/Maximum Sizes

```cpp
// Minimum width
engine->AddConstraint(SDK::LayoutConstraint(
    widget, SDK::LayoutConstraint::Attribute::WIDTH,
    SDK::LayoutConstraint::Type::GREATER_THAN_OR_EQUAL, 100));

// Maximum height
engine->AddConstraint(SDK::LayoutConstraint(
    widget, SDK::LayoutConstraint::Attribute::HEIGHT,
    SDK::LayoutConstraint::Type::LESS_THAN_OR_EQUAL, 200));
```

### Equal Sizing

```cpp
// Make widgets same width
for (size_t i = 1; i < widgets.size(); ++i) {
    engine->AddConstraint(SDK::LayoutConstraint(
        widgets[i], SDK::LayoutConstraint::Attribute::WIDTH,
        SDK::LayoutConstraint::Type::EQUAL,
        widgets[0], SDK::LayoutConstraint::Attribute::WIDTH, 0));
}
```

## Performance Considerations

### Solver Convergence

The constraint solver uses iterative relaxation:

- **Typical iterations**: 5-20 for simple layouts
- **Complex layouts**: May require up to 100 iterations
- **Non-convergence**: Occurs with conflicting constraints

### Optimization Tips

1. **Use base layouts**: Apply a base layout first, then add constraints for fine-tuning
2. **Minimize constraints**: Only add necessary constraints
3. **Set priorities**: Use `SetPriority()` on critical constraints
4. **Cache layouts**: Reuse layout objects when possible
5. **Batch updates**: Apply layout once per frame, not per widget change

### Priority System

Constraints can have priorities (higher = more important):

```cpp
auto criticalConstraint = SDK::LayoutConstraint(...);
criticalConstraint.SetPriority(2000); // High priority

auto niceToHaveConstraint = SDK::LayoutConstraint(...);
niceToHaveConstraint.SetPriority(500); // Lower priority

solver.AddConstraint(criticalConstraint);
solver.AddConstraint(niceToHaveConstraint);
```

## Examples

### Dashboard Layout

```cpp
auto engine = std::make_shared<SDK::LayoutEngine>();

// Header at top, full width
engine->AddConstraint(SDK::LayoutConstraint(
    header, SDK::LayoutConstraint::Attribute::LEFT,
    SDK::LayoutConstraint::Type::EQUAL, 0));
engine->AddConstraint(SDK::LayoutConstraint(
    header, SDK::LayoutConstraint::Attribute::TOP,
    SDK::LayoutConstraint::Type::EQUAL, 0));
engine->AddConstraint(SDK::LayoutConstraint(
    header, SDK::LayoutConstraint::Attribute::RIGHT,
    SDK::LayoutConstraint::Type::EQUAL, containerWidth));

// Sidebar on left
engine->AddConstraint(SDK::LayoutConstraint(
    sidebar, SDK::LayoutConstraint::Attribute::LEFT,
    SDK::LayoutConstraint::Type::EQUAL, 0));
engine->AddConstraint(SDK::LayoutConstraint(
    sidebar, SDK::LayoutConstraint::Attribute::TOP,
    SDK::LayoutConstraint::Type::EQUAL,
    header, SDK::LayoutConstraint::Attribute::BOTTOM, 0));
engine->AddConstraint(SDK::LayoutConstraint(
    sidebar, SDK::LayoutConstraint::Attribute::WIDTH,
    SDK::LayoutConstraint::Type::EQUAL, 200));

// Main content fills remaining space
engine->AddConstraint(SDK::LayoutConstraint(
    content, SDK::LayoutConstraint::Attribute::LEFT,
    SDK::LayoutConstraint::Type::EQUAL,
    sidebar, SDK::LayoutConstraint::Attribute::RIGHT, 0));
engine->AddConstraint(SDK::LayoutConstraint(
    content, SDK::LayoutConstraint::Attribute::TOP,
    SDK::LayoutConstraint::Type::EQUAL,
    header, SDK::LayoutConstraint::Attribute::BOTTOM, 0));
```

### Form Layout

```cpp
auto engine = std::make_shared<SDK::LayoutEngine>();

std::vector<std::shared_ptr<Widget>> labels = {...};
std::vector<std::shared_ptr<Widget>> inputs = {...};

for (size_t i = 0; i < labels.size(); ++i) {
    // Labels aligned left
    engine->AddConstraint(SDK::LayoutConstraint(
        labels[i], SDK::LayoutConstraint::Attribute::LEFT,
        SDK::LayoutConstraint::Type::EQUAL, 20));
    
    // Inputs aligned right
    engine->AddConstraint(SDK::LayoutConstraint(
        inputs[i], SDK::LayoutConstraint::Attribute::LEFT,
        SDK::LayoutConstraint::Type::EQUAL, 200));
    
    // Align vertically
    engine->AddConstraint(SDK::LayoutConstraint(
        labels[i], SDK::LayoutConstraint::Attribute::CENTER_Y,
        SDK::LayoutConstraint::Type::EQUAL,
        inputs[i], SDK::LayoutConstraint::Attribute::CENTER_Y, 0));
    
    // Stack vertically
    if (i > 0) {
        engine->AddConstraint(SDK::LayoutConstraint(
            labels[i], SDK::LayoutConstraint::Attribute::TOP,
            SDK::LayoutConstraint::Type::EQUAL,
            labels[i-1], SDK::LayoutConstraint::Attribute::BOTTOM, 20));
    }
}
```

## Demo Application

See `examples/layout_engine_demo.cpp` for a complete demonstration:

- Automatic layout mode
- Grid, flow, and stack layouts
- Constraint-based custom layouts
- Press SPACE to cycle through modes
- Resize window to see responsive behavior

## Troubleshooting

### Constraints Not Satisfied

**Problem**: Solver reports non-convergence

**Solutions**:
1. Check for conflicting constraints
2. Increase max iterations
3. Relax tolerance
4. Use priorities to resolve conflicts

### Layout Jumps or Flickers

**Problem**: Widgets move unexpectedly

**Solutions**:
1. Apply layout less frequently
2. Use damping in solver (built-in)
3. Set initial positions before solving
4. Check constraint order

### Performance Issues

**Problem**: Layout is slow

**Solutions**:
1. Reduce constraint count
2. Use simpler base layouts
3. Cache solver results
4. Profile constraint evaluation

## See Also

- [Widget Guide](WIDGET_GUIDE.md) - Widget system basics
- [API Documentation](API.md) - Complete API reference
- [Examples](examples/) - More code examples
- [Architecture](ARCHITECTURE.md) - System internals

---

**Version**: 1.3.1  
**Last Updated**: December 2025
