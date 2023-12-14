#include <cstddef>
#include <cstring>

#include "utils.hpp"

namespace {
    inline const char* strrfind_default_impl(const char* const haystack, const std::size_t count, const char* const needle, const std::size_t needle_len) {
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


namespace {
    const char* strrfind_resolver(const char* const haystack, const std::size_t count, const char* const needle, const std::size_t needle_len);
}

namespace bs::detail {
    extern const char* strrfind_string_avx2(const char* const, const std::size_t, const char* const, const std::size_t);

    const char*(*strrfind_impl)(const char*, std::size_t, const char*, std::size_t) = &strrfind_resolver;
}

namespace {
    const char* strrfind_resolver(const char* const haystack, const std::size_t count, const char* const needle, const std::size_t needle_len) {
        using namespace bs::detail;
        if (is_cpu_supports_avx2()) {
            //strrfind_impl = &strrfind_default_impl;
            strrfind_impl = &strrfind_string_avx2;
        } else {
            strrfind_impl = &strrfind_default_impl;
        }
        return bs::detail::strrfind_impl(haystack, count, needle, needle_len);
    }
}
