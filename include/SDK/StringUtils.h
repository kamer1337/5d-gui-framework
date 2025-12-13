#pragma once

#include <string>
#include <cstdint>
#include <cwchar>

namespace SDK {

/**
 * Helper function to convert wstring to UTF-8 (replaces deprecated std::wstring_convert)
 * Works correctly with both UTF-16 (Windows) and UTF-32 (Linux) wchar_t representations
 */
inline std::string WStringToUTF8(const std::wstring& wstr) {
    if (wstr.empty()) {
        return std::string();
    }
    
    std::string result;
    result.reserve(wstr.size() * 4); // Reserve worst case (4 bytes per UTF-8 char)
    
    for (size_t i = 0; i < wstr.size(); ++i) {
        wchar_t wc = wstr[i];
        
        // Convert to unsigned to avoid sign extension issues on platforms with signed wchar_t
        uint32_t codepoint;
        if (sizeof(wchar_t) == 2) {
            // 16-bit wchar_t (Windows) - mask to 16 bits
            codepoint = static_cast<uint32_t>(static_cast<uint16_t>(wc));
        } else {
            // 32-bit wchar_t (Linux) - mask to 32 bits
            codepoint = static_cast<uint32_t>(static_cast<unsigned int>(wc));
        }
        
        // Handle UTF-16 surrogate pairs (Windows with 16-bit wchar_t)
        if (sizeof(wchar_t) == 2 && codepoint >= 0xD800 && codepoint <= 0xDBFF && i + 1 < wstr.size()) {
            // High surrogate - combine with low surrogate
            uint32_t low = static_cast<uint32_t>(static_cast<uint16_t>(wstr[i + 1]));
            if (low >= 0xDC00 && low <= 0xDFFF) {
                // Valid surrogate pair - decode to code point
                codepoint = 0x10000 + (((codepoint & 0x3FF) << 10) | (low & 0x3FF));
                ++i; // Skip the low surrogate
                
                // Encode as 4-byte UTF-8
                result.push_back(static_cast<char>(0xF0 | ((codepoint >> 18) & 0x07)));
                result.push_back(static_cast<char>(0x80 | ((codepoint >> 12) & 0x3F)));
                result.push_back(static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F)));
                result.push_back(static_cast<char>(0x80 | (codepoint & 0x3F)));
                continue;
            }
            // Invalid surrogate pair - skip the invalid high surrogate
            continue;
        }
        
        // Skip invalid low surrogates without preceding high surrogate
        if (sizeof(wchar_t) == 2 && codepoint >= 0xDC00 && codepoint <= 0xDFFF) {
            continue; // Invalid - skip
        }
        
        // Skip invalid surrogates on 32-bit wchar_t platforms
        if (codepoint >= 0xD800 && codepoint <= 0xDFFF) {
            continue; // Surrogates are invalid in UTF-32
        }
        
        if (codepoint < 0x80) {
            // ASCII range (0x00-0x7F)
            result.push_back(static_cast<char>(codepoint));
        } else if (codepoint < 0x800) {
            // 2-byte UTF-8 (0x80-0x7FF)
            result.push_back(static_cast<char>(0xC0 | ((codepoint >> 6) & 0x1F)));
            result.push_back(static_cast<char>(0x80 | (codepoint & 0x3F)));
        } else if (codepoint < 0x10000) {
            // 3-byte UTF-8 (0x800-0xFFFF)
            result.push_back(static_cast<char>(0xE0 | ((codepoint >> 12) & 0x0F)));
            result.push_back(static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F)));
            result.push_back(static_cast<char>(0x80 | (codepoint & 0x3F)));
        } else if (codepoint < 0x110000) {
            // 4-byte UTF-8 (0x10000-0x10FFFF) - for 32-bit wchar_t
            result.push_back(static_cast<char>(0xF0 | ((codepoint >> 18) & 0x07)));
            result.push_back(static_cast<char>(0x80 | ((codepoint >> 12) & 0x3F)));
            result.push_back(static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F)));
            result.push_back(static_cast<char>(0x80 | (codepoint & 0x3F)));
        }
        // Invalid code points (>= 0x110000) are silently skipped
    }
    
    return result;
}

} // namespace SDK
