`<betterstring/string_view.hpp>`

- [Type Aliases](#type-aliases)
- [Template Parameters](#template-parameters)
- [Member Types](#member-types)
- [Member Functions](#member-functions)
- [Literals](#literals)

# `bs::string_viewt`

```cpp
template<class Traits>
class string_viewt;
```
The class that stores a view over the sequence of characters. \
`bs::string_viewt` uses `Traits` template parameter to determine character type, efficient methods implementation.

The `bs::string_viewt` is successor to `std::basic_view_string`: it provides almost identical interface to `std::basic_view_string` with additional methods with more fast implementation.

> [!NOTE]
> The **`std::char_traits`** *does not* work with `bs::string_viewt`. `bs::string_viewt` uses special static member functions for efficient and fast operations, but on the other hand **`bs::char_traits`** is backward compatible to work with **`std::basic_view_string`**.

# Type Aliases
The `<betterstring/string_view.hpp>` header provides these type aliases for `bs::string_viewt`.

| Type                     | Definition                                    |
| ------------------------ | --------------------------------------------- |
| **`bs::string_view`**    | `bs::string_viewt<bs::char_traits<char>>`     |
| **`bs::wstring_view`**   | `bs::string_viewt<bs::char_traits<wchar_t>>`  |
| **`bs::u16string_view`** | `bs::string_viewt<bs::char_traits<char16_t>>` |
| **`bs::u32string_view`** | `bs::string_viewt<bs::char_traits<char32_t>>` |
| **`bs::u8string_view`**  | `bs::string_viewt<bs::char_traits<char8_t>>`  |

# Template Parameters
**`Traits`** - Type that specifies how `bs::string_viewt` should work with characters. `bs::string_viewt` derives character type from it.

# Member Types
| Member type                  | Definition                              |
| ---------------------------- | --------------------------------------- |
| **`traits_type`**            | `Traits`                                |
| **`value_type`**             | `typename Traits::char_type`            |
| **`pointer`**                | `value_type*`                           |
| **`const_pointer`**          | `const value_type*`                     |
| **`reference`**              | `value_type&`                           |
| **`const_reference`**        | `const value_type&`                     |
| **`const_iterator`**         | `const value_type*`                     |
| **`iterator`**               | `const_iterator`                        |
| **`const_reverse_iterator`** | `std::reverse_iterator<const_iterator>` |
| **`reverse_iterator`**       | `const_reverse_iterator`                |
| **`size_type`**              | `typename Traits::size_type`            |
| **`difference_type`**        | `typename Traits::difference_type`      |

# Member Functions
- [Constructor](#constructor)
- [**`operator=`**](#operator)
- [**`begin`**](#begin)
- [**`end`**](#end)
- [**`cbegin`**](#cbegin)
- [**`cend`**](#cend)
- [**`rbegin`**](#rbegin)
- [**`rend`**](#rend)
- [**`crbegin`**](#crbegin)
- [**`crend`**](#crend)
- [**`operator[]`**](#operator-1)
- [**`front`**](#front)
- [**`back`**](#back)
- [**`data`**](#data)
- [**`data_end`**](#data_end)
- [**`size`**](#size)
- [**`length`**](#length)
- [**`max_size`**](#max_size)
- [**`empty`**](#empty)
- [**`remove_prefix`**](#remove_prefix)
- [**`remove_suffix`**](#remove_suffix)
- [**`operator()`**](#operator-2)
- [**`substr`**](#substr)
- [**`starts_with`**](#starts_with)
- [**`ends_with`**](#ends_with)
- [**`find`**](#find)
- [**`rfind`**](#rfind)
- [**`find_first_of`**](#find_first_of)
- [**`find_last_of`**](#find_last_of)
- [**`contains`**](#contains)
- [**`split`**](#split)
- [**`count`**](#count)
- [**`strip`**](#strip)
- [**`lstrip`**](#lstrip)
- [**`rstrip`**](#rstrip)
- [**`strip_first`**](#strip_first)
- [**`strip_last`**](#strip_last)
- [**`all_of`**](#all_of)
- [**`any_of`**](any_of)
- [**`none_of`**](#none_of)
- [**`operator==`**](#operator-3)
- [**`operator!=`**](#operator-4)
- [**`operator>`**](#operator-5)
- [**`operator>=`**](#operator-6)
- [**`operator<`**](#operator-7)
- [**`operator<=`**](#operator-8)
- [**`operator std::basic_string_view<value_type, Tr>()`**](#operator-stdbasic_string_viewvalue_type-tr)
- [**`operator std::basic_string<value_type, Tr>()`**](#operator-stdbasic_stringvalue_type-tr)

## Constructor
```cpp
constexpr string_viewt() noexcept;
```
Default constructor.
After construction `data()` method returns `nullptr` and `size()` method returns `0`.
<br/><br/>

```cpp
constexpr string_viewt(const string_viewt&) noexcept = default;
```
Copy constructor.
<br/><br/>

```cpp
constexpr string_viewt(const_pointer str, size_type count);
```
Constructs `bs::string_viewt` from a range [`str`, `str + count`).
> [!TIP]
> If `str` is `nullptr` with non-zero `count` assentation will be invoked.

<br/>

```cpp
template<std::size_t N>
constexpr string_viewt(const value_type(&array)[N]) noexcept;
```
Constructs from a range [`array`, `array + N - 1`). \
Static assertion will be used if `N` is equal to `0`.

> [!CAUTION]
> Use this constructor **only** with string literals. The null terminator will be striped away automatically. \
> If you use this constructor with non-null terminated character arrays, *the last character* will be removed from the string.

<br/>

```cpp
template<class Begin, class End = Begin>
constexpr string_viewt(Begin first, End last) noexcept(noexcept(last - first));
```
Constructs `bs::string_viewt` from the range [`first`, `last`). \
The `Begin` type must be available to obtain the address represented by `first`,
and the type of an expression `last - first` must be integer type.

This constructor is enabled when `End` is not convertible to `size_type`.

## `operator=`
```cpp
constexpr string_viewt& operator=(const string_viewt&) noexcept = default;
```
Copy assignment operator.

## `begin`
```cpp
constexpr const_iterator begin() const noexcept;
```
Currently just returns `data()`.

## `end`
```cpp
constexpr const_iterator end() const noexcept;
```
Currently just returns `data() + size()`.

## `cbegin`
```cpp
constexpr const_iterator cbegin() const noexcept;
```
Returns `begin()`.

## `cend`
```cpp
constexpr const_iterator cend() const noexcept;
```
Returns `end()`.

## `rbegin`
```cpp
constexpr const_reverse_iterator rbegin() const noexcept;
```
Returns `std::reverse_iterator(begin())`.

## `rend`
```cpp
constexpr const_reverse_iterator rend() const noexcept
```
Returns `std::reverse_iterator(end())`.

## `crbegin`
```cpp
constexpr const_reverse_iterator crbegin() const noexcept;
```
Returns `rbegin()`.

## `crend`
```cpp
constexpr const_reverse_iterator crend() const noexcept;
```
Returns `rend()`.

## `operator[]`
```cpp
template<class Int>
constexpr const_reference operator[](Int index) const noexcept;
```
Returns a constant reference to the element at the `index` position.
- If `index` is positive or zero number, the expression `data()[index]` is returned.
- If `index` is negative number, the expression `data()[index + size()]` is returned.

Enabled only when type `Int` is **integer type**.

> [!NOTE]
> If a character at the `index` position is outside of the range [`data()`, `data() + size()`), **assertion will be invoked**.

<br/>

```cpp
constexpr string_viewt operator[](bs::slice slice) const noexcept;
```
Returns a view to the range [`data() + slice.start`, `data() + slice.stop`). \
The `slice.start` and `slice.stop` indexes can be negative, and will be wrapped around.

> [!NOTE]
> If `slice.start` or `slice.stop` will point outside of the string, **assertion will be invoked**.

## `front`
```cpp
constexpr const_reference front() const noexcept;
```
Returns a constant reference to the first character in the string.

> [!NOTE]
> If the string is empty, **assertion will be invoked**.

## `back`
```cpp
constexpr const_reference back() const noexcept;
```
Returns a constant reference to the last character in the string.

> [!NOTE]
> If the string is empty, **assertion will be invoked**.

## `data`
```cpp
constexpr const_pointer data() const noexcept;
```
Returns a constant pointer to the first character in the string,
or, if the string is constructed through default constructor, `nullptr` is returned.

## `data_end`
```cpp
constexpr const_pointer data_end() const noexcept;
```
Returns a constant pointer to the past-the-end character in the string,
or, if the string is constructed through default constructor, `nullptr` is returned.

## `size`
```cpp
constexpr size_type size() const noexcept;
```
Returns length of the contained string.

## `length`
```cpp
constexpr size_type length() const noexcept;
```
Equivalent to `size()`.

## `max_size`
```cpp
constexpr size_type max_size() const noexcept;
```
Returns `static_cast<size_type>(-1)`.

## `empty`
```cpp
[[nodiscard]] constexpr bool empty() const noexcept;
```
Checks if the string is empty (length is zero).

## `remove_prefix`
```cpp
constexpr void remove_prefix(size_type count) noexcept;
```
Removes first `count` character by moving the start of the view forward.

> [!WARNING]
> Assertion will be called if `count` is greater than length of string.

## `remove_suffix`
```cpp
constexpr void remove_suffix(size_type count) noexcept;
```
Removes last `count` character by moving the end of the view back.

> [!WARNING]
> Assertion will be called if `count` is greater than length of string.

## `operator()`
```cpp
template<class Int1, class Int2>
constexpr string_viewt operator()(Int1 start, Int2 finish) const noexcept;
```
Returns a view to the range [`data() + first`, `data() + finish`). \
The indexes can be negative, and will be wrapped around.

Enabled only when types `Int1` and `Int2` are **integer types**.

> [!NOTE]
> If `start` or `finish` will point outside of the string, **assertion will be invoked**.

<br/>

```cpp
template<class Int1, class SliceEnd = /* implementation defined type */>
constexpr string_viewt operator()(Int1 start, SliceEnd) const noexcept;
```
Returns a view to the range [`data() + start`, `data() + size()`). \
The `start` index can be negative, and will be wrapped around.

Enabled only when type `Int1` is **integer type** and template parameter `SliceEnd` is selected from the default template argument.

> [!NOTE]
> If `start` index will point outside of the string, **assertion will be invoked**.

## `substr`
```cpp
constexpr string_viewt substr(size_type position) const noexcept;
```
Returns a view of the substring [`position`, `size()`).

> [!WARNING]
> If the `position` index will greater than string length, **assertion will be invoked**.

<br/>

```cpp
constexpr string_viewt substr(size_type position, size_type count) const noexcept;
```
Return a view of the substring [`position`, `position + count`).

> [!WARNING]
> If the `position` index will greater than string length, **assertion will be invoked**.

## `starts_with`
```cpp
constexpr bool starts_with(string_viewt str) const noexcept;
```
Checks if string starts with the range [`str.data()`, `str.data() + str.size()`). \
If `str` length is greater than or equal to current string length, `false` is returned.

<br/>

```cpp
constexpr bool starts_with(value_type ch) const noexcept;
```
Checks is string starts with the character `ch`. \
If string is empty, `false` is returned.

## `ends_with`
```cpp
constexpr bool ends_with(string_viewt str) const noexcept
```
Checks if string ends with the range [`str.data()`, `str.data() + str.size()`). \
If `str` length is greater than or equal to current string length, `false` is returned.

<br/>

```cpp
constexpr bool ends_with(value_type ch) const noexcept;
```
Checks is string ends with the character `ch`. \
If string is empty, `false` is returned.

## `find`
```cpp
constexpr bs::find_result<const value_type> find(value_type ch) const noexcept;
```
Returns a position to the first occurrence of the character `ch`.

<br/>

```cpp
constexpr bs::find_result<const value_type> find(string_viewt str) const noexcept;
```
Returns a position to the first occurrence of the substring `str`.

<br/>

```cpp
constexpr bs::find_result<const value_type> find(value_type ch, size_type start) const noexcept;
```
Returns a position to the first occurrence of the character `ch` starting at the `start` position.
> [!WARNING]
> If the `start` is greater than string length, **assertion will be invoked**.

<br/>

```cpp
constexpr bs::find_result<const value_type> find(string_viewt str, size_type start) const noexcept;
```
Returns a position to the first occurrence of the substring `str` starting at the `start` position.
> [!WARNING]
> If the `start` is greater than string length, **assertion will be invoked**.

<br/>

```cpp
constexpr bs::find_result<const value_type> find(value_type ch, const_pointer start) const noexcept;
```
Returns a position to the first occurrence of the character `ch` starting at the position that `start` points to.
> [!WARNING]
> If the `start` is not in the range [`data()`, `data() + size()`), **assertion will be invoked**.

<br/>

```cpp
constexpr bs::find_result<const value_type> find(string_viewt str, const_pointer start) const noexcept;
```
Returns a position to the first occurrence of the substring `str` starting at the position that `start` points to.
> [!WARNING]
> If the `start` is not in the range [`data()`, `data() + size()`), **assertion will be invoked**.

## `rfind`
```cpp
constexpr bs::find_result<const value_type> rfind(value_type ch) const noexcept;
```
Returns a position to the last occurrence of the character `ch` in the string.

<br/>

```cpp
constexpr bs::find_result<const value_type> rfind(string_viewt str) const noexcept
```
Returns a position to the last occurrence of the substring `ch` in the string.

<br/>

```cpp
constexpr bs::find_result<const value_type> rfind(value_type ch, size_type start) const noexcept;
```
Returns a position to the last occurrence of the character `ch` in the string starting at the `start` position.

<br/>

```cpp
constexpr bs::find_result<const value_type> rfind(string_viewt str, size_type start) const noexcept;
```
Returns a position to the last occurrence of the substring `str` in the string starting at the `start` position.

<br/>

```cpp
constexpr bs::find_result<const value_type> rfind(value_type ch, const_pointer start) const noexcept;
```
Returns a position to the last occurrence of the character `ch` in starting at the position that `start` points to.
> [!WARNING]
> If the `start` is not in the range [`data()`, `data() + size()`), **assertion will be invoked**.

<br/>

```cpp
constexpr bs::find_result<const value_type> rfind(string_viewt str, const_pointer start) const noexcept;
```
Returns a position to the last occurrence of the substring `str` in starting at the position that `start` points to.
> [!WARNING]
> If the `start` is not in the range [`data()`, `data() + size()`), **assertion will be invoked**.

## `find_first_of`
```cpp
constexpr bs::find_result<const value_type> find_first_of(value_type ch) const noexcept;
```
Equivalent to `this->find(ch)`.

<br/>

```cpp
constexpr bs::find_result<const value_type> find_first_of(string_viewt str) const noexcept;
```
Returns a position to the first character that equal to any of the characters in the character sequence [`str.data()`, `str.data() + str.size()`).

## `find_last_of`
```cpp
constexpr bs::find_result<const value_type> find_last_of(value_type ch) const noexcept;
```
Equivalent to `this->rfind(ch)`.

<br/>

```cpp
constexpr bs::find_result<const value_type> find_last_of(string_viewt str) const noexcept;
```
Returns a position to the last character that equal to any of the characters in the character sequence [`str.data()`, `str.data() + str.size()`).

## `contains`
```cpp
constexpr bool contains(value_type ch) const noexcept;
```
Checks if current string contains character `ch`.

<br/>

```cpp
constexpr bool contains(string_viewt str) const noexcept;
```
Checks if current string contains substring `str`.

## `split`
```cpp
constexpr splited_string<string_viewt, string_viewt> split(string_viewt separator) const noexcept;
```
Splits current string into a `bs::splited_string` adapter. \
Equivalent to `bs::splited_string<string_viewt, string_viewt>(*this, separator)`.

<br/>

```cpp
constexpr splited_string<string_viewt, value_type> split(value_type character) const noexcept;
```
Splits current string into a `bs::splited_string` adapter. \
Equivalent to `bs::splited_string<string_viewt, value_type>(*this, character)`.

## `count`
```cpp
constexpr size_type count(value_type ch) const noexcept;
```
Counts number of `ch`.
Returns a number of occurrences of the character `ch` in the string.

<br/>

```cpp
constexpr size_type count(string_viewt str) const noexcept;
```
Counts number of substrings `str`.
Returns a number of occurrences of the substring `str` in the current string. \
If string `str` is empty, `size() + 1` is returned.

## `strip`
```cpp
constexpr string_viewt strip(value_type strip_ch) const noexcept;
```
Removes leading and trailing characters `strip_ch`.

<br/>

```cpp
constexpr string_viewt strip(string_viewt chs) const noexcept;
```
Removes leading and trailing characters that is in the character sequence [`chs.data()`, `chs.data() + chs.size()`).

## `lstrip`
```cpp
constexpr string_viewt lstrip(value_type strip_ch) const noexcept;
```
Removes leading characters `strip_ch`.

<br/>

```cpp
constexpr string_viewt lstrip(string_viewt chs) const noexcept;
```
Removes leading characters that is in the character sequence [`chs.data()`, `chs.data() + chs.size()`).

## `rstrip`
```cpp
constexpr string_viewt rstrip(value_type strip_ch) const noexcept;
```
Removes trailing characters `strip_ch`.

<br/>

```cpp
constexpr string_viewt rstrip(string_viewt chs) const noexcept;
```
Removes trailing characters that is in the character sequence [`chs.data()`, `chs.data() + chs.size()`).

## `strip_first`
```cpp
constexpr string_viewt strip_first(value_type ch) const noexcept;
```
Removes first character `ch`. \
If the string starts with character `ch`, it will be removed from the view; otherwise, `*this` is returned.

<br/>

```cpp
constexpr string_viewt strip_first(string_viewt str) const noexcept;
```
Removes first substring `str`. \
If the string starts with the substring `str`, it will be removed from the view; otherwise, `*this` is returned.

## `strip_last`
```cpp
constexpr string_viewt strip_last(value_type ch) const noexcept;
```
Removes last character `ch`. \
If the string ends with the character `ch`, it will be removed from the view; otherwise, `*this` is returned.

<br/>

```cpp
constexpr string_viewt strip_last(string_viewt str) const noexcept;
```
Removes last substring `str`. \
If the string ends with the substring `str`, it will be removed from the view; otherwise, `*this` is returned.

## `all_of`
```cpp
template<class Predicate>
constexpr bool all_of(Predicate pred) const;
```
Checks if each character matches the `pred`.

## `any_of`
```cpp
template<class Predicate>
constexpr bool any_of(Predicate pred) const;
```
Checks if at least one character matches the `pred`.

## `none_of`
```cpp
template<class Predicate>
constexpr bool none_of(Predicate pred) const;
```
Checks if no characters matches the `pred`.

## `operator==`
```cpp
friend constexpr bool operator==(string_viewt left, string_viewt right) noexcept;
```
Compares `left` and `right` for equality.

## `operator!=`
```cpp
friend constexpr bool operator!=(string_viewt left, string_viewt right) noexcept;
```
Compares `left` and `right` for inequality.

## `operator>`
```cpp
friend constexpr bool operator>(string_viewt left, string_viewt right) noexcept;
```
Compares `left` and `right` for lexicographically *greater*.

## `operator>=`
```cpp
friend constexpr bool operator>=(string_viewt left, string_viewt right) noexcept;
```
Compares `left` and `right` for lexicographically *greater or equal*.

## `operator<`
```cpp
friend constexpr bool operator<(string_viewt left, string_viewt right) noexcept;
```
Compares `left` and `right` for lexicographically *less*.

## `operator<=`
```cpp
friend constexpr bool operator<=(string_viewt left, string_viewt right) noexcept;
```
Compares `left` and `right` for lexicographically *less or equal*.

## `operator std::basic_string_view<value_type, Tr>()`
```cpp
#ifndef BS_DONT_INCLUDE_STRING
template<class Tr>
constexpr operator std::basic_string_view<value_type, Tr>() const noexcept;
#endif
```
Converts `bs::string_viewt` into `std::basic_string_view`. \
Equivalent to `std::basic_string_view<value_type, Tr>(data(), size())`.

## `operator std::basic_string<value_type, Tr>()`
```cpp
#ifndef BS_DONT_INCLUDE_STRING
template<class Tr>
explicit constexpr operator std::basic_string<value_type, Tr>() const noexcept;
#endif
```
Converts `bs::string_viewt` into `std::basic_string<value_type, Tr>`. \
Equivalent to `std::basic_string<value_type, Tr>(data(), size())`.


# Literals
This operators are declared in the namespace `bs::literals`, where `literals` is `inline namespace`.

Access to these operators can be gained with either
- `using namespace bs::literals`, or
- `using namespace bs`.

<br/>

```cpp
constexpr bs::string_view operator ""_sv(const char* str, std::size_t len) noexcept;
```
Returns `bs::string_view{str, len}`.

<br/>

```cpp
constexpr bs::wstring_view operator ""_sv(const wchar_t* str, std::size_t len) noexcept;
```
Returns `bs::wstring_view{str, len}`.

<br/>

```cpp
constexpr bs::u16string_view operator ""_sv(const char16_t* str, std::size_t len) noexcept;
```
Returns `bs::u16string_view{str, len}`.

<br/>

```cpp
constexpr bs::u32string_view operator ""_sv(const char32_t* str, std::size_t len) noexcept;
```
Returns `bs::u32string_view{str, len}`.

