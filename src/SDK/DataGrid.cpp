#include "../../include/SDK/DataGrid.h"
#include "../../include/SDK/Renderer.h"
#include <algorithm>
#include <map>

namespace SDK {

DataGrid::DataGrid()
    : Widget()
    , m_sortColumn(-1)
    , m_sortOrder(SortOrder::NONE)
    , m_selectionMode(SelectionMode::SINGLE)
    , m_hoveredRow(-1)
    , m_hoveredColumn(-1)
    , m_editingRow(-1)
    , m_editingColumn(-1)
    , m_virtualScrolling(false)
    , m_visibleRowCount(20)
    , m_firstVisibleRow(0)
    , m_scrollOffset(0)
    , m_headerHeight(30)
    , m_rowHeight(25)
    , m_gridLineColor(200, 200, 200, 255)
    , m_headerColor(240, 240, 240, 255)
    , m_alternateRowColor(250, 250, 250, 255)
    , m_selectionColor(200, 220, 255, 255)
{
    m_width = 600;
    m_height = 400;
}

DataGrid::~DataGrid() {
}

// Column management
void DataGrid::AddColumn(const std::wstring& header, int width) {
    Column col(header, width);
    m_columns.push_back(col);
}

void DataGrid::AddColumn(const Column& column) {
    m_columns.push_back(column);
}

void DataGrid::RemoveColumn(int index) {
    if (index >= 0 && index < (int)m_columns.size()) {
        m_columns.erase(m_columns.begin() + index);
        
        // Remove cells from all rows
        for (auto& row : m_rows) {
            if (index < (int)row.cells.size()) {
                row.cells.erase(row.cells.begin() + index);
            }
        }
    }
}

void DataGrid::ClearColumns() {
    m_columns.clear();
    ClearRows();
}

void DataGrid::SetColumnWidth(int columnIndex, int width) {
    if (columnIndex >= 0 && columnIndex < (int)m_columns.size()) {
        m_columns[columnIndex].width = width;
    }
}

void DataGrid::SetColumnSortable(int columnIndex, bool sortable) {
    if (columnIndex >= 0 && columnIndex < (int)m_columns.size()) {
        m_columns[columnIndex].sortable = sortable;
    }
}

void DataGrid::SetColumnEditable(int columnIndex, bool editable) {
    if (columnIndex >= 0 && columnIndex < (int)m_columns.size()) {
        m_columns[columnIndex].editable = editable;
    }
}

// Row management
void DataGrid::AddRow(const std::vector<std::wstring>& values) {
    Row row;
    for (const auto& value : values) {
        row.cells.push_back(Cell(value));
    }
    m_rows.push_back(row);
    
    if (IsFiltered()) {
        ApplyFilter();
    }
}

void DataGrid::AddRow(const Row& row) {
    m_rows.push_back(row);
    
    if (IsFiltered()) {
        ApplyFilter();
    }
}

void DataGrid::InsertRow(int index, const std::vector<std::wstring>& values) {
    if (index >= 0 && index <= (int)m_rows.size()) {
        Row row;
        for (const auto& value : values) {
            row.cells.push_back(Cell(value));
        }
        m_rows.insert(m_rows.begin() + index, row);
        
        if (IsFiltered()) {
            ApplyFilter();
        }
    }
}

void DataGrid::RemoveRow(int index) {
    if (index >= 0 && index < (int)m_rows.size()) {
        m_rows.erase(m_rows.begin() + index);
        
        if (IsFiltered()) {
            ApplyFilter();
        }
    }
}

void DataGrid::ClearRows() {
    m_rows.clear();
    m_filteredRows.clear();
}

// Cell access
void DataGrid::SetCellValue(int row, int column, const std::wstring& value) {
    if (row >= 0 && row < (int)m_rows.size() && column >= 0 && column < (int)m_rows[row].cells.size()) {
        m_rows[row].cells[column].value = value;
        
        if (IsFiltered()) {
            ApplyFilter();
        }
    }
}

std::wstring DataGrid::GetCellValue(int row, int column) const {
    if (row >= 0 && row < (int)m_rows.size() && column >= 0 && column < (int)m_rows[row].cells.size()) {
        return m_rows[row].cells[column].value;
    }
    return L"";
}

DataGrid::Cell& DataGrid::GetCell(int row, int column) {
    return m_rows[row].cells[column];
}

const DataGrid::Cell& DataGrid::GetCell(int row, int column) const {
    return m_rows[row].cells[column];
}

// Sorting
void DataGrid::SortByColumn(int columnIndex, SortOrder order) {
    if (columnIndex < 0 || columnIndex >= (int)m_columns.size()) return;
    if (!m_columns[columnIndex].sortable) return;
    
    m_sortColumn = columnIndex;
    m_sortOrder = order;
    
    ApplySorting();
    
    if (m_sortCallback) {
        m_sortCallback(columnIndex, order);
    }
}

void DataGrid::ToggleSort(int columnIndex) {
    if (m_sortColumn == columnIndex) {
        // Cycle through sort orders
        switch (m_sortOrder) {
            case SortOrder::NONE:
                m_sortOrder = SortOrder::ASCENDING;
                break;
            case SortOrder::ASCENDING:
                m_sortOrder = SortOrder::DESCENDING;
                break;
            case SortOrder::DESCENDING:
                m_sortOrder = SortOrder::NONE;
                break;
        }
    } else {
        m_sortColumn = columnIndex;
        m_sortOrder = SortOrder::ASCENDING;
    }
    
    if (m_sortOrder != SortOrder::NONE) {
        ApplySorting();
    }
    
    if (m_sortCallback) {
        m_sortCallback(columnIndex, m_sortOrder);
    }
}

void DataGrid::ApplySorting() {
    if (m_sortColumn < 0 || m_sortOrder == SortOrder::NONE) return;
    
    auto& rows = GetDisplayRows();
    
    std::sort(rows.begin(), rows.end(), [this](const Row& a, const Row& b) {
        if (m_sortColumn >= (int)a.cells.size() || m_sortColumn >= (int)b.cells.size()) {
            return false;
        }
        
        const std::wstring& aVal = a.cells[m_sortColumn].value;
        const std::wstring& bVal = b.cells[m_sortColumn].value;
        
        bool result = aVal < bVal;
        return m_sortOrder == SortOrder::ASCENDING ? result : !result;
    });
}

// Filtering
void DataGrid::SetFilter(const std::wstring& filterText) {
    m_filterText = filterText;
    ApplyFilter();
}

void DataGrid::SetColumnFilter(int columnIndex, const std::wstring& filterText) {
    if (columnIndex >= 0 && columnIndex < (int)m_columns.size()) {
        if (filterText.empty()) {
            m_columnFilters.erase(columnIndex);
        } else {
            m_columnFilters[columnIndex] = filterText;
        }
        ApplyFilter();
    }
}

void DataGrid::ClearFilter() {
    m_filterText.clear();
    m_columnFilters.clear();
    m_filteredRows.clear();
}

void DataGrid::ApplyFilter() {
    m_filteredRows.clear();
    
    if (m_filterText.empty() && m_columnFilters.empty()) {
        return;
    }
    
    for (const auto& row : m_rows) {
        bool matches = true;
        
        // Check global filter
        if (!m_filterText.empty()) {
            bool foundInRow = false;
            for (const auto& cell : row.cells) {
                if (cell.value.find(m_filterText) != std::wstring::npos) {
                    foundInRow = true;
                    break;
                }
            }
            if (!foundInRow) {
                matches = false;
            }
        }
        
        // Check column filters
        if (matches && !m_columnFilters.empty()) {
            for (const auto& filter : m_columnFilters) {
                int columnIndex = filter.first;
                const std::wstring& filterText = filter.second;
                
                if (columnIndex < (int)row.cells.size()) {
                    if (row.cells[columnIndex].value.find(filterText) == std::wstring::npos) {
                        matches = false;
                        break;
                    }
                }
            }
        }
        
        if (matches) {
            m_filteredRows.push_back(row);
        }
    }
}

// Selection
void DataGrid::SelectRow(int index, bool selected) {
    auto& rows = GetDisplayRows();
    
    if (index >= 0 && index < (int)rows.size()) {
        if (m_selectionMode == SelectionMode::SINGLE && selected) {
            // Clear other selections
            for (auto& row : rows) {
                row.selected = false;
            }
        }
        rows[index].selected = selected;
    }
}

void DataGrid::SelectAll() {
    if (m_selectionMode == SelectionMode::MULTI) {
        auto& rows = GetDisplayRows();
        for (auto& row : rows) {
            row.selected = true;
        }
    }
}

void DataGrid::ClearSelection() {
    auto& rows = GetDisplayRows();
    for (auto& row : rows) {
        row.selected = false;
    }
}

bool DataGrid::IsRowSelected(int index) const {
    const auto& rows = GetDisplayRows();
    if (index >= 0 && index < (int)rows.size()) {
        return rows[index].selected;
    }
    return false;
}

std::vector<int> DataGrid::GetSelectedRows() const {
    std::vector<int> selected;
    const auto& rows = GetDisplayRows();
    
    for (size_t i = 0; i < rows.size(); i++) {
        if (rows[i].selected) {
            selected.push_back(static_cast<int>(i));
        }
    }
    
    return selected;
}

int DataGrid::GetSelectedRowCount() const {
    return static_cast<int>(GetSelectedRows().size());
}

// Editing
void DataGrid::BeginEdit(int row, int column) {
    auto& rows = GetDisplayRows();
    
    if (row >= 0 && row < (int)rows.size() && column >= 0 && column < (int)m_columns.size()) {
        if (m_columns[column].editable) {
            m_editingRow = row;
            m_editingColumn = column;
            m_editBuffer = rows[row].cells[column].value;
        }
    }
}

void DataGrid::EndEdit(bool commit) {
    if (m_editingRow >= 0 && m_editingColumn >= 0) {
        if (commit) {
            auto& rows = GetDisplayRows();
            if (m_editingRow < (int)rows.size() && m_editingColumn < (int)rows[m_editingRow].cells.size()) {
                std::wstring oldValue = rows[m_editingRow].cells[m_editingColumn].value;
                rows[m_editingRow].cells[m_editingColumn].value = m_editBuffer;
                
                if (m_cellEditCallback) {
                    m_cellEditCallback(m_editingRow, m_editingColumn, oldValue, m_editBuffer);
                }
            }
        }
        
        m_editingRow = -1;
        m_editingColumn = -1;
        m_editBuffer.clear();
    }
}

void DataGrid::CancelEdit() {
    EndEdit(false);
}

// Virtual scrolling
void DataGrid::ScrollToRow(int index) {
    const auto& rows = GetDisplayRows();
    
    if (index >= 0 && index < (int)rows.size()) {
        m_firstVisibleRow = index;
        CalculateVisibleRows();
    }
}

void DataGrid::CalculateVisibleRows() {
    RECT bounds;
    GetBounds(bounds);
    
    int availableHeight = bounds.bottom - bounds.top - m_headerHeight;
    m_visibleRowCount = availableHeight / m_rowHeight;
}

// Helper methods
bool DataGrid::GetCellRect(int row, int column, RECT& rect) const {
    RECT bounds;
    GetBounds(bounds);
    
    int x = bounds.left;
    for (int i = 0; i < column && i < (int)m_columns.size(); i++) {
        x += m_columns[i].width;
    }
    
    if (column >= (int)m_columns.size()) return false;
    
    int displayRow = row - m_firstVisibleRow;
    int y = bounds.top + m_headerHeight + displayRow * m_rowHeight;
    
    rect.left = x;
    rect.top = y;
    rect.right = x + m_columns[column].width;
    rect.bottom = y + m_rowHeight;
    
    return true;
}

bool DataGrid::HitTestCell(int x, int y, int& row, int& column) const {
    RECT bounds;
    GetBounds(bounds);
    
    if (y < bounds.top + m_headerHeight) return false;
    
    // Calculate row
    int relativeY = y - (bounds.top + m_headerHeight);
    int displayRow = relativeY / m_rowHeight;
    row = displayRow + m_firstVisibleRow;
    
    const auto& rows = GetDisplayRows();
    if (row < 0 || row >= (int)rows.size()) return false;
    
    // Calculate column
    int currentX = bounds.left;
    for (size_t i = 0; i < m_columns.size(); i++) {
        if (x >= currentX && x < currentX + m_columns[i].width) {
            column = static_cast<int>(i);
            return true;
        }
        currentX += m_columns[i].width;
    }
    
    return false;
}

bool DataGrid::HitTestHeader(int x, int y, int& column) const {
    RECT bounds;
    GetBounds(bounds);
    
    if (y < bounds.top || y >= bounds.top + m_headerHeight) return false;
    
    int currentX = bounds.left;
    for (size_t i = 0; i < m_columns.size(); i++) {
        if (x >= currentX && x < currentX + m_columns[i].width) {
            column = static_cast<int>(i);
            return true;
        }
        currentX += m_columns[i].width;
    }
    
    return false;
}

void DataGrid::RenderHeader(HDC hdc, const RECT& bounds) {
    RECT headerRect = {bounds.left, bounds.top, bounds.right, bounds.top + m_headerHeight};
    
    // Draw header background
    HBRUSH headerBrush = CreateSolidBrush(m_headerColor.ToCOLORREF());
    FillRect(hdc, &headerRect, headerBrush);
    DeleteObject(headerBrush);
    
    // Draw columns
    int x = bounds.left;
    for (size_t i = 0; i < m_columns.size(); i++) {
        RECT colRect = {x, headerRect.top, x + m_columns[i].width, headerRect.bottom};
        
        // Draw header text
        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, RGB(0, 0, 0));
        
        HFONT hFont = CreateFontW(14, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
            CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
        HFONT oldFont = (HFONT)SelectObject(hdc, hFont);
        
        RECT textRect = colRect;
        textRect.left += 5;
        textRect.right -= 5;
        DrawTextW(hdc, m_columns[i].header.c_str(), -1, &textRect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
        
        SelectObject(hdc, oldFont);
        DeleteObject(hFont);
        
        // Draw sort indicator
        if (m_sortColumn == (int)i && m_sortOrder != SortOrder::NONE) {
            int arrowX = colRect.right - 15;
            int arrowY = (colRect.top + colRect.bottom) / 2;
            
            HPEN pen = CreatePen(PS_SOLID, 2, RGB(50, 50, 50));
            HPEN oldPen = (HPEN)SelectObject(hdc, pen);
            
            if (m_sortOrder == SortOrder::ASCENDING) {
                // Up arrow
                MoveToEx(hdc, arrowX - 4, arrowY + 2, nullptr);
                LineTo(hdc, arrowX, arrowY - 2);
                LineTo(hdc, arrowX + 4, arrowY + 2);
            } else {
                // Down arrow
                MoveToEx(hdc, arrowX - 4, arrowY - 2, nullptr);
                LineTo(hdc, arrowX, arrowY + 2);
                LineTo(hdc, arrowX + 4, arrowY - 2);
            }
            
            SelectObject(hdc, oldPen);
            DeleteObject(pen);
        }
        
        // Draw column separator
        HPEN gridPen = CreatePen(PS_SOLID, 1, m_gridLineColor.ToCOLORREF());
        HPEN oldPen = (HPEN)SelectObject(hdc, gridPen);
        
        MoveToEx(hdc, x + m_columns[i].width, headerRect.top, nullptr);
        LineTo(hdc, x + m_columns[i].width, headerRect.bottom);
        
        SelectObject(hdc, oldPen);
        DeleteObject(gridPen);
        
        x += m_columns[i].width;
    }
    
    // Draw header bottom border
    HPEN borderPen = CreatePen(PS_SOLID, 2, m_gridLineColor.ToCOLORREF());
    HPEN oldPen = (HPEN)SelectObject(hdc, borderPen);
    
    MoveToEx(hdc, headerRect.left, headerRect.bottom, nullptr);
    LineTo(hdc, headerRect.right, headerRect.bottom);
    
    SelectObject(hdc, oldPen);
    DeleteObject(borderPen);
}

void DataGrid::RenderCell(HDC hdc, const RECT& rect, const Cell& cell, bool selected, bool editing) {
    // Draw cell background
    HBRUSH cellBrush;
    if (selected) {
        cellBrush = CreateSolidBrush(m_selectionColor.ToCOLORREF());
    } else {
        cellBrush = CreateSolidBrush(RGB(255, 255, 255));
    }
    FillRect(hdc, &rect, cellBrush);
    DeleteObject(cellBrush);
    
    if (editing) {
        RenderEditBox(hdc, rect);
    } else {
        // Draw cell text
        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, RGB(0, 0, 0));
        
        RECT textRect = rect;
        textRect.left += 5;
        textRect.right -= 5;
        
        DrawTextW(hdc, cell.value.c_str(), -1, &textRect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    }
}

void DataGrid::RenderEditBox(HDC hdc, const RECT& rect) {
    // Draw edit background
    HBRUSH brush = CreateSolidBrush(RGB(255, 255, 200));
    FillRect(hdc, &rect, brush);
    DeleteObject(brush);
    
    // Draw edit text
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(0, 0, 0));
    
    RECT textRect = rect;
    textRect.left += 5;
    textRect.right -= 5;
    
    std::wstring displayText = m_editBuffer + L"|"; // Cursor
    DrawTextW(hdc, displayText.c_str(), -1, &textRect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    
    // Draw border
    HPEN pen = CreatePen(PS_SOLID, 2, RGB(0, 120, 215));
    HPEN oldPen = (HPEN)SelectObject(hdc, pen);
    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));
    
    Rectangle(hdc, rect.left, rect.top, rect.right, rect.bottom);
    
    SelectObject(hdc, oldPen);
    SelectObject(hdc, oldBrush);
    DeleteObject(pen);
}

void DataGrid::RenderRows(HDC hdc, const RECT& bounds) {
    const auto& rows = GetDisplayRows();
    
    CalculateVisibleRows();
    
    int startRow = m_virtualScrolling ? m_firstVisibleRow : 0;
    int endRow = m_virtualScrolling ? std::min(m_firstVisibleRow + m_visibleRowCount, (int)rows.size()) : (int)rows.size();
    
    for (int i = startRow; i < endRow; i++) {
        const Row& row = rows[i];
        int displayRow = i - m_firstVisibleRow;
        int y = bounds.top + m_headerHeight + displayRow * m_rowHeight;
        
        // Draw row background (alternating colors)
        if (i % 2 == 1 && !row.selected) {
            RECT rowRect = {bounds.left, y, bounds.right, y + m_rowHeight};
            HBRUSH brush = CreateSolidBrush(m_alternateRowColor.ToCOLORREF());
            FillRect(hdc, &rowRect, brush);
            DeleteObject(brush);
        }
        
        // Draw cells
        int x = bounds.left;
        for (size_t j = 0; j < m_columns.size() && j < row.cells.size(); j++) {
            RECT cellRect = {x, y, x + m_columns[j].width, y + m_rowHeight};
            
            bool editing = (m_editingRow == i && m_editingColumn == (int)j);
            RenderCell(hdc, cellRect, row.cells[j], row.selected, editing);
            
            // Draw grid lines
            HPEN gridPen = CreatePen(PS_SOLID, 1, m_gridLineColor.ToCOLORREF());
            HPEN oldPen = (HPEN)SelectObject(hdc, gridPen);
            
            // Vertical line
            MoveToEx(hdc, x + m_columns[j].width, cellRect.top, nullptr);
            LineTo(hdc, x + m_columns[j].width, cellRect.bottom);
            
            // Horizontal line
            MoveToEx(hdc, cellRect.left, cellRect.bottom, nullptr);
            LineTo(hdc, cellRect.right, cellRect.bottom);
            
            SelectObject(hdc, oldPen);
            DeleteObject(gridPen);
            
            x += m_columns[j].width;
        }
    }
}

void DataGrid::Render(HDC hdc) {
    if (!m_visible) return;
    
    RECT bounds;
    GetBounds(bounds);
    
    // Draw background
    HBRUSH brush = CreateSolidBrush(RGB(255, 255, 255));
    FillRect(hdc, &bounds, brush);
    DeleteObject(brush);
    
    // Draw border
    Renderer::DrawRoundedRect(hdc, bounds, 4, Color(255, 255, 255, 255), Color(180, 180, 180, 255), 1);
    
    // Set clipping region
    HRGN clipRegion = CreateRectRgn(bounds.left, bounds.top, bounds.right, bounds.bottom);
    SelectClipRgn(hdc, clipRegion);
    
    // Render header
    RenderHeader(hdc, bounds);
    
    // Render rows
    RenderRows(hdc, bounds);
    
    // Reset clipping
    SelectClipRgn(hdc, nullptr);
    DeleteObject(clipRegion);
    
    Widget::Render(hdc);
}

bool DataGrid::HandleMouseDown(int x, int y, int button) {
    if (!m_visible || !m_enabled) return false;
    if (!HitTest(x, y)) return false;
    
    // Check header click (for sorting)
    int headerColumn = -1;
    if (HitTestHeader(x, y, headerColumn)) {
        if (m_columns[headerColumn].sortable) {
            ToggleSort(headerColumn);
        }
        return true;
    }
    
    // Check cell click
    int row = -1, column = -1;
    if (HitTestCell(x, y, row, column)) {
        // Handle selection
        if (m_selectionMode != SelectionMode::NONE) {
            if (button == 0) { // Left click
                SelectRow(row, true);
            }
        }
        
        // Trigger callback
        if (m_cellClickCallback) {
            m_cellClickCallback(row, column);
        }
        
        return true;
    }
    
    return false;
}

bool DataGrid::HandleMouseUp(int x, int y, int button) {
    return false;
}

bool DataGrid::HandleMouseMove(int x, int y) {
    if (!m_visible || !m_enabled) return false;
    
    int row = -1, column = -1;
    if (HitTestCell(x, y, row, column)) {
        m_hoveredRow = row;
        m_hoveredColumn = column;
        return true;
    }
    
    m_hoveredRow = -1;
    m_hoveredColumn = -1;
    return false;
}

bool DataGrid::HandleKeyDown(int keyCode) {
    if (IsEditing()) {
        if (keyCode == VK_RETURN) {
            EndEdit(true);
            return true;
        } else if (keyCode == VK_ESCAPE) {
            CancelEdit();
            return true;
        } else if (keyCode == VK_BACK) {
            if (!m_editBuffer.empty()) {
                m_editBuffer.pop_back();
            }
            return true;
        }
    } else {
        // Handle navigation
        auto& rows = GetDisplayRows();
        std::vector<int> selected = GetSelectedRows();
        
        if (!selected.empty()) {
            int currentRow = selected[0];
            
            if (keyCode == VK_UP && currentRow > 0) {
                SelectRow(currentRow, false);
                SelectRow(currentRow - 1, true);
                return true;
            } else if (keyCode == VK_DOWN && currentRow < (int)rows.size() - 1) {
                SelectRow(currentRow, false);
                SelectRow(currentRow + 1, true);
                return true;
            } else if (keyCode == VK_F2 && m_hoveredColumn >= 0) {
                // Begin edit
                if (m_columns[m_hoveredColumn].editable) {
                    BeginEdit(currentRow, m_hoveredColumn);
                }
                return true;
            }
        }
    }
    
    return false;
}

bool DataGrid::HandleChar(wchar_t ch) {
    if (IsEditing()) {
        // Allow printable characters (including Unicode)
        if (ch >= 32) {  // Accept all printable characters including Unicode
            m_editBuffer += ch;
            return true;
        }
    }
    return false;
}

void DataGrid::Update(float deltaTime) {
    Widget::Update(deltaTime);
}

} // namespace SDK
