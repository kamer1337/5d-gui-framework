# Building the 5D GUI SDK

This document provides instructions for building the 5D GUI SDK and demo applications on Windows and Linux.

## Prerequisites

### Windows

- **Operating System**: Windows 7 or later
- **Compiler**: One of the following:
  - Visual Studio 2017 or later (MSVC)
  - MinGW-w64 with GCC 7.0+
  - Clang for Windows
- **C++ Standard**: C++17 or later
- **Build Tools**: 
  - CMake 3.10+ (recommended)
  - OR Make (MinGW)

#### Windows SDK

The Windows SDK is required for Win32 API headers and libraries:
- Usually included with Visual Studio
- Standalone download: https://developer.microsoft.com/windows/downloads/windows-sdk/

### Linux

- **Operating System**: Any modern Linux distribution with X11
- **Compiler**: One of the following:
  - GCC 7.0+ (recommended)
  - Clang 5.0+
- **C++ Standard**: C++17 or later
- **Build Tools**: CMake 3.10+ (required)
- **Required Libraries**:
  - X11 development libraries
  - pthread (usually included)

#### Installing Linux Dependencies

**Ubuntu/Debian:**
```bash
sudo apt-get update
sudo apt-get install build-essential cmake libx11-dev
```

**Fedora/RHEL:**
```bash
sudo dnf install gcc-c++ cmake libX11-devel
```

**Arch Linux:**
```bash
sudo pacman -S base-devel cmake libx11
```

## Build Methods

### Method 1: CMake (Recommended)

#### Windows - With Visual Studio

```cmd
# Create build directory
mkdir build
cd build

# Generate Visual Studio project
cmake .. -G "Visual Studio 16 2019" -A x64

# Build
cmake --build . --config Release

# Or open the generated .sln in Visual Studio
```

#### Windows - With MinGW

```cmd
# Create build directory
mkdir build
cd build

# Generate Makefiles
cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build .
```

#### Linux - GCC or Clang

```bash
# Create build directory
mkdir build
cd build

# Generate Makefiles
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build
make -j$(nproc)

# Or with specific compiler
cmake .. -DCMAKE_CXX_COMPILER=g++ -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

#### With Ninja (Cross-platform)

```bash
# Create build directory
mkdir build
cd build

# Generate Ninja files
cmake .. -G "Ninja" -DCMAKE_BUILD_TYPE=Release

# Build
ninja
```

### Method 2: Makefile (MinGW - Windows Only)

```cmd
# Build everything
mingw32-make all

# Build SDK only
mingw32-make sdk

# Build demo only (requires SDK)
mingw32-make demo

# Clean build files
mingw32-make clean
```

### Method 3: Visual Studio (Manual - Windows Only)

1. Open Visual Studio
2. Create New Project → Empty C++ Project
3. Add SDK source files from `src/SDK/`
4. Add SDK headers from `include/SDK/`
5. Set include directories: `include/`
6. Link libraries: `dwmapi.lib gdi32.lib user32.lib kernel32.lib`
7. Set C++ Language Standard: ISO C++17
8. Build as Static Library (.lib)

For the demo:
1. Create second project in same solution
2. Add `examples/demo.cpp`
3. Reference SDK library project
4. Set Subsystem: Windows (/SUBSYSTEM:WINDOWS)
5. Build as Application (.exe)

## Build Output

After successful build:

### Windows
```
build/
├── bin/
│   ├── lib5DGUI_SDK.a           # SDK static library (MinGW)
│   ├── 5DGUI_SDK.lib            # SDK static library (MSVC)
│   ├── 5DGUI_Demo.exe           # Main demo application
│   ├── 5DGUI_WidgetDemo.exe     # Widget system demo
│   ├── Widget_Showcase.exe      # Complete widget showcase
│   ├── Neural_Network_Demo.exe  # Neural network GUI creation demo
│   ├── Camera_Demo.exe          # 3D camera controller demo
│   ├── Toolbar_Demo.exe         # Toolbar widget demo
│   └── Advanced_Window_Demo.exe # Advanced window features demo
└── obj/
    └── *.o                       # Object files
```

### Linux
```
build/
├── lib5DGUI_SDK.a               # SDK static library
├── 5DGUI_Demo_Linux             # X11 rendering demo
├── 5DGUI_WidgetDemo_Linux       # Widget system demo
└── *.o                          # Object files
```

**Note**: Demo applications use X11 for window management and rendering.

## Configuration Options

### CMake Variables

```bash
# Build type
-DCMAKE_BUILD_TYPE=Debug|Release|RelWithDebInfo

# Install prefix
-DCMAKE_INSTALL_PREFIX=C:/path/to/install

# Compiler selection
-DCMAKE_CXX_COMPILER=g++|clang++|cl
```

### Compiler Flags

Default flags in CMakeLists.txt:
- C++17 standard
- Warning level: High
- Optimization: O2 (Release)

To customize:
```cmd
cmake .. -DCMAKE_CXX_FLAGS="-O3 -march=native"
```

## Installation

### CMake Install

```cmd
# After building
cmake --install . --prefix C:/SDK/5DGUI
```

This installs:
- Headers to `include/SDK/`
- Library to `lib/`
- Demo to `bin/`
- Documentation to `docs/`

### Manual Install

Copy files to your project:
```
YourProject/
├── include/SDK/          # Copy from include/SDK/
├── lib/
│   └── 5DGUI_SDK.lib    # Copy from build output
└── src/
    └── main.cpp          # Your code
```

Link in your project:
```cpp
#include "SDK/SDK.h"
#pragma comment(lib, "5DGUI_SDK.lib")
#pragma comment(lib, "dwmapi.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "user32.lib")
```

## Troubleshooting

### "windows.h: No such file or directory"

**Problem**: Windows SDK not found

**Solution**:
- Install Windows SDK
- Visual Studio: Install "Desktop development with C++" workload
- MinGW: Ensure w64 variant is installed

### "undefined reference to CreateWindowExW"

**Problem**: Windows libraries not linked

**Solution**:
- Add to CMakeLists.txt: `target_link_libraries(... user32)`
- Or in code: `#pragma comment(lib, "user32.lib")`
- Or in Makefile: Add `-luser32` to LDFLAGS

### CMake can't find compiler

**Problem**: Compiler not in PATH

**Solution**:
```cmd
# Visual Studio
"C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat"

# MinGW
set PATH=C:\mingw64\bin;%PATH%
```

### "error LNK2019: unresolved external symbol"

**Problem**: Missing library or object file

**Solution**:
- Ensure all .cpp files are compiled
- Check all required libraries are linked
- Verify correct architecture (x64 vs x86)

### Build succeeds but demo crashes

**Problem**: Missing DLLs or incompatible runtime

**Solution**:
- Run from Visual Studio Developer Command Prompt
- Copy required DLLs to exe directory
- Use static runtime linking: `/MT` flag

## Platform-Specific Notes

### Windows 7

- Requires Platform Update
- DWM must be enabled (Aero theme)
- Some visual effects may be limited

### Windows 10/11

- Full feature support
- Enhanced transparency effects
- Better DPI scaling

### 32-bit vs 64-bit

Both architectures supported:
```cmd
# 64-bit (recommended)
cmake .. -A x64

# 32-bit
cmake .. -A Win32
```

## Development Build

For development with debugging:

```cmd
mkdir build-debug
cd build-debug
cmake .. -DCMAKE_BUILD_TYPE=Debug
cmake --build .
```

Debug features:
- No optimization
- Full symbols
- Runtime checks
- Assertions enabled

## Release Build

For distribution:

```cmd
mkdir build-release
cd build-release
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .
strip bin/5DGUI_Demo.exe  # Optional: remove symbols
```

Release features:
- Full optimization
- No symbols
- Small binary size
- Maximum performance

## Continuous Integration

### Example GitHub Actions

```yaml
name: Build SDK

on: [push, pull_request]

jobs:
  build:
    runs-on: windows-latest
    steps:
    - uses: actions/checkout@v2
    - name: Configure
      run: cmake -B build -DCMAKE_BUILD_TYPE=Release
    - name: Build
      run: cmake --build build --config Release
    - name: Test
      run: build/bin/Release/5DGUI_Demo.exe --test
```

## Build Performance

Typical build times:
- SDK library: 5-10 seconds
- Demo application: 2-5 seconds
- Total (clean build): 15-20 seconds

For faster builds:
- Use Ninja generator
- Enable parallel builds: `cmake --build . -j8`
- Use ccache (if available)

## Next Steps

After building:
1. Run the demos:
   
   **Windows:**
   - `build/bin/5DGUI_Demo.exe` - Main 5D rendering demo
   - `build/bin/5DGUI_WidgetDemo.exe` - Widget system demo
   - `build/bin/Widget_Showcase.exe` - Complete widget showcase
   - `build/bin/Neural_Network_Demo.exe` - Neural network GUI creation demo
   - `build/bin/Camera_Demo.exe` - 3D camera controller demo
   - `build/bin/Toolbar_Demo.exe` - Toolbar widget demo
   - `build/bin/Advanced_Window_Demo.exe` - Advanced window features demo
   - `build/bin/ML_Optimizer_Demo.exe` - Machine learning renderer optimization demo
   
   **Linux:**
   - `build/5DGUI_Demo_Linux` - X11 rendering demo
   - `build/5DGUI_WidgetDemo_Linux` - Widget system demo

2. Read the [API Documentation](API.md)
3. See [Examples](examples/) for usage patterns
4. Read [ARCHITECTURE.md](ARCHITECTURE.md) for internals
5. For Linux-specific information, see [LINUX_SUPPORT.md](LINUX_SUPPORT.md)

## Support

For build issues:
1. Check this document
2. Review compiler output
3. Verify prerequisites installed
4. Check GitHub Issues
5. Create detailed bug report with:
   - OS version
   - Compiler version
   - Full build log
   - CMake version
