cmake_minimum_required(VERSION 3.10)
project(MaterialGameEngine_5DGUI VERSION 2.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Platform detection and configuration
if(WIN32)
    message(STATUS "Building for Windows platform")
    set(PLATFORM_WINDOWS TRUE)
elseif(UNIX AND NOT APPLE)
    message(STATUS "Building for Linux platform")
    set(PLATFORM_LINUX TRUE)
    # Find X11 libraries for Linux (optional for now)
    find_package(X11)
    if(X11_FOUND)
        message(STATUS "X11 found: ${X11_LIBRARIES}")
    else()
        message(WARNING "X11 not found. Library will build but window features will be limited.")
        message(WARNING "Install X11 development libraries: sudo apt-get install libx11-dev")
    endif()
elseif(APPLE)
    message(WARNING "macOS support is experimental")
    set(PLATFORM_MACOS TRUE)
else()
    message(FATAL_ERROR "Unsupported platform")
endif()

# SDK Library - Platform-independent sources
set(SDK_COMMON_SOURCES
    src/SDK/NeuralNetwork.cpp
    src/SDK/Theme.cpp
    src/SDK/InstructionDecoder.cpp
    src/SDK/RenderBackend.cpp
    src/SDK/Layout.cpp
)

# Platform-specific sources
if(PLATFORM_WINDOWS)
    set(SDK_PLATFORM_SOURCES
        src/SDK/SDK.cpp
        src/SDK/WindowHook.cpp
        src/SDK/Window.cpp
        src/SDK/WindowManager.cpp
        src/SDK/WindowGroup.cpp
        src/SDK/WindowSnapping.cpp
        src/SDK/WindowAnimation.cpp
        src/SDK/Renderer.cpp
        src/SDK/GDIRenderBackend.cpp
        src/SDK/D2DRenderBackend.cpp
        src/SDK/RendererOptimizer.cpp
        src/SDK/Widget.cpp
        src/SDK/ProgressBar.cpp
        src/SDK/Tooltip.cpp
        src/SDK/WidgetManager.cpp
        src/SDK/PromptWindowBuilder.cpp
        src/SDK/NeuralPromptBuilder.cpp
        src/SDK/AdvancedWidgets.cpp
        src/SDK/CameraController.cpp
        src/SDK/Widget3D.cpp
        src/SDK/Toolbar.cpp
        src/SDK/RichText.cpp
        src/SDK/DataGrid.cpp
        src/SDK/DPIManager.cpp
        src/SDK/MonitorManager.cpp
    )
elseif(PLATFORM_LINUX)
    # Linux: X11 backend and window implementation
    set(SDK_PLATFORM_SOURCES
        src/SDK/X11RenderBackend.cpp
        src/SDK/WindowX11.cpp
    )
endif()

set(SDK_SOURCES ${SDK_COMMON_SOURCES} ${SDK_PLATFORM_SOURCES})

set(SDK_HEADERS
    include/SDK/SDK.h
    include/SDK/Platform.h
    include/SDK/WindowHook.h
    include/SDK/Window.h
    include/SDK/WindowManager.h
    include/SDK/WindowGroup.h
    include/SDK/WindowSnapping.h
    include/SDK/WindowAnimation.h
    include/SDK/Theme.h
    include/SDK/Renderer.h
    include/SDK/RenderBackend.h
    include/SDK/GDIRenderBackend.h
    include/SDK/D2DRenderBackend.h
    include/SDK/X11RenderBackend.h
    include/SDK/WindowX11.h
    include/SDK/RendererOptimizer.h
    include/SDK/InstructionDecoder.h
    include/SDK/Widget.h
    include/SDK/ProgressBar.h
    include/SDK/Tooltip.h
    include/SDK/WidgetManager.h
    include/SDK/PromptWindowBuilder.h
    include/SDK/NeuralNetwork.h
    include/SDK/NeuralPromptBuilder.h
    include/SDK/AdvancedWidgets.h
    include/SDK/CameraController.h
    include/SDK/Widget3D.h
    include/SDK/Toolbar.h
    include/SDK/RichText.h
    include/SDK/DataGrid.h
    include/SDK/DPIManager.h
    include/SDK/MonitorManager.h
)

# Create static library
add_library(5DGUI_SDK STATIC ${SDK_SOURCES} ${SDK_HEADERS})

# Include directories
target_include_directories(5DGUI_SDK PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Link platform-specific libraries
if(PLATFORM_WINDOWS)
    target_link_libraries(5DGUI_SDK PUBLIC
        dwmapi
        gdi32
        user32
        kernel32
    )
elseif(PLATFORM_LINUX)
    # Link pthread (always available)
    target_link_libraries(5DGUI_SDK PUBLIC
        pthread
    )
    # Link X11 if available
    if(X11_FOUND)
        target_link_libraries(5DGUI_SDK PUBLIC ${X11_LIBRARIES})
        target_include_directories(5DGUI_SDK PUBLIC ${X11_INCLUDE_DIR})
    endif()
endif()

# Demo applications (Windows only for now)
if(PLATFORM_WINDOWS)
    # Demo application
    add_executable(5DGUI_Demo examples/demo.cpp)
    target_link_libraries(5DGUI_Demo PRIVATE 5DGUI_SDK)
    
    # Widget demo application
    add_executable(5DGUI_WidgetDemo examples/widget_demo.cpp)
    target_link_libraries(5DGUI_WidgetDemo PRIVATE 5DGUI_SDK)
    
    # Widget showcase application
    add_executable(Widget_Showcase examples/widget_showcase.cpp)
    target_link_libraries(Widget_Showcase PRIVATE 5DGUI_SDK)
    
    # Camera demo application
    add_executable(Camera_Demo examples/camera_demo.cpp)
    target_link_libraries(Camera_Demo PRIVATE 5DGUI_SDK)
    
    # Toolbar demo application
    add_executable(Toolbar_Demo examples/toolbar_demo.cpp)
    target_link_libraries(Toolbar_Demo PRIVATE 5DGUI_SDK)
    
    # Advanced window features demo application
    add_executable(Advanced_Window_Demo examples/advanced_window_demo.cpp)
    target_link_libraries(Advanced_Window_Demo PRIVATE 5DGUI_SDK)
    
    # Neural network demo application
    add_executable(Neural_Network_Demo examples/neural_network_demo.cpp)
    target_link_libraries(Neural_Network_Demo PRIVATE 5DGUI_SDK)
    
    # Optimization demo application
    add_executable(Optimization_Demo examples/optimization_demo.cpp)
    target_link_libraries(Optimization_Demo PRIVATE 5DGUI_SDK)
    
    # New widgets demo application
    add_executable(New_Widgets_Demo examples/new_widgets_demo.cpp)
    target_link_libraries(New_Widgets_Demo PRIVATE 5DGUI_SDK)
    
    # Widget properties demo application
    add_executable(Widget_Properties_Demo examples/widget_properties_demo.cpp)
    target_link_libraries(Widget_Properties_Demo PRIVATE 5DGUI_SDK)
    
    # Hardware acceleration demo application
    add_executable(Hardware_Accel_Demo examples/hardware_accel_demo.cpp)
    target_link_libraries(Hardware_Accel_Demo PRIVATE 5DGUI_SDK)
    
    # Rich text and data grid demo application
    add_executable(RichText_DataGrid_Demo examples/richtext_datagrid_demo.cpp)
    target_link_libraries(RichText_DataGrid_Demo PRIVATE 5DGUI_SDK)
    
    # ML Optimizer demo application
    add_executable(ML_Optimizer_Demo examples/ml_optimizer_demo.cpp)
    target_link_libraries(ML_Optimizer_Demo PRIVATE 5DGUI_SDK)
    
    # Layout Engine demo application
    add_executable(Layout_Engine_Demo examples/layout_engine_demo.cpp)
    target_link_libraries(Layout_Engine_Demo PRIVATE 5DGUI_SDK)
    
    # DPI and Multi-Monitor demo application (v2.0)
    add_executable(DPI_MultiMonitor_Demo examples/dpi_multimonitor_demo.cpp)
    target_link_libraries(DPI_MultiMonitor_Demo PRIVATE 5DGUI_SDK)
    
    # V1.3 Features Showcase demo application
    add_executable(V13_Showcase_Demo examples/v13_showcase_demo.cpp)
    target_link_libraries(V13_Showcase_Demo PRIVATE 5DGUI_SDK)
    
    # Widget Enhancements demo application
    add_executable(Widget_Enhancements_Demo examples/widget_enhancements_demo.cpp)
    target_link_libraries(Widget_Enhancements_Demo PRIVATE 5DGUI_SDK)
    
    # Advanced Neural Network demo application
    add_executable(Neural_Network_Advanced_Demo examples/neural_network_advanced_demo.cpp)
    target_link_libraries(Neural_Network_Advanced_Demo PRIVATE 5DGUI_SDK)
elseif(PLATFORM_LINUX)
    # Linux demo applications
    add_executable(5DGUI_Demo_Linux examples/demo_linux.cpp)
    target_link_libraries(5DGUI_Demo_Linux PRIVATE 5DGUI_SDK)
    
    # Widget demo application
    add_executable(5DGUI_WidgetDemo_Linux examples/widget_demo_linux.cpp)
    target_link_libraries(5DGUI_WidgetDemo_Linux PRIVATE 5DGUI_SDK)
endif()

# Set subsystem to Windows (GUI application) - Windows only
if(PLATFORM_WINDOWS)
    if(MSVC)
        set_target_properties(5DGUI_Demo PROPERTIES
            LINK_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:WinMainCRTStartup"
        )
        set_target_properties(5DGUI_WidgetDemo PROPERTIES
            LINK_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:WinMainCRTStartup"
        )
        set_target_properties(Widget_Showcase PROPERTIES
            LINK_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:WinMainCRTStartup"
        )
        set_target_properties(Camera_Demo PROPERTIES
            LINK_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:WinMainCRTStartup"
        )
        set_target_properties(Toolbar_Demo PROPERTIES
            LINK_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:WinMainCRTStartup"
        )
        set_target_properties(Advanced_Window_Demo PROPERTIES
            LINK_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:WinMainCRTStartup"
        )
        set_target_properties(Neural_Network_Demo PROPERTIES
            LINK_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:WinMainCRTStartup"
        )
        set_target_properties(Optimization_Demo PROPERTIES
            LINK_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:WinMainCRTStartup"
        )
        set_target_properties(New_Widgets_Demo PROPERTIES
            LINK_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:WinMainCRTStartup"
        )
        set_target_properties(Widget_Properties_Demo PROPERTIES
            LINK_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:WinMainCRTStartup"
        )
        set_target_properties(Hardware_Accel_Demo PROPERTIES
            LINK_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:WinMainCRTStartup"
        )
        set_target_properties(RichText_DataGrid_Demo PROPERTIES
            LINK_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:WinMainCRTStartup"
        )
        set_target_properties(ML_Optimizer_Demo PROPERTIES
            LINK_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:WinMainCRTStartup"
        )
        set_target_properties(Layout_Engine_Demo PROPERTIES
            LINK_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:WinMainCRTStartup"
        )
        set_target_properties(DPI_MultiMonitor_Demo PROPERTIES
            LINK_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:WinMainCRTStartup"
        )
        set_target_properties(V13_Showcase_Demo PROPERTIES
            LINK_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:WinMainCRTStartup"
        )
        set_target_properties(Widget_Enhancements_Demo PROPERTIES
            LINK_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:WinMainCRTStartup"
        )
        set_target_properties(Neural_Network_Advanced_Demo PROPERTIES
            LINK_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:WinMainCRTStartup"
        )
    else()
        set_target_properties(5DGUI_Demo PROPERTIES
            LINK_FLAGS "-mwindows"
        )
        set_target_properties(5DGUI_WidgetDemo PROPERTIES
            LINK_FLAGS "-mwindows"
        )
        set_target_properties(Widget_Showcase PROPERTIES
            LINK_FLAGS "-mwindows"
        )
        set_target_properties(Camera_Demo PROPERTIES
            LINK_FLAGS "-mwindows"
        )
        set_target_properties(Toolbar_Demo PROPERTIES
            LINK_FLAGS "-mwindows"
        )
        set_target_properties(Advanced_Window_Demo PROPERTIES
            LINK_FLAGS "-mwindows"
        )
        set_target_properties(Neural_Network_Demo PROPERTIES
            LINK_FLAGS "-mwindows"
        )
        set_target_properties(Optimization_Demo PROPERTIES
            LINK_FLAGS "-mwindows"
        )
        set_target_properties(New_Widgets_Demo PROPERTIES
            LINK_FLAGS "-mwindows"
        )
        set_target_properties(Widget_Properties_Demo PROPERTIES
            LINK_FLAGS "-mwindows"
        )
        set_target_properties(Hardware_Accel_Demo PROPERTIES
            LINK_FLAGS "-mwindows"
        )
        set_target_properties(RichText_DataGrid_Demo PROPERTIES
            LINK_FLAGS "-mwindows"
        )
        set_target_properties(ML_Optimizer_Demo PROPERTIES
            LINK_FLAGS "-mwindows"
        )
        set_target_properties(Layout_Engine_Demo PROPERTIES
            LINK_FLAGS "-mwindows"
        )
        set_target_properties(DPI_MultiMonitor_Demo PROPERTIES
            LINK_FLAGS "-mwindows"
        )
        set_target_properties(V13_Showcase_Demo PROPERTIES
            LINK_FLAGS "-mwindows"
        )
        set_target_properties(Widget_Enhancements_Demo PROPERTIES
            LINK_FLAGS "-mwindows"
        )
        set_target_properties(Neural_Network_Advanced_Demo PROPERTIES
            LINK_FLAGS "-mwindows"
        )
    endif()
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

if(PLATFORM_WINDOWS)
    install(TARGETS 5DGUI_Demo 5DGUI_WidgetDemo Widget_Showcase Camera_Demo Toolbar_Demo Advanced_Window_Demo Neural_Network_Demo Optimization_Demo New_Widgets_Demo Widget_Properties_Demo Hardware_Accel_Demo RichText_DataGrid_Demo ML_Optimizer_Demo Layout_Engine_Demo DPI_MultiMonitor_Demo V13_Showcase_Demo Widget_Enhancements_Demo Neural_Network_Advanced_Demo
        RUNTIME DESTINATION bin
    )
elseif(PLATFORM_LINUX)
    install(TARGETS 5DGUI_Demo_Linux 5DGUI_WidgetDemo_Linux
        RUNTIME DESTINATION bin
    )
endif()

# Documentation
install(FILES README.md
    DESTINATION docs
)
