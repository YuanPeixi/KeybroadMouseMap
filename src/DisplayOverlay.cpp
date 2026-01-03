#include "DisplayOverlay.h"
#include <iostream>
#include <dwmapi.h>

#pragma comment(lib, "dwmapi.lib")

// Display overlay constants
#define OVERLAY_ALPHA_VALUE     200
#define KEY_INDICATOR_RADIUS    30
#define KEY_FONT_SIZE           20

DisplayOverlay* DisplayOverlay::s_instance = nullptr;

DisplayOverlay::DisplayOverlay()
    : m_hwnd(nullptr)
    , m_visible(false) {
    s_instance = this;
}

DisplayOverlay::~DisplayOverlay() {
    Destroy();
    s_instance = nullptr;
}

bool DisplayOverlay::Create() {
    if (m_hwnd != nullptr) {
        return true;
    }
    
    const wchar_t CLASS_NAME[] = L"KeyboardMapOverlay";
    
    WNDCLASSW wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(nullptr);
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
    
    RegisterClassW(&wc);
    
    // Create a layered window that covers the entire screen
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    
    m_hwnd = CreateWindowExW(
        WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOPMOST | WS_EX_TOOLWINDOW,
        CLASS_NAME,
        L"Keyboard Map Overlay",
        WS_POPUP,
        0, 0, screenWidth, screenHeight,
        nullptr, nullptr, GetModuleHandle(nullptr), nullptr
    );
    
    if (m_hwnd == nullptr) {
        std::cerr << "Failed to create overlay window. Error: " << GetLastError() << std::endl;
        return false;
    }
    
    // Make the window click-through and semi-transparent
    SetLayeredWindowAttributes(m_hwnd, RGB(0, 0, 0), OVERLAY_ALPHA_VALUE, LWA_ALPHA);
    
    // Enable transparency for the window
    MARGINS margins = {-1, -1, -1, -1};
    DwmExtendFrameIntoClientArea(m_hwnd, &margins);
    
    std::cout << "Overlay window created successfully." << std::endl;
    return true;
}

void DisplayOverlay::Destroy() {
    if (m_hwnd != nullptr) {
        DestroyWindow(m_hwnd);
        m_hwnd = nullptr;
    }
}

void DisplayOverlay::SetVisible(bool visible) {
    if (m_hwnd == nullptr) {
        if (visible) {
            Create();
        } else {
            return;
        }
    }
    
    m_visible = visible;
    ShowWindow(m_hwnd, visible ? SW_SHOW : SW_HIDE);
    
    if (visible) {
        UpdateWindow(m_hwnd);
        Redraw();
    }
}

bool DisplayOverlay::IsVisible() const {
    return m_visible;
}

void DisplayOverlay::UpdateMappings(const std::map<int, KeyMapping>& mappings) {
    m_mappings = mappings;
    if (m_visible) {
        Redraw();
    }
}

void DisplayOverlay::Redraw() {
    if (m_hwnd != nullptr) {
        InvalidateRect(m_hwnd, nullptr, TRUE);
        UpdateWindow(m_hwnd);
    }
}

LRESULT CALLBACK DisplayOverlay::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (s_instance == nullptr) {
        return DefWindowProcW(hwnd, uMsg, wParam, lParam);
    }
    
    switch (uMsg) {
        case WM_PAINT:
            s_instance->OnPaint();
            return 0;
            
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
            
        case WM_ERASEBKGND:
            return 1; // Don't erase background
    }
    
    return DefWindowProcW(hwnd, uMsg, wParam, lParam);
}

void DisplayOverlay::OnPaint() {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(m_hwnd, &ps);
    
    // Create memory DC for double buffering
    HDC memDC = CreateCompatibleDC(hdc);
    RECT rect;
    GetClientRect(m_hwnd, &rect);
    HBITMAP memBitmap = CreateCompatibleBitmap(hdc, rect.right, rect.bottom);
    HBITMAP oldBitmap = (HBITMAP)SelectObject(memDC, memBitmap);
    
    // Fill with transparent black
    HBRUSH brush = CreateSolidBrush(RGB(0, 0, 0));
    FillRect(memDC, &rect, brush);
    DeleteObject(brush);
    
    // Set up drawing
    SetBkMode(memDC, TRANSPARENT);
    SetTextColor(memDC, RGB(255, 255, 255));
    
    // Create font
    HFONT hFont = CreateFontW(
        KEY_FONT_SIZE, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
        L"Arial"
    );
    HFONT oldFont = (HFONT)SelectObject(memDC, hFont);
    
    // Draw each mapping
    for (const auto& pair : m_mappings) {
        const KeyMapping& mapping = pair.second;
        
        // Draw grey circle
        int radius = KEY_INDICATOR_RADIUS;
        HBRUSH circleBrush = CreateSolidBrush(RGB(128, 128, 128));
        HPEN circlePen = CreatePen(PS_SOLID, 2, RGB(200, 200, 200));
        
        HBRUSH oldBrush = (HBRUSH)SelectObject(memDC, circleBrush);
        HPEN oldPen = (HPEN)SelectObject(memDC, circlePen);
        
        Ellipse(memDC, 
            mapping.x - radius, mapping.y - radius,
            mapping.x + radius, mapping.y + radius);
        
        SelectObject(memDC, oldBrush);
        SelectObject(memDC, oldPen);
        DeleteObject(circleBrush);
        DeleteObject(circlePen);
        
        // Draw key name
        std::wstring wKeyName(mapping.keyName.begin(), mapping.keyName.end());
        RECT textRect;
        textRect.left = mapping.x - radius;
        textRect.right = mapping.x + radius;
        textRect.top = mapping.y - 10;
        textRect.bottom = mapping.y + 10;
        
        DrawTextW(memDC, wKeyName.c_str(), -1, &textRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    }
    
    SelectObject(memDC, oldFont);
    DeleteObject(hFont);
    
    // Copy to screen
    BitBlt(hdc, 0, 0, rect.right, rect.bottom, memDC, 0, 0, SRCCOPY);
    
    // Cleanup
    SelectObject(memDC, oldBitmap);
    DeleteObject(memBitmap);
    DeleteDC(memDC);
    
    EndPaint(m_hwnd, &ps);
}
