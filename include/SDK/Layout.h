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

} // namespace SDK
