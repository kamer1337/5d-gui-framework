#include "../../include/SDK/InstructionDecoder.h"
#include <cstring>

namespace SDK {

bool InstructionDecoder::IsPrefix(uint8_t byte) {
    // Check for common x86/x64 prefixes
    switch (byte) {
        // Lock and repeat prefixes
        case 0xF0: case 0xF2: case 0xF3:
        // Segment override prefixes
        case 0x26: case 0x2E: case 0x36: case 0x3E:
        case 0x64: case 0x65:
        // Operand-size override
        case 0x66:
        // Address-size override
        case 0x67:
            return true;
#ifdef _WIN64
        // REX prefixes (x64 only)
        case 0x40: case 0x41: case 0x42: case 0x43:
        case 0x44: case 0x45: case 0x46: case 0x47:
        case 0x48: case 0x49: case 0x4A: case 0x4B:
        case 0x4C: case 0x4D: case 0x4E: case 0x4F:
            return true;
#endif
        default:
            return false;
    }
}

size_t InstructionDecoder::GetModRMSize(uint8_t modrm, uint8_t addressSize) {
    uint8_t mod = (modrm >> 6) & 0x03;
    uint8_t rm = modrm & 0x07;
    
    size_t size = 1; // ModR/M byte itself
    
    if (addressSize == 16) {
        // 16-bit addressing
        if (mod == 0 && rm == 6) {
            size += 2; // Direct addressing
        } else if (mod == 1) {
            size += 1; // 8-bit displacement
        } else if (mod == 2) {
            size += 2; // 16-bit displacement
        }
    } else {
        // 32-bit or 64-bit addressing
        if (mod == 0 && rm == 5) {
            size += 4; // RIP-relative or direct addressing
        } else if (mod == 1) {
            size += 1; // 8-bit displacement
        } else if (mod == 2) {
            size += 4; // 32-bit displacement
        }
        
        // Check for SIB byte
        if (mod != 3 && rm == 4) {
            size += 1; // SIB byte
            // Note: SIB can add more displacement, handled by mod above
        }
    }
    
    return size;
}

size_t InstructionDecoder::CalculateLengthInternal(const uint8_t* code) {
    if (!code) {
        return 0;
    }
    
    size_t length = 0;
    bool hasREX = false;
    uint8_t operandSize = 32;
    uint8_t addressSize = 32;
    
#ifdef _WIN64
    operandSize = 32; // Default in x64 is still 32-bit
    addressSize = 64; // But addresses are 64-bit
#endif
    
    // Skip prefixes
    while (IsPrefix(code[length])) {
        if (code[length] == 0x66) {
            operandSize = 16;
        }
        if (code[length] == 0x67) {
            addressSize = (addressSize == 64) ? 32 : 16;
        }
#ifdef _WIN64
        if (code[length] >= 0x40 && code[length] <= 0x4F) {
            hasREX = true;
            if (code[length] & 0x08) { // REX.W bit
                operandSize = 64;
            }
        }
#endif
        length++;
        if (length > 15) return 0; // Too many prefixes, invalid
    }
    
    uint8_t opcode = code[length++];
    
    // Two-byte opcodes
    if (opcode == 0x0F) {
        if (length >= 15) return 0;
        opcode = code[length++];
        
        // Three-byte opcodes
        if (opcode == 0x38 || opcode == 0x3A) {
            if (length >= 15) return 0;
            length++; // Skip third opcode byte
        }
    }
    
    // Determine if instruction has ModR/M byte
    bool hasModRM = false;
    bool hasImmediate = false;
    size_t immediateSize = 0;
    
    // Simplified opcode analysis (covers most common cases)
    switch (opcode) {
        // Instructions without ModR/M
        case 0x50: case 0x51: case 0x52: case 0x53: // PUSH reg
        case 0x54: case 0x55: case 0x56: case 0x57:
        case 0x58: case 0x59: case 0x5A: case 0x5B: // POP reg
        case 0x5C: case 0x5D: case 0x5E: case 0x5F:
        case 0x90: // NOP
        case 0xC3: // RET
        case 0xC9: // LEAVE
        case 0xF4: // HLT
        case 0xCC: // INT 3
            break;
            
        // JMP rel8
        case 0xEB:
            immediateSize = 1;
            break;
            
        // JMP rel32, CALL rel32
        case 0xE8: case 0xE9:
            immediateSize = 4;
            break;
            
        // Conditional jumps rel8
        case 0x70: case 0x71: case 0x72: case 0x73:
        case 0x74: case 0x75: case 0x76: case 0x77:
        case 0x78: case 0x79: case 0x7A: case 0x7B:
        case 0x7C: case 0x7D: case 0x7E: case 0x7F:
            immediateSize = 1;
            break;
            
        // MOV immediate to register
        case 0xB0: case 0xB1: case 0xB2: case 0xB3:
        case 0xB4: case 0xB5: case 0xB6: case 0xB7:
            immediateSize = 1;
            break;
            
        case 0xB8: case 0xB9: case 0xBA: case 0xBB:
        case 0xBC: case 0xBD: case 0xBE: case 0xBF:
            // MOV immediate to register
            // In x64, still uses 32-bit immediate (sign-extended to 64-bit)
            immediateSize = 4;
            break;
            
        // Most other opcodes use ModR/M
        default:
            hasModRM = true;
            
            // Check for immediate operands in common instructions
            if (opcode >= 0x80 && opcode <= 0x83) {
                // Immediate group
                hasImmediate = true;
                immediateSize = (opcode == 0x81) ? (operandSize / 8) : 1;
            }
            break;
    }
    
    // Process ModR/M if present
    if (hasModRM) {
        if (length >= 15) return 0;
        uint8_t modrm = code[length];
        size_t modrmSize = GetModRMSize(modrm, addressSize);
        length += modrmSize;
    }
    
    // Add immediate
    length += immediateSize;
    
    if (length > 15) return 0; // Invalid instruction
    
    return length;
}

size_t InstructionDecoder::GetInstructionLength(const void* code) {
    if (!code) {
        return 0;
    }
    
    return CalculateLengthInternal(static_cast<const uint8_t*>(code));
}

size_t InstructionDecoder::GetSafeHookLength(const void* code, size_t minBytes, size_t maxBytes) {
    if (!code || minBytes == 0) {
        return 0;
    }
    
    const uint8_t* bytes = static_cast<const uint8_t*>(code);
    size_t totalLength = 0;
    
    while (totalLength < minBytes && totalLength < maxBytes) {
        size_t instrLength = GetInstructionLength(bytes + totalLength);
        if (instrLength == 0) {
            // Invalid instruction encountered
            return 0;
        }
        totalLength += instrLength;
    }
    
    return totalLength;
}

} // namespace SDK
