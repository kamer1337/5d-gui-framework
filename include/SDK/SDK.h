#pragma once

/**
 * 5D GUI SDK - Main Include File
 * 
 * A custom complex C++ GUI SDK with CreateWindowExW hooking
 * Optimized for multimodal windows with advanced theming
 * 
 * Features:
 * - Window creation hooking via CreateWindowExW
 * - 5D depth rendering system (5 depth levels)
 * - Multi-directional gradients (vertical, horizontal, radial)
 * - Layered window support with alpha transparency
 * - Rounded corners with anti-aliasing
 * - Depth-aware shadows
 * - Particle and glow effects
 * - Icon system with depth levels
 * - Multimodal window management
 * - Themed interface system
 * 
 * No external dependencies - pure Win32 API
 */

#include "WindowHook.h"
#include "Window.h"
#include "WindowManager.h"
#include "Theme.h"
#include "Renderer.h"
#include "Widget.h"
#include "AdvancedWidgets.h"
#include "CameraController.h"
#include "Widget3D.h"

namespace SDK {

/**
 * Initialize the 5D GUI SDK
 * Must be called before creating any windows
 */
bool Initialize();

/**
 * Shutdown the SDK and cleanup resources
 */
void Shutdown();

/**
 * Get SDK version
 */
const char* GetVersion();

/**
 * Check if SDK is initialized
 */
bool IsInitialized();

} // namespace SDK
