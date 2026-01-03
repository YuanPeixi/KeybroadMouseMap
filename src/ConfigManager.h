#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include <windows.h>
#include <string>
#include <map>
#include <fstream>

struct KeyMapping {
    int x;
    int y;
    std::string keyName;
};

class ConfigManager {
public:
    ConfigManager(const std::string& configFile = "keymap_config.txt");
    ~ConfigManager();

    // Save a key mapping
    bool SaveMapping(int virtualKey, int x, int y, const std::string& keyName);
    
    // Get mapping for a key
    bool GetMapping(int virtualKey, KeyMapping& mapping);
    
    // Remove a mapping
    bool RemoveMapping(int virtualKey);
    
    // Load all mappings from file
    bool LoadMappings();
    
    // Save all mappings to file
    bool SaveMappings();
    
    // Get all mappings (for display)
    const std::map<int, KeyMapping>& GetAllMappings() const;
    
    // Clear all mappings
    void ClearMappings();
    
    // Get/Set hold behavior configuration
    bool GetHoldTriggersContinuousTap() const;
    void SetHoldTriggersContinuousTap(bool enabled);

private:
    std::string m_configFile;
    std::map<int, KeyMapping> m_mappings;
    bool m_holdTriggersContinuousTap;
    
    std::string GetKeyName(int virtualKey);
};

#endif // CONFIG_MANAGER_H
