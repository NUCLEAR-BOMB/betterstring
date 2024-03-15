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

template<class Container>
struct enumerate {
    using container_const_iterator = typename Container::const_iterator;
    using container_const_reference = typename Container::const_reference;

    Container& container;

    struct value_type {
        container_const_reference value;
        std::size_t index;
    };

    struct const_iterator {
        container_const_iterator my_it;
        std::size_t index = 0;

        const_iterator(const container_const_iterator it)
            : my_it(it) {}

        const_iterator& operator++() {
            ++my_it;
            ++index;
            return *this;
        }
        bool operator!=(const_iterator other) {
            return my_it != other.my_it;
        }
        value_type operator*() {
            return value_type{*my_it, index};
        }
    };
    enumerate(Container& cont) : container(cont) {}

    const_iterator begin() const { return const_iterator{container.begin()}; }
    const_iterator end() const { return const_iterator{container.end()}; }
};
