#include "../../include/SDK/PromptWindowBuilder.h"
#include "../../include/SDK/WindowManager.h"
#include <algorithm>
#include <cctype>
#include <sstream>

namespace SDK {

PromptWindowBuilder::PromptWindowBuilder() {
    // Register default widget factories
    RegisterWidgetFactory(L"progressbar", [](const std::wstring&) {
        return std::make_shared<ProgressBar>();
    });
    
    RegisterWidgetFactory(L"tooltip", [](const std::wstring&) {
        auto tooltip = std::make_shared<Tooltip>();
        tooltip->SetText(L"Tooltip");
        return tooltip;
    });
}

std::vector<PromptWindowBuilder::Token> PromptWindowBuilder::Tokenize(const std::wstring& prompt) {
    std::vector<Token> tokens;
    std::wstring current;
    bool inString = false;
    
    for (size_t i = 0; i < prompt.length(); i++) {
        wchar_t ch = prompt[i];
        
        if (inString) {
            if (ch == L'\'') {
                inString = false;
                tokens.push_back({Token::STRING, current});
                current.clear();
            } else {
                current += ch;
            }
        } else {
            if (ch == L'\'') {
                inString = true;
            } else if (std::isspace(ch)) {
                if (!current.empty()) {
                    // Determine token type
                    Token::Type type = Token::WORD;
                    
                    if (std::all_of(current.begin(), current.end(), ::iswdigit)) {
                        type = Token::NUMBER;
                    } else if (current == L"with") {
                        type = Token::WITH;
                    } else if (current == L"and") {
                        type = Token::AND;
                    }
                    
                    tokens.push_back({type, current});
                    current.clear();
                }
            } else {
                current += ch;
            }
        }
    }
    
    if (!current.empty()) {
        Token::Type type = Token::WORD;
        if (std::all_of(current.begin(), current.end(), ::iswdigit)) {
            type = Token::NUMBER;
        }
        tokens.push_back({type, current});
    }
    
    return tokens;
}

void PromptWindowBuilder::ParseWindowDimensions(const std::vector<Token>& tokens, size_t& index, WindowSpec& spec) {
    // Look for patterns like "800x600" or "width:800 height:600"
    while (index < tokens.size()) {
        const auto& token = tokens[index];
        
        if (token.type == Token::WORD && token.value.find(L"x") != std::wstring::npos) {
            // Parse "800x600" format
            size_t xPos = token.value.find(L'x');
            std::wstring widthStr = token.value.substr(0, xPos);
            std::wstring heightStr = token.value.substr(xPos + 1);
            
            spec.width = std::stoi(widthStr);
            spec.height = std::stoi(heightStr);
            index++;
            break;
        } else if (token.type == Token::NUMBER) {
            // Look for adjacent number (width height pattern)
            if (index + 1 < tokens.size() && tokens[index + 1].type == Token::NUMBER) {
                spec.width = std::stoi(token.value);
                spec.height = std::stoi(tokens[index + 1].value);
                index += 2;
                break;
            }
        }
        
        index++;
    }
}

void PromptWindowBuilder::ParseWidgets(const std::vector<Token>& tokens, size_t& index, WindowSpec& spec) {
    // Look for widget names after "with" keyword
    while (index < tokens.size()) {
        const auto& token = tokens[index];
        
        if (token.type == Token::WITH) {
            index++;
            continue;
        }
        
        if (token.type == Token::AND) {
            index++;
            continue;
        }
        
        if (token.type == Token::WORD) {
            std::wstring widgetType = token.value;
            std::transform(widgetType.begin(), widgetType.end(), widgetType.begin(), ::towlower);
            
            // Check if it's a known widget type
            if (m_widgetFactories.find(widgetType) != m_widgetFactories.end()) {
                spec.widgets.push_back(widgetType);
            }
        } else if (token.type == Token::NUMBER && index + 1 < tokens.size()) {
            // Check for numeric prefix (e.g., "3 progressbars")
            int count = std::stoi(token.value);
            std::wstring nextWidget = tokens[index + 1].value;
            std::transform(nextWidget.begin(), nextWidget.end(), nextWidget.begin(), ::towlower);
            
            // Remove plural 's' if present
            if (!nextWidget.empty() && nextWidget.back() == L's') {
                nextWidget.pop_back();
            }
            
            if (m_widgetFactories.find(nextWidget) != m_widgetFactories.end()) {
                for (int i = 0; i < count; i++) {
                    spec.widgets.push_back(nextWidget);
                }
                index++;
            }
        }
        
        index++;
        
        // Stop at certain keywords
        if (token.type == Token::WORD && 
            (token.value == L"subwindow" || token.value == L"child")) {
            break;
        }
    }
}

void PromptWindowBuilder::ParseSubwindows(const std::vector<Token>& tokens, size_t& index, WindowSpec& spec) {
    // Parse subwindow specifications
    while (index < tokens.size()) {
        const auto& token = tokens[index];
        
        if (token.type == Token::WORD && 
            (token.value == L"subwindow" || token.value == L"child")) {
            
            WindowSpec subwindow;
            subwindow.width = 300;
            subwindow.height = 200;
            subwindow.x = 50;
            subwindow.y = 50;
            
            index++;
            
            // Look for title
            if (index < tokens.size() && tokens[index].type == Token::STRING) {
                subwindow.title = tokens[index].value;
                index++;
            }
            
            // Parse subwindow widgets
            ParseWidgets(tokens, index, subwindow);
            
            spec.subwindows.push_back(subwindow);
        }
        
        index++;
    }
}

PromptWindowBuilder::WindowSpec PromptWindowBuilder::ParsePrompt(const std::wstring& prompt) {
    WindowSpec spec;
    
    // Set defaults
    spec.width = 800;
    spec.height = 600;
    spec.x = CW_USEDEFAULT;
    spec.y = CW_USEDEFAULT;
    spec.title = L"Window";
    spec.isDialog = false;
    
    auto tokens = Tokenize(prompt);
    
    size_t index = 0;
    while (index < tokens.size()) {
        const auto& token = tokens[index];
        
        // Look for "window" or "dialog" keyword
        if (token.type == Token::WORD) {
            if (token.value == L"window") {
                index++;
                ParseWindowDimensions(tokens, index, spec);
                
                // Look for title
                if (index < tokens.size() && tokens[index].type == Token::STRING) {
                    spec.title = tokens[index].value;
                    index++;
                }
                
                ParseWidgets(tokens, index, spec);
                ParseSubwindows(tokens, index, spec);
                break;
            } else if (token.value == L"dialog") {
                spec.isDialog = true;
                spec.width = 400;
                spec.height = 300;
                index++;
                
                ParseWidgets(tokens, index, spec);
                break;
            } else if (token.value == L"create") {
                index++;
                continue;
            }
        }
        
        index++;
    }
    
    return spec;
}

HWND PromptWindowBuilder::CreateWindowFromSpec(const WindowSpec& spec, HINSTANCE hInstance, HWND parent) {
    DWORD style = WS_OVERLAPPEDWINDOW;
    DWORD exStyle = WS_EX_LAYERED;
    
    if (spec.isDialog) {
        style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU;
        if (parent) {
            style |= WS_CHILD;
        }
    }
    
    HWND hwnd = CreateWindowExW(
        exStyle,
        L"5DGUIDemo",  // Assuming this class is registered
        spec.title.c_str(),
        style,
        spec.x, spec.y, spec.width, spec.height,
        parent, nullptr, hInstance, nullptr
    );
    
    return hwnd;
}

std::vector<std::shared_ptr<Widget>> PromptWindowBuilder::CreateWidgetsFromSpec(const WindowSpec& spec) {
    std::vector<std::shared_ptr<Widget>> widgets;
    
    int widgetId = 1;
    for (const auto& widgetType : spec.widgets) {
        auto widget = CreateWidget(widgetType);
        if (widget) {
            widget->SetId(widgetId++);
            widgets.push_back(widget);
        }
    }
    
    // Auto-layout widgets
    LayoutWidgets(widgets, spec.width, spec.height);
    
    return widgets;
}

void PromptWindowBuilder::RegisterWidgetFactory(const std::wstring& widgetType, WidgetFactory factory) {
    m_widgetFactories[widgetType] = factory;
}

HWND PromptWindowBuilder::BuildFromPrompt(const std::wstring& prompt, HINSTANCE hInstance, HWND parent) {
    auto spec = ParsePrompt(prompt);
    
    HWND hwnd = CreateWindowFromSpec(spec, hInstance, parent);
    if (!hwnd) {
        return nullptr;
    }
    
    // Create widgets
    auto widgets = CreateWidgetsFromSpec(spec);
    
    // Create widget manager
    m_lastWidgetManager = std::make_shared<WidgetManager>();
    for (auto& widget : widgets) {
        m_lastWidgetManager->AddWidget(widget);
    }
    
    // Create subwindows
    for (const auto& subSpec : spec.subwindows) {
        CreateWindowFromSpec(subSpec, hInstance, hwnd);
    }
    
    return hwnd;
}

std::shared_ptr<Widget> PromptWindowBuilder::CreateWidget(const std::wstring& widgetType) {
    auto it = m_widgetFactories.find(widgetType);
    if (it != m_widgetFactories.end()) {
        return it->second(widgetType);
    }
    return nullptr;
}

void PromptWindowBuilder::LayoutWidgets(std::vector<std::shared_ptr<Widget>>& widgets, int windowWidth, int windowHeight) {
    if (widgets.empty()) return;
    
    // Simple vertical layout with margins
    const int margin = 20;
    const int spacing = 10;
    const int widgetWidth = windowWidth - 2 * margin;
    const int widgetHeight = 30;
    
    int y = margin;
    
    for (auto& widget : widgets) {
        widget->SetBounds(margin, y, widgetWidth, widgetHeight);
        y += widgetHeight + spacing;
    }
}

HWND PromptWindowBuilder::CreateWidgetsWindow(const WindowConfig& config, HINSTANCE hInstance) {
    // Create the window
    HWND hwnd = CreateWindowExW(
        config.exStyle,
        config.className.c_str(),
        config.title.c_str(),
        config.style,
        config.x, config.y, config.width, config.height,
        config.parent, nullptr, hInstance, nullptr
    );
    
    if (!hwnd) {
        return nullptr;
    }
    
    // Register window with SDK
    auto window = WindowManager::GetInstance().RegisterWindow(hwnd);
    if (window) {
        // Apply theme (use provided or default to ModernTheme)
        auto theme = config.theme ? config.theme : std::make_shared<Theme>(Theme::CreateModernTheme());
        window->SetTheme(theme);
        
        // Set depth
        window->SetDepth(config.depth);
        
        // Set rounded corners if enabled
        if (config.roundedCorners) {
            window->SetRoundedCorners(true, config.cornerRadius);
        }
        
        // Set render callback if provided
        if (config.renderCallback) {
            window->SetRenderCallback(config.renderCallback);
        }
        
        // Update appearance
        window->UpdateAppearance();
    }
    
    return hwnd;
}

} // namespace SDK
