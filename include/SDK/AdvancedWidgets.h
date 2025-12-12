#pragma once


#include "Platform.h"
#include <string>
#include <vector>
#include <memory>
#include "Widget.h"

namespace SDK {

// ComboBox widget
class ComboBox : public Widget {
public:
    ComboBox();
    virtual ~ComboBox();
    
    void AddItem(const std::wstring& item);
    void RemoveItem(int index);
    void ClearItems();
    
    void SetSelectedIndex(int index);
    int GetSelectedIndex() const { return m_selectedIndex; }
    std::wstring GetSelectedItem() const;
    
    void Render(HDC hdc) override;
    bool HandleMouseDown(int x, int y, int button) override;
    
private:
    std::vector<std::wstring> m_items;
    int m_selectedIndex;
    bool m_dropdownOpen;
    Color m_backgroundColor;
    Color m_textColor;
};

// ListBox widget
class ListBox : public Widget {
public:
    ListBox();
    virtual ~ListBox();
    
    void AddItem(const std::wstring& item);
    void RemoveItem(int index);
    void ClearItems();
    
    void SetSelectedIndex(int index);
    int GetSelectedIndex() const { return m_selectedIndex; }
    
    void SetMultiSelect(bool multiSelect) { m_multiSelect = multiSelect; }
    std::vector<int> GetSelectedIndices() const { return m_selectedIndices; }
    
    void Render(HDC hdc) override;
    bool HandleMouseDown(int x, int y, int button) override;
    
private:
    std::vector<std::wstring> m_items;
    int m_selectedIndex;
    bool m_multiSelect;
    std::vector<int> m_selectedIndices;
    int m_scrollOffset;
};

// ListView widget with checkbox support
class ListView : public Widget {
public:
    struct ListViewItem {
        std::wstring text;
        bool checked;
        void* userData;
        
        ListViewItem(const std::wstring& t = L"") 
            : text(t), checked(false), userData(nullptr) {}
    };
    
    ListView();
    virtual ~ListView();
    
    void AddItem(const std::wstring& text, bool checked = false);
    void RemoveItem(int index);
    void ClearItems();
    
    void SetItemChecked(int index, bool checked);
    bool IsItemChecked(int index) const;
    
    void SetCheckboxEnabled(bool enabled) { m_checkboxEnabled = enabled; }
    bool IsCheckboxEnabled() const { return m_checkboxEnabled; }
    
    std::vector<int> GetCheckedItems() const;
    
    void Render(HDC hdc) override;
    bool HandleMouseDown(int x, int y, int button) override;
    
private:
    std::vector<ListViewItem> m_items;
    bool m_checkboxEnabled;
    int m_scrollOffset;
    int m_itemHeight;
};

// TabControl widget
class TabControl : public Widget {
public:
    struct Tab {
        std::wstring title;
        std::shared_ptr<Widget> content;
        
        Tab(const std::wstring& t) : title(t) {}
    };
    
    TabControl();
    virtual ~TabControl();
    
    void AddTab(const std::wstring& title, std::shared_ptr<Widget> content = nullptr);
    void RemoveTab(int index);
    
    void SetActiveTab(int index);
    int GetActiveTab() const { return m_activeTab; }
    
    std::shared_ptr<Widget> GetTabContent(int index);
    
    void Render(HDC hdc) override;
    bool HandleMouseDown(int x, int y, int button) override;
    
private:
    std::vector<Tab> m_tabs;
    int m_activeTab;
    int m_tabHeight;
};

// FileTree widget
class FileTree : public Widget {
public:
    enum class Orientation {
        VERTICAL,
        HORIZONTAL
    };
    
    struct TreeNode {
        std::wstring name;
        std::wstring fullPath;
        bool isDirectory;
        bool expanded;
        std::vector<std::shared_ptr<TreeNode>> children;
        int depth;
        
        TreeNode(const std::wstring& n, const std::wstring& path, bool isDir)
            : name(n), fullPath(path), isDirectory(isDir), expanded(false), depth(0) {}
    };
    
    FileTree();
    virtual ~FileTree();
    
    void SetRootPath(const std::wstring& path);
    std::wstring GetRootPath() const { return m_rootPath; }
    
    void SetSelectedPath(const std::wstring& path);
    std::wstring GetSelectedPath() const;
    
    // Orientation support
    void SetOrientation(Orientation orientation) { m_orientation = orientation; }
    Orientation GetOrientation() const { return m_orientation; }
    
    // Expand all/collapse all
    void ExpandAll();
    void CollapseAll();
    void ExpandNode(const std::wstring& path);
    void CollapseNode(const std::wstring& path);
    
    void Render(HDC hdc) override;
    bool HandleMouseDown(int x, int y, int button) override;
    
private:
    void LoadDirectory(std::shared_ptr<TreeNode> node);
    void RenderNode(HDC hdc, std::shared_ptr<TreeNode> node, int& offset);
    void RenderNodeVertical(HDC hdc, std::shared_ptr<TreeNode> node, int& yOffset);
    void RenderNodeHorizontal(HDC hdc, std::shared_ptr<TreeNode> node, int& xOffset);
    void RenderExpandIndicator(HDC hdc, std::shared_ptr<TreeNode> node, int x, int y);
    std::shared_ptr<TreeNode> HitTestNode(int x, int y);
    std::shared_ptr<TreeNode> HitTestNodeRecursive(std::shared_ptr<TreeNode> node, int x, int y, int& offset);
    std::shared_ptr<TreeNode> FindNodeByPath(std::shared_ptr<TreeNode> node, const std::wstring& path);
    void ExpandAllRecursive(std::shared_ptr<TreeNode> node);
    void CollapseAllRecursive(std::shared_ptr<TreeNode> node);
    
    std::wstring m_rootPath;
    std::shared_ptr<TreeNode> m_rootNode;
    std::shared_ptr<TreeNode> m_selectedNode;
    int m_scrollOffset;
    int m_itemHeight;
    Orientation m_orientation;
};

// SyntaxHighlightTextEditor widget
class SyntaxHighlightTextEditor : public Widget {
public:
    enum class Language {
        PLAIN_TEXT,
        CPP,
        PYTHON,
        JAVASCRIPT,
        HTML,
        CSS
    };
    
    SyntaxHighlightTextEditor();
    virtual ~SyntaxHighlightTextEditor();
    
    void SetText(const std::wstring& text);
    std::wstring GetText() const { return m_text; }
    
    void SetLanguage(Language language) { m_language = language; }
    Language GetLanguage() const { return m_language; }
    
    void SetLineNumbers(bool show) { m_showLineNumbers = show; }
    void SetWordWrap(bool wrap) { m_wordWrap = wrap; }
    
    void Render(HDC hdc) override;
    bool HandleMouseDown(int x, int y, int button) override;
    bool HandleKeyDown(int keyCode) override;
    bool HandleChar(wchar_t ch) override;
    
private:
    struct SyntaxToken {
        std::wstring text;
        Color color;
    };
    
    std::vector<SyntaxToken> TokenizeLine(const std::wstring& line);
    void UpdateSyntaxHighlighting();
    
    std::wstring m_text;
    std::vector<std::wstring> m_lines;
    Language m_language;
    bool m_showLineNumbers;
    bool m_wordWrap;
    int m_cursorLine;
    int m_cursorColumn;
    int m_scrollOffsetX;
    int m_scrollOffsetY;
    
    // Syntax colors
    Color m_keywordColor;
    Color m_stringColor;
    Color m_commentColor;
    Color m_numberColor;
    Color m_operatorColor;
};

// FileExplorer window widget
class FileExplorer : public Widget {
public:
    FileExplorer();
    virtual ~FileExplorer();
    
    void SetCurrentPath(const std::wstring& path);
    std::wstring GetCurrentPath() const { return m_currentPath; }
    
    void SetSelectedFile(const std::wstring& filename);
    std::wstring GetSelectedFile() const;
    
    void SetFilter(const std::wstring& filter) { m_filter = filter; }
    
    void Render(HDC hdc) override;
    bool HandleMouseDown(int x, int y, int button) override;
    
private:
    struct FileItem {
        std::wstring name;
        std::wstring fullPath;
        bool isDirectory;
        ULONGLONG size;
        FILETIME modifiedTime;
    };
    
    void LoadDirectory();
    void RenderAddressBar(HDC hdc, RECT& rect);
    void RenderFileList(HDC hdc, RECT& rect);
    
    std::wstring m_currentPath;
    std::vector<FileItem> m_items;
    int m_selectedIndex;
    std::wstring m_filter;
    int m_scrollOffset;
};

} // namespace SDK
