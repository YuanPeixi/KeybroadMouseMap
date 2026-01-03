cmake_minimum_required(VERSION 3.15)
project(KeyboardMouseMap VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set Windows target version to Windows 8 to get touch API definitions
if(WIN32)
    add_definitions(-DWINVER=0x0602 -D_WIN32_WINNT=0x0602)
endif()

# Add source files
set(SOURCES
    src/main.cpp
    src/ConfigManager.cpp
    src/KeyboardHook.cpp
    src/TouchInjector.cpp
    src/DisplayOverlay.cpp
    src/Application.cpp
)

set(HEADERS
    src/ConfigManager.h
    src/KeyboardHook.h
    src/TouchInjector.h
    src/DisplayOverlay.h
    src/Application.h
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Link Windows libraries
if(WIN32)
    target_link_libraries(${PROJECT_NAME} 
        user32
        gdi32
        dwmapi
    )
endif()

# Set output directory
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Enable console for debugging (can be disabled for release)
if(MSVC)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        LINK_FLAGS "/SUBSYSTEM:CONSOLE"
    )
endif()
