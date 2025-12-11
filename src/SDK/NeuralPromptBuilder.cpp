#include "../../include/SDK/NeuralPromptBuilder.h"
#include "../../include/SDK/Widget.h"
#include "../../include/SDK/WindowManager.h"
#include <windows.h>

namespace SDK {

NeuralPromptBuilder::NeuralPromptBuilder() 
    : PromptWindowBuilder() {
    m_neuralNetwork = std::make_shared<NeuralNetwork>();
    m_neuralNetwork->Initialize();
    
    // Register additional widget factories for neural network support
    RegisterWidgetFactory(L"button", [](const std::wstring&) {
        auto button = std::make_shared<Button>(L"Button");
        return button;
    });
    
    RegisterWidgetFactory(L"label", [](const std::wstring&) {
        auto label = std::make_shared<Label>(L"Label");
        return label;
    });
    
    RegisterWidgetFactory(L"textbox", [](const std::wstring&) {
        auto textbox = std::make_shared<TextBox>();
        textbox->SetPlaceholder(L"Enter text...");
        return textbox;
    });
    
    RegisterWidgetFactory(L"checkbox", [](const std::wstring&) {
        auto checkbox = std::make_shared<CheckBox>(L"Checkbox");
        return checkbox;
    });
}

PromptWindowBuilder::WindowSpec NeuralPromptBuilder::ParsePrompt(const std::wstring& prompt) {
    // Use neural network to parse the prompt
    auto parsed = m_neuralNetwork->ParsePrompt(prompt);
    
    // Convert to WindowSpec
    return ConvertToWindowSpec(parsed);
}

PromptWindowBuilder::WindowSpec NeuralPromptBuilder::ConvertToWindowSpec(const NeuralNetwork::ParsedPrompt& parsed) {
    WindowSpec spec;
    
    // Set defaults
    spec.width = parsed.GetWidth();
    spec.height = parsed.GetHeight();
    spec.title = parsed.GetTitle();
    spec.x = CW_USEDEFAULT;
    spec.y = CW_USEDEFAULT;
    spec.isDialog = false;
    
    // Determine what widgets to add based on intent
    switch (parsed.intent) {
        case NeuralNetwork::Intent::CREATE_WINDOW:
            // Window creation - check for additional widget specifications
            break;
            
        case NeuralNetwork::Intent::ADD_BUTTON:
            spec.widgets.push_back(L"button");
            break;
            
        case NeuralNetwork::Intent::ADD_LABEL:
            spec.widgets.push_back(L"label");
            break;
            
        case NeuralNetwork::Intent::ADD_TEXTBOX:
            spec.widgets.push_back(L"textbox");
            break;
            
        case NeuralNetwork::Intent::ADD_CHECKBOX:
            spec.widgets.push_back(L"checkbox");
            break;
            
        case NeuralNetwork::Intent::ADD_PROGRESSBAR:
            spec.widgets.push_back(L"progressbar");
            break;
            
        case NeuralNetwork::Intent::ADD_TOOLTIP:
            spec.widgets.push_back(L"tooltip");
            break;
            
        default:
            break;
    }
    
    return spec;
}

std::vector<std::shared_ptr<Widget>> NeuralPromptBuilder::GenerateWidgetsFromParsed(const NeuralNetwork::ParsedPrompt& parsed) {
    std::vector<std::shared_ptr<Widget>> widgets;
    
    // Create widget based on intent
    std::shared_ptr<Widget> widget;
    
    switch (parsed.intent) {
        case NeuralNetwork::Intent::ADD_BUTTON: {
            auto button = std::make_shared<Button>(parsed.GetWidgetText());
            widget = button;
            break;
        }
        
        case NeuralNetwork::Intent::ADD_LABEL: {
            auto label = std::make_shared<Label>(parsed.GetWidgetText());
            widget = label;
            break;
        }
        
        case NeuralNetwork::Intent::ADD_TEXTBOX: {
            auto textbox = std::make_shared<TextBox>();
            textbox->SetPlaceholder(parsed.GetWidgetText());
            widget = textbox;
            break;
        }
        
        case NeuralNetwork::Intent::ADD_CHECKBOX: {
            auto checkbox = std::make_shared<CheckBox>(parsed.GetWidgetText());
            widget = checkbox;
            break;
        }
        
        case NeuralNetwork::Intent::ADD_PROGRESSBAR: {
            auto progressbar = std::make_shared<ProgressBar>();
            widget = progressbar;
            break;
        }
        
        case NeuralNetwork::Intent::ADD_TOOLTIP: {
            auto tooltip = std::make_shared<Tooltip>();
            tooltip->SetText(parsed.GetWidgetText());
            widget = tooltip;
            break;
        }
        
        default:
            break;
    }
    
    if (widget) {
        widgets.push_back(widget);
    }
    
    return widgets;
}

HWND NeuralPromptBuilder::BuildFromPrompt(const std::wstring& prompt, HINSTANCE hInstance, HWND parent) {
    // Parse the prompt with neural network
    auto parsed = m_neuralNetwork->ParsePrompt(prompt);
    
    // Convert to WindowSpec
    auto spec = ConvertToWindowSpec(parsed);
    
    // Create the window
    HWND hwnd = CreateWindowFromSpec(spec, hInstance, parent);
    if (!hwnd) {
        return nullptr;
    }
    
    // Generate widgets from parsed intent
    auto widgets = GenerateWidgetsFromParsed(parsed);
    
    // Also create widgets from spec (for multi-widget prompts)
    auto specWidgets = CreateWidgetsFromSpec(spec);
    widgets.insert(widgets.end(), specWidgets.begin(), specWidgets.end());
    
    // Create widget manager
    m_lastWidgetManager = std::make_shared<WidgetManager>();
    
    int widgetId = 1;
    for (auto& widget : widgets) {
        widget->SetId(widgetId++);
        
        // Apply callback if specified
        if (parsed.GetCallbackType() == NeuralNetwork::CallbackType::ON_CLICK) {
            auto callback = GenerateCallback(prompt);
            widget->SetEventCallback(callback);
        }
        
        m_lastWidgetManager->AddWidget(widget);
    }
    
    // Layout widgets
    LayoutWidgets(widgets, spec.width, spec.height);
    
    return hwnd;
}

NeuralPromptBuilder::CallbackSpec NeuralPromptBuilder::ParseCallbackPrompt(const std::wstring& prompt) {
    CallbackSpec spec;
    
    // Parse with neural network
    auto parsed = m_neuralNetwork->ParsePrompt(prompt);
    
    spec.widgetText = parsed.GetWidgetText();
    spec.type = parsed.GetCallbackType();
    
    // Extract action description
    // Look for phrases like "that does X" or "to do X"
    auto lowerPrompt = prompt;
    std::transform(lowerPrompt.begin(), lowerPrompt.end(), lowerPrompt.begin(), ::towlower);
    
    size_t thatPos = lowerPrompt.find(L"that ");
    size_t toPos = lowerPrompt.find(L" to ");
    
    if (thatPos != std::wstring::npos) {
        spec.action = prompt.substr(thatPos + 5);
    } else if (toPos != std::wstring::npos) {
        spec.action = prompt.substr(toPos + 4);
    } else {
        spec.action = L"Click action";
    }
    
    return spec;
}

std::function<void(Widget*, WidgetEvent, void*)> NeuralPromptBuilder::GenerateCallback(const std::wstring& prompt) {
    auto lowerPrompt = prompt;
    std::transform(lowerPrompt.begin(), lowerPrompt.end(), lowerPrompt.begin(), ::towlower);
    
    // Check for common callback patterns
    if (lowerPrompt.find(L"show message") != std::wstring::npos ||
        lowerPrompt.find(L"display message") != std::wstring::npos) {
        return CreateShowMessageCallback(L"Button clicked!");
    }
    
    if (lowerPrompt.find(L"tooltip") != std::wstring::npos ||
        lowerPrompt.find(L"show tip") != std::wstring::npos) {
        return CreateShowTooltipCallback(L"This is a tooltip");
    }
    
    if (lowerPrompt.find(L"close") != std::wstring::npos ||
        lowerPrompt.find(L"exit") != std::wstring::npos) {
        return CreateCloseWindowCallback();
    }
    
    // Default callback - show a simple message
    return [](Widget* widget, WidgetEvent event, void* data) {
        if (event == WidgetEvent::CLICK) {
            MessageBoxW(nullptr, L"Button clicked!", L"Callback", MB_OK | MB_ICONINFORMATION);
        }
    };
}

std::function<void(Widget*, WidgetEvent, void*)> NeuralPromptBuilder::CreateShowMessageCallback(const std::wstring& message) {
    return [message](Widget* widget, WidgetEvent event, void* data) {
        if (event == WidgetEvent::CLICK) {
            MessageBoxW(nullptr, message.c_str(), L"Message", MB_OK | MB_ICONINFORMATION);
        }
    };
}

std::function<void(Widget*, WidgetEvent, void*)> NeuralPromptBuilder::CreateShowTooltipCallback(const std::wstring& tooltipText) {
    return [tooltipText](Widget* widget, WidgetEvent event, void* data) {
        if (event == WidgetEvent::MOUSE_ENTER) {
            // Show tooltip (would need tooltip widget reference)
            MessageBoxW(nullptr, tooltipText.c_str(), L"Tooltip", MB_OK | MB_ICONINFORMATION);
        }
    };
}

std::function<void(Widget*, WidgetEvent, void*)> NeuralPromptBuilder::CreateCloseWindowCallback() {
    return [](Widget* widget, WidgetEvent event, void* data) {
        if (event == WidgetEvent::CLICK) {
            // Close the parent window
            HWND hwnd = GetActiveWindow();
            if (hwnd) {
                PostMessage(hwnd, WM_CLOSE, 0, 0);
            }
        }
    };
}

void NeuralPromptBuilder::TrainOnData(const std::vector<std::pair<std::wstring, NeuralNetwork::ParsedPrompt>>& trainingData) {
    m_neuralNetwork->Train(trainingData);
}

} // namespace SDK
