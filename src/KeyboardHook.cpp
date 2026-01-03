#include "KeyboardHook.h"
#include <iostream>

KeyboardHook* KeyboardHook::s_instance = nullptr;

KeyboardHook::KeyboardHook()
    : m_hook(nullptr) {
    s_instance = this;
}

KeyboardHook::~KeyboardHook() {
    Uninstall();
    s_instance = nullptr;
}

bool KeyboardHook::Install() {
    if (m_hook != nullptr) {
        std::cout << "Keyboard hook already installed." << std::endl;
        return true;
    }
    
    m_hook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, GetModuleHandle(nullptr), 0);
    
    if (m_hook == nullptr) {
        std::cerr << "Failed to install keyboard hook. Error: " << GetLastError() << std::endl;
        return false;
    }
    
    std::cout << "Keyboard hook installed successfully." << std::endl;
    return true;
}

void KeyboardHook::Uninstall() {
    if (m_hook != nullptr) {
        UnhookWindowsHookEx(m_hook);
        m_hook = nullptr;
        std::cout << "Keyboard hook uninstalled." << std::endl;
    }
}

void KeyboardHook::SetKeyCallback(KeyCallback callback) {
    m_callback = callback;
}

bool KeyboardHook::IsInstalled() const {
    return m_hook != nullptr;
}

LRESULT CALLBACK KeyboardHook::KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0 && s_instance != nullptr && s_instance->m_callback) {
        KBDLLHOOKSTRUCT* pKbd = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);
        
        bool isDown = (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN);
        
        // Call the callback
        s_instance->m_callback(pKbd->vkCode, isDown);
    }
    
    // Pass to next hook
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}
