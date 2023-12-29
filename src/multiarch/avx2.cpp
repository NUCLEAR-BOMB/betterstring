#include <betterstring/detail/preprocessor.hpp>
#include <betterstring/detail/multiarch/strrfind_string_avx2.hpp>

namespace bs::detail {
    // https://stackoverflow.com/a/25884902
    // https://www.reddit.com/r/cpp/comments/weegza/comment/iinrhei/?utm_source=share&utm_medium=web3x&utm_name=web3xcss&utm_term=1&utm_content=share_button

    BS_FLATTEN
    const char* strrfind_string_avx2(const char* haystack, std::size_t count, const char* needle, std::size_t needle_len) {
        return bs::detail::multiarch::strrfind_string_avx2(haystack, count, needle, needle_len);
    }
}
