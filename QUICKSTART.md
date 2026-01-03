# Quick Reference Card

## KeyboardMouseMap - Quick Start Guide

### Running the Application
```bash
# Run as Administrator (required)
KeyboardMouseMap.exe
```

### Keyboard Shortcuts Reference

| Shortcut | Function | Description |
|----------|----------|-------------|
| `Ctrl+Shift+R` | Recording Mode | Record key positions - press keys to save mouse location |
| `Ctrl+Shift+M` | Mapping Mode | Use mapped keys - press to trigger touch at saved position |
| `Ctrl+Shift+I` | Idle Mode | Safe mode - no action on key press |
| `Ctrl+Shift+D` | Toggle Display | Show/hide visual overlay with key indicators |
| `Ctrl+Shift+T` | Toggle Hold Behavior | Switch between hold touch (default) and repeated taps |
| `Ctrl+Shift+C` | Clear All | Remove all saved key mappings |
| `Ctrl+Shift+H` | Help | Show help message in console |
| `Ctrl+Shift+Q` | Quit | Exit the application |

### Basic Workflow

#### 1. Record Keys (First Time Setup)
```
1. Press Ctrl+Shift+R     → Enter Recording Mode
2. Move mouse to position
3. Press a key (e.g., A)  → Position saved
4. Repeat for other keys
5. Press Ctrl+Shift+I     → Exit Recording Mode
```

#### 2. Use Mapped Keys
```
1. Press Ctrl+Shift+M     → Enter Mapping Mode
2. Press mapped keys      → Touch triggered at saved positions
3. Press multiple keys    → Multi-point touch activated
4. Press Ctrl+Shift+I     → Exit Mapping Mode
```

#### 3. View Visual Overlay
```
1. Press Ctrl+Shift+D     → Show grey circles at mapped positions
2. Press Ctrl+Shift+D     → Hide overlay
```

### Tips

- **Always run as Administrator** - Required for keyboard hooks and touch injection
- **Start in Recording Mode** - Set up your keys first
- **Use Idle Mode** - Switch to Idle when not actively using the tool
- **Hold Behavior** - Default is to maintain touch when holding a key; use `Ctrl+Shift+T` to enable repeated taps
- **Overlay is non-blocking** - You can click through it
- **Multi-point touch** - Press multiple mapped keys simultaneously
- **Saved automatically** - Mappings persist in `keymap_config.txt`

### Troubleshooting

| Problem | Solution |
|---------|----------|
| Touch not working | Ensure Windows 8+ or accept mouse simulation fallback |
| Keys not recording | Check if in Recording Mode (`Ctrl+Shift+R`) |
| Overlay not visible | Toggle with `Ctrl+Shift+D`, check if DWM is running |
| App won't start | Run as Administrator |

### Configuration File

Location: `keymap_config.txt` (same directory as executable)

Format:
```
# Configuration Options
hold_triggers_continuous_tap=0  (0=hold maintains touch, 1=hold triggers repeated taps)

# VirtualKeyCode X Y KeyName
65 100 200 A
66 300 400 B
```

Manually edit if needed, changes apply on next restart.

---

For detailed documentation, see BUILD.md and IMPLEMENTATION.md
