#pragma once

#include <string>
#include <iterator>

namespace bs
{

template<class T>
struct string_traits {
    using char_traits = std::char_traits<T>;
    using value_type = T;
};

template<class Traits = string_traits<char>>
class string_view {
public:
    using traits_type = Traits;
    using value_type = typename Traits::value_type;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using reference = value_type&;
    using const_reference = const value_type&;
    using const_iterator = const value_type*;
    using iterator = const_iterator;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    using reverse_iterator = const_reverse_iterator;
    using size_type = typename Traits::size_type;
    using difference_type = typename Traits::difference_type;

private:

};

}
