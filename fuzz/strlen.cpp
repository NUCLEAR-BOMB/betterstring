#include <cstdint>
#include <cstring>

#include <betterstring/functions.hpp>

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* Data, size_t Size) {
    char* const str = new char[Size + 1];
    std::memcpy(str, Data, Size);
    str[Size] = '\0';

    const std::size_t size = bs::strlen(str);
    const std::size_t true_size = std::strlen(str);

    if (size != true_size) {
        std::abort();
    }

    delete[] str;

    return 0;
}
