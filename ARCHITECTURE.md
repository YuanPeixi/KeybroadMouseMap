# Architecture Overview

## System Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                     KeyboardMouseMap                        │
│                      Application                            │
└─────────────────────────────────────────────────────────────┘
                              │
                              │ Coordinates
                              ▼
        ┌──────────────────────────────────────────┐
        │          Application Core                │
        │  - Mode Management (Recording/Mapping)   │
        │  - Keyboard Shortcut Handling            │
        │  - Component Coordination                │
        └──────────────────────────────────────────┘
                 │         │         │         │
        ┌────────┘         │         │         └────────┐
        ▼                  ▼         ▼                  ▼
┌──────────────┐  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐
│ KeyboardHook │  │ConfigManager │  │TouchInjector │  │DisplayOverlay│
│              │  │              │  │              │  │              │
│ - Low-level  │  │ - Load/Save  │  │ - Touch API  │  │ - Layered    │
│   KB hook    │  │   mappings   │  │ - Mouse      │  │   window     │
│ - Key events │  │ - Text file  │  │   fallback   │  │ - GDI render │
│ - Callbacks  │  │ - Validation │  │ - Multi-pt   │  │ - Click-thru │
└──────────────┘  └──────────────┘  └──────────────┘  └──────────────┘
        │                  │                  │                  │
        ▼                  ▼                  ▼                  ▼
┌──────────────┐  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐
│   Windows    │  │ Config File  │  │   Windows    │  │   Windows    │
│  Keyboard    │  │keymap_config │  │Touch Inject  │  │     DWM      │
│     API      │  │    .txt      │  │     API      │  │     API      │
└──────────────┘  └──────────────┘  └──────────────┘  └──────────────┘
```

## Data Flow

### Recording Mode
```
User presses key
       ↓
KeyboardHook detects event
       ↓
Application.OnKeyEvent() called
       ↓
Get current mouse position (Windows API)
       ↓
ConfigManager.SaveMapping()
       ↓
Write to keymap_config.txt
       ↓
DisplayOverlay.UpdateMappings()
       ↓
Visual indicator appears (if display enabled)
```

### Mapping Mode
```
User presses mapped key
       ↓
KeyboardHook detects event
       ↓
Application.OnKeyEvent() called
       ↓
ConfigManager.GetMapping()
       ↓
TouchInjector.TouchTap(x, y, touchId)
       ↓
┌─────────────┴─────────────┐
▼                           ▼
Windows Touch API    OR    Mouse Simulation
(Windows 8+)              (Windows 7)
       ↓                           ↓
Touch at position         Click at position
```

## Component Details

### KeyboardHook
- **Purpose**: Global keyboard input capture
- **Technology**: Windows Low-Level Keyboard Hook (WH_KEYBOARD_LL)
- **Thread**: Runs in message loop
- **Security**: Requires Administrator privileges

### ConfigManager
- **Purpose**: Persistent storage of key-position mappings
- **Format**: Plain text, human-readable
- **Validation**: 
  - Virtual key codes: 0-255
  - Coordinates: Clamped to screen bounds
  - Safe parsing with C++ streams

### TouchInjector
- **Purpose**: Multi-point touch event injection
- **Primary**: Windows Touch Injection API (InitializeTouchInjection, InjectTouchInput)
- **Fallback**: Mouse simulation (SetCursorPos, SendInput)
- **Capacity**: Up to 10 simultaneous touch points
- **Detection**: Runtime feature detection

### DisplayOverlay
- **Purpose**: Visual feedback for mapped positions
- **Technology**: 
  - Layered Window (WS_EX_LAYERED)
  - Click-through (WS_EX_TRANSPARENT)
  - Topmost (WS_EX_TOPMOST)
  - DWM transparency
- **Rendering**: GDI (circles, text)
- **Update**: Real-time when mappings change

## Mode State Machine

```
┌──────┐  Ctrl+Shift+R  ┌───────────┐  Ctrl+Shift+M  ┌─────────┐
│ IDLE │ ──────────────▶│ RECORDING │ ──────────────▶│ MAPPING │
└──────┘                └───────────┘                └─────────┘
   ▲                          │                           │
   │        Ctrl+Shift+I      │                           │
   └──────────────────────────┴───────────────────────────┘
```

**IDLE**: No action on key press, safe mode  
**RECORDING**: Key press → save mouse position  
**MAPPING**: Key press → trigger touch event  

## Threading Model

```
Main Thread
├─ Message Loop (GetMessage/DispatchMessage)
│  └─ Processes window messages
│
├─ Keyboard Hook Callback
│  └─ Called by Windows for each key event
│  └─ Invokes Application.OnKeyEvent()
│
└─ Display Overlay
   └─ WM_PAINT messages trigger rendering
```

**Note**: Single-threaded design, all operations on main thread

## File Structure

```
KeybroadMouseMap/
├── CMakeLists.txt           # Build configuration
├── README.md                # Project overview
├── BUILD.md                 # Build instructions
├── QUICKSTART.md            # Quick reference
├── IMPLEMENTATION.md        # Implementation details
├── ARCHITECTURE.md          # This file
├── build.bat               # Windows build script
├── build.sh                # Unix build script
├── keymap_config_example.txt  # Example config
├── .gitignore              # Git ignore rules
└── src/
    ├── main.cpp            # Entry point (25 lines)
    ├── Application.h       # App header (53 lines)
    ├── Application.cpp     # App impl (236 lines)
    ├── ConfigManager.h     # Config header (48 lines)
    ├── ConfigManager.cpp   # Config impl (157 lines)
    ├── KeyboardHook.h      # Hook header (34 lines)
    ├── KeyboardHook.cpp    # Hook impl (61 lines)
    ├── TouchInjector.h     # Touch header (70 lines)
    ├── TouchInjector.cpp   # Touch impl (245 lines)
    ├── DisplayOverlay.h    # Display header (43 lines)
    └── DisplayOverlay.cpp  # Display impl (207 lines)

Total: ~1,226 lines of code
```

## Dependencies

### Runtime
- **Windows 7 or later** (Windows 8+ recommended)
- **user32.dll** - Keyboard hooks, input injection, windowing
- **dwmapi.dll** - Desktop Window Manager for transparency
- **gdi32.dll** - Graphics rendering

### Build-time
- **CMake 3.15+**
- **C++ Compiler** (MSVC, MinGW, or compatible)
- **Windows SDK**

### Libraries
All dependencies are Windows system libraries, no external packages required.

## Security Model

### Privilege Requirements
- **Administrator rights** required for:
  - Installing low-level keyboard hooks
  - Touch injection API
  - System-wide input simulation

### Input Validation
```
Virtual Key Codes: 0 ≤ vk ≤ 255
Screen X:         0 ≤ x ≤ SM_CXSCREEN
Screen Y:         0 ≤ y ≤ SM_CYSCREEN
Touch IDs:        0 ≤ id < 10
```

### Safe Practices
- No dynamic code execution
- No shell commands
- No network operations
- C++ STL for memory safety
- Input sanitization for config files

## Performance Characteristics

- **Startup time**: < 100ms
- **Key event latency**: < 10ms
- **Touch injection latency**: 50-100ms (includes deliberate hold time)
- **Memory footprint**: ~2-5 MB
- **CPU usage**: Near zero when idle, < 1% during operation

## Extension Points

Future enhancements could add:

1. **Configuration GUI** - Visual key mapping editor
2. **Profile System** - Multiple mapping configurations
3. **Custom Hotkeys** - User-definable shortcuts
4. **Recording Feedback** - Visual indicator during recording
5. **Touch Gestures** - Swipe, pinch, etc.
6. **Network Sync** - Share configs across machines

---

**Last Updated**: 2026-01-03  
**Version**: 1.0.0
