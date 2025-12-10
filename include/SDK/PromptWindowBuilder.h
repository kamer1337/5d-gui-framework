#pragma once

#include "Widget.h"
#include "WidgetManager.h"
#include "ProgressBar.h"
#include "Tooltip.h"
#include "Window.h"
#include <string>
#include <vector>
#include <map>
#include <memory>

namespace SDK {

/**
 * PromptWindowBuilder - Template-based window and widget generation
 * 
 * Parses simple text prompts to generate windows with widgets and subwindows.
 * Designed to be extensible for future neural network integration.
 * 
 * Example prompts:
 * - "window 800x600 'My App' with progressbar and tooltip"
 * - "create dialog with 3 progressbars and ok button"
 * - "window with subwindow 'Settings' and 2 progressbars"
 */
class PromptWindowBuilder {
public:
    struct WindowSpec {
        std::wstring title;
        int width;
        int height;
        int x;
        int y;
        bool isDialog;
        std::vector<std::wstring> widgets;
        std::vector<WindowSpec> subwindows;
    };
    
    PromptWindowBuilder();
    ~PromptWindowBuilder() = default;
    
    // Parse a prompt and generate window specification
    WindowSpec ParsePrompt(const std::wstring& prompt);
    
    // Create a window from specification
    HWND CreateWindowFromSpec(const WindowSpec& spec, HINSTANCE hInstance, HWND parent = nullptr);
    
    // Create widgets from specification
    std::vector<std::shared_ptr<Widget>> CreateWidgetsFromSpec(const WindowSpec& spec);
    
    // Register custom widget factory
    using WidgetFactory = std::function<std::shared_ptr<Widget>(const std::wstring&)>;
    void RegisterWidgetFactory(const std::wstring& widgetType, WidgetFactory factory);
    
    // Parse and create a complete window with widgets in one call
    HWND BuildFromPrompt(const std::wstring& prompt, HINSTANCE hInstance, HWND parent = nullptr);
    
    // Universal window creation function that consolidates all window creation patterns
    // Creates window, registers with SDK, applies theme, and optionally adds widgets
    struct WindowConfig {
        std::wstring className;
        std::wstring title;
        int width;
        int height;
        int x;
        int y;
        DWORD style;
        DWORD exStyle;
        HWND parent;
        std::shared_ptr<Theme> theme;
        WindowDepth depth;
        bool roundedCorners;
        int cornerRadius;
        std::function<void(HDC)> renderCallback;
        
        // Default constructor with sensible defaults
        WindowConfig()
            : className(L"5DGUIDemo")
            , title(L"Window")
            , width(800)
            , height(600)
            , x(CW_USEDEFAULT)
            , y(CW_USEDEFAULT)
            , style(WS_OVERLAPPEDWINDOW)
            , exStyle(WS_EX_LAYERED)
            , parent(nullptr)
            , theme(nullptr)
            , depth(WindowDepth::FOREGROUND)
            , roundedCorners(false)
            , cornerRadius(12)
            , renderCallback(nullptr)
        {}
    };
    
    static HWND CreateWidgetsWindow(const WindowConfig& config, HINSTANCE hInstance);
    
    // Get last created widget manager (for accessing widgets)
    std::shared_ptr<WidgetManager> GetLastWidgetManager() const { return m_lastWidgetManager; }
    
private:
    struct Token {
        enum Type {
            WORD,
            NUMBER,
            STRING,
            WITH,
            AND,
            UNKNOWN
        };
        
        Type type;
        std::wstring value;
    };
    
    std::vector<Token> Tokenize(const std::wstring& prompt);
    void ParseWindowDimensions(const std::vector<Token>& tokens, size_t& index, WindowSpec& spec);
    void ParseWidgets(const std::vector<Token>& tokens, size_t& index, WindowSpec& spec);
    void ParseSubwindows(const std::vector<Token>& tokens, size_t& index, WindowSpec& spec);
    
    std::shared_ptr<Widget> CreateWidget(const std::wstring& widgetType);
    void LayoutWidgets(std::vector<std::shared_ptr<Widget>>& widgets, int windowWidth, int windowHeight);
    
    std::map<std::wstring, WidgetFactory> m_widgetFactories;
    std::shared_ptr<WidgetManager> m_lastWidgetManager;
};

} // namespace SDK
