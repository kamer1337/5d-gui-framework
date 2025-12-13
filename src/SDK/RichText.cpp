#include "../../include/SDK/RichText.h"
#include "../../include/SDK/Renderer.h"
#include <algorithm>
#include <sstream>

namespace SDK {

// RichTextDocument implementation
RichTextDocument::RichTextDocument() {
}

void RichTextDocument::Clear() {
    m_spans.clear();
}

void RichTextDocument::AddSpan(const TextSpan& span) {
    m_spans.push_back(span);
}

void RichTextDocument::InsertSpan(size_t index, const TextSpan& span) {
    if (index <= m_spans.size()) {
        m_spans.insert(m_spans.begin() + index, span);
    }
}

void RichTextDocument::RemoveSpan(size_t index) {
    if (index < m_spans.size()) {
        m_spans.erase(m_spans.begin() + index);
    }
}

void RichTextDocument::AddText(const std::wstring& text, bool bold, bool italic) {
    TextSpan span(text);
    span.bold = bold;
    span.italic = italic;
    AddSpan(span);
}

void RichTextDocument::AddHeading(const std::wstring& text, int level) {
    TextSpan span(text);
    span.bold = true;
    span.fontSize = RichTextDefaults::DEFAULT_FONT_SIZE + (4 - level) * 4;
    AddSpan(span);
}

void RichTextDocument::AddLink(const std::wstring& text, const std::wstring& url) {
    TextSpan span(text);
    span.isLink = true;
    span.linkUrl = url;
    span.foregroundColor = Color(0, 0, 255, 255); // Blue
    span.underline = true;
    AddSpan(span);
}

void RichTextDocument::AddParagraph(const std::wstring& text) {
    AddText(text + L"\n", false, false);
}

void RichTextDocument::ParseMarkdown(const std::wstring& markdown) {
    Clear();
    
    std::wistringstream stream(markdown);
    std::wstring line;
    
    while (std::getline(stream, line)) {
        if (line.empty()) {
            AddText(L"\n");
            continue;
        }
        
        // Heading
        if (line[0] == L'#') {
            int level = 0;
            size_t pos = 0;
            while (pos < line.length() && line[pos] == L'#') {
                level++;
                pos++;
            }
            while (pos < line.length() && line[pos] == L' ') {
                pos++;
            }
            AddHeading(line.substr(pos), level);
            AddText(L"\n");
            continue;
        }
        
        // Parse inline formatting
        size_t pos = 0;
        while (pos < line.length()) {
            // Bold: **text**
            if (pos + 1 < line.length() && line[pos] == L'*' && line[pos + 1] == L'*') {
                size_t end = line.find(L"**", pos + 2);
                if (end != std::wstring::npos) {
                    AddText(line.substr(pos + 2, end - pos - 2), true, false);
                    pos = end + 2;
                    continue;
                }
            }
            
            // Italic: *text*
            if (line[pos] == L'*') {
                size_t end = line.find(L'*', pos + 1);
                if (end != std::wstring::npos) {
                    AddText(line.substr(pos + 1, end - pos - 1), false, true);
                    pos = end + 1;
                    continue;
                }
            }
            
            // Link: [text](url)
            if (line[pos] == L'[') {
                size_t textEnd = line.find(L']', pos + 1);
                if (textEnd != std::wstring::npos && textEnd + 1 < line.length() && line[textEnd + 1] == L'(') {
                    size_t urlEnd = line.find(L')', textEnd + 2);
                    if (urlEnd != std::wstring::npos) {
                        std::wstring text = line.substr(pos + 1, textEnd - pos - 1);
                        std::wstring url = line.substr(textEnd + 2, urlEnd - textEnd - 2);
                        AddLink(text, url);
                        pos = urlEnd + 1;
                        continue;
                    }
                }
            }
            
            // Regular text
            size_t nextSpecial = line.find_first_of(L"*[", pos);
            if (nextSpecial == std::wstring::npos) {
                AddText(line.substr(pos));
                break;
            } else {
                if (nextSpecial > pos) {
                    AddText(line.substr(pos, nextSpecial - pos));
                }
                pos = nextSpecial;
            }
        }
        
        AddText(L"\n");
    }
}

std::wstring RichTextDocument::ToPlainText() const {
    std::wstring result;
    for (const auto& span : m_spans) {
        result += span.text;
    }
    return result;
}

std::wstring RichTextDocument::ToHtml() const {
    std::wstring html;
    for (const auto& span : m_spans) {
        std::wstring spanHtml = span.text;
        
        if (span.bold) {
            spanHtml = L"<b>" + spanHtml + L"</b>";
        }
        if (span.italic) {
            spanHtml = L"<i>" + spanHtml + L"</i>";
        }
        if (span.underline) {
            spanHtml = L"<u>" + spanHtml + L"</u>";
        }
        if (span.strikethrough) {
            spanHtml = L"<s>" + spanHtml + L"</s>";
        }
        if (span.isLink) {
            spanHtml = L"<a href=\"" + span.linkUrl + L"\">" + spanHtml + L"</a>";
        }
        
        html += spanHtml;
    }
    return html;
}

// RichTextBox implementation
RichTextBox::RichTextBox()
    : Widget()
    , m_document(std::make_shared<RichTextDocument>())
    , m_readOnly(false)
    , m_editable(true)
    , m_scrollable(true)
    , m_selectionStart(0)
    , m_selectionEnd(0)
    , m_cursorPosition(0)
    , m_scrollOffset(0)
    , m_maxScrollOffset(0)
    , m_lineSpacing(1.2f)
    , m_paragraphSpacing(5)
    , m_hoveredSpanIndex(-1)
{
    m_width = 300;
    m_height = 200;
}

void RichTextBox::SetText(const std::wstring& text) {
    m_document->Clear();
    m_document->AddText(text);
    m_selectionStart = 0;
    m_selectionEnd = 0;
    m_cursorPosition = 0;
}

std::wstring RichTextBox::GetText() const {
    return m_document->ToPlainText();
}

void RichTextBox::AppendText(const std::wstring& text) {
    m_document->AddText(text);
}

void RichTextBox::Clear() {
    m_document->Clear();
    m_selectionStart = 0;
    m_selectionEnd = 0;
    m_cursorPosition = 0;
    m_scrollOffset = 0;
}

void RichTextBox::SelectAll() {
    std::wstring text = GetText();
    m_selectionStart = 0;
    m_selectionEnd = text.length();
}

void RichTextBox::ClearSelection() {
    m_selectionStart = 0;
    m_selectionEnd = 0;
}

void RichTextBox::SetSelectionBold(bool bold) {
    // Apply formatting to selected spans
    // This is a simplified implementation that applies to all currently selected text
    // A complete implementation would track character positions and split spans accordingly
    if (!HasSelection()) return;
    
    // For now, apply to the last added span (typical use case for formatting toolbar)
    auto& spans = m_document->GetSpans();
    if (!spans.empty()) {
        // In a full implementation, we would calculate which spans fall within
        // m_selectionStart and m_selectionEnd and modify only those
        // For this basic version, we apply to recently added content
        for (size_t i = 0; i < spans.size(); i++) {
            // Only modify spans that don't have special formatting (links, headings)
            if (!spans[i].isLink) {
                spans[i].bold = bold;
            }
        }
    }
}

void RichTextBox::SetSelectionItalic(bool italic) {
    if (!HasSelection()) return;
    
    auto& spans = m_document->GetSpans();
    if (!spans.empty()) {
        for (size_t i = 0; i < spans.size(); i++) {
            if (!spans[i].isLink) {
                spans[i].italic = italic;
            }
        }
    }
}

void RichTextBox::SetSelectionUnderline(bool underline) {
    if (!HasSelection()) return;
    
    auto& spans = m_document->GetSpans();
    if (!spans.empty()) {
        for (size_t i = 0; i < spans.size(); i++) {
            // Don't override link underlines
            if (!spans[i].isLink) {
                spans[i].underline = underline;
            }
        }
    }
}

void RichTextBox::SetSelectionColor(Color color) {
    if (!HasSelection()) return;
    
    auto& spans = m_document->GetSpans();
    if (!spans.empty()) {
        for (size_t i = 0; i < spans.size(); i++) {
            if (!spans[i].isLink) {
                spans[i].foregroundColor = color;
            }
        }
    }
}

void RichTextBox::SetSelectionFontSize(int size) {
    if (!HasSelection()) return;
    
    auto& spans = m_document->GetSpans();
    if (!spans.empty()) {
        for (size_t i = 0; i < spans.size(); i++) {
            spans[i].fontSize = size;
        }
    }
}

void RichTextBox::ScrollToTop() {
    m_scrollOffset = 0;
}

void RichTextBox::ScrollToBottom() {
    m_scrollOffset = m_maxScrollOffset;
}

void RichTextBox::ScrollToPosition(int position) {
    m_scrollOffset = std::max(0, std::min(position, m_maxScrollOffset));
}

int RichTextBox::GetLineHeight(const TextSpan& span) const {
    return static_cast<int>(span.fontSize * m_lineSpacing);
}

void RichTextBox::RenderSpan(HDC hdc, const RECT& rect, const TextSpan& span, int& x, int& y) {
    // Create font with formatting
    int fontWeight = span.bold ? FW_BOLD : FW_NORMAL;
    BOOL italic = span.italic ? TRUE : FALSE;
    BOOL underline = span.underline ? TRUE : FALSE;
    BOOL strikeout = span.strikethrough ? TRUE : FALSE;
    
    HFONT hFont = CreateFontW(
        span.fontSize, 0, 0, 0, fontWeight, italic, underline, strikeout,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
        span.fontFamily.c_str()
    );
    
    HFONT oldFont = (HFONT)SelectObject(hdc, hFont);
    
    // Set colors
    SetTextColor(hdc, span.foregroundColor.ToCOLORREF());
    SetBkMode(hdc, TRANSPARENT);
    
    if (span.backgroundColor.a > 0) {
        SetBkMode(hdc, OPAQUE);
        SetBkColor(hdc, span.backgroundColor.ToCOLORREF());
    }
    
    // Draw text with word wrapping
    std::wstring text = span.text;
    for (size_t i = 0; i < text.length(); i++) {
        wchar_t ch = text[i];
        
        if (ch == L'\n') {
            y += GetLineHeight(span);
            x = rect.left + 5;
            continue;
        }
        
        std::wstring charStr(1, ch);
        SIZE textSize;
        GetTextExtentPoint32W(hdc, charStr.c_str(), 1, &textSize);
        
        // Word wrap
        if (x + textSize.cx > rect.right - 5) {
            y += GetLineHeight(span);
            x = rect.left + 5;
        }
        
        // Draw character
        TextOutW(hdc, x, y - m_scrollOffset, charStr.c_str(), 1);
        x += textSize.cx;
    }
    
    SelectObject(hdc, oldFont);
    DeleteObject(hFont);
}

void RichTextBox::CalculateLayout() {
    RECT bounds;
    GetBounds(bounds);
    
    int totalHeight = 0;
    int y = bounds.top + 5;
    
    for (const auto& span : m_document->GetSpans()) {
        totalHeight += GetLineHeight(span);
    }
    
    m_maxScrollOffset = std::max(0, totalHeight - (bounds.bottom - bounds.top));
}

void RichTextBox::HandleMouseMove(int x, int y) {
    RECT bounds;
    GetBounds(bounds);
    
    if (!HitTest(x, y)) {
        m_hoveredSpanIndex = -1;
        return;
    }
    
    // Find hovered span for link highlighting
    int currentX = bounds.left + 5;
    int currentY = bounds.top + 5 - m_scrollOffset;
    
    const auto& spans = m_document->GetSpans();
    for (size_t i = 0; i < spans.size(); i++) {
        const auto& span = spans[i];
        
        if (span.isLink) {
            // Simple hit test for links
            int spanHeight = GetLineHeight(span);
            if (y >= currentY && y < currentY + spanHeight) {
                m_hoveredSpanIndex = static_cast<int>(i);
                return;
            }
        }
        
        currentY += GetLineHeight(span);
    }
    
    m_hoveredSpanIndex = -1;
}

void RichTextBox::HandleMouseClick(int x, int y) {
    if (m_hoveredSpanIndex >= 0 && m_hoveredSpanIndex < (int)m_document->GetSpans().size()) {
        const auto& span = m_document->GetSpans()[m_hoveredSpanIndex];
        if (span.isLink) {
            // Trigger link clicked event
            TriggerEvent(WidgetEvent::CLICK, (void*)span.linkUrl.c_str());
        }
    }
}

void RichTextBox::Render(HDC hdc) {
    if (!m_visible) return;
    
    RECT bounds;
    GetBounds(bounds);
    
    // Draw background
    HBRUSH brush = CreateSolidBrush(RGB(255, 255, 255));
    FillRect(hdc, &bounds, brush);
    DeleteObject(brush);
    
    // Draw border
    Renderer::DrawRoundedRect(hdc, bounds, 4, Color(255, 255, 255, 255), Color(180, 180, 180, 255), 1);
    
    // Calculate layout
    CalculateLayout();
    
    // Set clipping region
    HRGN clipRegion = CreateRectRgn(bounds.left, bounds.top, bounds.right, bounds.bottom);
    SelectClipRgn(hdc, clipRegion);
    
    // Render spans
    int x = bounds.left + 5;
    int y = bounds.top + 5;
    
    for (size_t i = 0; i < m_document->GetSpans().size(); i++) {
        const auto& span = m_document->GetSpans()[i];
        
        // Highlight hovered links
        if (span.isLink && m_hoveredSpanIndex == (int)i) {
            SetCursor(LoadCursor(nullptr, IDC_HAND));
        }
        
        RenderSpan(hdc, bounds, span, x, y);
    }
    
    // Reset clipping
    SelectClipRgn(hdc, nullptr);
    DeleteObject(clipRegion);
    
    Widget::Render(hdc);
}

void RichTextBox::HandleEvent(WidgetEvent event, void* data) {
    switch (event) {
        case WidgetEvent::MOUSE_MOVE:
            if (data) {
                int* coords = static_cast<int*>(data);
                HandleMouseMove(coords[0], coords[1]);
            }
            break;
        case WidgetEvent::CLICK:
            if (data) {
                int* coords = static_cast<int*>(data);
                HandleMouseClick(coords[0], coords[1]);
            }
            break;
        default:
            break;
    }
}

// RichTextLabel implementation
RichTextLabel::RichTextLabel() : RichTextBox() {
    SetReadOnly(true);
    SetEditable(false);
}

void RichTextLabel::Render(HDC hdc) {
    if (!m_visible) return;
    
    RECT bounds;
    GetBounds(bounds);
    
    // Draw background (transparent for label)
    SetBkMode(hdc, TRANSPARENT);
    
    // Calculate layout
    CalculateLayout();
    
    // Render spans
    int x = bounds.left;
    int y = bounds.top;
    
    for (const auto& span : GetDocument()->GetSpans()) {
        RenderSpan(hdc, bounds, span, x, y);
    }
    
    Widget::Render(hdc);
}

} // namespace SDK
