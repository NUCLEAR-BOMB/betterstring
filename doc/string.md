
# Table of contents
- [Type Aliases](#type-aliases)
- [Template Parameters](#template-parameters)
- [Member Types](#member-types)
- [Member functions](#member-functions)
- [Non-member functions](#non-member-functions)
- [Literals](#literals)

```cpp
template<class Traits>
class stringt;
```
The class that stores a sequence of characters. To manipulate the characters inside `bs::stringt`, the template parameter **`Traits`** is used, which defines how `bs::stringt` should work with them.

The `bs::stringt` is successor to `std::basic_string`: it provides almost identical interface to `std::basic_string` with additional methods with more fast implementation.

> [!IMPORTANT]
> The **`bs::stringt`** does not have a null terminator (value `value_type()`) on the end of the character sequence unlike **`std::basic_string`**, i.e. accessing to `*(str.data() + str.size())` is **undefined behavior**.

> [!NOTE]
> The **`std::char_traits`** *does not* work with `bs::stringt`. `bs::stringt` uses special static member functions for efficient and fast operations, but on the other hand **`bs::char_traits`** is designed to work with **`std::basic_string`**.

## Type Aliases
The `<betterstring/string.hpp>` header provides these type aliases for `bs::stringt`.

| Type              | Definition                           |
| ----------------- | ------------------------------------ |
| **`bs::string`**  | `bs::stringt<bs::char_traits<char>>` |


## Template Parameters
**`Traits`** - Type that specifies how `bs::stringt` should work with characters. `bs::stringt` derives character type from it.

## Member Types
| Member type           | Definition                   |
| --------------------- | ---------------------------- |
| **`value_type`**      | `typename Traits::char_type` |
| **`size_type`**       | `typename Traits::size_type` |
| **`allocator_type`**  | `std::allocator<value_type>` |
| **`pointer`**         | `value_type*`                |
| **`const_pointer`**   | `const value_type*`          |
| **`reference`**       | `value_type&`                |
| **`const_reference`** | `const value_type&`          |
| **`iterator`**        | `value_type*`                |
| **`const_iterator`**  | `const value_type*`          |
| **`traits_type`**     | `Traits`                     |

# Member functions
- [Constructor](#constructor)
- [Named constructors](#named-constructors)
- [Destructor](#destructor)
- [**`operator=`**](#operator)
- [**`reserve`**](#reserve)
- [**`reserve_add`**](#reserve_add)
- [**`reserve_exact`**](#reserve_exact)
- [**`reserve_exact_add`**](#reserve_exact_add)
- [**`clear`**](#clear)
- [**`push_back`**](#push_back)
- [**`pop_back`**](#pop_back)
- [**`append`**](#append)
- [**`substr`**](#substr)
- [**`contains`**](#contains)
- [**`starts_with`**](#starts_with)
- [**`ends_with`**](#ends_with)
- [**`data`**](#data)
- [**`size`**](#size)
- [**`capacity`**](#capacity)
- [**`begin`**](#begin)
- [**`end`**](#end)
- [**`operator[]`**](#operator-1)
- [**`at`**](#at)
- [**`front`**](#front)
- [**`back`**](#back)
- [**`at_front`**](#at_front)
- [**`at_back`**](#at_back)
- [**`operator bs::string_viewt<traits_type>`**](#operator-bsstring_viewttraits_type)

## Constructor

```cpp
constexpr stringt() noexcept;
```
Default constructor. Creates empty string, i.e. `str.size() == 0`.

```cpp
explicit constexpr stringt(const_pointer str, size_type str_len);
```
Creates new string with size `str_len` and a copy of the elements of range [`str`, `str + str_len`).

```cpp
constexpr stringt(const stringt& other);
```
Copy constructor. Creates new string with size `other.size()` and a copy of the contents of `other`.

```cpp
constexpr stringt(stringt&& other) noexcept;
```
Move constructor. Creates new string from `other` without copying the elements.

```cpp
template<class Begin, class End>
explicit constexpr stringt(Begin first, End last);
```
Constructs string with the contents of the range [`first`, `last`). \
This constructor is enabled only when the type **`Begin`** is an **input iterator**,
and the type **`End`** is not convertible to `size_type`.

```cpp
explicit constexpr stringt(const bs::string_viewt<traits_type> str_view);
```
Constructs new string from the elements of the `str_view`, i.e. range [`str_view.data()`, `str_view.data() + str_view.size()`).

## Named constructors
The `bs::stringt` class also supports **named constructors** (static member function that returns an instance of the class).

```cpp
static constexpr stringt filled(value_type ch, size_type count);
```
Creates new string filled with `ch` and the size of `count`.

```cpp
static constexpr stringt with_capacity(size_type cap)
```
Creates new string with capacity `cap`. The capacity of created string does not required to be exactly `cap`.

```cpp
static constexpr stringt transfer_ownership(pointer ptr, size_type cap) noexcept;
```
Transferes ownership to new string using the provided `ptr` and `cap` without any copying.
After construction the size of returned string is `0` (`str.size() == 0`).
The memory pointed to by `ptr` must be not deallocated outside the string, `bs::stringt` will automatically deallocate it in its destructor.

```cpp
static constexpr stringt transfer_ownership(pointer ptr, size_type size, size_type cap) noexcept;
```
Transferes ownership to new string with size of `size` using the provided `ptr` and `cap` without any copying.
After construction the size of returned string is equal to `size`.
The memory pointed to by `ptr` must be not deallocated outside the string, `bs::stringt` will automatically deallocate it in its destructor.

## Destructor

```cpp
(constexpr C++20) ~stringt() noexcept;
```
Deallocates underlying memory buffer. The destructor of the elements are called and the used memory is deallocated. \
`constexpr` when current standard is `C++20`.

## operator=

```cpp
constexpr stringt& operator=(bs::string_viewt<traits_type> str_view);
```
Assigns characters from range [`str_view.data()`, `str_view.data() + str_view.size()`).
After assignment, the string contains elements from the `str_view` view and has the size `str_view.size()`.

```cpp
constexpr stringt& operator=(const stringt& other);
```
Copy assignment operator.
Assigns content from `other`.
After assigment, the string contains elements from the `other`.

The address of the `other` can be equal to **`this`**.

```cpp
constexpr stringt& operator=(stringt&& other) noexcept;
```
Move assignment operator.
Moves characters from `other` to current string.

The address of the `other` can be equal to **`this`**.

## reserve
```cpp
constexpr void reserve(size_type new_req_cap);
```
Changes the capacity of the string.

- If `new_req_cap` is greater than the current capacity, new storage is allocated, and capacity is made at least `new_req_cap`.
- If `new_req_cap` is less than or equal to the current capacity, there is not effect.

## reserve_add
```cpp
constexpr void reserve_add(size_type additional_cap);
```
Reserves the capacity of the string to be at least `size() + additional_cap`.
Same as `reserve(size() + additional_cap)`.

## reserve_exact
```cpp
constexpr void reserve_exact(size_type new_cap);
```
Changes the capacity of the string to be exactly `new_cap`.

> [!CAUTION]
> Consider performance degradation when using this method. Recommended to use `reserve` method instead.

- If `new_cap` is less than or equal to the current capacity, there is not effect.
- If `new_cap` is greater than current capacity, the new capacity will be exactly equal to `new_cap`.

## reserve_exact_add
```cpp
constexpr void reserve_exact_add(size_type additional_cap);
```
Reserves the capacity of the string to be exactly `size() + additional_cap`.
Same as `reserve_exact(size() + additional_cap)`.

> [!CAUTION]
> Consider performance degradation when using this method. Recommended to use `reserve_add` method instead.

## clear
```cpp
constexpr void clear() noexcept;
```
Resizes the string to `0`.
After this operation the size of the string will be equal to `0`.

## push_back
```cpp
constexpr void push_back(value_type ch);
```
Appends `ch` to the end of the string.

## pop_back
```cpp
constexpr void pop_back() noexcept;
```
Removes last character from the string.
**Undefined behavior** when `size()` is equal to `0`.

## append
```cpp
constexpr void append(const_pointer str, size_type str_len);
```
Appends the range of elements [`str`, `str + str_len`) to the end of the string.

```cpp
constexpr void append(bs::string_viewt<traits_type> str_view);
```
Appends the range [`str_view.data()`, `str_view.data() + str_view.size()`) to the end of the string.
Calls `append(str_view.data(), str_view.size())`.

```cpp
template<class Begin, class End>
constexpr void append(Begin first, End last);
```
Appends the range [`first`, `last`) to the end of the string.

This method is enabled only when the type `Begin` is **random access iterator**,
and the type `End` is not convertible to `size_type`. 

## substr
```cpp
constexpr bs::string_viewt<traits_type> substr(size_type position) const noexcept;
```
Returns a substring [`position`, `size()`).
Equivalent to `bs::string_viewt<traits_type>{data() + position, size() - position}`.
**Undefined behavior** when `position` is greater than size of the string.

```cpp
constexpr bs::string_viewt<traits_type> substr(size_type position, size_type count) const noexcept;
```
Returns a substring [`position`, `position + count`).
Equivalent to `bs::string_viewt<traits_type>{data() + position, count}`.
**Undefined behavior** when `position` is greater than size of the string
**or** when `count - position` is greater than size of the string.

## contains
```cpp
constexpr bool contains(value_type ch) const noexcept;
```
Checks if current string contains the character `ch`.
`false` when string is empty.

```cpp
constexpr bool contains(bs::string_viewt<traits_type> str) const noexcept;
```
Checks if current string contains the substring `str`.
`false` when current string length is less than length of the string `str`.

## starts_with
```cpp
constexpr bool starts_with(value_type ch) const noexcept;
```
Checks if current string starts with character `ch`.
`false` when the string is empty; 

```cpp
constexpr bool starts_with(bs::string_viewt<traits_type> str) const noexcept;
```
Checks if current string starts with the substring `str`.
`false` when current string length is less than length of the string `str`.

## ends_with
```cpp
constexpr bool ends_with(value_type ch) const noexcept;
```
Checks if current string ends with the character `ch`.
`false` when the string is empty.

```cpp
constexpr bool ends_with(bs::string_viewt<traits_type> str) const noexcept;
```
Checks if current string ends with the substring `str`.
`false` when current string length is less than length of the string `str`.

## data
```cpp
constexpr pointer data() noexcept;
```
Returns a pointer to beginning of the string.
Only the [`data()`, `data() + size()`) range is valid.

```cpp
constexpr const_pointer data() const noexcept;
```
Returns a pointer to immutable beginning of the string.
Only the [`data()`, `data() + size()`) range is valid.

## size
```cpp
constexpr size_type size() const noexcept;
```
Returns size of the string.
Only the [`data()`, `data() + size()`) range is valid.

## capacity
```cpp
constexpr size_type capacity() const noexcept;
```
Returns capacity of the string.
Capacity is always at least the size of the string.

## begin
```cpp
constexpr iterator begin() noexcept;
```
Returns an iterator to beginning of the string.
Currenly equivalent to `data()`. 

```cpp
constexpr const_iterator begin() const noexcept;
```
Returns an iterator to immutable beginning of the string.
Currenly equivalent to `data()`. 

## end
```cpp
constexpr iterator end() noexcept;
```
Returns an iterator to past-the-last element.
Currenly equivalent to `data() + size()`. 

```cpp
constexpr const_iterator end() const noexcept;
```
Returns an iterator to immutable past-the-last element.
Currenly equivalent to `data() + size()`. 

## operator[]
```cpp
template<class Int>
constexpr const_reference operator[](Int index) const noexcept;
```
Returns a reference to the character at position `index`.
- If `index` is positive or zero, a reference is calculated using `data()[index]`.
- If `index` is negative, a reference is calculated using `data()[index + size()]`.

This operator is enabled only then the `Int` type is **integer type**.

If `index` is greater or equal to the size of the string, the **behavior is undefined**.

```cpp
template<class Int>
constexpr reference operator[](Int index) noexcept;
```
Returns a reference to the character at position `index`.
- If `index` is positive or zero, a reference is calculated using `data()[index]`.
- If `index` is negative, a reference is calculated using `data()[index + size()]`.

This operator is enabled only then the `Int` type is **integer type**.

If `index` is greater or equal to the size of the string, the **behavior is undefined**.

## at
```cpp
template<class Int>
constexpr std::optional<value_type> at(Int index) const noexcept;
```
Returns an **`std::optional`** character at position `index`.
- If `index` is positive or zero and it is in valid range,
  an optional value is calculated using `data()[index]`
- If `index` is negative and it is in valid range,
  a reference is calculated using `data()[index + size()]`.
- If `index` is outside of the valid range, returns an empty **`std::optional`**.

This method is enabled only when the type `Int` is **integer type**.

## front
```cpp
constexpr const_reference front() const noexcept;
```
Returns a const reference to the first character in the string.

**Undefined behavior** when the string is empty.

```cpp
constexpr reference front() noexcept;
```
Returns a reference to the first character in the string.

**Undefined behavior** when the string is empty.

## back
```cpp
constexpr const_reference back() const noexcept;
```
Returns a const reference to the last character in the string.

**Undefined behavior** when the string is empty.

```cpp
constexpr reference back() noexcept;
```
Returns a reference to the last character in the string.

**Undefined behavior** when the string is empty.

## at_front
```cpp
constexpr std::optional<value_type> at_front() const noexcept;
```
Returns an `std::optional` character to the first character in the string if it is not empty; otherwise, returns an empty optional.

## at_back
```cpp
constexpr std::optional<value_type> at_back() const noexcept;
```
Returns an `std::optional` character to the alst character in the string if it is not empty; otherwise, returns an empty optional.

## operator `bs::string_viewt<traits_type>`
```cpp
constexpr operator bs::string_viewt<traits_type>() const noexcept;
```
Returns a `bs::string_viewt<traits_type>` constructed from current string.
Equivalent to `bs::string_viewt<traits_type>{data(), size()}`.

# Non-member functions
- [**`operator==`**](#operator-2)
- [**`operator!=`**](#operator-3)

## operator==
```cpp
template<class Tr>
constexpr bool operator==(const stringt<Tr>& left, const stringt<Tr>& right) noexcept;
```
Checks if two strings are equal.

If ranges [`left.data()`, `left.data() + left.size()`) and [`right.data()`, `right.data() + right.size()`) are equal, returns `true`; otherwise `false`. 

```cpp
template<class Tr, std::size_t N>
constexpr bool operator==(const stringt<Tr>& left, const typename Tr::char_type(&right)[N]) noexcept;
```
Checks if two strings are equal.
**Does not** compare null terminator at the end of the `right` string.

If ranges [`left.data()`, `left.data() + left.size()`) and [`right`, `right + N - 1`) are equal, returns `true`; otherwise `false`. 

```cpp
template<class Tr, std::size_t N>
constexpr bool operator==(const typename Tr::char_type(&left)[N], const stringt<Tr>& right) noexcept;
```
Checks if two strings are equal.
**Does not** compare null terminator at the end of the `left` string.

If ranges [`left`, `left + N - 1`) and [`right.data()`, `right.data() + right.size()`) are equal, returns `true`; otherwise `false`. 

## operator!=
```cpp
template<class Tr>
constexpr bool operator!=(const stringt<Tr>& left, const stringt<Tr>& right) noexcept;
```
Checks if two string are **not** equal.
Equivalent to `!(left == right)`.

```cpp
template<class Tr, std::size_t N>
constexpr bool operator!=(const stringt<Tr>& left, const typename Tr::char_type(&right)[N]) noexcept
```
Checks if two string are **not** equal.
Equivalent to `!(left == right)`.

```cpp
template<class Tr, std::size_t N>
constexpr bool operator!=(const typename Tr::char_type(&left)[N], const stringt<Tr>& right) noexcept
```
Checks if two string are **not** equal.
Equivalent to `!(left == right)`.

# Literals
This operator is declared in the namespace `bs::literals`, where `literals` is `inline namespace`.

Access to these operators can be gained with either
- `using namespace bs::literals`, or
- `using namespace bs`.

```cpp
(constexpr C++20) bs::string operator ""_s(const char* str, std::size_t length);
```
Returns `bs::string{str, length}`.

