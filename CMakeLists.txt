cmake_minimum_required(VERSION 3.10)
project(MaterialGameEngine_5DGUI VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Platform check
if(NOT WIN32)
    message(FATAL_ERROR "This SDK is designed for Windows only (Win32 API)")
endif()

# SDK Library
set(SDK_SOURCES
    src/SDK/SDK.cpp
    src/SDK/WindowHook.cpp
    src/SDK/Window.cpp
    src/SDK/WindowManager.cpp
    src/SDK/Theme.cpp
    src/SDK/Renderer.cpp
    src/SDK/Widget.cpp
    src/SDK/ProgressBar.cpp
    src/SDK/Tooltip.cpp
    src/SDK/WidgetManager.cpp
    src/SDK/PromptWindowBuilder.cpp
    src/SDK/AdvancedWidgets.cpp
)

set(SDK_HEADERS
    include/SDK/SDK.h
    include/SDK/WindowHook.h
    include/SDK/Window.h
    include/SDK/WindowManager.h
    include/SDK/Theme.h
    include/SDK/Renderer.h
    include/SDK/Widget.h
    include/SDK/ProgressBar.h
    include/SDK/Tooltip.h
    include/SDK/WidgetManager.h
    include/SDK/PromptWindowBuilder.h
    include/SDK/AdvancedWidgets.h
)

# Create static library
add_library(5DGUI_SDK STATIC ${SDK_SOURCES} ${SDK_HEADERS})

# Include directories
target_include_directories(5DGUI_SDK PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Link Windows libraries
target_link_libraries(5DGUI_SDK PUBLIC
    dwmapi
    gdi32
    user32
    kernel32
)

# Demo application
add_executable(5DGUI_Demo examples/demo.cpp)
target_link_libraries(5DGUI_Demo PRIVATE 5DGUI_SDK)

# Widget demo application
add_executable(5DGUI_WidgetDemo examples/widget_demo.cpp)
target_link_libraries(5DGUI_WidgetDemo PRIVATE 5DGUI_SDK)
# Widget showcase application
add_executable(Widget_Showcase examples/widget_showcase.cpp)
target_link_libraries(Widget_Showcase PRIVATE 5DGUI_SDK)

# Set subsystem to Windows (GUI application)
if(MSVC)
    set_target_properties(5DGUI_Demo PROPERTIES
        LINK_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:WinMainCRTStartup"
    )
    set_target_properties(5DGUI_WidgetDemo PROPERTIES
    set_target_properties(Widget_Showcase PROPERTIES
        LINK_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:WinMainCRTStartup"
    )
else()
    set_target_properties(5DGUI_Demo PROPERTIES
        LINK_FLAGS "-mwindows"
    )
    set_target_properties(5DGUI_WidgetDemo PROPERTIES
    set_target_properties(Widget_Showcase PROPERTIES
        LINK_FLAGS "-mwindows"
    )
endif()

# Installation
install(TARGETS 5DGUI_SDK
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY include/SDK
    DESTINATION include
    FILES_MATCHING PATTERN "*.h"
)

install(TARGETS 5DGUI_Demo 5DGUI_WidgetDemo
install(TARGETS 5DGUI_Demo Widget_Showcase
    RUNTIME DESTINATION bin
)

# Documentation
install(FILES README.md
    DESTINATION docs
)
