#include "../../include/SDK/NeuralPromptBuilder.h"
#include "../../include/SDK/Widget.h"
#include "../../include/SDK/AdvancedWidgets.h"
#include "../../include/SDK/WindowManager.h"
#include <windows.h>
#include <cmath>
#include <algorithm>

namespace SDK {

NeuralPromptBuilder::NeuralPromptBuilder() 
    : PromptWindowBuilder() {
    m_neuralNetwork = std::make_shared<NeuralNetwork>();
    m_neuralNetwork->Initialize();
    
    // Register widget factories for all supported widget types
    RegisterWidgetFactory(L"button", [](const std::wstring&) {
        return std::make_shared<Button>(L"Button");
    });
    
    RegisterWidgetFactory(L"label", [](const std::wstring&) {
        return std::make_shared<Label>(L"Label");
    });
    
    RegisterWidgetFactory(L"textbox", [](const std::wstring&) {
        auto textbox = std::make_shared<TextBox>();
        textbox->SetPlaceholder(L"Enter text...");
        return textbox;
    });
    
    RegisterWidgetFactory(L"checkbox", [](const std::wstring&) {
        return std::make_shared<CheckBox>(L"Checkbox");
    });
    
    RegisterWidgetFactory(L"progressbar", [](const std::wstring&) {
        return std::make_shared<ProgressBar>();
    });
    
    RegisterWidgetFactory(L"slider", [](const std::wstring&) {
        auto slider = std::make_shared<Slider>();
        slider->SetRange(0, 100);
        return slider;
    });
    
    RegisterWidgetFactory(L"combobox", [](const std::wstring&) {
        return std::make_shared<ComboBox>();
    });
    
    RegisterWidgetFactory(L"listbox", [](const std::wstring&) {
        return std::make_shared<ListBox>();
    });
    
    RegisterWidgetFactory(L"listview", [](const std::wstring&) {
        return std::make_shared<ListView>();
    });
    
    RegisterWidgetFactory(L"radiobutton", [](const std::wstring&) {
        return std::make_shared<RadioButton>(L"Radio", 0);
    });
    
    RegisterWidgetFactory(L"spinbox", [](const std::wstring&) {
        return std::make_shared<SpinBox>();
    });
    
    RegisterWidgetFactory(L"separator", [](const std::wstring&) {
        return std::make_shared<Separator>();
    });
    
    RegisterWidgetFactory(L"panel", [](const std::wstring&) {
        return std::make_shared<Panel>();
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
    
    // Map primary intent to widget type
    auto addWidgetForIntent = [&](NeuralNetwork::Intent intent) {
        switch (intent) {
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
            case NeuralNetwork::Intent::ADD_SLIDER:
                spec.widgets.push_back(L"slider");
                break;
            case NeuralNetwork::Intent::ADD_COMBOBOX:
                spec.widgets.push_back(L"combobox");
                break;
            case NeuralNetwork::Intent::ADD_LISTBOX:
                spec.widgets.push_back(L"listbox");
                break;
            case NeuralNetwork::Intent::ADD_LISTVIEW:
                spec.widgets.push_back(L"listview");
                break;
            case NeuralNetwork::Intent::ADD_RADIOBUTTON:
                spec.widgets.push_back(L"radiobutton");
                break;
            case NeuralNetwork::Intent::ADD_SPINBOX:
                spec.widgets.push_back(L"spinbox");
                break;
            case NeuralNetwork::Intent::ADD_SEPARATOR:
                spec.widgets.push_back(L"separator");
                break;
            case NeuralNetwork::Intent::ADD_PANEL:
                spec.widgets.push_back(L"panel");
                break;
            default:
                break;
        }
    };
    
    // Add primary widget
    addWidgetForIntent(parsed.intent);
    
    // Add additional widgets found in prompt
    for (const auto& widgetIntent : parsed.additionalWidgets) {
        addWidgetForIntent(widgetIntent);
    }
    
    return spec;
}

std::vector<std::shared_ptr<Widget>> NeuralPromptBuilder::GenerateWidgetsFromParsed(const NeuralNetwork::ParsedPrompt& parsed) {
    std::vector<std::shared_ptr<Widget>> widgets;
    
    // Helper lambda to create widget from intent
    auto createWidget = [&](NeuralNetwork::Intent intent) -> std::shared_ptr<Widget> {
        std::shared_ptr<Widget> widget;
        
        switch (intent) {
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
            
            case NeuralNetwork::Intent::ADD_SLIDER: {
                auto slider = std::make_shared<Slider>();
                slider->SetRange(0, 100);
                slider->SetValue(50);
                widget = slider;
                break;
            }
            
            case NeuralNetwork::Intent::ADD_COMBOBOX: {
                auto combobox = std::make_shared<ComboBox>();
                // Add items if specified in entities
                auto items = parsed.GetItems();
                for (const auto& item : items) {
                    combobox->AddItem(item);
                }
                widget = combobox;
                break;
            }
            
            case NeuralNetwork::Intent::ADD_LISTBOX: {
                auto listbox = std::make_shared<ListBox>();
                // Add items if specified in entities
                auto items = parsed.GetItems();
                for (const auto& item : items) {
                    listbox->AddItem(item);
                }
                widget = listbox;
                break;
            }
            
            case NeuralNetwork::Intent::ADD_LISTVIEW: {
                auto listview = std::make_shared<ListView>();
                // Add items if specified in entities
                auto items = parsed.GetItems();
                for (const auto& item : items) {
                    listview->AddItem(item);
                }
                widget = listview;
                break;
            }
            
            case NeuralNetwork::Intent::ADD_RADIOBUTTON: {
                auto radio = std::make_shared<RadioButton>(parsed.GetWidgetText(), 0);
                widget = radio;
                break;
            }
            
            case NeuralNetwork::Intent::ADD_SPINBOX: {
                auto spinbox = std::make_shared<SpinBox>();
                widget = spinbox;
                break;
            }
            
            case NeuralNetwork::Intent::ADD_SEPARATOR: {
                auto separator = std::make_shared<Separator>();
                widget = separator;
                break;
            }
            
            case NeuralNetwork::Intent::ADD_PANEL: {
                auto panel = std::make_shared<Panel>();
                auto title = parsed.GetWidgetText();
                if (!title.empty()) {
                    panel->SetTitle(title);
                }
                widget = panel;
                break;
            }
            
            default:
                break;
        }
        
        return widget;
    };
    
    // Create primary widget
    auto primaryWidget = createWidget(parsed.intent);
    if (primaryWidget) {
        widgets.push_back(primaryWidget);
    }
    
    // Create additional widgets
    for (const auto& additionalIntent : parsed.additionalWidgets) {
        auto additionalWidget = createWidget(additionalIntent);
        if (additionalWidget) {
            widgets.push_back(additionalWidget);
        }
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
    
    // Apply layout based on parsed layout type
    if (parsed.layoutType != NeuralNetwork::LayoutType::NONE) {
        ApplyLayout(widgets, spec.width, spec.height, parsed.layoutType);
    } else {
        // Default layout
        LayoutWidgets(widgets, spec.width, spec.height);
    }
    
    return hwnd;
}

void NeuralPromptBuilder::ApplyLayout(std::vector<std::shared_ptr<Widget>>& widgets, 
                                       int windowWidth, int windowHeight,
                                       NeuralNetwork::LayoutType layout) {
    if (widgets.empty()) return;
    
    const int padding = 20;
    const int spacing = 10;
    
    switch (layout) {
        case NeuralNetwork::LayoutType::VERTICAL: {
            // Stack widgets vertically
            int yPos = padding;
            int widgetWidth = windowWidth - (2 * padding);
            int widgetHeight = 40;
            
            for (auto& widget : widgets) {
                widget->SetBounds(padding, yPos, widgetWidth, widgetHeight);
                yPos += widgetHeight + spacing;
            }
            break;
        }
        
        case NeuralNetwork::LayoutType::HORIZONTAL: {
            // Arrange widgets horizontally
            int widgetCount = static_cast<int>(widgets.size());
            int totalSpacing = spacing * (widgetCount - 1);
            int availableWidth = windowWidth - (2 * padding) - totalSpacing;
            int widgetWidth = availableWidth / widgetCount;
            int widgetHeight = 40;
            int xPos = padding;
            int yPos = padding;
            
            for (auto& widget : widgets) {
                widget->SetBounds(xPos, yPos, widgetWidth, widgetHeight);
                xPos += widgetWidth + spacing;
            }
            break;
        }
        
        case NeuralNetwork::LayoutType::GRID: {
            // Arrange widgets in a grid
            int cols = static_cast<int>(std::ceil(std::sqrt(widgets.size())));
            int rows = static_cast<int>(std::ceil(static_cast<float>(widgets.size()) / cols));
            
            int cellWidth = (windowWidth - (2 * padding) - (spacing * (cols - 1))) / cols;
            int cellHeight = (windowHeight - (2 * padding) - (spacing * (rows - 1))) / rows;
            
            for (size_t i = 0; i < widgets.size(); i++) {
                int row = static_cast<int>(i / cols);
                int col = static_cast<int>(i % cols);
                
                int x = padding + (col * (cellWidth + spacing));
                int y = padding + (row * (cellHeight + spacing));
                
                widgets[i]->SetBounds(x, y, cellWidth - 10, cellHeight - 10);
            }
            break;
        }
        
        default:
            // Use default layout
            LayoutWidgets(widgets, windowWidth, windowHeight);
            break;
    }
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
