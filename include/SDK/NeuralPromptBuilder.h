#pragma once

#include "PromptWindowBuilder.h"
#include "NeuralNetwork.h"
#include <memory>

namespace SDK {

/**
 * NeuralPromptBuilder - Neural network-powered window and widget generation
 * 
 * Extends PromptWindowBuilder to use a trained neural network for
 * natural language understanding of GUI creation prompts.
 * 
 * Example prompts:
 * - "Create a window 800x600 called 'My App' with a button"
 * - "Make a button with text 'Submit' that shows a tooltip on hover"
 * - "Add a progress bar and set a click callback"
 * - "Create a login window with textbox for username and password"
 */
class NeuralPromptBuilder : public PromptWindowBuilder {
public:
    NeuralPromptBuilder();
    ~NeuralPromptBuilder() = default;
    
    // Parse a prompt using neural network
    WindowSpec ParsePrompt(const std::wstring& prompt) override;
    
    // Create a complete window with widgets and callbacks from a natural language prompt
    HWND BuildFromPrompt(const std::wstring& prompt, HINSTANCE hInstance, HWND parent = nullptr) override;
    
    // Parse and extract button callback information
    struct CallbackSpec {
        std::wstring widgetText;
        NeuralNetwork::CallbackType type;
        std::wstring action;  // Description of what the callback should do
    };
    
    CallbackSpec ParseCallbackPrompt(const std::wstring& prompt);
    
    // Generate a callback function from a prompt
    // Returns a lambda that can be used with SetEventCallback
    std::function<void(Widget*, WidgetEvent, void*)> GenerateCallback(const std::wstring& prompt);
    
    // Get the neural network instance (for training or inspection)
    std::shared_ptr<NeuralNetwork> GetNeuralNetwork() const { return m_neuralNetwork; }
    
    // Train the network on custom data
    void TrainOnData(const std::vector<std::pair<std::wstring, NeuralNetwork::ParsedPrompt>>& trainingData);
    
private:
    std::shared_ptr<NeuralNetwork> m_neuralNetwork;
    
    // Convert neural network result to WindowSpec
    WindowSpec ConvertToWindowSpec(const NeuralNetwork::ParsedPrompt& parsed);
    
    // Generate widgets based on neural network interpretation
    std::vector<std::shared_ptr<Widget>> GenerateWidgetsFromParsed(const NeuralNetwork::ParsedPrompt& parsed);
    
    // Apply layout to widgets
    void ApplyLayout(std::vector<std::shared_ptr<Widget>>& widgets, 
                     int windowWidth, int windowHeight,
                     NeuralNetwork::LayoutType layout);
    
    // Helper to create common callback actions
    std::function<void(Widget*, WidgetEvent, void*)> CreateShowMessageCallback(const std::wstring& message);
    std::function<void(Widget*, WidgetEvent, void*)> CreateShowTooltipCallback(const std::wstring& tooltipText);
    std::function<void(Widget*, WidgetEvent, void*)> CreateCloseWindowCallback();
};

} // namespace SDK
