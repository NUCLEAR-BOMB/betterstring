`<betterstring/functions.hpp>`

- [**`bs::cstr`**](#bscstr)
- [**`bs::data`**](#bsdata)
- [**`bs::array_size`**](#bsarray_size)
- [**`bs::strlen`**](#bsstrlen)
- [**`bs::strcopy`**](#bsstrcopy)
- [**`bs::strcomp`**](#bsstrcomp)
- [**`bs::strfind`**](#bsstrfind)
- [**`bs::strrfind`**](#bsstrrfind)
- [**`bs::strfill`**](#bsstrfill)
- [**`bs::strmove`**](#bsstrmove)
- [**`bs::strcount`**](#bsstrcount)
- [**`bs::strfindn`**](#bsstrfindn)
- [**`bs::strfirst_of`**](#bsstrfirst_of)

## `bs::cstr`
```cpp
template<class T>
constexpr auto* cstr(T& str) noexcept;
```
Returns underlying C-string (null terminated) of a container.

- If `str` is just a pointer to any character type, returns it.
- If `str` has the `.c_str()` method, returns result of this method.
- Otherwise makes `static_assert` error.

```cpp
template<class T, std::size_t N>
constexpr auto* cstr(T(&array)[N]) noexcept;
```
Returns `array`.

## `bs::data`
```cpp
template<class T>
constexpr auto* data(T& str) noexcept;
```
Returns underlying pointer to character sequence of a container.

- If `str` is just a pointer to any character type, returns it.
- If `str` has the `.data()` method, returns result of this method.
- If `str` has the `.c_str()` method, returns result of this method.
- Otherwise makes `static_assert` error.

```cpp
template<class T, std::size_t N>
constexpr T* data(T(&array)[N]) noexcept;
```
Returns `array`.

## `bs::array_size`
```cpp
template<class T>
constexpr auto array_size(const T& x) noexcept;
```
Returns length of the array, or, if it is single character returns `1`.

- If `x` is any character, `1` is returned.
- If `x` has the `.size()` method, returns result of this method.
- Otherwise makes `static_assert` error.

## `bs::strlen`
```cpp
template<class T>
constexpr std::size_t strlen(const T* str) noexcept;
```
Returns the length of the null terminated character sequence (not including null character) pointed by the `str`.

> [!NOTE]
> Note that `str` cannot be a null pointer (`nullptr`), otherwise, it will invoke **undefined behavior**.

Supports fast implementation only for `char` type with processors having AVX2 and BMI2 processor extensions.

## `bs::strcopy`
```cpp
template<class T>
constexpr void strcopy(T* dest, const T* src, std::size_t count) noexcept;
```
Copies the `count` number of characters of type `T` from memory location pointed by `src` to the memory location pointed by `dest`. \
**Undefined behavior** when the objects overlap.

> [!IMPORTANT]
> `dest` and `src` cannot be null pointers, even if `count` is zero. 

```cpp
template<class T>
constexpr void strcopy(const T* dest, const T* src, std::size_t) noexcept = delete;
```
`dest` cannot be a constant pointer.

## `bs::strcomp`
```cpp
template<class T>
constexpr int strcomp(const T* left, const T* right, std::size_t count) noexcept;
```
Compares first `count` elements of `left` and `right` strings.

- If first element that is different from another corresponding element is less than the corresponding element, negative value is returned.
- If every element in there string are equal, returns `0`.
- If first element that is different from another corresponding element is greater than the corresponding element,
positive value is returned.

> [!IMPORTANT]
> `left` and `right` cannot be null pointers, even if `count` is zero. 

> [!NOTE]
> Checking the return value of the function for strictly equality `1` or `-1` can give wrong result.\
> Use less than or greater than operation instead.

```cpp
template<class T>
constexpr int strcomp(const T* left, std::size_t left_len, const T* right, std::size_t right_len) noexcept;
```
Compares character sequence ranges [`left`, `left + left_len`) and [`right`, `right + right_len`).

> [!IMPORTANT]
> `left` and `right` cannot be a null pointer, even if `left_len` or `right_len` is zero.

- If `left_len` is greater than `right_len`, positive value is returned.
- If `left_len` is less than `right_len`, negative value is returned.
- Otherwise, if `left_len` is equal to `right_len`, returns the result value of `bs::strcomp(left, right, left_len)`.

## `bs::strfind`
```cpp
template<class T>
constexpr T* strfind(T* str, std::size_t count, T ch) noexcept;
```
Returns a pointer to **first** occurrence of character `ch` in the range [`str`, `str + count`). \
If there is no character in this range, `nullptr` is returned.

```cpp
template<class T>
constexpr T* strfind(T* haystack, std::size_t count, const T* needle, std::size_t needle_len) noexcept;
```
Returns a pointer to **first** occurrence of substring [`needle`, `needle + needle_len`) in the range [`haystack`, `haystack + count`). \
If there is no substring [`needle`, `needle + needle_len`), `nullptr` is returned.

Specific behavior:
- If `needle_len` is zero, `haystack` is returned.
- If `needle_len` is greater than `count`, `nullptr` is returned.

## `bs::strrfind`
```cpp
template<class T>
constexpr T* strrfind(T* str, std::size_t count, T ch) noexcept;
```
Returns a pointer to **last** occurrence of character `ch` in the range [`str`, `str + count`). \
If there is no character in this range, `nullptr` is returned.

Supports fast implementation only for `char` type with processors having AVX2 and BMI2 processor extensions.

```cpp
template<class T>
constexpr T* strrfind(T* haystack, std::size_t count, const T* needle, std::size_t needle_len) noexcept;
```
Returns a pointer to **last** occurrence of substring [`needle`, `needle + needle_len`) in the range [`haystack`, `haystack + count`). \
If there is no substring [`needle`, `needle + needle_len`), `nullptr` is returned.

Specific behavior:
- If `needle_len` is zero, `haystack + count` is returned.
- If `needle_len` is greater than `count`, `nullptr` is returned.

## `bs::strfill`
```cpp
template<class T>
constexpr void strfill(T* dest, std::size_t count, T ch) noexcept;
```
Copies the character `ch` into each character of the range [`dest`, `dest + count`).

> [!IMPORTANT]
> `dest` cannot be a null pointer, even if `count` is zero. 

## `bs::strmove`
```cpp
template<class T>
constexpr void strmove(T* dest, const T* src, std::size_t count) noexcept;
```
Copies from the range [`dest`, `dest + count`) to range [`src`, `src + count`) `count` characters. \
The ranges **can** overlap.

> [!IMPORTANT]
> `dest` or `src` cannot be null pointers, even if `count` is zero.

```cpp
template<class T>
constexpr void strmove(const T* dest, const T* src, std::size_t) noexcept = delete;
```
`dest` cannot be a constant pointer.

## `bs::strcount`
```cpp
template<class T>
constexpr std::size_t strcount(const T* str, std::size_t count, T ch) noexcept;
```
Counts number of occurrences of the character `ch` in the range [`str`, `str + count`).

Supports fast implementation only for `char` type with processors having AVX2, BMI2 and POPCNT processor extensions.

## `bs::strfindn`
```cpp
template<class T>
constexpr T* strfindn(T* str, std::size_t count, T ch) noexcept;
```
Returns a pointer to first occurrence of the character that is **not** `ch` in the range [`str`, `str + count`). \
If no match is found, `nullptr` is returned.

## `bs::strfirst_of`
```cpp
template<class T>
constexpr T* strfirst_of(T* str, std::size_t count, const T* needle, std::size_t needle_size) noexcept;
```
Returns a pointer to first occurrence of the any character in the sequence [`needle`, `needle + needle_size`) in the range [`str`, `str + count`).

Supports fast implementation only for `char` type with processors having AVX2 and BMI2 processor extensions.
