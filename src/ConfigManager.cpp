#include "ConfigManager.h"
#include <sstream>
#include <iostream>

ConfigManager::ConfigManager(const std::string& configFile)
    : m_configFile(configFile) {
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
        
        std::istringstream iss(line);
        int virtualKey, x, y;
        std::string keyName;
        
        if (iss >> virtualKey >> x >> y) {
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
