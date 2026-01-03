#ifndef KEYBOARD_HOOK_H
#define KEYBOARD_HOOK_H

#include <windows.h>
#include <functional>

class KeyboardHook {
public:
    using KeyCallback = std::function<void(int virtualKey, bool isDown)>;
    
    KeyboardHook();
    ~KeyboardHook();
    
    // Install the keyboard hook
    bool Install();
    
    // Uninstall the keyboard hook
    void Uninstall();
    
    // Set callback for key events
    void SetKeyCallback(KeyCallback callback);
    
    // Check if hook is installed
    bool IsInstalled() const;

private:
    HHOOK m_hook;
    KeyCallback m_callback;
    
    static KeyboardHook* s_instance;
    static LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
};

#endif // KEYBOARD_HOOK_H
