#ifndef TOUCH_INJECTOR_H
#define TOUCH_INJECTOR_H

#include <windows.h>
#include <vector>

// Touch input structures (compatible with Windows 7+)
#ifndef POINTER_FLAG_DOWN
#define POINTER_FLAG_DOWN           0x00010000
#define POINTER_FLAG_UPDATE         0x00020000
#define POINTER_FLAG_UP             0x00040000
#define POINTER_FLAG_INRANGE        0x00000002
#define POINTER_FLAG_INCONTACT      0x00000004
#define POINTER_FLAG_PRIMARY        0x00002000
#endif

#ifndef TOUCH_FEEDBACK_DEFAULT
#define TOUCH_FEEDBACK_DEFAULT      0x1
#define TOUCH_FEEDBACK_INDIRECT     0x2
#define TOUCH_FEEDBACK_NONE         0x3
#endif

struct TouchPoint {
    int x;
    int y;
    int id;
    bool isActive;
};

class TouchInjector {
public:
    TouchInjector();
    ~TouchInjector();
    
    // Initialize touch injection
    bool Initialize();
    
    // Inject a touch down event
    bool TouchDown(int x, int y, int touchId = 0);
    
    // Inject a touch up event
    bool TouchUp(int touchId = 0);
    
    // Inject a touch update event (maintains active touch)
    bool TouchUpdate(int touchId = 0);
    
    // Inject a complete touch (down, brief hold, up)
    bool TouchTap(int x, int y, int touchId = 0);
    
    // Release all active touches
    void ReleaseAllTouches();
    
    // Check if touch injection is supported
    bool IsSupported() const;

private:
    bool m_initialized;
    bool m_supported;
    std::vector<TouchPoint> m_activeTouches;
    
    // Function pointers for Windows Touch API
    typedef BOOL (WINAPI *InitializeTouchInjectionFunc)(UINT32, DWORD);
    typedef BOOL (WINAPI *InjectTouchInputFunc)(UINT32, const void*);
    
    HMODULE m_user32Module;
    InitializeTouchInjectionFunc m_initializeTouchInjection;
    InjectTouchInputFunc m_injectTouchInput;
    
    // Fallback to mouse simulation if touch not supported
    bool MouseSimulateTap(int x, int y);
};

#endif // TOUCH_INJECTOR_H
