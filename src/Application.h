#ifndef APPLICATION_H
#define APPLICATION_H

#include "ConfigManager.h"
#include "KeyboardHook.h"
#include "TouchInjector.h"
#include "DisplayOverlay.h"
#include <memory>

enum class AppMode {
    RECORDING,
    MAPPING,
    IDLE
};

class Application {
public:
    Application();
    ~Application();
    
    // Initialize the application
    bool Initialize();
    
    // Run the application
    void Run();
    
    // Shutdown the application
    void Shutdown();
    
private:
    std::unique_ptr<ConfigManager> m_config;
    std::unique_ptr<KeyboardHook> m_keyboardHook;
    std::unique_ptr<TouchInjector> m_touchInjector;
    std::unique_ptr<DisplayOverlay> m_overlay;
    
    AppMode m_mode;
    bool m_running;
    bool m_displayEnabled;
    
    // Callback for keyboard events
    void OnKeyEvent(int virtualKey, bool isDown);
    
    // Handle mode switching
    void SetMode(AppMode mode);
    
    // Print current status
    void PrintStatus();
    
    // Print help
    void PrintHelp();
};

#endif // APPLICATION_H
