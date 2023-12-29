#pragma once

#include <cstddef>
#include <cstring>

namespace bs::detail::multiarch {
    inline const char* strrfind_char_default(const char* str, std::size_t count, const char ch) {
        if (count == 0) return nullptr;
        str += count;
        do {
            --str;
            if (*str == ch) return str;
        } while (--count != 0);
        return nullptr;
    }

}
