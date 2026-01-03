#include "Application.h"
#include <iostream>
#include <windows.h>

int main() {
    // Set console to UTF-8
    SetConsoleOutputCP(CP_UTF8);
    
    Application app;
    
    if (!app.Initialize()) {
        std::cerr << "Failed to initialize application." << std::endl;
        std::cerr << "Press Enter to exit..." << std::endl;
        std::cin.get();
        return 1;
    }
    
    std::cout << "Application started successfully!" << std::endl;
    std::cout << "Use Ctrl+Shift+H to show help anytime." << std::endl;
    std::cout << std::endl;
    
    app.Run();
    
    return 0;
}
