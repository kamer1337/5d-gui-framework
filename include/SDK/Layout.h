#pragma once

#include "Widget.h"
#include <memory>
#include <vector>

namespace SDK {

/**
 * Layout - Base class for automatic widget layout
 */
class Layout {
public:
    enum class LayoutType {
        NONE,
        GRID,
        FLOW,
        STACK
    };
    
    virtual ~Layout() = default;
    
    // Layout widgets within the given bounds
    virtual void Apply(const RECT& bounds, std::vector<std::shared_ptr<Widget>>& widgets) = 0;
    
    // Get layout type
    virtual LayoutType GetType() const = 0;
    
    // Spacing configuration
    void SetSpacing(int spacing) { m_spacing = spacing; }
    int GetSpacing() const { return m_spacing; }
    
    void SetPadding(int left, int top, int right, int bottom) {
        m_paddingLeft = left;
        m_paddingTop = top;
        m_paddingRight = right;
        m_paddingBottom = bottom;
    }
    
protected:
    int m_spacing = 5;
    int m_paddingLeft = 10;
    int m_paddingTop = 10;
    int m_paddingRight = 10;
    int m_paddingBottom = 10;
};

/**
 * GridLayout - Arranges widgets in a grid
 */
class GridLayout : public Layout {
public:
    GridLayout(int columns = 3, int rows = 0);  // rows=0 means auto
    ~GridLayout() override = default;
    
    void Apply(const RECT& bounds, std::vector<std::shared_ptr<Widget>>& widgets) override;
    LayoutType GetType() const override { return LayoutType::GRID; }
    
    // Grid configuration
    void SetColumns(int columns) { m_columns = columns; }
    int GetColumns() const { return m_columns; }
    
    void SetRows(int rows) { m_rows = rows; }
    int GetRows() const { return m_rows; }
    
    // Cell sizing
    void SetUniformCellSize(bool uniform) { m_uniformCellSize = uniform; }
    bool IsUniformCellSize() const { return m_uniformCellSize; }
    
private:
    int m_columns;
    int m_rows;
    bool m_uniformCellSize;
};

/**
 * FlowLayout - Arranges widgets left-to-right, top-to-bottom with wrapping
 */
class FlowLayout : public Layout {
public:
    enum class Direction {
        LEFT_TO_RIGHT,
        RIGHT_TO_LEFT,
        TOP_TO_BOTTOM,
        BOTTOM_TO_TOP
    };
    
    FlowLayout(Direction direction = Direction::LEFT_TO_RIGHT);
    ~FlowLayout() override = default;
    
    void Apply(const RECT& bounds, std::vector<std::shared_ptr<Widget>>& widgets) override;
    LayoutType GetType() const override { return LayoutType::FLOW; }
    
    // Flow configuration
    void SetDirection(Direction direction) { m_direction = direction; }
    Direction GetDirection() const { return m_direction; }
    
    void SetWrap(bool wrap) { m_wrap = wrap; }
    bool IsWrap() const { return m_wrap; }
    
    // Alignment
    enum class Alignment {
        START,
        CENTER,
        END,
        STRETCH
    };
    
    void SetAlignment(Alignment alignment) { m_alignment = alignment; }
    Alignment GetAlignment() const { return m_alignment; }
    
private:
    Direction m_direction;
    bool m_wrap;
    Alignment m_alignment;
};

/**
 * StackLayout - Stacks widgets vertically or horizontally
 */
class StackLayout : public Layout {
public:
    enum class Orientation {
        VERTICAL,
        HORIZONTAL
    };
    
    StackLayout(Orientation orientation = Orientation::VERTICAL);
    ~StackLayout() override = default;
    
    void Apply(const RECT& bounds, std::vector<std::shared_ptr<Widget>>& widgets) override;
    LayoutType GetType() const override { return LayoutType::STACK; }
    
    // Stack configuration
    void SetOrientation(Orientation orientation) { m_orientation = orientation; }
    Orientation GetOrientation() const { return m_orientation; }
    
    // Distribution
    enum class Distribution {
        START,      // Pack to start
        END,        // Pack to end
        CENTER,     // Pack to center
        SPACE_BETWEEN,  // Equal space between widgets
        SPACE_AROUND,   // Equal space around widgets
        SPACE_EVENLY    // Equal space including edges
    };
    
    void SetDistribution(Distribution distribution) { m_distribution = distribution; }
    Distribution GetDistribution() const { return m_distribution; }
    
private:
    Orientation m_orientation;
    Distribution m_distribution;
};

/**
 * LayoutConstraint - Represents a layout constraint between widgets or to container
 */
class LayoutConstraint {
public:
    enum class Type {
        EQUAL,              // Widget1.attr == Widget2.attr + constant
        LESS_THAN_OR_EQUAL, // Widget1.attr <= Widget2.attr + constant
        GREATER_THAN_OR_EQUAL // Widget1.attr >= Widget2.attr + constant
    };
    
    enum class Attribute {
        LEFT,
        RIGHT,
        TOP,
        BOTTOM,
        WIDTH,
        HEIGHT,
        CENTER_X,
        CENTER_Y
    };
    
    LayoutConstraint(std::shared_ptr<Widget> widget1, Attribute attr1,
                     Type type, 
                     std::shared_ptr<Widget> widget2, Attribute attr2,
                     int constant = 0)
        : m_widget1(widget1), m_attr1(attr1), m_type(type),
          m_widget2(widget2), m_attr2(attr2), m_constant(constant),
          m_priority(1000) {}
    
    // Constraint to container bounds
    LayoutConstraint(std::shared_ptr<Widget> widget, Attribute attr,
                     Type type, int value)
        : m_widget1(widget), m_attr1(attr), m_type(type),
          m_widget2(nullptr), m_attr2(Attribute::LEFT), m_constant(value),
          m_priority(1000) {}
    
    std::shared_ptr<Widget> GetWidget1() const { return m_widget1; }
    Attribute GetAttribute1() const { return m_attr1; }
    Type GetType() const { return m_type; }
    std::shared_ptr<Widget> GetWidget2() const { return m_widget2; }
    Attribute GetAttribute2() const { return m_attr2; }
    int GetConstant() const { return m_constant; }
    
    void SetPriority(int priority) { m_priority = priority; }
    int GetPriority() const { return m_priority; }
    
private:
    std::shared_ptr<Widget> m_widget1;
    Attribute m_attr1;
    Type m_type;
    std::shared_ptr<Widget> m_widget2;
    Attribute m_attr2;
    int m_constant;
    int m_priority;  // Higher priority constraints are satisfied first
};

/**
 * LayoutConstraintSolver - Solves layout constraints using iterative relaxation
 */
class LayoutConstraintSolver {
public:
    LayoutConstraintSolver();
    ~LayoutConstraintSolver();
    
    // Add constraint to the solver
    void AddConstraint(const LayoutConstraint& constraint);
    void ClearConstraints();
    
    // Solve constraints for widgets within bounds
    bool Solve(const RECT& bounds, std::vector<std::shared_ptr<Widget>>& widgets,
               int maxIterations = 100, float tolerance = 0.1f);
    
    // Get solver status
    bool DidConverge() const { return m_converged; }
    int GetIterationCount() const { return m_iterations; }
    
private:
    std::vector<LayoutConstraint> m_constraints;
    bool m_converged;
    int m_iterations;
    
    // Helper methods
    int GetAttributeValue(std::shared_ptr<Widget> widget, LayoutConstraint::Attribute attr) const;
    void SetAttributeValue(std::shared_ptr<Widget> widget, LayoutConstraint::Attribute attr, int value);
    float EvaluateConstraintError(const LayoutConstraint& constraint) const;
};

/**
 * LayoutEngine - Automatic layout management with constraint-based positioning
 */
class LayoutEngine {
public:
    LayoutEngine();
    ~LayoutEngine();
    
    // Set the base layout (grid, flow, stack)
    void SetBaseLayout(std::shared_ptr<Layout> layout);
    std::shared_ptr<Layout> GetBaseLayout() const { return m_baseLayout; }
    
    // Add constraints for fine-tuning
    void AddConstraint(const LayoutConstraint& constraint);
    void ClearConstraints();
    
    // Apply layout with constraints
    void Apply(const RECT& bounds, std::vector<std::shared_ptr<Widget>>& widgets);
    
    // Auto-layout mode: automatically determines best layout
    void SetAutoLayoutMode(bool enabled) { m_autoLayout = enabled; }
    bool IsAutoLayoutMode() const { return m_autoLayout; }
    
    // Get suggested layout based on widget count and container size
    static std::shared_ptr<Layout> SuggestLayout(int widgetCount, int containerWidth, int containerHeight);
    
private:
    std::shared_ptr<Layout> m_baseLayout;
    LayoutConstraintSolver m_solver;
    bool m_autoLayout;
    
    // Auto-layout heuristics
    std::shared_ptr<Layout> DetermineOptimalLayout(const RECT& bounds, 
                                                    const std::vector<std::shared_ptr<Widget>>& widgets);
};

} // namespace SDK
