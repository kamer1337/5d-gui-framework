#include "SDK/Layout.h"
#include <algorithm>
#include <cmath>

namespace SDK {

// ============================================================================
// GridLayout Implementation
// ============================================================================

GridLayout::GridLayout(int columns, int rows)
    : m_columns(columns), m_rows(rows), m_uniformCellSize(true) {
}

void GridLayout::Apply(const RECT& bounds, std::vector<std::shared_ptr<Widget>>& widgets) {
    if (widgets.empty()) return;
    
    int widgetCount = static_cast<int>(widgets.size());
    
    // Calculate actual rows
    int actualRows = m_rows;
    if (actualRows == 0) {
        actualRows = (widgetCount + m_columns - 1) / m_columns;
    }
    
    // Calculate available space
    int availableWidth = (bounds.right - bounds.left) - m_paddingLeft - m_paddingRight;
    int availableHeight = (bounds.bottom - bounds.top) - m_paddingTop - m_paddingBottom;
    
    // Calculate cell dimensions
    int cellWidth = (availableWidth - (m_columns - 1) * m_spacing) / m_columns;
    int cellHeight = (actualRows > 0) ? (availableHeight - (actualRows - 1) * m_spacing) / actualRows : 0;
    
    // Position widgets
    int index = 0;
    for (int row = 0; row < actualRows && index < widgetCount; ++row) {
        for (int col = 0; col < m_columns && index < widgetCount; ++col) {
            auto& widget = widgets[index];
            
            int x = bounds.left + m_paddingLeft + col * (cellWidth + m_spacing);
            int y = bounds.top + m_paddingTop + row * (cellHeight + m_spacing);
            
            if (m_uniformCellSize) {
                widget->SetBounds(x, y, cellWidth, cellHeight);
            } else {
                // Respect widget's preferred size
                int widgetWidth, widgetHeight;
                widget->GetSize(widgetWidth, widgetHeight);
                if (widgetWidth <= 0) widgetWidth = cellWidth;
                if (widgetHeight <= 0) widgetHeight = cellHeight;
                widget->SetPosition(x, y);
                widget->SetSize(std::min(widgetWidth, cellWidth), std::min(widgetHeight, cellHeight));
            }
            
            ++index;
        }
    }
}

// ============================================================================
// FlowLayout Implementation
// ============================================================================

FlowLayout::FlowLayout(Direction direction)
    : m_direction(direction), m_wrap(true), m_alignment(Alignment::START) {
}

void FlowLayout::Apply(const RECT& bounds, std::vector<std::shared_ptr<Widget>>& widgets) {
    if (widgets.empty()) return;
    
    int availableWidth = (bounds.right - bounds.left) - m_paddingLeft - m_paddingRight;
    int availableHeight = (bounds.bottom - bounds.top) - m_paddingTop - m_paddingBottom;
    
    if (m_direction == Direction::LEFT_TO_RIGHT || m_direction == Direction::RIGHT_TO_LEFT) {
        // Horizontal flow
        int currentX = (m_direction == Direction::LEFT_TO_RIGHT) ? 
                       bounds.left + m_paddingLeft : bounds.right - m_paddingRight;
        int currentY = bounds.top + m_paddingTop;
        int rowHeight = 0;
        
        for (auto& widget : widgets) {
            int widgetWidth, widgetHeight;
            widget->GetSize(widgetWidth, widgetHeight);
            
            // Check if we need to wrap
            if (m_wrap && currentX + widgetWidth > bounds.right - m_paddingRight && 
                currentX != bounds.left + m_paddingLeft) {
                // Move to next row
                currentY += rowHeight + m_spacing;
                currentX = (m_direction == Direction::LEFT_TO_RIGHT) ? 
                           bounds.left + m_paddingLeft : bounds.right - m_paddingRight;
                rowHeight = 0;
            }
            
            // Position widget
            int x = (m_direction == Direction::LEFT_TO_RIGHT) ? currentX : currentX - widgetWidth;
            widget->SetPosition(x, currentY);
            
            // Update position for next widget
            currentX += (m_direction == Direction::LEFT_TO_RIGHT) ? 
                       (widgetWidth + m_spacing) : -(widgetWidth + m_spacing);
            rowHeight = std::max(rowHeight, widgetHeight);
        }
    } else {
        // Vertical flow
        int currentX = bounds.left + m_paddingLeft;
        int currentY = (m_direction == Direction::TOP_TO_BOTTOM) ? 
                       bounds.top + m_paddingTop : bounds.bottom - m_paddingBottom;
        int colWidth = 0;
        
        for (auto& widget : widgets) {
            int widgetWidth, widgetHeight;
            widget->GetSize(widgetWidth, widgetHeight);
            
            // Check if we need to wrap
            if (m_wrap && currentY + widgetHeight > bounds.bottom - m_paddingBottom && 
                currentY != bounds.top + m_paddingTop) {
                // Move to next column
                currentX += colWidth + m_spacing;
                currentY = (m_direction == Direction::TOP_TO_BOTTOM) ? 
                           bounds.top + m_paddingTop : bounds.bottom - m_paddingBottom;
                colWidth = 0;
            }
            
            // Position widget
            int y = (m_direction == Direction::TOP_TO_BOTTOM) ? currentY : currentY - widgetHeight;
            widget->SetPosition(currentX, y);
            
            // Update position for next widget
            currentY += (m_direction == Direction::TOP_TO_BOTTOM) ? 
                       (widgetHeight + m_spacing) : -(widgetHeight + m_spacing);
            colWidth = std::max(colWidth, widgetWidth);
        }
    }
}

// ============================================================================
// StackLayout Implementation
// ============================================================================

StackLayout::StackLayout(Orientation orientation)
    : m_orientation(orientation), m_distribution(Distribution::START) {
}

void StackLayout::Apply(const RECT& bounds, std::vector<std::shared_ptr<Widget>>& widgets) {
    if (widgets.empty()) return;
    
    int availableWidth = (bounds.right - bounds.left) - m_paddingLeft - m_paddingRight;
    int availableHeight = (bounds.bottom - bounds.top) - m_paddingTop - m_paddingBottom;
    
    // Calculate total size of all widgets
    int totalSize = 0;
    for (auto& widget : widgets) {
        int w, h;
        widget->GetSize(w, h);
        totalSize += (m_orientation == Orientation::VERTICAL) ? h : w;
    }
    
    // Add spacing between widgets
    int totalSpacing = m_spacing * (static_cast<int>(widgets.size()) - 1);
    totalSize += totalSpacing;
    
    // Calculate starting position based on distribution
    int currentPos = 0;
    int availableSpace = (m_orientation == Orientation::VERTICAL) ? availableHeight : availableWidth;
    int extraSpace = std::max(0, availableSpace - totalSize);
    
    switch (m_distribution) {
        case Distribution::START:
            currentPos = (m_orientation == Orientation::VERTICAL) ? 
                        bounds.top + m_paddingTop : bounds.left + m_paddingLeft;
            break;
        case Distribution::END:
            currentPos = (m_orientation == Orientation::VERTICAL) ? 
                        bounds.bottom - m_paddingBottom - totalSize : 
                        bounds.right - m_paddingRight - totalSize;
            break;
        case Distribution::CENTER:
            currentPos = (m_orientation == Orientation::VERTICAL) ? 
                        bounds.top + m_paddingTop + extraSpace / 2 : 
                        bounds.left + m_paddingLeft + extraSpace / 2;
            break;
        case Distribution::SPACE_BETWEEN: {
            currentPos = (m_orientation == Orientation::VERTICAL) ? 
                        bounds.top + m_paddingTop : bounds.left + m_paddingLeft;
            int widgetCount = static_cast<int>(widgets.size());
            if (widgetCount > 1) {
                totalSpacing = extraSpace;
            }
            break;
        }
        case Distribution::SPACE_AROUND: {
            currentPos = (m_orientation == Orientation::VERTICAL) ? 
                        bounds.top + m_paddingTop : bounds.left + m_paddingLeft;
            int widgetCount = static_cast<int>(widgets.size());
            int spaceAround = extraSpace / (widgetCount * 2);
            currentPos += spaceAround;
            break;
        }
        case Distribution::SPACE_EVENLY: {
            int widgetCount = static_cast<int>(widgets.size());
            int evenSpace = extraSpace / (widgetCount + 1);
            currentPos = (m_orientation == Orientation::VERTICAL) ? 
                        bounds.top + m_paddingTop + evenSpace : 
                        bounds.left + m_paddingLeft + evenSpace;
            break;
        }
    }
    
    // Position widgets
    int index = 0;
    for (auto& widget : widgets) {
        int widgetWidth, widgetHeight;
        widget->GetSize(widgetWidth, widgetHeight);
        
        if (m_orientation == Orientation::VERTICAL) {
            int x = bounds.left + m_paddingLeft;
            widget->SetPosition(x, currentPos);
            
            // Calculate spacing for next widget
            int spacing = m_spacing;
            if (m_distribution == Distribution::SPACE_BETWEEN && widgets.size() > 1) {
                spacing = extraSpace / (static_cast<int>(widgets.size()) - 1);
            } else if (m_distribution == Distribution::SPACE_AROUND) {
                spacing = extraSpace / static_cast<int>(widgets.size());
            } else if (m_distribution == Distribution::SPACE_EVENLY) {
                spacing = extraSpace / (static_cast<int>(widgets.size()) + 1);
            }
            
            currentPos += widgetHeight + spacing;
        } else {
            int y = bounds.top + m_paddingTop;
            widget->SetPosition(currentPos, y);
            
            // Calculate spacing for next widget
            int spacing = m_spacing;
            if (m_distribution == Distribution::SPACE_BETWEEN && widgets.size() > 1) {
                spacing = extraSpace / (static_cast<int>(widgets.size()) - 1);
            } else if (m_distribution == Distribution::SPACE_AROUND) {
                spacing = extraSpace / static_cast<int>(widgets.size());
            } else if (m_distribution == Distribution::SPACE_EVENLY) {
                spacing = extraSpace / (static_cast<int>(widgets.size()) + 1);
            }
            
            currentPos += widgetWidth + spacing;
        }
        
        ++index;
    }
}

// ============================================================================
// LayoutConstraintSolver Implementation
// ============================================================================

LayoutConstraintSolver::LayoutConstraintSolver()
    : m_converged(false), m_iterations(0) {
}

LayoutConstraintSolver::~LayoutConstraintSolver() {
}

void LayoutConstraintSolver::AddConstraint(const LayoutConstraint& constraint) {
    m_constraints.push_back(constraint);
}

void LayoutConstraintSolver::ClearConstraints() {
    m_constraints.clear();
}

int LayoutConstraintSolver::GetAttributeValue(std::shared_ptr<Widget> widget, 
                                               LayoutConstraint::Attribute attr) const {
    if (!widget) return 0;
    
    RECT bounds;
    widget->GetBounds(bounds);
    
    switch (attr) {
        case LayoutConstraint::Attribute::LEFT:
            return bounds.left;
        case LayoutConstraint::Attribute::RIGHT:
            return bounds.right;
        case LayoutConstraint::Attribute::TOP:
            return bounds.top;
        case LayoutConstraint::Attribute::BOTTOM:
            return bounds.bottom;
        case LayoutConstraint::Attribute::WIDTH:
            return bounds.right - bounds.left;
        case LayoutConstraint::Attribute::HEIGHT:
            return bounds.bottom - bounds.top;
        case LayoutConstraint::Attribute::CENTER_X:
            return (bounds.left + bounds.right) / 2;
        case LayoutConstraint::Attribute::CENTER_Y:
            return (bounds.top + bounds.bottom) / 2;
    }
    return 0;
}

void LayoutConstraintSolver::SetAttributeValue(std::shared_ptr<Widget> widget, 
                                                LayoutConstraint::Attribute attr, int value) {
    if (!widget) return;
    
    RECT bounds;
    widget->GetBounds(bounds);
    int width = bounds.right - bounds.left;
    int height = bounds.bottom - bounds.top;
    
    switch (attr) {
        case LayoutConstraint::Attribute::LEFT:
            widget->SetPosition(value, bounds.top);
            break;
        case LayoutConstraint::Attribute::RIGHT:
            widget->SetPosition(value - width, bounds.top);
            break;
        case LayoutConstraint::Attribute::TOP:
            widget->SetPosition(bounds.left, value);
            break;
        case LayoutConstraint::Attribute::BOTTOM:
            widget->SetPosition(bounds.left, value - height);
            break;
        case LayoutConstraint::Attribute::WIDTH:
            widget->SetSize(value, height);
            break;
        case LayoutConstraint::Attribute::HEIGHT:
            widget->SetSize(width, value);
            break;
        case LayoutConstraint::Attribute::CENTER_X:
            widget->SetPosition(value - width / 2, bounds.top);
            break;
        case LayoutConstraint::Attribute::CENTER_Y:
            widget->SetPosition(bounds.left, value - height / 2);
            break;
    }
}

float LayoutConstraintSolver::EvaluateConstraintError(const LayoutConstraint& constraint) const {
    int value1 = GetAttributeValue(constraint.GetWidget1(), constraint.GetAttribute1());
    int value2 = 0;
    
    if (constraint.GetWidget2()) {
        value2 = GetAttributeValue(constraint.GetWidget2(), constraint.GetAttribute2());
    }
    
    int targetValue = value2 + constraint.GetConstant();
    
    switch (constraint.GetType()) {
        case LayoutConstraint::Type::EQUAL:
            return static_cast<float>(std::abs(value1 - targetValue));
        case LayoutConstraint::Type::LESS_THAN_OR_EQUAL:
            return (value1 > targetValue) ? static_cast<float>(value1 - targetValue) : 0.0f;
        case LayoutConstraint::Type::GREATER_THAN_OR_EQUAL:
            return (value1 < targetValue) ? static_cast<float>(targetValue - value1) : 0.0f;
    }
    return 0.0f;
}

bool LayoutConstraintSolver::Solve(const RECT& bounds, 
                                    std::vector<std::shared_ptr<Widget>>& widgets,
                                    int maxIterations, float tolerance) {
    m_converged = false;
    m_iterations = 0;
    
    if (m_constraints.empty()) {
        m_converged = true;
        return true;
    }
    
    // Sort constraints by priority
    std::sort(m_constraints.begin(), m_constraints.end(),
              [](const LayoutConstraint& a, const LayoutConstraint& b) {
                  return a.GetPriority() > b.GetPriority();
              });
    
    // Iterative constraint solving
    for (int iter = 0; iter < maxIterations; ++iter) {
        m_iterations = iter + 1;
        float maxError = 0.0f;
        
        // Process each constraint
        for (const auto& constraint : m_constraints) {
            float error = EvaluateConstraintError(constraint);
            maxError = std::max(maxError, error);
            
            if (error > tolerance) {
                // Adjust widget position to satisfy constraint
                int value1 = GetAttributeValue(constraint.GetWidget1(), constraint.GetAttribute1());
                int value2 = 0;
                
                if (constraint.GetWidget2()) {
                    value2 = GetAttributeValue(constraint.GetWidget2(), constraint.GetAttribute2());
                } else {
                    // Constraint to fixed value
                    value2 = 0;
                }
                
                int targetValue = value2 + constraint.GetConstant();
                
                // Apply correction (damped to improve convergence)
                float damping = 0.5f;
                int correction = static_cast<int>((targetValue - value1) * damping);
                
                if (constraint.GetType() == LayoutConstraint::Type::EQUAL) {
                    int newValue = value1 + correction;
                    SetAttributeValue(constraint.GetWidget1(), constraint.GetAttribute1(), newValue);
                } else if (constraint.GetType() == LayoutConstraint::Type::LESS_THAN_OR_EQUAL) {
                    if (value1 > targetValue) {
                        int newValue = value1 + correction;
                        SetAttributeValue(constraint.GetWidget1(), constraint.GetAttribute1(), newValue);
                    }
                } else if (constraint.GetType() == LayoutConstraint::Type::GREATER_THAN_OR_EQUAL) {
                    if (value1 < targetValue) {
                        int newValue = value1 + correction;
                        SetAttributeValue(constraint.GetWidget1(), constraint.GetAttribute1(), newValue);
                    }
                }
            }
        }
        
        // Check for convergence
        if (maxError <= tolerance) {
            m_converged = true;
            return true;
        }
    }
    
    return m_converged;
}

// ============================================================================
// LayoutEngine Implementation
// ============================================================================

LayoutEngine::LayoutEngine()
    : m_baseLayout(nullptr), m_autoLayout(false) {
}

LayoutEngine::~LayoutEngine() {
}

void LayoutEngine::SetBaseLayout(std::shared_ptr<Layout> layout) {
    m_baseLayout = layout;
}

void LayoutEngine::AddConstraint(const LayoutConstraint& constraint) {
    m_solver.AddConstraint(constraint);
}

void LayoutEngine::ClearConstraints() {
    m_solver.ClearConstraints();
}

void LayoutEngine::Apply(const RECT& bounds, std::vector<std::shared_ptr<Widget>>& widgets) {
    if (widgets.empty()) return;
    
    // Determine layout
    std::shared_ptr<Layout> layout = m_baseLayout;
    if (m_autoLayout) {
        layout = DetermineOptimalLayout(bounds, widgets);
    }
    
    // Apply base layout if available
    if (layout) {
        layout->Apply(bounds, widgets);
    }
    
    // Apply constraints for fine-tuning
    m_solver.Solve(bounds, widgets);
}

std::shared_ptr<Layout> LayoutEngine::SuggestLayout(int widgetCount, 
                                                     int containerWidth, 
                                                     int containerHeight) {
    if (widgetCount <= 0) {
        return std::make_shared<StackLayout>(StackLayout::Orientation::VERTICAL);
    }
    
    float aspectRatio = static_cast<float>(containerWidth) / static_cast<float>(containerHeight);
    
    // For few widgets, use stack layout
    if (widgetCount <= 3) {
        if (aspectRatio > 1.5f) {
            return std::make_shared<StackLayout>(StackLayout::Orientation::HORIZONTAL);
        } else {
            return std::make_shared<StackLayout>(StackLayout::Orientation::VERTICAL);
        }
    }
    
    // For many widgets, use grid layout
    if (widgetCount > 10) {
        // Calculate optimal columns based on aspect ratio
        int columns = static_cast<int>(std::sqrt(widgetCount * aspectRatio));
        columns = std::max(2, std::min(columns, 6)); // Clamp between 2-6 columns
        return std::make_shared<GridLayout>(columns, 0);
    }
    
    // For medium number of widgets, use flow layout
    return std::make_shared<FlowLayout>(FlowLayout::Direction::LEFT_TO_RIGHT);
}

std::shared_ptr<Layout> LayoutEngine::DetermineOptimalLayout(
    const RECT& bounds, const std::vector<std::shared_ptr<Widget>>& widgets) {
    
    int containerWidth = bounds.right - bounds.left;
    int containerHeight = bounds.bottom - bounds.top;
    int widgetCount = static_cast<int>(widgets.size());
    
    return SuggestLayout(widgetCount, containerWidth, containerHeight);
}

} // namespace SDK
