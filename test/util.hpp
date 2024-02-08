#pragma once
#include <betterstring/detail/preprocessor.hpp>

#if BS_OS_WINDOWS
    #include <Windows.h>
#endif

#if BS_OS_WINDOWS
void* page_alloc() {
    void* addr = VirtualAlloc(NULL, 4096, MEM_COMMIT, PAGE_READWRITE);
    if (addr == nullptr) {
        BS_VERIFY(false, "Failed to allocate");
    }
    return addr;
}
void page_free(void* addr) {
    if (!VirtualFree(addr, 0, MEM_RELEASE)) {
        BS_VERIFY(false, "Failed to free");
    }
}
#else
void* page_alloc() {
    void* addr = malloc(4096);
    if (addr == nullptr) {
        BS_VERIFY(false, "Failed to allocate");
    }
    return addr;
}
void page_free(void* addr) {
    free(addr);
}
#endif

