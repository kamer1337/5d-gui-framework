#include "../../include/SDK/AdvancedWidgets.h"
#include "../../include/SDK/Renderer.h"
#include <algorithm>

namespace SDK {

// ComboBox implementation
ComboBox::ComboBox()
    : Widget()
    , m_selectedIndex(-1)
    , m_dropdownOpen(false)
    , m_backgroundColor(255, 255, 255, 255)
    , m_textColor(0, 0, 0, 255)
{
    m_height = 30;
}

ComboBox::~ComboBox() {
}

void ComboBox::AddItem(const std::wstring& item) {
    m_items.push_back(item);
}

void ComboBox::RemoveItem(int index) {
    if (index >= 0 && index < (int)m_items.size()) {
        m_items.erase(m_items.begin() + index);
        if (m_selectedIndex == index) {
            m_selectedIndex = -1;
        } else if (m_selectedIndex > index) {
            m_selectedIndex--;
        }
    }
}

void ComboBox::ClearItems() {
    m_items.clear();
    m_selectedIndex = -1;
}

void ComboBox::SetSelectedIndex(int index) {
    if (index >= -1 && index < (int)m_items.size()) {
        m_selectedIndex = index;
        TriggerEvent(WidgetEvent::VALUE_CHANGED, &m_selectedIndex);
    }
}

std::wstring ComboBox::GetSelectedItem() const {
    if (m_selectedIndex >= 0 && m_selectedIndex < (int)m_items.size()) {
        return m_items[m_selectedIndex];
    }
    return L"";
}

void ComboBox::Render(HDC hdc) {
    if (!m_visible) return;
    
    RECT bounds = GetBounds();
    
    // Draw main box
    Renderer::DrawRoundedRect(hdc, bounds, 4, m_backgroundColor, Color(128, 128, 128, 255), 1);
    
    // Draw selected text
    if (m_selectedIndex >= 0) {
        RECT textRect = bounds;
        textRect.left += 5;
        textRect.right -= 25;
        
        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, m_textColor.ToCOLORREF());
        DrawTextW(hdc, m_items[m_selectedIndex].c_str(), -1, &textRect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    }
    
    // Draw dropdown arrow
    int arrowX = bounds.right - 15;
    int arrowY = (bounds.top + bounds.bottom) / 2;
    HPEN pen = CreatePen(PS_SOLID, 2, RGB(50, 50, 50));
    HPEN oldPen = (HPEN)SelectObject(hdc, pen);
    
    MoveToEx(hdc, arrowX - 4, arrowY - 2, nullptr);
    LineTo(hdc, arrowX, arrowY + 2);
    LineTo(hdc, arrowX + 4, arrowY - 2);
    
    SelectObject(hdc, oldPen);
    DeleteObject(pen);
    
    // Draw dropdown list if open
    if (m_dropdownOpen && !m_items.empty()) {
        RECT dropRect = {bounds.left, bounds.bottom, bounds.right, bounds.bottom + (int)m_items.size() * 25};
        Renderer::DrawRoundedRect(hdc, dropRect, 4, m_backgroundColor, Color(128, 128, 128, 255), 1);
        
        SetBkMode(hdc, TRANSPARENT);
        for (size_t i = 0; i < m_items.size(); i++) {
            RECT itemRect = {dropRect.left + 5, dropRect.top + (int)i * 25, dropRect.right - 5, dropRect.top + (int)(i + 1) * 25};
            
            if ((int)i == m_selectedIndex) {
                HBRUSH brush = CreateSolidBrush(RGB(200, 220, 255));
                FillRect(hdc, &itemRect, brush);
                DeleteObject(brush);
            }
            
            SetTextColor(hdc, m_textColor.ToCOLORREF());
            DrawTextW(hdc, m_items[i].c_str(), -1, &itemRect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
        }
    }
    
    Widget::Render(hdc);
}

bool ComboBox::HandleMouseDown(int x, int y, int button) {
    if (!m_visible || !m_enabled) return false;
    
    RECT bounds = GetBounds();
    
    if (m_dropdownOpen) {
        // Check dropdown items
        RECT dropRect = {bounds.left, bounds.bottom, bounds.right, bounds.bottom + (int)m_items.size() * 25};
        if (x >= dropRect.left && x < dropRect.right && y >= dropRect.top && y < dropRect.bottom) {
            int index = (y - dropRect.top) / 25;
            SetSelectedIndex(index);
            m_dropdownOpen = false;
            return true;
        }
        m_dropdownOpen = false;
    } else if (HitTest(x, y)) {
        m_dropdownOpen = true;
        return true;
    }
    
    return false;
}

// ListBox implementation
ListBox::ListBox()
    : Widget()
    , m_selectedIndex(-1)
    , m_multiSelect(false)
    , m_scrollOffset(0)
{
    m_height = 150;
}

ListBox::~ListBox() {
}

void ListBox::AddItem(const std::wstring& item) {
    m_items.push_back(item);
}

void ListBox::RemoveItem(int index) {
    if (index >= 0 && index < (int)m_items.size()) {
        m_items.erase(m_items.begin() + index);
    }
}

void ListBox::ClearItems() {
    m_items.clear();
    m_selectedIndex = -1;
    m_selectedIndices.clear();
}

void ListBox::SetSelectedIndex(int index) {
    if (index >= -1 && index < (int)m_items.size()) {
        m_selectedIndex = index;
        if (!m_multiSelect) {
            m_selectedIndices.clear();
            if (index >= 0) {
                m_selectedIndices.push_back(index);
            }
        }
        TriggerEvent(WidgetEvent::VALUE_CHANGED, &m_selectedIndex);
    }
}

void ListBox::Render(HDC hdc) {
    if (!m_visible) return;
    
    RECT bounds = GetBounds();
    
    // Draw background
    Renderer::DrawRoundedRect(hdc, bounds, 4, Color(255, 255, 255, 255), Color(128, 128, 128, 255), 1);
    
    // Draw items
    SetBkMode(hdc, TRANSPARENT);
    int itemHeight = 25;
    int visibleItems = m_height / itemHeight;
    
    for (int i = m_scrollOffset; i < (int)m_items.size() && i < m_scrollOffset + visibleItems; i++) {
        RECT itemRect = {
            bounds.left + 5,
            bounds.top + (i - m_scrollOffset) * itemHeight,
            bounds.right - 5,
            bounds.top + (i - m_scrollOffset + 1) * itemHeight
        };
        
        // Highlight selected items
        bool isSelected = (i == m_selectedIndex) || 
                         (m_multiSelect && std::find(m_selectedIndices.begin(), m_selectedIndices.end(), i) != m_selectedIndices.end());
        
        if (isSelected) {
            HBRUSH brush = CreateSolidBrush(RGB(100, 149, 237));
            FillRect(hdc, &itemRect, brush);
            DeleteObject(brush);
            SetTextColor(hdc, RGB(255, 255, 255));
        } else {
            SetTextColor(hdc, RGB(0, 0, 0));
        }
        
        DrawTextW(hdc, m_items[i].c_str(), -1, &itemRect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    }
    
    Widget::Render(hdc);
}

bool ListBox::HandleMouseDown(int x, int y, int button) {
    if (!m_visible || !m_enabled) return false;
    
    if (HitTest(x, y)) {
        RECT bounds = GetBounds();
        int itemHeight = 25;
        int relY = y - bounds.top;
        int index = m_scrollOffset + relY / itemHeight;
        
        if (index >= 0 && index < (int)m_items.size()) {
            if (m_multiSelect) {
                auto it = std::find(m_selectedIndices.begin(), m_selectedIndices.end(), index);
                if (it != m_selectedIndices.end()) {
                    m_selectedIndices.erase(it);
                } else {
                    m_selectedIndices.push_back(index);
                }
            }
            SetSelectedIndex(index);
            return true;
        }
    }
    
    return false;
}

// ListView implementation
ListView::ListView()
    : Widget()
    , m_checkboxEnabled(false)
    , m_scrollOffset(0)
    , m_itemHeight(25)
{
    m_height = 200;
}

ListView::~ListView() {
}

void ListView::AddItem(const std::wstring& text, bool checked) {
    ListViewItem item(text);
    item.checked = checked;
    m_items.push_back(item);
}

void ListView::RemoveItem(int index) {
    if (index >= 0 && index < (int)m_items.size()) {
        m_items.erase(m_items.begin() + index);
    }
}

void ListView::ClearItems() {
    m_items.clear();
}

void ListView::SetItemChecked(int index, bool checked) {
    if (index >= 0 && index < (int)m_items.size()) {
        m_items[index].checked = checked;
        TriggerEvent(WidgetEvent::VALUE_CHANGED, &index);
    }
}

bool ListView::IsItemChecked(int index) const {
    if (index >= 0 && index < (int)m_items.size()) {
        return m_items[index].checked;
    }
    return false;
}

std::vector<int> ListView::GetCheckedItems() const {
    std::vector<int> checked;
    for (size_t i = 0; i < m_items.size(); i++) {
        if (m_items[i].checked) {
            checked.push_back((int)i);
        }
    }
    return checked;
}

void ListView::Render(HDC hdc) {
    if (!m_visible) return;
    
    RECT bounds = GetBounds();
    
    // Draw background
    Renderer::DrawRoundedRect(hdc, bounds, 4, Color(255, 255, 255, 255), Color(128, 128, 128, 255), 1);
    
    // Draw items
    SetBkMode(hdc, TRANSPARENT);
    int visibleItems = m_height / m_itemHeight;
    
    for (int i = m_scrollOffset; i < (int)m_items.size() && i < m_scrollOffset + visibleItems; i++) {
        int yOffset = bounds.top + (i - m_scrollOffset) * m_itemHeight;
        int xOffset = bounds.left + 5;
        
        // Draw checkbox if enabled
        if (m_checkboxEnabled) {
            RECT checkRect = {xOffset, yOffset + 2, xOffset + 18, yOffset + 20};
            Renderer::DrawRoundedRect(hdc, checkRect, 3, Color(255, 255, 255, 255), Color(128, 128, 128, 255), 1);
            
            if (m_items[i].checked) {
                HPEN pen = CreatePen(PS_SOLID, 2, RGB(0, 128, 0));
                HPEN oldPen = (HPEN)SelectObject(hdc, pen);
                
                MoveToEx(hdc, xOffset + 3, yOffset + 10, nullptr);
                LineTo(hdc, xOffset + 7, yOffset + 14);
                LineTo(hdc, xOffset + 15, yOffset + 6);
                
                SelectObject(hdc, oldPen);
                DeleteObject(pen);
            }
            
            xOffset += 25;
        }
        
        // Draw text
        RECT textRect = {xOffset, yOffset, bounds.right - 5, yOffset + m_itemHeight};
        SetTextColor(hdc, RGB(0, 0, 0));
        DrawTextW(hdc, m_items[i].text.c_str(), -1, &textRect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    }
    
    Widget::Render(hdc);
}

bool ListView::HandleMouseDown(int x, int y, int button) {
    if (!m_visible || !m_enabled) return false;
    
    if (HitTest(x, y)) {
        RECT bounds = GetBounds();
        int relY = y - bounds.top;
        int index = m_scrollOffset + relY / m_itemHeight;
        
        if (index >= 0 && index < (int)m_items.size()) {
            if (m_checkboxEnabled) {
                int xOffset = bounds.left + 5;
                if (x >= xOffset && x < xOffset + 18) {
                    SetItemChecked(index, !m_items[index].checked);
                    return true;
                }
            }
            
            TriggerEvent(WidgetEvent::CLICK, &index);
            return true;
        }
    }
    
    return false;
}

// TabControl implementation
TabControl::TabControl()
    : Widget()
    , m_activeTab(0)
    , m_tabHeight(30)
{
    m_height = 200;
}

TabControl::~TabControl() {
}

void TabControl::AddTab(const std::wstring& title, std::shared_ptr<Widget> content) {
    Tab tab(title);
    tab.content = content;
    m_tabs.push_back(tab);
    
    if (content) {
        AddChild(content);
        content->SetVisible(m_tabs.size() - 1 == (size_t)m_activeTab);
    }
}

void TabControl::RemoveTab(int index) {
    if (index >= 0 && index < (int)m_tabs.size()) {
        if (m_tabs[index].content) {
            RemoveChild(m_tabs[index].content);
        }
        m_tabs.erase(m_tabs.begin() + index);
        
        if (m_activeTab >= (int)m_tabs.size()) {
            m_activeTab = (int)m_tabs.size() - 1;
        }
    }
}

void TabControl::SetActiveTab(int index) {
    if (index >= 0 && index < (int)m_tabs.size() && index != m_activeTab) {
        // Hide old tab content
        if (m_activeTab >= 0 && m_activeTab < (int)m_tabs.size() && m_tabs[m_activeTab].content) {
            m_tabs[m_activeTab].content->SetVisible(false);
        }
        
        m_activeTab = index;
        
        // Show new tab content
        if (m_tabs[m_activeTab].content) {
            m_tabs[m_activeTab].content->SetVisible(true);
        }
        
        TriggerEvent(WidgetEvent::VALUE_CHANGED, &m_activeTab);
    }
}

std::shared_ptr<Widget> TabControl::GetTabContent(int index) {
    if (index >= 0 && index < (int)m_tabs.size()) {
        return m_tabs[index].content;
    }
    return nullptr;
}

void TabControl::Render(HDC hdc) {
    if (!m_visible) return;
    
    RECT bounds = GetBounds();
    
    // Draw tab headers
    int tabWidth = m_width / (std::max)(1, (int)m_tabs.size());
    for (size_t i = 0; i < m_tabs.size(); i++) {
        RECT tabRect = {
            bounds.left + (int)i * tabWidth,
            bounds.top,
            bounds.left + (int)(i + 1) * tabWidth,
            bounds.top + m_tabHeight
        };
        
        Color bgColor = (int)i == m_activeTab ? Color(220, 220, 220, 255) : Color(180, 180, 180, 255);
        Renderer::DrawRoundedRect(hdc, tabRect, 4, bgColor, Color(128, 128, 128, 255), 1);
        
        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, RGB(0, 0, 0));
        DrawTextW(hdc, m_tabs[i].title.c_str(), -1, &tabRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    }
    
    // Draw content area background
    RECT contentRect = {bounds.left, bounds.top + m_tabHeight, bounds.right, bounds.bottom};
    Renderer::DrawRoundedRect(hdc, contentRect, 4, Color(240, 240, 240, 255), Color(128, 128, 128, 255), 1);
    
    // Update content widget positions
    if (m_activeTab >= 0 && m_activeTab < (int)m_tabs.size() && m_tabs[m_activeTab].content) {
        m_tabs[m_activeTab].content->SetPosition(bounds.left + 5, bounds.top + m_tabHeight + 5);
        m_tabs[m_activeTab].content->SetSize(m_width - 10, m_height - m_tabHeight - 10);
    }
    
    Widget::Render(hdc);
}

bool TabControl::HandleMouseDown(int x, int y, int button) {
    if (!m_visible || !m_enabled) return false;
    
    RECT bounds = GetBounds();
    
    // Check if clicking on tab headers
    if (y >= bounds.top && y < bounds.top + m_tabHeight) {
        int tabWidth = m_width / (std::max)(1, (int)m_tabs.size());
        int index = (x - bounds.left) / tabWidth;
        
        if (index >= 0 && index < (int)m_tabs.size()) {
            SetActiveTab(index);
            return true;
        }
    }
    
    // Pass to active tab content
    if (m_activeTab >= 0 && m_activeTab < (int)m_tabs.size() && m_tabs[m_activeTab].content) {
        if (m_tabs[m_activeTab].content->HandleMouseDown(x, y, button)) {
            return true;
        }
    }
    
    return Widget::HandleMouseDown(x, y, button);
}

// FileTree, SyntaxHighlightTextEditor, and FileExplorer implementations
// These are complex widgets that would require extensive implementation
// For now, providing basic structure

FileTree::FileTree()
    : Widget()
    , m_scrollOffset(0)
    , m_itemHeight(20)
{
    m_height = 300;
}

FileTree::~FileTree() {
}

void FileTree::SetRootPath(const std::wstring& path) {
    m_rootPath = path;
    m_rootNode = std::make_shared<TreeNode>(path, path, true);
    LoadDirectory(m_rootNode);
}

std::wstring FileTree::GetSelectedPath() const {
    if (m_selectedNode) {
        return m_selectedNode->fullPath;
    }
    return L"";
}

void FileTree::SetSelectedPath(const std::wstring& path) {
    if (!m_rootNode) return;
    
    m_selectedNode = FindNodeByPath(m_rootNode, path);
}

void FileTree::LoadDirectory(std::shared_ptr<TreeNode> node) {
    if (!node || !node->isDirectory) return;
    
    // SECURITY: Validate path to prevent directory traversal attacks
    // In production, add additional path validation to prevent malicious paths
    
    WIN32_FIND_DATAW findData;
    std::wstring searchPath = node->fullPath + L"\\*";
    HANDLE hFind = FindFirstFileW(searchPath.c_str(), &findData);
    
    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            if (wcscmp(findData.cFileName, L".") != 0 && wcscmp(findData.cFileName, L"..") != 0) {
                std::wstring fullPath = node->fullPath + L"\\" + findData.cFileName;
                bool isDir = (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
                
                auto child = std::make_shared<TreeNode>(findData.cFileName, fullPath, isDir);
                child->depth = node->depth + 1;
                node->children.push_back(child);
            }
        } while (FindNextFileW(hFind, &findData));
        
        FindClose(hFind);
    }
}

void FileTree::Render(HDC hdc) {
    if (!m_visible || !m_rootNode) return;
    
    RECT bounds = GetBounds();
    Renderer::DrawRoundedRect(hdc, bounds, 4, Color(255, 255, 255, 255), Color(128, 128, 128, 255), 1);
    
    int yOffset = bounds.top;
    RenderNode(hdc, m_rootNode, yOffset);
    
    Widget::Render(hdc);
}

void FileTree::RenderNode(HDC hdc, std::shared_ptr<TreeNode> node, int& yOffset) {
    if (!node) return;
    
    RECT bounds = GetBounds();
    int indent = node->depth * 15;
    
    RECT nodeRect = {bounds.left + indent, yOffset, bounds.right, yOffset + m_itemHeight};
    
    if (node == m_selectedNode) {
        HBRUSH brush = CreateSolidBrush(RGB(200, 220, 255));
        FillRect(hdc, &nodeRect, brush);
        DeleteObject(brush);
    }
    
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(0, 0, 0));
    
    std::wstring displayText = node->isDirectory ? L"📁 " : L"📄 ";
    displayText += node->name;
    
    DrawTextW(hdc, displayText.c_str(), -1, &nodeRect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    
    yOffset += m_itemHeight;
    
    if (node->expanded && node->isDirectory) {
        for (auto& child : node->children) {
            RenderNode(hdc, child, yOffset);
        }
    }
}

std::shared_ptr<FileTree::TreeNode> FileTree::HitTestNode(int x, int y) {
    if (!m_rootNode) return nullptr;
    
    RECT bounds = GetBounds();
    int yOffset = bounds.top;
    return HitTestNodeRecursive(m_rootNode, x, y, yOffset);
}

std::shared_ptr<FileTree::TreeNode> FileTree::HitTestNodeRecursive(std::shared_ptr<TreeNode> node, int x, int y, int& yOffset) {
    if (!node) return nullptr;
    
    RECT bounds = GetBounds();
    int indent = node->depth * 15;
    RECT nodeRect = {bounds.left + indent, yOffset, bounds.right, yOffset + m_itemHeight};
    
    if (x >= nodeRect.left && x < nodeRect.right && y >= nodeRect.top && y < nodeRect.bottom) {
        return node;
    }
    
    yOffset += m_itemHeight;
    
    if (node->expanded && node->isDirectory) {
        for (auto& child : node->children) {
            auto result = HitTestNodeRecursive(child, x, y, yOffset);
            if (result) return result;
        }
    }
    
    return nullptr;
}

std::shared_ptr<FileTree::TreeNode> FileTree::FindNodeByPath(std::shared_ptr<TreeNode> node, const std::wstring& path) {
    if (!node) return nullptr;
    
    if (node->fullPath == path) {
        return node;
    }
    
    for (auto& child : node->children) {
        auto result = FindNodeByPath(child, path);
        if (result) return result;
    }
    
    return nullptr;
}

bool FileTree::HandleMouseDown(int x, int y, int button) {
    if (!m_visible || !m_enabled) return false;
    
    if (HitTest(x, y)) {
        auto node = HitTestNode(x, y);
        if (node) {
            m_selectedNode = node;
            if (node->isDirectory) {
                node->expanded = !node->expanded;
                if (node->expanded && node->children.empty()) {
                    LoadDirectory(node);
                }
            }
            TriggerEvent(WidgetEvent::CLICK, node.get());
            return true;
        }
    }
    
    return false;
}

// SyntaxHighlightTextEditor implementation
SyntaxHighlightTextEditor::SyntaxHighlightTextEditor()
    : Widget()
    , m_language(Language::PLAIN_TEXT)
    , m_showLineNumbers(true)
    , m_wordWrap(false)
    , m_cursorLine(0)
    , m_cursorColumn(0)
    , m_scrollOffsetX(0)
    , m_scrollOffsetY(0)
    , m_keywordColor(0, 0, 255, 255)
    , m_stringColor(163, 21, 21, 255)
    , m_commentColor(0, 128, 0, 255)
    , m_numberColor(255, 140, 0, 255)
    , m_operatorColor(128, 0, 128, 255)
{
    m_height = 400;
}

SyntaxHighlightTextEditor::~SyntaxHighlightTextEditor() {
}

void SyntaxHighlightTextEditor::SetText(const std::wstring& text) {
    m_text = text;
    m_lines.clear();
    
    // Split into lines
    size_t pos = 0;
    size_t newlinePos;
    while ((newlinePos = text.find(L'\n', pos)) != std::wstring::npos) {
        m_lines.push_back(text.substr(pos, newlinePos - pos));
        pos = newlinePos + 1;
    }
    if (pos < text.length()) {
        m_lines.push_back(text.substr(pos));
    }
    
    UpdateSyntaxHighlighting();
}

std::vector<SyntaxHighlightTextEditor::SyntaxToken> SyntaxHighlightTextEditor::TokenizeLine(const std::wstring& line) {
    std::vector<SyntaxToken> tokens;
    
    if (m_language == Language::PLAIN_TEXT || line.empty()) {
        SyntaxToken token;
        token.text = line;
        token.color = Color(0, 0, 0, 255);
        tokens.push_back(token);
        return tokens;
    }
    
    // Simple tokenization for C++
    if (m_language == Language::CPP) {
        static const std::vector<std::wstring> keywords = {
            L"int", L"float", L"double", L"char", L"bool", L"void",
            L"if", L"else", L"for", L"while", L"do", L"switch", L"case", L"break", L"continue",
            L"return", L"class", L"struct", L"namespace", L"public", L"private", L"protected",
            L"const", L"static", L"virtual", L"override", L"new", L"delete", L"this",
            L"true", L"false", L"nullptr", L"auto", L"typedef", L"using", L"template"
        };
        
        size_t pos = 0;
        while (pos < line.length()) {
            wchar_t ch = line[pos];
            
            // Skip whitespace
            if (iswspace(ch)) {
                pos++;
                continue;
            }
            
            // Comments
            if (pos + 1 < line.length() && line[pos] == L'/' && line[pos + 1] == L'/') {
                SyntaxToken token;
                token.text = line.substr(pos);
                token.color = m_commentColor;
                tokens.push_back(token);
                break;
            }
            
            // String literals
            if (ch == L'"' || ch == L'\'') {
                size_t end = pos + 1;
                wchar_t quote = ch;
                while (end < line.length() && line[end] != quote) {
                    if (line[end] == L'\\' && end + 1 < line.length()) {
                        end += 2;
                    } else {
                        end++;
                    }
                }
                if (end < line.length()) end++;
                
                SyntaxToken token;
                token.text = line.substr(pos, end - pos);
                token.color = m_stringColor;
                tokens.push_back(token);
                pos = end;
                continue;
            }
            
            // Numbers
            if (iswdigit(ch)) {
                size_t end = pos + 1;
                bool hasDot = false;
                bool isHex = false;
                
                // Check for hex prefix (0x or 0X)
                if (ch == L'0' && end < line.length() && (line[end] == L'x' || line[end] == L'X')) {
                    isHex = true;
                    end++; // Skip 'x' or 'X'
                }
                
                while (end < line.length()) {
                    wchar_t c = line[end];
                    
                    if (isHex) {
                        // Hex digits: 0-9, a-f, A-F
                        if (!iswdigit(c) && !(c >= L'a' && c <= L'f') && !(c >= L'A' && c <= L'F')) {
                            break;
                        }
                    } else {
                        // Decimal: digits, one decimal point, f/F suffix
                        if (iswdigit(c)) {
                            // Continue
                        } else if (c == L'.' && !hasDot) {
                            hasDot = true;
                        } else if ((c == L'f' || c == L'F') && end + 1 == line.length()) {
                            // f/F suffix only at end
                            end++;
                            break;
                        } else {
                            break;
                        }
                    }
                    end++;
                }
                
                SyntaxToken token;
                token.text = line.substr(pos, end - pos);
                token.color = m_numberColor;
                tokens.push_back(token);
                pos = end;
                continue;
            }
            
            // Identifiers and keywords
            if (iswalpha(ch) || ch == L'_') {
                size_t end = pos + 1;
                while (end < line.length() && (iswalnum(line[end]) || line[end] == L'_')) {
                    end++;
                }
                
                std::wstring word = line.substr(pos, end - pos);
                SyntaxToken token;
                token.text = word;
                
                // Check if keyword
                bool isKeyword = false;
                for (const auto& keyword : keywords) {
                    if (word == keyword) {
                        token.color = m_keywordColor;
                        isKeyword = true;
                        break;
                    }
                }
                
                if (!isKeyword) {
                    token.color = Color(0, 0, 0, 255);
                }
                
                tokens.push_back(token);
                pos = end;
                continue;
            }
            
            // Operators and punctuation
            SyntaxToken token;
            token.text = std::wstring(1, ch);
            token.color = m_operatorColor;
            tokens.push_back(token);
            pos++;
        }
    }
    
    if (tokens.empty()) {
        SyntaxToken token;
        token.text = line;
        token.color = Color(0, 0, 0, 255);
        tokens.push_back(token);
    }
    
    return tokens;
}

void SyntaxHighlightTextEditor::UpdateSyntaxHighlighting() {
    // Tokenize all lines for syntax highlighting
    // This method is called when text or language changes
    // In a real implementation, this would build a token cache for faster rendering
}

void SyntaxHighlightTextEditor::Render(HDC hdc) {
    if (!m_visible) return;
    
    RECT bounds = GetBounds();
    Renderer::DrawRoundedRect(hdc, bounds, 4, Color(255, 255, 255, 255), Color(128, 128, 128, 255), 1);
    
    SetBkMode(hdc, TRANSPARENT);
    int lineHeight = 18;
    int lineNumberWidth = m_showLineNumbers ? 40 : 0;
    
    for (size_t i = m_scrollOffsetY; i < m_lines.size(); i++) {
        int yPos = bounds.top + (int)(i - m_scrollOffsetY) * lineHeight;
        if (yPos >= bounds.bottom) break;
        
        // Draw line number
        if (m_showLineNumbers) {
            RECT lineNumRect = {bounds.left, yPos, bounds.left + lineNumberWidth, yPos + lineHeight};
            SetTextColor(hdc, RGB(128, 128, 128));
            std::wstring lineNum = std::to_wstring(i + 1);
            DrawTextW(hdc, lineNum.c_str(), -1, &lineNumRect, DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
        }
        
        // Draw line text with syntax highlighting
        int xPos = bounds.left + lineNumberWidth + 5;
        auto tokens = TokenizeLine(m_lines[i]);
        
        for (const auto& token : tokens) {
            RECT textRect = {xPos, yPos, bounds.right, yPos + lineHeight};
            SetTextColor(hdc, token.color.ToCOLORREF());
            DrawTextW(hdc, token.text.c_str(), -1, &textRect, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);
            
            // Calculate width of token for next position
            SIZE tokenSize;
            GetTextExtentPoint32W(hdc, token.text.c_str(), (int)token.text.length(), &tokenSize);
            xPos += tokenSize.cx;
        }
    }
    
    Widget::Render(hdc);
}

bool SyntaxHighlightTextEditor::HandleMouseDown(int x, int y, int button) {
    if (!m_visible || !m_enabled) return false;
    
    if (HitTest(x, y)) {
        SetFocused(true);
        
        // Calculate cursor position from click
        RECT bounds = GetBounds();
        int lineHeight = 18;
        int lineNumberWidth = m_showLineNumbers ? 40 : 0;
        
        // Calculate clicked line
        int relY = y - bounds.top;
        int clickedLine = m_scrollOffsetY + relY / lineHeight;
        
        if (clickedLine >= 0 && clickedLine < (int)m_lines.size()) {
            m_cursorLine = clickedLine;
            
            // Calculate column position (approximate)
            int relX = x - (bounds.left + lineNumberWidth + 5);
            if (relX < 0) {
                m_cursorColumn = 0;
            } else {
                // Use average character width for approximation
                HDC hdc = GetDC(nullptr);
                SIZE charSize;
                GetTextExtentPoint32W(hdc, L"W", 1, &charSize);
                ReleaseDC(nullptr, hdc);
                
                int charWidth = charSize.cx;
                m_cursorColumn = relX / charWidth;
                
                // Clamp to line length
                if (m_cursorColumn > (int)m_lines[m_cursorLine].length()) {
                    m_cursorColumn = (int)m_lines[m_cursorLine].length();
                }
            }
        }
        
        return true;
    }
    
    return false;
}

bool SyntaxHighlightTextEditor::HandleKeyDown(int keyCode) {
    if (!Widget::HandleKeyDown(keyCode)) return false;
    
    // Handle text editor key commands
    return true;
}

bool SyntaxHighlightTextEditor::HandleChar(wchar_t ch) {
    if (!Widget::HandleChar(ch)) return false;
    
    // Insert character at cursor
    return true;
}

// FileExplorer implementation
FileExplorer::FileExplorer()
    : Widget()
    , m_selectedIndex(-1)
    , m_scrollOffset(0)
{
    m_height = 400;
}

FileExplorer::~FileExplorer() {
}

void FileExplorer::SetCurrentPath(const std::wstring& path) {
    m_currentPath = path;
    LoadDirectory();
}

void FileExplorer::SetSelectedFile(const std::wstring& filename) {
    for (size_t i = 0; i < m_items.size(); i++) {
        if (m_items[i].name == filename) {
            m_selectedIndex = (int)i;
            break;
        }
    }
}

std::wstring FileExplorer::GetSelectedFile() const {
    if (m_selectedIndex >= 0 && m_selectedIndex < (int)m_items.size()) {
        return m_items[m_selectedIndex].fullPath;
    }
    return L"";
}

void FileExplorer::LoadDirectory() {
    m_items.clear();
    m_selectedIndex = -1;
    
    WIN32_FIND_DATAW findData;
    std::wstring searchPath = m_currentPath + L"\\*";
    HANDLE hFind = FindFirstFileW(searchPath.c_str(), &findData);
    
    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            if (wcscmp(findData.cFileName, L".") != 0 && wcscmp(findData.cFileName, L"..") != 0) {
                FileItem item;
                item.name = findData.cFileName;
                item.fullPath = m_currentPath + L"\\" + findData.cFileName;
                item.isDirectory = (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
                item.size = ((ULONGLONG)findData.nFileSizeHigh << 32) | findData.nFileSizeLow;
                item.modifiedTime = findData.ftLastWriteTime;
                
                m_items.push_back(item);
            }
        } while (FindNextFileW(hFind, &findData));
        
        FindClose(hFind);
    }
}

void FileExplorer::Render(HDC hdc) {
    if (!m_visible) return;
    
    RECT bounds = GetBounds();
    Renderer::DrawRoundedRect(hdc, bounds, 4, Color(255, 255, 255, 255), Color(128, 128, 128, 255), 1);
    
    RECT addrBarRect = {bounds.left, bounds.top, bounds.right, bounds.top + 30};
    RenderAddressBar(hdc, addrBarRect);
    
    RECT fileListRect = {bounds.left, bounds.top + 30, bounds.right, bounds.bottom};
    RenderFileList(hdc, fileListRect);
    
    Widget::Render(hdc);
}

void FileExplorer::RenderAddressBar(HDC hdc, RECT& rect) {
    Renderer::DrawRoundedRect(hdc, rect, 4, Color(240, 240, 240, 255), Color(128, 128, 128, 255), 1);
    
    RECT textRect = rect;
    textRect.left += 5;
    
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(0, 0, 0));
    DrawTextW(hdc, m_currentPath.c_str(), -1, &textRect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
}

void FileExplorer::RenderFileList(HDC hdc, RECT& rect) {
    SetBkMode(hdc, TRANSPARENT);
    int itemHeight = 25;
    int visibleItems = (rect.bottom - rect.top) / itemHeight;
    
    for (int i = m_scrollOffset; i < (int)m_items.size() && i < m_scrollOffset + visibleItems; i++) {
        RECT itemRect = {
            rect.left + 5,
            rect.top + (i - m_scrollOffset) * itemHeight,
            rect.right - 5,
            rect.top + (i - m_scrollOffset + 1) * itemHeight
        };
        
        if (i == m_selectedIndex) {
            HBRUSH brush = CreateSolidBrush(RGB(200, 220, 255));
            FillRect(hdc, &itemRect, brush);
            DeleteObject(brush);
        }
        
        std::wstring displayText = m_items[i].isDirectory ? L"📁 " : L"📄 ";
        displayText += m_items[i].name;
        
        SetTextColor(hdc, RGB(0, 0, 0));
        DrawTextW(hdc, displayText.c_str(), -1, &itemRect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    }
}

bool FileExplorer::HandleMouseDown(int x, int y, int button) {
    if (!m_visible || !m_enabled) return false;
    
    if (HitTest(x, y)) {
        RECT bounds = GetBounds();
        int itemHeight = 25;
        int relY = y - (bounds.top + 30);
        
        if (relY >= 0) {
            int index = m_scrollOffset + relY / itemHeight;
            
            if (index >= 0 && index < (int)m_items.size()) {
                m_selectedIndex = index;
                
                if (m_items[index].isDirectory) {
                    // Double-click to enter directory (simplified)
                    SetCurrentPath(m_items[index].fullPath);
                }
                
                TriggerEvent(WidgetEvent::CLICK, &index);
                return true;
            }
        }
    }
    
    return false;
}

} // namespace SDK
