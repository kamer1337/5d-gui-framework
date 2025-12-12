#pragma once

#include "Platform.h"
#include <cstdint>
#include <cstddef>

namespace SDK {

/**
 * InstructionDecoder - Lightweight x86/x64 instruction length decoder
 * Used for safe inline hooking to ensure we don't split instructions
 */
class InstructionDecoder {
public:
    /**
     * Get the length of an instruction at the given address
     * @param code Pointer to the instruction bytes
     * @return Length of the instruction in bytes, or 0 if invalid
     */
    static size_t GetInstructionLength(const void* code);
    
    /**
     * Calculate the minimum number of bytes needed to cover at least minBytes
     * without splitting any instruction
     * @param code Pointer to the start of code
     * @param minBytes Minimum number of bytes required
     * @param maxBytes Maximum bytes to scan (safety limit)
     * @return Actual number of bytes that safely covers complete instructions
     */
    static size_t GetSafeHookLength(const void* code, size_t minBytes, size_t maxBytes = 32);

private:
    // Internal instruction length calculation
    static size_t CalculateLengthInternal(const uint8_t* code);
    
    // Check if byte is a valid opcode prefix
    static bool IsPrefix(uint8_t byte);
    
    // Get ModR/M displacement size
    static size_t GetModRMSize(uint8_t modrm, uint8_t addressSize);
};

} // namespace SDK
