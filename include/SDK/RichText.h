#pragma once

#include "Widget.h"
#include "Theme.h"
#include <string>
#include <vector>
#include <memory>

namespace SDK {

/**
 * TextSpan - Represents a segment of text with formatting
 */
struct TextSpan {
    std::wstring text;
    
    // Formatting
    bool bold = false;
    bool italic = false;
    bool underline = false;
    bool strikethrough = false;
    
    // Colors
    Color foregroundColor = Color(0, 0, 0, 255);
    Color backgroundColor = Color(255, 255, 255, 0);  // Transparent by default
    
    // Font
    std::wstring fontFamily = L"Segoe UI";
    int fontSize = 12;
    
    // Link
    bool isLink = false;
    std::wstring linkUrl;
    
    TextSpan() = default;
    TextSpan(const std::wstring& txt) : text(txt) {}
};

/**
 * RichTextDocument - Container for rich text content
 */
class RichTextDocument {
public:
    RichTextDocument();
    ~RichTextDocument() = default;
    
    // Content management
    void Clear();
    void AddSpan(const TextSpan& span);
    void InsertSpan(size_t index, const TextSpan& span);
    void RemoveSpan(size_t index);
    
    std::vector<TextSpan>& GetSpans() { return m_spans; }
    const std::vector<TextSpan>& GetSpans() const { return m_spans; }
    
    // Quick text addition with formatting
    void AddText(const std::wstring& text, bool bold = false, bool italic = false);
    void AddHeading(const std::wstring& text, int level = 1);
    void AddLink(const std::wstring& text, const std::wstring& url);
    void AddParagraph(const std::wstring& text);
    
    // Markdown parsing (basic)
    void ParseMarkdown(const std::wstring& markdown);
    
    // Export
    std::wstring ToPlainText() const;
    std::wstring ToHtml() const;
    
private:
    std::vector<TextSpan> m_spans;
};

/**
 * RichTextBox - Widget for displaying and editing rich text
 */
class RichTextBox : public Widget {
public:
    RichTextBox();
    ~RichTextBox() override = default;
    
    // Document
    void SetDocument(std::shared_ptr<RichTextDocument> document) { m_document = document; }
    std::shared_ptr<RichTextDocument> GetDocument() const { return m_document; }
    
    // Text operations
    void SetText(const std::wstring& text);
    std::wstring GetText() const;
    
    void AppendText(const std::wstring& text);
    void Clear();
    
    // Editing
    void SetReadOnly(bool readOnly) { m_readOnly = readOnly; }
    bool IsReadOnly() const { return m_readOnly; }
    
    void SetEditable(bool editable) { m_editable = editable; }
    bool IsEditable() const { return m_editable; }
    
    // Selection
    void SelectAll();
    void ClearSelection();
    bool HasSelection() const { return m_selectionStart != m_selectionEnd; }
    
    void SetSelectionRange(size_t start, size_t end) {
        m_selectionStart = start;
        m_selectionEnd = end;
    }
    
    // Formatting operations
    void SetSelectionBold(bool bold);
    void SetSelectionItalic(bool italic);
    void SetSelectionUnderline(bool underline);
    void SetSelectionColor(Color color);
    void SetSelectionFontSize(int size);
    
    // Scrolling
    void SetScrollable(bool scrollable) { m_scrollable = scrollable; }
    bool IsScrollable() const { return m_scrollable; }
    
    void ScrollToTop();
    void ScrollToBottom();
    void ScrollToPosition(int position);
    
    // Widget overrides
    void Render(HDC hdc) override;
    void HandleEvent(WidgetEvent event, void* data) override;
    
    // Appearance
    void SetLineSpacing(float spacing) { m_lineSpacing = spacing; }
    float GetLineSpacing() const { return m_lineSpacing; }
    
    void SetParagraphSpacing(int spacing) { m_paragraphSpacing = spacing; }
    int GetParagraphSpacing() const { return m_paragraphSpacing; }
    
private:
    std::shared_ptr<RichTextDocument> m_document;
    
    bool m_readOnly;
    bool m_editable;
    bool m_scrollable;
    
    size_t m_selectionStart;
    size_t m_selectionEnd;
    size_t m_cursorPosition;
    
    int m_scrollOffset;
    int m_maxScrollOffset;
    
    float m_lineSpacing;
    int m_paragraphSpacing;
    
    // Rendering helpers
    void RenderSpan(HDC hdc, const RECT& rect, const TextSpan& span, int& x, int& y);
    void CalculateLayout();
    int GetLineHeight(const TextSpan& span) const;
    
    // Interaction
    int m_hoveredSpanIndex;
    void HandleMouseMove(int x, int y);
    void HandleMouseClick(int x, int y);
};

/**
 * RichTextLabel - Simple read-only rich text display
 */
class RichTextLabel : public RichTextBox {
public:
    RichTextLabel();
    ~RichTextLabel() override = default;
    
    // Widget overrides
    void Render(HDC hdc) override;
};

} // namespace SDK
