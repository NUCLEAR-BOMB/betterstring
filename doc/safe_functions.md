`<betterstring/safe_functions.hpp>`

- [Functions](#functions)
- [**`bs::safe::errorc`**](#bssafeerrorc)
- [**`bs::safe::error`**](#bssafeerror)

# `bs::safe::errorc`
```cpp
enum class errorc;
```
Enumeration that defines error codes.

## Member Constants
| Member constants        | Value |
| ----------------------- | ----- |
| **`null_pointer`**      | `1`   |
| **`dest_null_pointer`** | `2`   |
| **`src_null_pointer`**  | `3`   |
| **`not_enough_space`**  | `4`   |
| **`overlapping`**       | `5`   |

**`null_pointer`** - Passed pointer is null pointer with non-zero buffer size.

**`dest_null_pointer`** - Destination pointer is null pointer with non-zero buffer size.

**`src_null_pointer`** - Source pointer is null pointer with non-zero buffer size.

**`not_enough_space`** - Not enough space to perform operation. \
Typically when requested source memory length is greater than destination memory length.

**`overlapping`** - Two memory buffers are overlapping, and operation cannot be performed. 

# `bs::safe::error`
```cpp
class error;
```
Simple abstraction over returned error code.

- [Constructor](#constructor)
- [**`code`**](#code)
- [**`error_message`**](#error_message)
- [**`operator bool()`**](#operator-bool)
- [**`operator==`**](#operator)
- [**`operator!=`**](#operator-1)

## Constructor
```cpp
constexpr error(bs::safe::errorc err) noexcept;
```
Constructs `bs::safe::error` using error code from `err`. \
Can be empty error code.

## `code`
```cpp
constexpr errorc code() const noexcept;
```
Returns contained error code.

## `error_message`
```cpp
constexpr bs::string_view error_message() const noexcept;
```
Returns error message to corresponding underlying error code.

> [!WARNING]
> This method must be called when there is actually error code. **Undefined behavior** when there is not.

## `operator bool()`
```cpp
explicit constexpr operator bool() const noexcept;
```
Checks if there error code, i.e. error code is not equal to `bs::safe::errorc{}`.

## `operator==`
```cpp
friend constexpr bool operator==(const error left, const errorc right) noexcept;
```
Checks if error codes are same. \
Equivalent to `left.code() == right`.

```cpp
friend constexpr bool operator==(const errorc left, const error right) noexcept;
```
Checks if error codes are same. \
Equivalent to `left == right.code()`.

## `operator!=`
```cpp
friend constexpr bool operator!=(const error left, const errorc right) noexcept;
```
Checks if error codes are different. \
Equivalent to `left.code() != right`.

```cpp
friend constexpr bool operator!=(const errorc left, const error right) noexcept
```
Checks if error codes are different. \
Equivalent to `left != right.code()`.

# Functions

- [**`bs::safe::strcopy`**](#bssafestrcopy)
- [**`bs::safe::strmove`**](#bssafestrmove)

## `bs::safe::strcopy`
```cpp
template<class T>
constexpr bs::safe::error strcopy(T* dest, std::size_t dest_len, const T* src, std::size_t count) noexcept;
```
Copies `count` character from `src` to `dest`.

- If `dest` is null pointer and `dest_len` is not zero, error `bs::safe::errorc::dest_null_pointer` is returned.
- If `src` is null pointer and `count` is not zero, error `bs::safe::errorc::src_null_pointer` is returned.
- If `count` is greater than `dest_len`, error `bs::safe::errorc::not_enough_space` is returned.
- If the ranges [`dest`, `dest + dest_len`) and [`src`, `src + count`) overlap, error `bs::safe::errorc::overlapping` is returned.
- Otherwise, copying is performed and `bs::safe::errorc{}` is returned.

> [!NOTE]
> `dest` and `src` pointers are allowed to be null pointers unlike `bs::strcopy`. 

## `bs::safe::strmove`
```cpp
template<class T>
constexpr bs::safe::error strmove(T* dest, std::size_t dest_len, const T* src, std::size_t count) noexcept;
```
Copies `count` character from `src` to `dest`. \
The ranges **can** overlap.

- If `dest` is null pointer and `dest_len` is not zero, error `bs::safe::errorc::dest_null_pointer` is returned.
- If `src` is null pointer and `count` is not zero, error `bs::safe::errorc::src_null_pointer` is returned.
- If `count` is greater than `dest_len`, error `bs::safe::errorc::not_enough_space` is returned.
- Otherwise, copying is performed and `bs::safe::errorc{}` is returned.

> [!NOTE]
> `dest` and `src` pointers are allowed to be null pointers unlike `bs::strmove`. 
