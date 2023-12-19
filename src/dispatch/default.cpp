#include <cstddef>
#include <cstring>

#include <betterstring/detail/dispatch.hpp>

namespace {
    const char* strrfind_str_default(const char* const haystack, const std::size_t count, const char* const needle, const std::size_t needle_len) {
        if (needle_len > count) return nullptr;
        if (needle_len == 0) return haystack + count;

        for (auto match_try = haystack + (count - needle_len);; --match_try) {
            if (match_try[0] == needle[0] && std::memcmp(match_try + 1, needle + 1, needle_len - 1) == 0) {
                return match_try;
            }
            if (match_try == haystack) return nullptr;
        }
    }

    const char* strrfind_ch_default(const char* const str, const std::size_t count, const char ch) {
        for (std::size_t i = count; i > 0; --i) {
            if (str[i - 1] == ch) return str + i - 1;
        }
        return nullptr;
    }
}

namespace {
    const char* strrfind_str_resolver(const char* const haystack, const std::size_t count, const char* const needle, const std::size_t needle_len);
    const char* strrfind_ch_resolver(const char* const str, const std::size_t count, const char ch);

    const char*(*strrfind_ch_impl)(const char*, std::size_t, char) = &strrfind_ch_resolver;
}

namespace bs::detail {
    extern const char* strrfind_string_avx2(const char* const, const std::size_t, const char* const, const std::size_t);
    extern const char* strrfind_character_avx2(const char*, std::size_t, const char);

    const char*(*strrfind_str_impl)(const char*, std::size_t, const char*, std::size_t) = &strrfind_str_resolver;

    const char* strrfind_ch_runtime(const char* haystack, std::size_t count, char ch) {
        return strrfind_ch_impl(haystack, count, ch);
    }
}

extern "C" const char* __cdecl betterstring_impl_strrfind_ch_avx2(const char*, std::size_t, char);

namespace {
    const char* strrfind_str_resolver(const char* const haystack, const std::size_t count, const char* const needle, const std::size_t needle_len) {
        using namespace bs::detail;
        if (is_cpu_supports_avx2()) {
            //strrfind_impl = &strrfind_str_default;
            strrfind_str_impl = &strrfind_string_avx2;
        } else {
            strrfind_str_impl = &strrfind_str_default;
        }
        return strrfind_str_impl(haystack, count, needle, needle_len);
    }
    const char* strrfind_ch_resolver(const char* const str, const std::size_t count, const char ch) {
        using namespace bs::detail;
        if (is_cpu_supports_avx2()) {
            //strrfind_ch_impl = &strrfind_ch_default;
            //strrfind_ch_impl = &strrfind_character_avx2;
            strrfind_ch_impl = &betterstring_impl_strrfind_ch_avx2;
        } else {
            strrfind_ch_impl = &strrfind_ch_default;
        }
        return strrfind_ch_impl(str, count, ch);
    }
}
