# Contributing to 5D GUI SDK

Thank you for your interest in contributing to the 5D GUI SDK! This document provides guidelines and instructions for contributing.

## Table of Contents

1. [Code of Conduct](#code-of-conduct)
2. [Getting Started](#getting-started)
3. [Development Setup](#development-setup)
4. [Coding Standards](#coding-standards)
5. [Submitting Changes](#submitting-changes)
6. [Testing Guidelines](#testing-guidelines)
7. [Documentation](#documentation)
8. [Community](#community)

## Code of Conduct

### Our Pledge

We are committed to providing a welcoming and inspiring community for all. Please be respectful and considerate in all interactions.

### Expected Behavior

- Use welcoming and inclusive language
- Be respectful of differing viewpoints
- Accept constructive criticism gracefully
- Focus on what is best for the community
- Show empathy towards other community members

## Getting Started

### Types of Contributions

We welcome:
- **Bug Fixes**: Fix issues reported in GitHub Issues
- **Feature Implementation**: Implement features from the roadmap
- **Documentation**: Improve or add documentation
- **Examples**: Create example applications
- **Performance**: Optimize existing code
- **Tests**: Add or improve test coverage

### Where to Start

1. **Check Issues**: Look for issues labeled `good-first-issue` or `help-wanted`
2. **Review Roadmap**: See [ROADMAP.md](ROADMAP.md) for planned features
3. **Ask Questions**: Open a discussion if you're unsure
4. **Small First**: Start with small contributions to learn the codebase

## Development Setup

### Prerequisites

- Windows 7 or later
- Visual Studio 2017+ or MinGW-w64
- CMake 3.10+ (optional)
- Git for version control

### Clone Repository

```bash
git clone https://github.com/kamer1337/MaterialGameEngine.git
cd MaterialGameEngine
```

### Build Development Version

```bash
mkdir build-dev
cd build-dev
cmake .. -DCMAKE_BUILD_TYPE=Debug
cmake --build .
```

### Run Tests

```bash
# After building
ctest --output-on-failure
```

## Coding Standards

### C++ Style Guide

#### Naming Conventions

```cpp
// Classes: PascalCase
class WindowManager { };

// Functions: PascalCase
void RegisterWindow();

// Variables: camelCase
int windowCount;

// Private members: m_ prefix
class Window {
private:
    int m_width;
    HWND m_hwnd;
};

// Constants: UPPER_SNAKE_CASE
const int MAX_WINDOWS = 100;

// Namespaces: PascalCase
namespace SDK { }
```

#### Code Formatting

```cpp
// Braces on same line for control structures
if (condition) {
    // code
} else {
    // code
}

// Braces on new line for functions and classes
void Function()
{
    // code
}

class MyClass
{
public:
    void Method();
};
```

#### Indentation

- Use **4 spaces** for indentation (no tabs)
- Align continuation lines with opening parenthesis

```cpp
auto result = SomeLongFunctionName(parameter1,
                                   parameter2,
                                   parameter3);
```

#### Comments

```cpp
// Single-line comments for brief explanations
int count = 0;  // Initialize counter

/**
 * Multi-line comments for function/class documentation
 * Describe purpose, parameters, and return values
 */
void ComplexFunction(int param);

// TODO: Add feature XYZ
// FIXME: Bug in edge case
// NOTE: Important implementation detail
```

### Header Files

#### Include Guards

```cpp
#pragma once

// Alternative (if pragma once not supported)
#ifndef SDK_WINDOW_H
#define SDK_WINDOW_H

// ... declarations ...

#endif // SDK_WINDOW_H
```

#### Include Order

```cpp
// 1. Corresponding header (if .cpp file)
#include "Window.h"

// 2. SDK headers
#include "WindowManager.h"
#include "Theme.h"

// 3. System headers
#include <windows.h>
#include <vector>
#include <memory>
```

### Best Practices

#### Memory Management

```cpp
// Prefer smart pointers
std::shared_ptr<Theme> theme;
std::unique_ptr<Window> window;

// Clean up GDI objects
HBRUSH brush = CreateSolidBrush(...);
// ... use brush ...
DeleteObject(brush);

// Use RAII for resources
class WindowContext {
public:
    WindowContext(HWND hwnd) : m_hdc(GetDC(hwnd)) {}
    ~WindowContext() { ReleaseDC(m_hwnd, m_hdc); }
    
    HDC Get() const { return m_hdc; }
    
private:
    HWND m_hwnd;
    HDC m_hdc;
};
```

#### Error Handling

```cpp
// Check for errors and return bool or handle appropriately
bool RegisterWindow(HWND hwnd) {
    if (!hwnd || !IsWindow(hwnd)) {
        return false;  // Invalid parameter
    }
    
    // ... implementation ...
    return true;
}

// Document error conditions
/**
 * Register a window with the SDK
 * @param hwnd Window handle (must be valid)
 * @return true on success, false if hwnd is invalid
 */
bool RegisterWindow(HWND hwnd);
```

#### Performance

```cpp
// Avoid unnecessary allocations
void Render() {
    static std::vector<int> buffer;  // Reuse buffer
    buffer.clear();
    // ... use buffer ...
}

// Use const references to avoid copies
void ProcessWindow(const Window& window);

// Inline small functions
inline int GetWidth() const { return m_width; }
```

## Submitting Changes

### Workflow

1. **Fork Repository**: Create your own fork on GitHub
2. **Create Branch**: Create a feature branch from `main`
3. **Make Changes**: Implement your changes
4. **Test**: Ensure all tests pass
5. **Commit**: Write clear commit messages
6. **Push**: Push to your fork
7. **Pull Request**: Open a PR to main repository

### Branch Naming

```
feature/add-blur-effect
bugfix/fix-shadow-rendering
docs/update-api-reference
perf/optimize-gradient-rendering
```

### Commit Messages

Use conventional commits format:

```
feat: add blur effect to renderer
fix: correct shadow offset calculation
docs: update API documentation for Window class
perf: optimize gradient rendering loop
test: add tests for particle system
refactor: simplify theme management code
```

Commit message structure:
```
<type>: <subject>

[optional body]

[optional footer]
```

Types:
- `feat`: New feature
- `fix`: Bug fix
- `docs`: Documentation changes
- `perf`: Performance improvement
- `test`: Adding or updating tests
- `refactor`: Code refactoring
- `style`: Code style changes
- `chore`: Maintenance tasks

### Pull Request Guidelines

#### PR Title

```
Add blur effect to renderer
Fix shadow rendering bug
Update API documentation
```

#### PR Description Template

```markdown
## Description
Brief description of changes

## Motivation
Why is this change needed?

## Changes
- List of specific changes
- Added feature X
- Fixed bug Y

## Testing
- [ ] Built successfully
- [ ] Existing tests pass
- [ ] New tests added
- [ ] Manual testing performed

## Screenshots
(If applicable)

## Checklist
- [ ] Code follows style guidelines
- [ ] Comments added for complex code
- [ ] Documentation updated
- [ ] No compiler warnings
- [ ] Backwards compatible (or breaking changes documented)
```

### Review Process

1. Automated checks run (build, tests)
2. Code review by maintainers
3. Address review comments
4. Final approval and merge

## Testing Guidelines

### Writing Tests

```cpp
// Example test structure
void TestWindowCreation() {
    // Arrange
    SDK::Initialize();
    HWND hwnd = CreateTestWindow();
    
    // Act
    auto window = SDK::WindowManager::GetInstance().RegisterWindow(hwnd);
    
    // Assert
    assert(window != nullptr);
    assert(window->IsValid());
    
    // Cleanup
    SDK::Shutdown();
}
```

### Test Categories

1. **Unit Tests**: Test individual components
2. **Integration Tests**: Test component interactions
3. **Performance Tests**: Benchmark critical code
4. **Visual Tests**: Verify rendering output

### Running Tests

```bash
# Build and run all tests
cmake --build . --target test

# Run specific test
./build/bin/WindowTests.exe

# Run with verbose output
ctest -V
```

## Documentation

### Code Documentation

```cpp
/**
 * @brief Brief description of function
 * 
 * Detailed description explaining the purpose,
 * algorithm, or important details.
 * 
 * @param hwnd Window handle to register
 * @param theme Theme to apply (optional)
 * @return Pointer to Window object, or nullptr on failure
 * 
 * @note Important notes about usage
 * @warning Warnings about edge cases
 * 
 * @example
 * auto window = RegisterWindow(hwnd, myTheme);
 * if (window) {
 *     window->SetDepth(WindowDepth::FOREGROUND);
 * }
 */
std::shared_ptr<Window> RegisterWindow(HWND hwnd, 
                                      std::shared_ptr<Theme> theme = nullptr);
```

### Documentation Files

When adding features, update:
- **API.md**: API reference
- **USAGE.md**: Usage examples
- **ARCHITECTURE.md**: Design changes
- **README.md**: Overview (if applicable)

## Community

### Communication Channels

- **GitHub Issues**: Bug reports and feature requests
- **GitHub Discussions**: General questions and discussions
- **Pull Requests**: Code review and contributions

### Getting Help

- Read existing documentation
- Search closed issues
- Ask in GitHub Discussions
- Tag relevant maintainers

### Recognition

Contributors are recognized in:
- CONTRIBUTORS.md file
- Release notes
- Project documentation

## Additional Guidelines

### Security

- Never commit credentials or secrets
- Report security vulnerabilities privately
- Follow secure coding practices

### License

By contributing, you agree that your contributions will be licensed under the MIT License.

### Attribution

- Maintain copyright notices
- Credit original authors
- Document third-party code

## Questions?

If you have questions about contributing:
1. Check existing documentation
2. Search GitHub Issues
3. Open a Discussion
4. Contact maintainers

Thank you for contributing to the 5D GUI SDK!
