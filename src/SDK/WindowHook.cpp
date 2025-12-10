#include "../../include/SDK/WindowHook.h"
#include "../../include/SDK/WindowManager.h"
#include <psapi.h>
#include <cstring>

namespace SDK {

// Static instance
WindowHook& WindowHook::GetInstance() {
    static WindowHook instance;
    return instance;
}

WindowHook::WindowHook()
    : m_pOriginalCreateWindowExW(nullptr)
    , m_pTrampoline(nullptr)
    , m_originalBytesSize(0)
    , m_bIsHooked(false)
    , m_hookType(HookType::INLINE)
    , m_createCallback(nullptr)
{
    memset(m_originalBytes, 0, sizeof(m_originalBytes));
}

WindowHook::~WindowHook() {
    Shutdown();
}

bool WindowHook::Initialize(HookType type) {
    if (m_bIsHooked) {
        return true; // Already hooked
    }
    
    m_hookType = type;
    
    // Store original function
    m_pOriginalCreateWindowExW = &CreateWindowExW;
    
    // Install hook based on type
    if (type == HookType::INLINE) {
        m_bIsHooked = InstallInlineHook();
    } else {
        m_bIsHooked = InstallIATHook();
    }
    
    return m_bIsHooked;
}

void WindowHook::Shutdown() {
    if (m_bIsHooked) {
        if (m_hookType == HookType::INLINE) {
            RemoveInlineHook();
        } else {
            RemoveIATHook();
        }
        m_bIsHooked = false;
    }
}

void WindowHook::RegisterCreateCallback(CreateWindowCallback callback) {
    m_createCallback = callback;
}

HWND WINAPI WindowHook::HookedCreateWindowExW(
    DWORD dwExStyle,
    LPCWSTR lpClassName,
    LPCWSTR lpWindowName,
    DWORD dwStyle,
    int X,
    int Y,
    int nWidth,
    int nHeight,
    HWND hWndParent,
    HMENU hMenu,
    HINSTANCE hInstance,
    LPVOID lpParam)
{
    // Call original function
    WindowHook& hook = GetInstance();
    HWND hwnd = hook.m_pOriginalCreateWindowExW(
        dwExStyle, lpClassName, lpWindowName, dwStyle,
        X, Y, nWidth, nHeight,
        hWndParent, hMenu, hInstance, lpParam
    );
    
    // Notify callback
    if (hwnd && hook.m_createCallback) {
        hook.m_createCallback(hwnd);
    }
    
    return hwnd;
}

// IAT Hooking implementation

// Helper function to find IAT entry
PIMAGE_THUNK_DATA WindowHook::FindIATEntry(HMODULE hModule, const char* dllName, const char* functionName) {
    if (!hModule) {
        return nullptr;
    }
    
    // Get DOS header
    PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)hModule;
    if (pDosHeader->e_magic != IMAGE_DOS_SIGNATURE) {
        return nullptr;
    }
    
    // Get NT headers
    PIMAGE_NT_HEADERS pNtHeaders = (PIMAGE_NT_HEADERS)((BYTE*)hModule + pDosHeader->e_lfanew);
    if (pNtHeaders->Signature != IMAGE_NT_SIGNATURE) {
        return nullptr;
    }
    
    // Get import directory RVA and size
    DWORD importDirRva = pNtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;
    DWORD importDirSize = pNtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].Size;
    
    // Check if import directory exists
    if (importDirRva == 0 || importDirSize == 0) {
        return nullptr;
    }
    
    // Get import descriptor
    PIMAGE_IMPORT_DESCRIPTOR pImportDesc = (PIMAGE_IMPORT_DESCRIPTOR)((BYTE*)hModule + importDirRva);
    
    // Get module information for bounds checking
    HANDLE hProcess = GetCurrentProcess();
    if (!hProcess) {
        return nullptr;
    }
    
    MODULEINFO modInfo;
    if (!GetModuleInformation(hProcess, hModule, &modInfo, sizeof(modInfo))) {
        return nullptr;
    }
    
    BYTE* moduleBase = (BYTE*)hModule;
    BYTE* moduleEnd = moduleBase + modInfo.SizeOfImage;
    
    // Validate import descriptor is within module bounds
    if ((BYTE*)pImportDesc < moduleBase || (BYTE*)pImportDesc >= moduleEnd) {
        return nullptr;
    }
    
    // Iterate through import descriptors to find the specified DLL
    for (; pImportDesc->Name != 0; pImportDesc++) {
        // Validate current pImportDesc is still within bounds
        if ((BYTE*)pImportDesc >= moduleEnd - sizeof(IMAGE_IMPORT_DESCRIPTOR)) {
            break;
        }
        
        // Validate Name RVA is within module bounds
        BYTE* pszModNameAddr = moduleBase + pImportDesc->Name;
        if (pszModNameAddr < moduleBase || pszModNameAddr >= moduleEnd) {
            continue;
        }
        
        const char* pszModName = (const char*)pszModNameAddr;
        
        if (_stricmp(pszModName, dllName) == 0) {
            // Found the DLL, now find the function in its IAT
            PIMAGE_THUNK_DATA pThunk = (PIMAGE_THUNK_DATA)(moduleBase + pImportDesc->FirstThunk);
            PIMAGE_THUNK_DATA pOrigThunk = (PIMAGE_THUNK_DATA)(moduleBase + pImportDesc->OriginalFirstThunk);
            
            // Validate thunk pointers are within bounds
            if ((BYTE*)pThunk < moduleBase || (BYTE*)pThunk >= moduleEnd ||
                (BYTE*)pOrigThunk < moduleBase || (BYTE*)pOrigThunk >= moduleEnd) {
                continue;
            }
            
            for (; pOrigThunk->u1.Function != 0; pThunk++, pOrigThunk++) {
                // Validate thunk pointers are still within bounds
                if ((BYTE*)pThunk >= moduleEnd - sizeof(IMAGE_THUNK_DATA) ||
                    (BYTE*)pOrigThunk >= moduleEnd - sizeof(IMAGE_THUNK_DATA)) {
                    break;
                }
                
                if (pOrigThunk->u1.Ordinal & IMAGE_ORDINAL_FLAG) {
                    continue; // Skip ordinal imports
                }
                
                // Validate AddressOfData RVA is within module bounds
                BYTE* pImportNameAddr = moduleBase + pOrigThunk->u1.AddressOfData;
                if (pImportNameAddr < moduleBase || pImportNameAddr >= moduleEnd) {
                    continue;
                }
                
                PIMAGE_IMPORT_BY_NAME pImportName = (PIMAGE_IMPORT_BY_NAME)pImportNameAddr;
                
                if (strcmp((const char*)pImportName->Name, functionName) == 0) {
                    return pThunk;
                }
            }
        }
    }
    
    return nullptr;
}

bool WindowHook::InstallIATHook() {
    // IAT hook installation - patches Import Address Table
    // This method finds and patches the CreateWindowExW entry in the IAT
    
    HMODULE hModule = GetModuleHandle(nullptr);
    if (!hModule) {
        return false;
    }
    
    PIMAGE_THUNK_DATA pThunk = FindIATEntry(hModule, "user32.dll", "CreateWindowExW");
    if (!pThunk) {
        return false;
    }
    
    DWORD oldProtect;
    
    // Save original function pointer
    m_pOriginalCreateWindowExW = (CreateWindowExW_t)pThunk->u1.Function;
    
    // Change memory protection to allow writing
    if (!VirtualProtect(&pThunk->u1.Function, sizeof(PVOID), PAGE_READWRITE, &oldProtect)) {
        return false;
    }
    
    // Patch the IAT entry
    pThunk->u1.Function = (DWORD_PTR)&HookedCreateWindowExW;
    
    // Restore memory protection
    DWORD dummy;
    VirtualProtect(&pThunk->u1.Function, sizeof(PVOID), oldProtect, &dummy);
    
    return true;
}

void WindowHook::RemoveIATHook() {
    // Cleanup IAT hook if installed
    if (!m_pOriginalCreateWindowExW) {
        return;
    }
    
    HMODULE hModule = GetModuleHandle(nullptr);
    if (!hModule) {
        return;
    }
    
    PIMAGE_THUNK_DATA pThunk = FindIATEntry(hModule, "user32.dll", "CreateWindowExW");
    if (!pThunk) {
        return;
    }
    
    DWORD oldProtect;
    
    // Change memory protection to allow writing
    if (!VirtualProtect(&pThunk->u1.Function, sizeof(PVOID), PAGE_READWRITE, &oldProtect)) {
        return;
    }
    
    // Restore the original IAT entry
    pThunk->u1.Function = (DWORD_PTR)m_pOriginalCreateWindowExW;
    
    // Restore memory protection
    DWORD dummy;
    VirtualProtect(&pThunk->u1.Function, sizeof(PVOID), oldProtect, &dummy);
    
    m_pOriginalCreateWindowExW = nullptr;
}

// Inline Hooking implementation
bool WindowHook::InstallInlineHook() {
    // SECURITY NOTE: Inline hooking of system APIs may trigger antivirus software
    // and Windows Defender. This is expected behavior for legitimate hooking.
    // Ensure proper error handling and consider fallback mechanisms if hooking fails.
    
    // THREAD SAFETY NOTE: This implementation is not thread-safe during installation.
    // Ensure hooks are installed during application initialization before multiple
    // threads are created, or add synchronization mechanisms.
    
    // Get the address of CreateWindowExW
    void* targetFunc = (void*)&CreateWindowExW;
    void* hookFunc = (void*)&HookedCreateWindowExW;
    
    // Save original bytes (first 5 bytes for JMP instruction on x86, or more for x64)
#ifdef _WIN64
    m_originalBytesSize = 14; // Longer for x64 absolute jump
#else
    m_originalBytesSize = 5;  // 5 bytes for x86 relative jump
#endif
    
    // Read original bytes
    memcpy(m_originalBytes, targetFunc, m_originalBytesSize);
    
    // Create trampoline
    if (!CreateTrampoline(targetFunc, hookFunc, &m_pTrampoline)) {
        return false;
    }
    
    // Update original function pointer to trampoline
    m_pOriginalCreateWindowExW = (CreateWindowExW_t)m_pTrampoline;
    
    // Write jump to hook function
#ifdef _WIN64
    // For x64, we need to write a 14-byte absolute jump
    BYTE jumpBytes[14] = {
        0xFF, 0x25, 0x00, 0x00, 0x00, 0x00, // JMP [RIP+0]
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 // Address (8 bytes)
    };
    *(UINT64*)(jumpBytes + 6) = (UINT64)hookFunc;
#else
    // For x86, we write a 5-byte relative jump
    BYTE jumpBytes[5] = { 0xE9, 0x00, 0x00, 0x00, 0x00 }; // JMP rel32
    *(INT32*)(jumpBytes + 1) = (INT32)((BYTE*)hookFunc - (BYTE*)targetFunc - 5);
#endif
    
    if (!WriteMemory(targetFunc, jumpBytes, m_originalBytesSize)) {
        FreeTrampoline();
        return false;
    }
    
    return true;
}

void WindowHook::RemoveInlineHook() {
    if (m_pTrampoline) {
        // Restore original bytes
        void* targetFunc = (void*)&CreateWindowExW;
        WriteMemory(targetFunc, m_originalBytes, m_originalBytesSize);
        
        // Free trampoline
        FreeTrampoline();
        
        // Restore original function pointer
        m_pOriginalCreateWindowExW = &CreateWindowExW;
    }
}

bool WindowHook::CreateTrampoline(void* target, void* hook, void** trampoline) {
    // NOTE: This implementation assumes m_originalBytesSize contains complete instructions.
    // In production code, use a disassembler library to ensure instruction boundaries
    // are respected when copying bytes. Incomplete instructions can cause crashes.
    
    // Allocate memory for trampoline near target function
    // Initially allocate as read-write for setup
    *trampoline = VirtualAlloc(nullptr, 256, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    
    if (!*trampoline) {
        return false;
    }
    
    // Copy original bytes to trampoline
    memcpy(*trampoline, target, m_originalBytesSize);
    
    // Add jump back to original function (after the hook)
    BYTE* trampolineBytes = (BYTE*)*trampoline;
    BYTE* jumpBackPtr = trampolineBytes + m_originalBytesSize;
    
#ifdef _WIN64
    // Write absolute jump back
    jumpBackPtr[0] = 0xFF;
    jumpBackPtr[1] = 0x25;
    *(UINT32*)(jumpBackPtr + 2) = 0; // RIP+0
    *(UINT64*)(jumpBackPtr + 6) = (UINT64)((BYTE*)target + m_originalBytesSize);
#else
    // Write relative jump back
    jumpBackPtr[0] = 0xE9;
    *(INT32*)(jumpBackPtr + 1) = (INT32)((BYTE*)target + m_originalBytesSize - jumpBackPtr - 5);
#endif
    
    // Change protection to execute-read (W^X compliance)
    DWORD oldProtect;
    if (!VirtualProtect(*trampoline, 256, PAGE_EXECUTE_READ, &oldProtect)) {
        VirtualFree(*trampoline, 0, MEM_RELEASE);
        *trampoline = nullptr;
        return false;
    }
    
    return true;
}

void WindowHook::FreeTrampoline() {
    if (m_pTrampoline) {
        VirtualFree(m_pTrampoline, 0, MEM_RELEASE);
        m_pTrampoline = nullptr;
    }
}

bool WindowHook::WriteMemory(void* address, const void* data, size_t size) {
    DWORD oldProtect;
    
    // Change memory protection to allow writing
    if (!VirtualProtect(address, size, PAGE_EXECUTE_READWRITE, &oldProtect)) {
        return false;
    }
    
    // Write data
    memcpy(address, data, size);
    
    // Restore original protection
    DWORD dummy;
    VirtualProtect(address, size, oldProtect, &dummy);
    
    // Flush instruction cache
    FlushInstructionCache(GetCurrentProcess(), address, size);
    
    return true;
}

} // namespace SDK
