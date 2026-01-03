#include "ConfigManager.h"
#include <sstream>
#include <iostream>

ConfigManager::ConfigManager(const std::string& configFile)
    : m_configFile(configFile)
    , m_holdTriggersContinuousTap(false) {  // Default: hold maintains touch
    LoadMappings();
}

ConfigManager::~ConfigManager() {
    SaveMappings();
}

std::string ConfigManager::GetKeyName(int virtualKey) {
    // Get the scan code
    UINT scanCode = MapVirtualKeyA(virtualKey, MAPVK_VK_TO_VSC);
    
    // Get key name
    char keyName[256];
    if (GetKeyNameTextA(scanCode << 16, keyName, sizeof(keyName)) > 0) {
        return std::string(keyName);
    }
    
    // Fallback to VK code
    return "VK_" + std::to_string(virtualKey);
}

bool ConfigManager::SaveMapping(int virtualKey, int x, int y, const std::string& keyName) {
    // Validate virtual key code
    if (virtualKey < 0 || virtualKey > 255) {
        std::cerr << "Invalid virtual key code: " << virtualKey << std::endl;
        return false;
    }
    
    // Get screen dimensions for validation
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    
    // Clamp coordinates to valid screen bounds
    if (x < 0) x = 0;
    if (x > screenWidth) x = screenWidth;
    if (y < 0) y = 0;
    if (y > screenHeight) y = screenHeight;
    
    KeyMapping mapping;
    mapping.x = x;
    mapping.y = y;
    mapping.keyName = keyName.empty() ? GetKeyName(virtualKey) : keyName;
    
    m_mappings[virtualKey] = mapping;
    return SaveMappings();
}

bool ConfigManager::GetMapping(int virtualKey, KeyMapping& mapping) {
    auto it = m_mappings.find(virtualKey);
    if (it != m_mappings.end()) {
        mapping = it->second;
        return true;
    }
    return false;
}

bool ConfigManager::RemoveMapping(int virtualKey) {
    auto it = m_mappings.find(virtualKey);
    if (it != m_mappings.end()) {
        m_mappings.erase(it);
        return SaveMappings();
    }
    return false;
}

bool ConfigManager::LoadMappings() {
    m_mappings.clear();
    
    std::ifstream file(m_configFile);
    if (!file.is_open()) {
        std::cout << "Config file not found, starting with empty mappings." << std::endl;
        return true; // Not an error for first run
    }
    
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;
        
        // Check for configuration options
        if (line.find("hold_triggers_continuous_tap=") == 0) {
            std::string value = line.substr(29); // Length of "hold_triggers_continuous_tap="
            m_holdTriggersContinuousTap = (value == "1" || value == "true");
            continue;
        }
        
        std::istringstream iss(line);
        int virtualKey, x, y;
        std::string keyName;
        
        if (iss >> virtualKey >> x >> y) {
            // Validate input ranges
            if (virtualKey < 0 || virtualKey > 255) {
                std::cerr << "Invalid virtual key code: " << virtualKey << std::endl;
                continue;
            }
            
            // Get screen dimensions for validation
            int screenWidth = GetSystemMetrics(SM_CXSCREEN);
            int screenHeight = GetSystemMetrics(SM_CYSCREEN);
            
            // Clamp coordinates to valid screen bounds
            if (x < 0) x = 0;
            if (x > screenWidth) x = screenWidth;
            if (y < 0) y = 0;
            if (y > screenHeight) y = screenHeight;
            
            // Read the rest as key name
            std::getline(iss, keyName);
            // Trim leading whitespace
            size_t pos = keyName.find_first_not_of(" \t");
            if (pos != std::string::npos) {
                keyName = keyName.substr(pos);
            }
            
            KeyMapping mapping;
            mapping.x = x;
            mapping.y = y;
            mapping.keyName = keyName.empty() ? GetKeyName(virtualKey) : keyName;
            
            m_mappings[virtualKey] = mapping;
        }
    }
    
    file.close();
    std::cout << "Loaded " << m_mappings.size() << " key mappings." << std::endl;
    return true;
}

bool ConfigManager::SaveMappings() {
    std::ofstream file(m_configFile);
    if (!file.is_open()) {
        std::cerr << "Failed to open config file for writing: " << m_configFile << std::endl;
        return false;
    }
    
    file << "# Keyboard to Touch Mapping Configuration" << std::endl;
    file << "# Format: VirtualKeyCode X Y KeyName" << std::endl;
    file << "#" << std::endl;
    file << "# Configuration Options:" << std::endl;
    file << "# hold_triggers_continuous_tap=0  (0=hold maintains touch, 1=hold triggers repeated taps)" << std::endl;
    file << std::endl;
    
    // Write configuration option
    file << "hold_triggers_continuous_tap=" << (m_holdTriggersContinuousTap ? "1" : "0") << std::endl;
    file << std::endl;
    
    for (const auto& pair : m_mappings) {
        file << pair.first << " " 
             << pair.second.x << " " 
             << pair.second.y << " " 
             << pair.second.keyName << std::endl;
    }
    
    file.close();
    return true;
}

const std::map<int, KeyMapping>& ConfigManager::GetAllMappings() const {
    return m_mappings;
}

void ConfigManager::ClearMappings() {
    m_mappings.clear();
    SaveMappings();
}

bool ConfigManager::GetHoldTriggersContinuousTap() const {
    return m_holdTriggersContinuousTap;
}

void ConfigManager::SetHoldTriggersContinuousTap(bool enabled) {
    m_holdTriggersContinuousTap = enabled;
    SaveMappings();
}
