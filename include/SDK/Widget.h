#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <string>
#include <memory>
#include "Theme.h"

namespace SDK {

/**
 * Widget - Base class for UI components
 * Provides common functionality for all widgets like position, size, visibility, etc.
 */
class Widget {
public:
    Widget();
    virtual ~Widget() = default;
    
    // Position and size
    void SetPosition(int x, int y);
    void GetPosition(int& x, int& y) const;
    
    void SetSize(int width, int height);
    void GetSize(int& width, int& height) const;
    
    void SetBounds(int x, int y, int width, int height);
    void GetBounds(RECT& rect) const;
    
    // Visibility and enabled state
    void SetVisible(bool visible);
    bool IsVisible() const { return m_visible; }
    
    void SetEnabled(bool enabled);
    bool IsEnabled() const { return m_enabled; }
    
    // Hover state
    void SetHovered(bool hovered);
    bool IsHovered() const { return m_hovered; }
    
    // Hit testing
    virtual bool HitTest(int x, int y) const;
    
    // Rendering
    virtual void Render(HDC hdc) = 0;
    
    // Update (for animations)
    virtual void Update(float deltaTime);
    
    // Mouse events
    virtual void OnMouseEnter();
    virtual void OnMouseLeave();
    virtual void OnMouseMove(int x, int y);
    virtual void OnMouseDown(int x, int y);
    virtual void OnMouseUp(int x, int y);
    virtual void OnClick();
    
    // ID for identification
    void SetId(int id) { m_id = id; }
    int GetId() const { return m_id; }
    
    // Tag for custom data
    void SetTag(void* tag) { m_tag = tag; }
    void* GetTag() const { return m_tag; }
    
protected:
    int m_x;
    int m_y;
    int m_width;
    int m_height;
    bool m_visible;
    bool m_enabled;
    bool m_hovered;
    int m_id;
    void* m_tag;
};

} // namespace SDK
