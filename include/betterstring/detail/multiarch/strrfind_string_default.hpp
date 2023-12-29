#pragma once

#include <cstddef>

namespace bs::detail::multiarch {
    inline const char* strrfind_string_default(const char* const haystack, const std::size_t count, const char* const needle, const std::size_t needle_len) {
        if (needle_len > count) return nullptr;
        if (needle_len == 0) return haystack + count;

        for (auto match_try = haystack + (count - needle_len);; --match_try) {
            if (match_try[0] == needle[0] && std::memcmp(match_try + 1, needle + 1, needle_len - 1) == 0) {
                return match_try;
            }
            if (match_try == haystack) return nullptr;
        }
    }
}
