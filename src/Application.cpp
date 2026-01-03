#include "Application.h"
#include <iostream>
#include <iomanip>

// Application constants
#define MAX_SIMULTANEOUS_TOUCHES 10

Application::Application()
    : m_mode(AppMode::IDLE)
    , m_running(false)
    , m_displayEnabled(false) {
}

Application::~Application() {
    Shutdown();
}

bool Application::Initialize() {
    std::cout << "========================================" << std::endl;
    std::cout << "  Keyboard to Touch Mapping Application" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << std::endl;
    
    // Create components
    m_config = std::make_unique<ConfigManager>();
    m_keyboardHook = std::make_unique<KeyboardHook>();
    m_touchInjector = std::make_unique<TouchInjector>();
    m_overlay = std::make_unique<DisplayOverlay>();
    
    // Initialize touch injector
    if (!m_touchInjector->Initialize()) {
        std::cerr << "Failed to initialize touch injector." << std::endl;
        return false;
    }
    
    // Create overlay window
    if (!m_overlay->Create()) {
        std::cerr << "Failed to create overlay window." << std::endl;
        return false;
    }
    
    // Install keyboard hook
    if (!m_keyboardHook->Install()) {
        std::cerr << "Failed to install keyboard hook." << std::endl;
        return false;
    }
    
    // Set keyboard callback
    m_keyboardHook->SetKeyCallback([this](int vk, bool isDown) {
        this->OnKeyEvent(vk, isDown);
    });
    
    m_running = true;
    PrintHelp();
    PrintStatus();
    
    return true;
}

void Application::Run() {
    MSG msg;
    while (m_running && GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

void Application::Shutdown() {
    m_running = false;
    
    if (m_keyboardHook) {
        m_keyboardHook->Uninstall();
    }
    
    if (m_overlay) {
        m_overlay->Destroy();
    }
    
    std::cout << "Application shutdown complete." << std::endl;
}

void Application::OnKeyEvent(int virtualKey, bool isDown) {
    // Handle control keys (check Ctrl+Shift combinations) - only on key down
    if (isDown) {
        bool ctrlPressed = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
        bool shiftPressed = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
        
        // Ctrl+Shift+R: Recording mode
        if (ctrlPressed && shiftPressed && virtualKey == 'R') {
            SetMode(AppMode::RECORDING);
            return;
        }
        
        // Ctrl+Shift+M: Mapping mode
        if (ctrlPressed && shiftPressed && virtualKey == 'M') {
            SetMode(AppMode::MAPPING);
            return;
        }
        
        // Ctrl+Shift+I: Idle mode
        if (ctrlPressed && shiftPressed && virtualKey == 'I') {
            SetMode(AppMode::IDLE);
            return;
        }
        
        // Ctrl+Shift+D: Toggle display
        if (ctrlPressed && shiftPressed && virtualKey == 'D') {
            m_displayEnabled = !m_displayEnabled;
            m_overlay->SetVisible(m_displayEnabled);
            std::cout << "Display overlay: " << (m_displayEnabled ? "ON" : "OFF") << std::endl;
            return;
        }
        
        // Ctrl+Shift+C: Clear all mappings
        if (ctrlPressed && shiftPressed && virtualKey == 'C') {
            m_config->ClearMappings();
            m_overlay->UpdateMappings(m_config->GetAllMappings());
            std::cout << "All mappings cleared." << std::endl;
            return;
        }
        
        // Ctrl+Shift+Q: Quit
        if (ctrlPressed && shiftPressed && virtualKey == 'Q') {
            std::cout << "Quitting application..." << std::endl;
            m_running = false;
            PostQuitMessage(0);
            return;
        }
        
        // Ctrl+Shift+H: Help
        if (ctrlPressed && shiftPressed && virtualKey == 'H') {
            PrintHelp();
            return;
        }
        
        // Ctrl+Shift+T: Toggle hold behavior
        if (ctrlPressed && shiftPressed && virtualKey == 'T') {
            bool newValue = !m_config->GetHoldTriggersContinuousTap();
            m_config->SetHoldTriggersContinuousTap(newValue);
            std::cout << "Hold behavior: " << (newValue ? "Continuous Tap (repeated clicks)" : "Maintain Touch (hold)") << std::endl;
            return;
        }
        
        // Ctrl+Shift+Delete: Remove mapping for next key
        if (ctrlPressed && shiftPressed && virtualKey == VK_DELETE) {
            std::cout << "Press a key to remove its mapping..." << std::endl;
            return;
        }
    }
    
    // Handle mode-specific key events
    switch (m_mode) {
        case AppMode::RECORDING: {
            // Only process key down events in recording mode
            if (!isDown) {
                return;
            }
            
            // Ignore modifier keys
            if (virtualKey == VK_CONTROL || virtualKey == VK_SHIFT || 
                virtualKey == VK_MENU || virtualKey == VK_LWIN || virtualKey == VK_RWIN) {
                return;
            }
            
            // Get current mouse position
            POINT cursorPos;
            GetCursorPos(&cursorPos);
            
            // Save mapping
            char keyName[256];
            UINT scanCode = MapVirtualKeyA(virtualKey, MAPVK_VK_TO_VSC);
            if (GetKeyNameTextA(scanCode << 16, keyName, sizeof(keyName)) > 0) {
                m_config->SaveMapping(virtualKey, cursorPos.x, cursorPos.y, keyName);
                std::cout << "Mapped key [" << keyName << "] to position (" 
                         << cursorPos.x << ", " << cursorPos.y << ")" << std::endl;
                
                // Update overlay
                m_overlay->UpdateMappings(m_config->GetAllMappings());
            }
            break;
        }
        
        case AppMode::MAPPING: {
            // Check if this key has a mapping
            KeyMapping mapping;
            if (m_config->GetMapping(virtualKey, mapping)) {
                // Use modulo to cycle through available touch IDs
                int touchId = virtualKey % MAX_SIMULTANEOUS_TOUCHES;
                
                if (m_config->GetHoldTriggersContinuousTap()) {
                    // Old behavior: trigger tap on every key down event (high frequency clicks)
                    if (isDown) {
                        if (m_touchInjector->TouchTap(mapping.x, mapping.y, touchId)) {
                            std::cout << "Touch tap for [" << mapping.keyName << "] at (" 
                                     << mapping.x << ", " << mapping.y << ")" << std::endl;
                        }
                    }
                } else {
                    // New default behavior: hold maintains touch
                    if (isDown) {
                        // Check if this is a new key press (not a repeat)
                        if (m_keyStates.find(virtualKey) == m_keyStates.end() || !m_keyStates[virtualKey]) {
                            // First press - touch down
                            m_keyStates[virtualKey] = true;
                            if (m_touchInjector->TouchDown(mapping.x, mapping.y, touchId)) {
                                std::cout << "Touch down for [" << mapping.keyName << "] at (" 
                                         << mapping.x << ", " << mapping.y << ")" << std::endl;
                            }
                        }
                        // Ignore repeat key down events while holding
                    } else {
                        // Key up - touch up
                        if (m_keyStates[virtualKey]) {
                            m_keyStates[virtualKey] = false;
                            if (m_touchInjector->TouchUp(touchId)) {
                                std::cout << "Touch up for [" << mapping.keyName << "]" << std::endl;
                            }
                        }
                    }
                }
            }
            break;
        }
        
        case AppMode::IDLE:
            // Do nothing in idle mode
            break;
    }
}

void Application::SetMode(AppMode mode) {
    m_mode = mode;
    PrintStatus();
}

void Application::PrintStatus() {
    std::cout << "\n--- Current Status ---" << std::endl;
    std::cout << "Mode: ";
    switch (m_mode) {
        case AppMode::RECORDING:
            std::cout << "RECORDING - Press keys to record their positions" << std::endl;
            break;
        case AppMode::MAPPING:
            std::cout << "MAPPING - Press keys to trigger touch events" << std::endl;
            break;
        case AppMode::IDLE:
            std::cout << "IDLE - No action on key press" << std::endl;
            break;
    }
    
    std::cout << "Display: " << (m_displayEnabled ? "ON" : "OFF") << std::endl;
    std::cout << "Hold behavior: " << (m_config->GetHoldTriggersContinuousTap() ? "Continuous Tap" : "Maintain Touch") << std::endl;
    std::cout << "Mappings: " << m_config->GetAllMappings().size() << " keys configured" << std::endl;
    
    if (m_touchInjector && m_touchInjector->IsSupported()) {
        std::cout << "Touch: Multi-point touch injection supported" << std::endl;
    } else {
        std::cout << "Touch: Using mouse simulation fallback" << std::endl;
    }
    
    std::cout << "----------------------\n" << std::endl;
}

void Application::PrintHelp() {
    std::cout << "\n=== Keyboard Shortcuts ===" << std::endl;
    std::cout << "Ctrl+Shift+R : Enter RECORDING mode" << std::endl;
    std::cout << "Ctrl+Shift+M : Enter MAPPING mode" << std::endl;
    std::cout << "Ctrl+Shift+I : Enter IDLE mode" << std::endl;
    std::cout << "Ctrl+Shift+D : Toggle display overlay" << std::endl;
    std::cout << "Ctrl+Shift+T : Toggle hold behavior (hold touch vs repeated taps)" << std::endl;
    std::cout << "Ctrl+Shift+C : Clear all mappings" << std::endl;
    std::cout << "Ctrl+Shift+H : Show this help" << std::endl;
    std::cout << "Ctrl+Shift+Q : Quit application" << std::endl;
    std::cout << "==========================\n" << std::endl;
}
