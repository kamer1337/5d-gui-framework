#pragma once

/**
 * 5D GUI SDK - Main Include File
 * 
 * A custom complex C++ GUI SDK with CreateWindowExW hooking
 * Optimized for multimodal windows with advanced theming
 * 
 * Features:
 * - Cross-platform support (Windows, Linux)
 * - Window creation hooking via CreateWindowExW (Windows)
 * - 5D depth rendering system (5 depth levels)
 * - Multi-directional gradients (vertical, horizontal, radial)
 * - Layered window support with alpha transparency
 * - Rounded corners with anti-aliasing
 * - Depth-aware shadows
 * - Particle and glow effects
 * - Icon system with depth levels
 * - Multimodal window management
 * - Themed interface system
 * - Widget system (ProgressBar, Tooltip, Toolbar)
 * - Prompt-based window generation
 * - Neural network for natural language GUI creation
 * - Advanced window features (groups, snapping, animations)
 * - Advanced visual effects (v1.3): depth-of-field, motion blur, chromatic aberration
 * - Effect presets (v1.3): CINEMATIC, GAME_UI, RETRO, DREAMY, MOTION
 * - Enhanced animations (v1.3): Bezier curves, pause/resume/reverse
 * - Layout system (v1.3): Grid, Flow, Stack layouts
 * - Menu system (v1.3): MenuBar, ContextMenu with icons and shortcuts
 * - Rich text rendering (v1.3): Markdown support, formatting
 * - DPI awareness (v2.0): Per-monitor DPI support, automatic scaling
 * - Multi-monitor support (v2.0): Monitor-specific themes, window migration
 * 
 * No external dependencies - pure Win32 API (Windows) or X11 (Linux)
 */

#include "Platform.h"
#include "WindowHook.h"
#include "Window.h"
#include "WindowManager.h"
#include "WindowGroup.h"
#include "WindowSnapping.h"
#include "WindowAnimation.h"
#include "Theme.h"
#include "Renderer.h"
#include "RenderBackend.h"
#include "GDIRenderBackend.h"
#include "D2DRenderBackend.h"
#include "Widget.h"
#include "ProgressBar.h"
#include "Tooltip.h"
#include "Toolbar.h"
#include "WidgetManager.h"
#include "PromptWindowBuilder.h"
#include "NeuralNetwork.h"
#include "NeuralPromptBuilder.h"
#include "AdvancedWidgets.h"
#include "CameraController.h"
#include "Widget3D.h"
#include "Layout.h"
#include "Menu.h"
#include "RichText.h"
#include "DPIManager.h"
#include "MonitorManager.h"

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
