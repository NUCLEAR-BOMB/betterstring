#pragma once

#include <cstddef>

namespace bs::detail::multiarch {
    inline const char* strrfind_string_default(const char* haystack, std::size_t count, const char* needle, std::size_t needle_len) {
#if 0
        if (needle_len > count) return nullptr;
        if (needle_len == 0) return haystack + count;

        for (auto match_try = haystack + (count - needle_len);; --match_try) {
            if (match_try[0] == needle[0] && std::memcmp(match_try + 1, needle + 1, needle_len - 1) == 0) {
                return match_try;
            }
            if (match_try == haystack) return nullptr;
        }
#else
        if (needle_len > count) return nullptr;

        haystack += count - needle_len;
        count -= needle_len - 1;
        if (needle_len == 0) return haystack;

        do {
            if (haystack[0] == needle[0] && std::memcmp(haystack + 1, needle + 1, needle_len - 1) == 0) {
                return haystack;
            }
            --haystack;
        } while (--count != 0);
        return nullptr;
#endif
    }
}
