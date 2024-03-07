`<betterstring/find_result.hpp>`

# `bs::find_result`
```cpp
template<class CharT>
class find_result;
```
Wrapper around result of the function that returns a pointer to a location (or `nullptr`) of something.

Allows easy manipulations of returned result, i.e. convert to index or pointer with access to verify the expected result.

## Template Parameters
**`CharT`** - Character type that will be used to represent returned pointer.

## Member Types
| Member type     | Definition    |
| --------------- | ------------- |
| **`size_type`** | `std::size_t` |
| **`pointer`**   | `CharT*`      |

## Member Functions
- [Constructor](#constructor)
- [**`index`**](#index)
- [**`index_or_end`**](#index_or_end)
- [**`index_or_npos`**](#index_or_npos)
- [**`index_or_throw`**](#index_or_throw)
- [**`operator size_type()`**](#operator-size_type)
- [**`ptr`**](#ptr)
- [**`ptr_or_end`**](#ptr_or_end)
- [**`ptr_or_null`**](#ptr_or_null)
- [**`ptr_or_throw`**](#ptr_or_throw)
- [**`found`**](#found)

### Constructor
```cpp
constexpr find_result(pointer string_data, size_type string_end, pointer find_ptr) noexcept;
```
Constructs `bs::find_result` using beginning of the string, string end and a pointer to the found location. \
If `find_ptr` is `nullptr`, this means that there is actually no matching character.

### `index`
```cpp
constexpr size_type index() const noexcept;
```
Returns index of the matching location.
> [!IMPORTANT]
> Will invoke assertion when there is no found result.

### `index_or_end`
```cpp
constexpr size_type index_or_end() const noexcept;
```
Returns index of the matching location, or, if it was not found, returns string length.

### `index_or_npos`
```cpp
constexpr size_type index_or_npos() const noexcept;
```
Returns index of the matching location, or, if it was not found, `size_type(-1)` is returned.

### `index_or_throw`
```cpp
constexpr size_type index_or_throw() const;
```
Returns index of the matching location. \
If there is no match, `std::logic_error` exception is thrown.

### `operator size_type()`
```cpp
constexpr operator size_type() const noexcept;
```
Returns `index_or_end()`.

### `ptr`
```cpp
constexpr pointer ptr() const noexcept;
```
Returns a pointer to the found location.
> [!IMPORTANT]
> Will invoke assertion when there is no found result.

### `ptr_or_end`
```cpp
constexpr pointer ptr_or_end() const noexcept;
```
Returns a pointer to the matching location, or, if it was not found, past-the-end pointer is returned (a pointer to the beginning of the string plus string length).

### `ptr_or_null`
```cpp
constexpr pointer ptr_or_null() const noexcept;
```
Returns a pointer to the matching location, or, if it was not found, `nullptr` is returned.

### `ptr_or_throw`
```cpp
constexpr pointer ptr_or_throw() const;
```
Returns a pointer to the matching location. \
If there is no match, `std::logic_error` exception is thrown.

### `found`
```cpp
constexpr bool found() const noexcept;
```
Checks if there is matching location.
