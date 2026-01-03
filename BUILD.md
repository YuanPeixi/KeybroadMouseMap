# Keyboard to Touch Mapping Application

A C++ application for Windows that maps keyboard inputs to multi-point touch events. This allows you to record screen positions and trigger touch events at those positions by pressing specific keys.

## Features

- **Recording Mode**: Press a key while hovering over a location to record the position
- **Mapping Mode**: Press mapped keys to simulate touch events at recorded positions
- **Multi-point Touch**: Supports true multi-point touch injection on Windows 8+ (falls back to mouse simulation on older systems)
- **Visual Overlay**: Display visual indicators showing where each key is mapped (can be toggled on/off)
- **Non-blocking Display**: Overlay is click-through and doesn't interfere with normal touch/mouse input
- **Persistent Configuration**: Mappings are saved to a configuration file and loaded on startup

## Requirements

- Windows 7 or later (Windows 8+ recommended for true multi-point touch support)
- CMake 3.15 or later
- Visual Studio 2017 or later (or MinGW-w64)

## Building

### Using Visual Studio

```bash
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

### Using MinGW

```bash
mkdir build
cd build
cmake -G "MinGW Makefiles" ..
cmake --build .
```

The executable will be located in `build/bin/KeyboardMouseMap.exe`

## Usage

1. Run `KeyboardMouseMap.exe`
2. Use keyboard shortcuts to control the application:

### Keyboard Shortcuts

| Shortcut | Action |
|----------|--------|
| `Ctrl+Shift+R` | Enter **RECORDING** mode |
| `Ctrl+Shift+M` | Enter **MAPPING** mode |
| `Ctrl+Shift+I` | Enter **IDLE** mode |
| `Ctrl+Shift+D` | Toggle display overlay ON/OFF |
| `Ctrl+Shift+C` | Clear all key mappings |
| `Ctrl+Shift+H` | Show help message |
| `Ctrl+Shift+Q` | Quit application |

### Workflow

#### Recording Key Positions

1. Press `Ctrl+Shift+R` to enter **Recording Mode**
2. Move your mouse to the desired screen position
3. Press any key (e.g., `A`, `B`, `1`, `2`, etc.) - this will record that key to the current mouse position
4. Repeat for other keys/positions
5. Press `Ctrl+Shift+I` to exit Recording Mode

#### Using Mapped Keys

1. Press `Ctrl+Shift+M` to enter **Mapping Mode**
2. Press any previously recorded key - this will simulate a touch event at the recorded position
3. You can press multiple keys simultaneously for multi-point touch
4. Press `Ctrl+Shift+I` to exit Mapping Mode

#### Visualizing Mappings

1. Press `Ctrl+Shift+D` to toggle the display overlay
2. Grey circles with key labels will appear at each mapped position
3. The overlay is transparent and click-through - it won't block your interactions
4. Press `Ctrl+Shift+D` again to hide the overlay

## Configuration File

Mappings are automatically saved to `keymap_config.txt` in the same directory as the executable. The file format is:

```
# Keyboard to Touch Mapping Configuration
# Format: VirtualKeyCode X Y KeyName

65 100 200 A
66 300 400 B
49 500 600 1
```

You can manually edit this file if needed. Changes will be loaded when the application restarts.

## Technical Details

### Multi-point Touch Support

- On **Windows 8 and later**: Uses `InitializeTouchInjection` and `InjectTouchInput` APIs for true multi-point touch
- On **Windows 7**: Falls back to mouse simulation (moves cursor and clicks)

### Display Overlay

- Implemented as a transparent, topmost, layered window
- Uses DWM (Desktop Window Manager) for click-through transparency
- Doesn't capture mouse events or keyboard focus

### Keyboard Hook

- Uses low-level keyboard hook (`WH_KEYBOARD_LL`) to capture all key events
- Hook remains active in all three modes
- Mode-switching keys are always active regardless of current mode

## Troubleshooting

### Touch events not working

- Ensure you're running on Windows 8 or later for multi-point touch
- Check if your display supports touch input
- On older systems, the application will fall back to mouse simulation

### Overlay not showing

- Try toggling with `Ctrl+Shift+D`
- Ensure Desktop Window Manager (DWM) is running
- Check if your graphics drivers support layered windows

### Application not starting

- Run as Administrator (required for keyboard hooks and touch injection)
- Ensure all required DLLs (user32.dll, dwmapi.dll) are available

## License

See LICENSE file for details.

## Contributing

Contributions are welcome! Please feel free to submit pull requests or open issues.
