`<betterstring/parsing.hpp>`

- [**`bs::parse`**](#bsparset)
- [**`bs::parse_error`**](#bsparse_error)
    - [Member Constants](#member-constants)
- [**`bs::parse_result`**](#bsparse_resultt)
    - [Member Functions](#member-functions)
    - [Non-member Functions](#non-member-functions)
- [**`bs::bad_parse`**](#bsbad_parse)

# `bs::parse<T>`

```cpp
template<class T, class Ch>
constexpr parse_result<T> parse(const Ch* str, std::size_t count);
```
Tries to parse an number from string [`str`, `str + count`).
If parsing was successful, you can access resulting value from using method [`.value()`](#value) on returned [`bs::parse_result<T>`](#bsparse_resultt) object.

Currently only supports unsigned integers.

Template parameter **`T`** must satisfy requirements of [`std::is_unsigned_v<T>`][std_is_unsigned].

# `bs::parse_error`
```cpp
enum class parse_error;
```
## Member Constants
| Member constants       | Value |
| ---------------------- | ----- |
| **`invalid_argument`** | `1`   |
| **`out_of_range`**     | `2`   |
| **`too_long`**         | `3`   |

**`invalid_argument`** - Given string does not contain a valid number.

**`out_of_range`** - The value in a given string cannot fit in requested type.

**`too_long`** - Given string is too long and cannot fit in requested type. \
Note that leading zeros (`0`) are part of the full number, so even if a number without leading zeros can fit the requested type,
the function will still return `too_long` if there are enough leading zeros. For example, if we trying to parse a 4-digit number in an unsigned type that can only have maximum 3 digits, the string will be considered "too long".

# `bs::parse_result<T>`

```cpp
template<class T>
class parse_result;
```
Wrapper around result of [`bs::parse`](#bsparset) function.
Assumed to be inlined, so it is basically zero-cost abstraction over the returned error code and the resulting value passed by reference.

- [Member Function](#member-functions)
- [Non-member Functions](#non-member-functions)

## Member Functions
- [**`value`**](#value)
- [**`unchecked_value`**](#unchecked_value)
- [**`value_or_throw`**](#value_or_throw)
- [**`error`**](#error)
- [**`has_error`**](#has_error)
- [**`operator bool()`**](#operator-bool)

## Constructor
```cpp
constexpr parse_result(bs::parse_error err) noexcept;
```
Constructs `parse_result` in an error state, with error value of `err`.
See [**`bs::parse_error`**](#bsparse_error).

```cpp
constexpr parse_result(T result) noexcept;
```
Constructs `parse_result` in a successful state, with underlying value of `result`.

```cpp
explicit constexpr parse_result(T result, bs::parse_error err) noexcept;
```
Constructs `parse_result` in a successful/an error state.

If `err` has the value of `bs::parse_error{}` (default constructor) when `parse_result` is in a successful state with value of `result`; otherwise, ignoring the `result` value, `parse_result` is in an error state (`err` is not `bs::parse_error{}`).
See [**`bs::parse_error`**](#bsparse_error).

```cpp
template<class U>
explicit constexpr parse_result(parse_result<U> other) noexcept;
```
Constructs `parse_result` using other `parse_result` with possibly different type that `T`.

If `other` is in an error state, current `parse_result` is too, regardless of the successful value of `other`; otherwise, if `other` is in a successful state, the current `parse_result` has its successful value constructed by using `static_cast<T>(other.value())`.

### value
```cpp
constexpr T value() const noexcept;
```
Returns successful value.

**Undefined behavior** when the current `parse_result` is in error state.

### unchecked_value
```cpp
constexpr T unchecked_value() const noexcept;
```
Returns successful value without trigging any assertions even if the current `parse_result` is in error state.

### value_or_throw
```cpp
constexpr T value_or_throw() const;
```
Returns successful value if in successful state; otherwise, if the current `parse_result` is in an error state, throws `bs::bad_parse` exception.

### error
```cpp
constexpr bs::parse_error error() const noexcept;
```
Returns current error code. If `parse_result` is in successful state, just returns `bs::parse_error{}` (default constructor).
See [**`parse_error`**](#bsparse_error).

### has_error
```cpp
constexpr bool has_error() const noexcept;
```
Checks if is in error state.

### operator bool()
```cpp
explicit constexpr operator bool() const noexcept;
```
Checks if is in error state.

Equivalent to [`has_error()`](#has_error).

## Non-member Functions

### operator==
```cpp
friend constexpr bool operator==(parse_result<T> left, parse_result<T> right) noexcept;
```
Compares two `parse_result<T>`. If `left` and `right` have the same error code **or** they have the same result value returns `true`; otherwise, `false`.

### operator!=
```cpp
friend constexpr bool operator!=(parse_result<T> left, parse_result<T> right) noexcept;
```
Equivalent to `!(left == right)`.

# `bs::bad_parse`
```cpp
struct bad_parse : std::exception {
    bs::parse_error err;
};
```
Simple exception that the [`.value_or_throw()`](#value_or_throw) method throws.

Contains `err` member variable of type [**`bs::parse_error`**](#bsparse_error) that identify parse error code.

## Constructors
```cpp
bad_parse(parse_error err) noexcept;
```
Constructs `bad_parse` field `.err` (error code) from `err`.

[std_is_unsigned]: https://en.cppreference.com/w/cpp/types/is_unsigned