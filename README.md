# KeyboardMouseMap
A C++ application for Windows that maps keyboard inputs to multi-point touch events.

## Features

- **Recording Mode**: Record screen positions by pressing keys at specific locations
- **Mapping Mode**: Trigger multi-point touch events by pressing mapped keys
- **Hold Behavior**: Choose between holding touch (default) or repeated taps when key is held
- **Visual Overlay**: Display indicators showing mapped key positions (toggleable, non-blocking)
- **Multi-point Touch**: True multi-point touch on Windows 8+ (mouse simulation fallback for older systems)

## Quick Start

See [BUILD.md](BUILD.md) for detailed build and usage instructions.

### Building

```bash
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

### Running

```bash
./build/bin/KeyboardMouseMap.exe
```

Use `Ctrl+Shift+H` to see all keyboard shortcuts.
