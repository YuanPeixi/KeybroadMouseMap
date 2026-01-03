#include "TouchInjector.h"
#include <iostream>

// Touch injection constants
#define TOUCH_MASK_CONTACTAREA  0x00000001
#define TOUCH_MASK_ORIENTATION  0x00000002
#define TOUCH_MASK_PRESSURE     0x00000004
#define TOUCH_HOLD_DURATION_MS  50
#define TOUCH_CONTACT_RADIUS    2
#define TOUCH_DEFAULT_PRESSURE  32000
#define TOUCH_DEFAULT_ORIENTATION 90

// POINTER_TOUCH_INFO structure definition
typedef struct tagPOINTER_INFO {
    UINT32 pointerType;
    UINT32 pointerId;
    UINT32 frameId;
    UINT32 pointerFlags;
    HANDLE sourceDevice;
    HWND hwndTarget;
    POINT ptPixelLocation;
    POINT ptHimetricLocation;
    POINT ptPixelLocationRaw;
    POINT ptHimetricLocationRaw;
    DWORD dwTime;
    UINT32 historyCount;
    INT32 InputData;
    DWORD dwKeyStates;
    UINT64 PerformanceCount;
    UINT32 ButtonChangeType;
} POINTER_INFO;

typedef struct tagPOINTER_TOUCH_INFO {
    POINTER_INFO pointerInfo;
    UINT32 touchFlags;
    UINT32 touchMask;
    RECT rcContact;
    RECT rcContactRaw;
    UINT32 orientation;
    UINT32 pressure;
} POINTER_TOUCH_INFO;

#define PT_TOUCH 0x00000002

TouchInjector::TouchInjector()
    : m_initialized(false)
    , m_supported(false)
    , m_user32Module(nullptr)
    , m_initializeTouchInjection(nullptr)
    , m_injectTouchInput(nullptr) {
}

TouchInjector::~TouchInjector() {
    ReleaseAllTouches();
    if (m_user32Module) {
        FreeLibrary(m_user32Module);
    }
}

bool TouchInjector::Initialize() {
    if (m_initialized) {
        return true;
    }
    
    // Try to load touch injection functions (Windows 8+)
    m_user32Module = LoadLibraryA("user32.dll");
    if (m_user32Module) {
        m_initializeTouchInjection = reinterpret_cast<InitializeTouchInjectionFunc>(
            GetProcAddress(m_user32Module, "InitializeTouchInjection"));
        m_injectTouchInput = reinterpret_cast<InjectTouchInputFunc>(
            GetProcAddress(m_user32Module, "InjectTouchInput"));
        
        if (m_initializeTouchInjection && m_injectTouchInput) {
            // Initialize for up to 10 simultaneous touch points
            if (m_initializeTouchInjection(10, TOUCH_FEEDBACK_NONE)) {
                m_supported = true;
                m_initialized = true;
                std::cout << "Touch injection initialized successfully." << std::endl;
                return true;
            } else {
                std::cerr << "Failed to initialize touch injection. Error: " << GetLastError() << std::endl;
            }
        }
    }
    
    // Fallback mode: use mouse simulation
    std::cout << "Touch injection not supported, will use mouse simulation as fallback." << std::endl;
    m_supported = false;
    m_initialized = true;
    return true;
}

bool TouchInjector::TouchDown(int x, int y, int touchId) {
    if (!m_initialized) {
        return false;
    }
    
    if (!m_supported) {
        // Fallback: just return true, actual tap will be done in TouchTap
        return true;
    }
    
    POINTER_TOUCH_INFO contact;
    memset(&contact, 0, sizeof(POINTER_TOUCH_INFO));
    
    contact.pointerInfo.pointerType = PT_TOUCH;
    contact.pointerInfo.pointerId = touchId;
    contact.pointerInfo.ptPixelLocation.x = x;
    contact.pointerInfo.ptPixelLocation.y = y;
    contact.pointerInfo.pointerFlags = POINTER_FLAG_DOWN | POINTER_FLAG_INRANGE | POINTER_FLAG_INCONTACT;
    
    if (touchId == 0) {
        contact.pointerInfo.pointerFlags |= POINTER_FLAG_PRIMARY;
    }
    
    // Set contact area (small circle)
    contact.rcContact.left = x - TOUCH_CONTACT_RADIUS;
    contact.rcContact.right = x + TOUCH_CONTACT_RADIUS;
    contact.rcContact.top = y - TOUCH_CONTACT_RADIUS;
    contact.rcContact.bottom = y + TOUCH_CONTACT_RADIUS;
    
    contact.touchFlags = 0;
    contact.touchMask = TOUCH_MASK_CONTACTAREA | TOUCH_MASK_ORIENTATION | TOUCH_MASK_PRESSURE;
    contact.orientation = TOUCH_DEFAULT_ORIENTATION;
    contact.pressure = TOUCH_DEFAULT_PRESSURE;
    
    if (m_injectTouchInput(1, &contact)) {
        TouchPoint tp;
        tp.x = x;
        tp.y = y;
        tp.id = touchId;
        tp.isActive = true;
        m_activeTouches.push_back(tp);
        return true;
    }
    
    return false;
}

bool TouchInjector::TouchUp(int touchId) {
    if (!m_initialized || !m_supported) {
        return false;
    }
    
    // Find the touch point
    auto it = m_activeTouches.begin();
    while (it != m_activeTouches.end()) {
        if (it->id == touchId) {
            POINTER_TOUCH_INFO contact;
            memset(&contact, 0, sizeof(POINTER_TOUCH_INFO));
            
            contact.pointerInfo.pointerType = PT_TOUCH;
            contact.pointerInfo.pointerId = touchId;
            contact.pointerInfo.ptPixelLocation.x = it->x;
            contact.pointerInfo.ptPixelLocation.y = it->y;
            contact.pointerInfo.pointerFlags = POINTER_FLAG_UP;
            
            bool result = m_injectTouchInput(1, &contact);
            m_activeTouches.erase(it);
            return result;
        }
        ++it;
    }
    
    return false;
}

bool TouchInjector::TouchTap(int x, int y, int touchId) {
    if (!m_initialized) {
        return false;
    }
    
    if (!m_supported) {
        return MouseSimulateTap(x, y);
    }
    
    // Touch down
    if (!TouchDown(x, y, touchId)) {
        return false;
    }
    
    // Brief hold
    Sleep(TOUCH_HOLD_DURATION_MS);
    
    // Touch up
    return TouchUp(touchId);
}

void TouchInjector::ReleaseAllTouches() {
    if (!m_supported) {
        return;
    }
    
    while (!m_activeTouches.empty()) {
        TouchUp(m_activeTouches[0].id);
    }
}

bool TouchInjector::IsSupported() const {
    return m_supported;
}

bool TouchInjector::MouseSimulateTap(int x, int y) {
    // Get current cursor position
    POINT originalPos;
    GetCursorPos(&originalPos);
    
    // Move to target position
    SetCursorPos(x, y);
    Sleep(10);
    
    // Simulate mouse click
    INPUT input[2];
    memset(input, 0, sizeof(input));
    
    // Mouse down
    input[0].type = INPUT_MOUSE;
    input[0].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
    
    // Mouse up
    input[1].type = INPUT_MOUSE;
    input[1].mi.dwFlags = MOUSEEVENTF_LEFTUP;
    
    SendInput(2, input, sizeof(INPUT));
    
    // Restore cursor position
    Sleep(50);
    SetCursorPos(originalPos.x, originalPos.y);
    
    return true;
}
