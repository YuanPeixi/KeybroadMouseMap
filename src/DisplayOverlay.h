#ifndef DISPLAY_OVERLAY_H
#define DISPLAY_OVERLAY_H

#include <windows.h>
#include <map>
#include <string>
#include "ConfigManager.h"

class DisplayOverlay {
public:
    DisplayOverlay();
    ~DisplayOverlay();
    
    // Create and show the overlay window
    bool Create();
    
    // Destroy the overlay window
    void Destroy();
    
    // Show or hide the overlay
    void SetVisible(bool visible);
    
    // Check if overlay is visible
    bool IsVisible() const;
    
    // Update the overlay with current mappings
    void UpdateMappings(const std::map<int, KeyMapping>& mappings);
    
    // Force redraw
    void Redraw();

private:
    HWND m_hwnd;
    bool m_visible;
    std::map<int, KeyMapping> m_mappings;
    
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    void OnPaint();
    
    static DisplayOverlay* s_instance;
};

#endif // DISPLAY_OVERLAY_H
