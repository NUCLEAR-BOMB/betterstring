#pragma once

#include <cstddef>
#include <cstring>

namespace bs::detail::multiarch {
    inline const char* strrfind_char_default(const char* const str, const std::size_t count, const char ch) {
        for (std::size_t i = count; i > 0; --i) {
            if (str[i - 1] == ch) return str + i - 1;
        }
        return nullptr;
    }

}
