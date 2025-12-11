#pragma once


#include "Platform.h"
#include <vector>
#include <memory>
#include <string>
#include <functional>
#include "Window.h"

namespace SDK {

/**
 * WindowGroup - Manages linked windows with synchronized properties
 * Supports linked movement, depth changes, and transformations
 */
class WindowGroup {
public:
    WindowGroup(const std::wstring& name = L"");
    ~WindowGroup();
    
    // Group management
    void AddWindow(std::shared_ptr<Window> window);
    void RemoveWindow(std::shared_ptr<Window> window);
    void ClearWindows();
    
    const std::vector<std::shared_ptr<Window>>& GetWindows() const { return m_windows; }
    size_t GetWindowCount() const { return m_windows.size(); }
    
    // Group properties
    void SetName(const std::wstring& name) { m_name = name; }
    const std::wstring& GetName() const { return m_name; }
    
    // Linked movement
    void SetLinkedMovement(bool enabled) { m_linkedMovement = enabled; }
    bool IsLinkedMovementEnabled() const { return m_linkedMovement; }
    
    void MoveGroup(int deltaX, int deltaY);
    
    // Synchronized depth changes
    void SetSynchronizedDepth(bool enabled) { m_synchronizedDepth = enabled; }
    bool IsSynchronizedDepthEnabled() const { return m_synchronizedDepth; }
    
    void SetGroupDepth(WindowDepth depth);
    void OffsetGroupDepth(int offset);
    
    // Group transformations
    void SetGroupAlpha(BYTE alpha);
    void SetGroupScale(float scale);
    void SetGroupOffset(int x, int y);
    
    // Apply transformation to all windows
    void ApplyTransformation(
        std::function<void(std::shared_ptr<Window>)> transformation);
    
private:
    std::wstring m_name;
    std::vector<std::shared_ptr<Window>> m_windows;
    
    bool m_linkedMovement;
    bool m_synchronizedDepth;
};

} // namespace SDK
