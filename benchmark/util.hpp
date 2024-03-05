#pragma once

#include <new>

template<class T>
T* aligned_new(const size_t num, const size_t alignment) {
    return static_cast<T*>(::operator new[](sizeof(T)* num, std::align_val_t{alignment}));
}
template<class T>
void aligned_delete(T* const ptr, const size_t num, const size_t alignment) {
    ::operator delete[](ptr, num, std::align_val_t{alignment});
}
