#include <cinttypes>
#include <cassert>

#include <betterstring/functions.hpp>

#if BS_OS_WINDOWS
    #include <Windows.h>
#endif

#if BS_OS_WINDOWS
inline void* page_alloc() {
    void* addr = VirtualAlloc(NULL, 4096, MEM_COMMIT, PAGE_READWRITE);
    if (addr == nullptr) {
        BS_VERIFY(false, "Failed to allocate");
    }
    return addr;
}
inline void page_free(void* addr) {
    if (!VirtualFree(addr, 0, MEM_RELEASE)) {
        BS_VERIFY(false, "Failed to free");
    }
}
#else
inline void* page_alloc() {
    void* addr = malloc(4096);
    if (addr == nullptr) {
        BS_VERIFY(false, "Failed to allocate");
    }
    return addr;
}
inline void page_free(void* addr) {
    free(addr);
}
#endif

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* Data, size_t Size) {
    if (Size < 4) { return -1; }

    const size_t needle_size = *reinterpret_cast<const uint16_t*>(Data);
    const int8_t needle_page_offset = *reinterpret_cast<const int8_t*>(Data + 2);
    const int8_t haystack_page_offset = *reinterpret_cast<const int8_t*>(Data + 3);

    if (needle_size > Size) { return -1; }

    const auto needle_src = reinterpret_cast<const char*>(Data) + 4;

    const size_t haystack_size = Size - 4 - needle_size;
    const auto haystack_src = reinterpret_cast<const char*>(Data) + 4 + needle_size;

    if (needle_size > 32) { return -1; }
    if (haystack_size > 96 + needle_size - 1) { return -1; }

    char* const haystack_base = (char*)page_alloc();
    char* const needle_base = (char*)page_alloc();

    char* const haystack = haystack_base + (haystack_page_offset < 0 ? 4096 + (haystack_page_offset + 1) - haystack_size : haystack_page_offset);
    char* const needle = needle_base + (needle_page_offset < 0 ? 4096 + (needle_page_offset + 1) - needle_size : needle_page_offset);

    std::memcpy(haystack, haystack_src, haystack_size);
    std::memcpy(needle, needle_src, needle_size);

    const auto result = bs::strfind(haystack, haystack_size, needle, needle_size);

    if (result == nullptr) {
        for (std::size_t i = 0; i + needle_size < haystack_size; ++i) {
            if (haystack[i] == needle[0] && std::memcmp(haystack, needle, needle_size) == 0) {
                std::abort();
            }
        }
    } else {
        if (result > haystack + haystack_size - needle_size) {
            std::abort();
        }
        for (auto it = haystack; it != result; ++it) {
            if (*it == needle[0] && std::memcmp(it, needle, needle_size) == 0) {
                std::abort();
            }
        }
    }

    page_free(haystack_base);
    page_free(needle_base);

    return 0;
}
