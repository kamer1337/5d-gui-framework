#pragma once

#include "Widget.h"
#include "Theme.h"
#include <string>
#include <vector>
#include <memory>
#include <functional>

namespace SDK {

/**
 * DataGrid - Advanced table widget with sorting, filtering, and editing
 */
class DataGrid : public Widget {
public:
    // Cell data structure
    struct Cell {
        std::wstring value;
        void* userData;
        
        Cell() : userData(nullptr) {}
        Cell(const std::wstring& val) : value(val), userData(nullptr) {}
    };
    
    // Column definition
    struct Column {
        std::wstring header;
        int width;
        bool sortable;
        bool editable;
        
        Column(const std::wstring& h, int w = 100)
            : header(h), width(w), sortable(true), editable(false) {}
    };
    
    // Row data
    struct Row {
        std::vector<Cell> cells;
        bool selected;
        void* userData;
        
        Row() : selected(false), userData(nullptr) {}
    };
    
    // Sort order
    enum class SortOrder {
        NONE,
        ASCENDING,
        DESCENDING
    };
    
    // Selection mode
    enum class SelectionMode {
        NONE,
        SINGLE,
        MULTI
    };
    
    DataGrid();
    virtual ~DataGrid();
    
    // Column management
    void AddColumn(const std::wstring& header, int width = 100);
    void AddColumn(const Column& column);
    void RemoveColumn(int index);
    void ClearColumns();
    
    Column& GetColumn(int index) { return m_columns[index]; }
    const Column& GetColumn(int index) const { return m_columns[index]; }
    int GetColumnCount() const { return static_cast<int>(m_columns.size()); }
    
    void SetColumnWidth(int columnIndex, int width);
    void SetColumnSortable(int columnIndex, bool sortable);
    void SetColumnEditable(int columnIndex, bool editable);
    
    // Row management
    void AddRow(const std::vector<std::wstring>& values);
    void AddRow(const Row& row);
    void InsertRow(int index, const std::vector<std::wstring>& values);
    void RemoveRow(int index);
    void ClearRows();
    
    Row& GetRow(int index) { return m_rows[index]; }
    const Row& GetRow(int index) const { return m_rows[index]; }
    int GetRowCount() const { return static_cast<int>(m_rows.size()); }
    
    // Cell access
    void SetCellValue(int row, int column, const std::wstring& value);
    std::wstring GetCellValue(int row, int column) const;
    
    Cell& GetCell(int row, int column);
    const Cell& GetCell(int row, int column) const;
    
    // Sorting
    void SortByColumn(int columnIndex, SortOrder order = SortOrder::ASCENDING);
    void ToggleSort(int columnIndex);
    
    int GetSortColumn() const { return m_sortColumn; }
    SortOrder GetSortOrder() const { return m_sortOrder; }
    
    // Filtering
    void SetFilter(const std::wstring& filterText);
    void SetColumnFilter(int columnIndex, const std::wstring& filterText);
    void ClearFilter();
    
    std::wstring GetFilter() const { return m_filterText; }
    bool IsFiltered() const { return !m_filterText.empty() || !m_columnFilters.empty(); }
    
    // Selection
    void SetSelectionMode(SelectionMode mode) { m_selectionMode = mode; }
    SelectionMode GetSelectionMode() const { return m_selectionMode; }
    
    void SelectRow(int index, bool selected = true);
    void SelectAll();
    void ClearSelection();
    
    bool IsRowSelected(int index) const;
    std::vector<int> GetSelectedRows() const;
    int GetSelectedRowCount() const;
    
    // Editing
    void BeginEdit(int row, int column);
    void EndEdit(bool commit = true);
    void CancelEdit();
    
    bool IsEditing() const { return m_editingRow >= 0; }
    int GetEditingRow() const { return m_editingRow; }
    int GetEditingColumn() const { return m_editingColumn; }
    
    // Virtual scrolling
    void SetVirtualScrolling(bool enabled) { m_virtualScrolling = enabled; }
    bool IsVirtualScrolling() const { return m_virtualScrolling; }
    
    void SetVisibleRowCount(int count) { m_visibleRowCount = count; }
    int GetVisibleRowCount() const { return m_visibleRowCount; }
    
    void ScrollToRow(int index);
    int GetFirstVisibleRow() const { return m_firstVisibleRow; }
    
    // Appearance
    void SetHeaderHeight(int height) { m_headerHeight = height; }
    int GetHeaderHeight() const { return m_headerHeight; }
    
    void SetRowHeight(int height) { m_rowHeight = height; }
    int GetRowHeight() const { return m_rowHeight; }
    
    void SetGridLineColor(Color color) { m_gridLineColor = color; }
    Color GetGridLineColor() const { return m_gridLineColor; }
    
    void SetHeaderColor(Color color) { m_headerColor = color; }
    void SetAlternateRowColor(Color color) { m_alternateRowColor = color; }
    void SetSelectionColor(Color color) { m_selectionColor = color; }
    
    // Events
    using CellClickCallback = std::function<void(int row, int column)>;
    using CellEditCallback = std::function<void(int row, int column, const std::wstring& oldValue, const std::wstring& newValue)>;
    using SortCallback = std::function<void(int column, SortOrder order)>;
    
    void SetCellClickCallback(CellClickCallback callback) { m_cellClickCallback = callback; }
    void SetCellEditCallback(CellEditCallback callback) { m_cellEditCallback = callback; }
    void SetSortCallback(SortCallback callback) { m_sortCallback = callback; }
    
    // Widget overrides
    void Render(HDC hdc) override;
    bool HandleMouseDown(int x, int y, int button) override;
    bool HandleMouseUp(int x, int y, int button) override;
    bool HandleMouseMove(int x, int y) override;
    bool HandleKeyDown(int keyCode) override;
    bool HandleChar(wchar_t ch) override;
    void Update(float deltaTime) override;
    
private:
    // Data
    std::vector<Column> m_columns;
    std::vector<Row> m_rows;
    std::vector<Row> m_filteredRows; // Filtered view of rows
    
    // Sorting
    int m_sortColumn;
    SortOrder m_sortOrder;
    
    // Filtering
    std::wstring m_filterText;
    std::map<int, std::wstring> m_columnFilters;
    
    // Selection
    SelectionMode m_selectionMode;
    int m_hoveredRow;
    int m_hoveredColumn;
    
    // Editing
    int m_editingRow;
    int m_editingColumn;
    std::wstring m_editBuffer;
    
    // Virtual scrolling
    bool m_virtualScrolling;
    int m_visibleRowCount;
    int m_firstVisibleRow;
    int m_scrollOffset;
    
    // Appearance
    int m_headerHeight;
    int m_rowHeight;
    Color m_gridLineColor;
    Color m_headerColor;
    Color m_alternateRowColor;
    Color m_selectionColor;
    
    // Callbacks
    CellClickCallback m_cellClickCallback;
    CellEditCallback m_cellEditCallback;
    SortCallback m_sortCallback;
    
    // Helper methods
    void RenderHeader(HDC hdc, const RECT& bounds);
    void RenderRows(HDC hdc, const RECT& bounds);
    void RenderCell(HDC hdc, const RECT& rect, const Cell& cell, bool selected, bool editing);
    void RenderEditBox(HDC hdc, const RECT& rect);
    
    void ApplyFilter();
    void ApplySorting();
    
    bool GetCellRect(int row, int column, RECT& rect) const;
    bool HitTestCell(int x, int y, int& row, int& column) const;
    bool HitTestHeader(int x, int y, int& column) const;
    
    void UpdateScrollbar();
    void CalculateVisibleRows();
    
    std::vector<Row>& GetDisplayRows() { return (!m_filterText.empty() || !m_columnFilters.empty()) ? m_filteredRows : m_rows; }
    const std::vector<Row>& GetDisplayRows() const { return (!m_filterText.empty() || !m_columnFilters.empty()) ? m_filteredRows : m_rows; }
};

} // namespace SDK
