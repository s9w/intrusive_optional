# intrusive_optional
The C++ standard library has a `std::optional<T>` type which either contains a `T` with some value or is empty. Such an optional type must always have a size bigger than the contained type. At least by one byte, but usually more than that because of alignment constraints. In most applications, that's not an issue. There are situations however where you really want an optional **without size overhead** - `intrusive_optional` is a C++20 library that does just that.

That is only possible if we give up one possible state of `T` which we consider *magic* or the *null state*. That value is then considered equivalent to a null or a `std::nullopt` value.

The type is instantiated with its null value and otherwise completely mirrors the interface of `std::optional`, see below of differences. Examples:
```c++
#include "intrusive_optional.h"

using optional_double = io::intrusive_optional<std::numeric_limits<double>::max()>;
static_assert(sizeof(optional_double) == sizeof(double));

constexpr optional_double default_value;
static_assert(default_value.has_value() == false);

constexpr optional_double assigne_value = 5.0;
static_assert(assigne_value.has_value() == true);
static_assert(assigne_value.value() == 5.0);
```
It's just a single header so grab that from Github.


I want to stress that this type is very special for particular needs. Please **don't hurt yourself** with this - `std::optional<T>` or [swl::optional](https://github.com/groundswellaudio/swl-optional) are good types.


## Safety mode
There's an apparent drawback to implementing an optional like this: The mirrored interface of `std::optional` has several functions that offer direct access to the underlying value. That access can be used to set the stored value to its null value. That's probably unintentional as there's `.reset()` and `operator=(std::nullopt_t)` and can be a source of bugs.

To prevent this, `intrusive_optional` has an optional second parameter that can be used to enable a special safety mode. It removes dangerous functions which can't be checked for unintentional nulling and throws where such checks are possible.

Safety mode can be enabled like this:
```c++
using safe_int_optional = io::intrusive_optional<-1, io::safety_mode::safe>
```

In detail, safety mode does this:

- [Constructors (6), (7) and (8)](https://en.cppreference.com/w/cpp/utility/optional/optional) throw `io::unintentionally_null` when resulting in the null value
- [`operator=` overload (4)](https://en.cppreference.com/w/cpp/utility/optional/operator%3D) throw `io::unintentionally_null` when resulting in the null value
- All [`emplace`](https://en.cppreference.com/w/cpp/utility/optional/emplace) overloads throw `io::unintentionally_null` when resulting in the null value
- Non-`const` overloads of `operator*()` and `value()` are disabled

By default the safety mode is disabled so you can ignore that if you prefer.

## Compatibility with `std::optional`
The first overload of [`std::make_optional`](https://en.cppreference.com/w/cpp/utility/optional/make_optional) is such that you can write `std::make_optional(5)` and the type (`int`) will be deduced automatically. That isn't possible with `intrusive_optional` since its instantiation requires a value and not just a type. Hence that overload is removed. You can still use the other overloads like `std::make_optional<my_type>(1, 2, 3)`.


## Motivation
My original motivation was building a concurrency type that was based on `std::atomic<std::optional<T>>`. Atomics are crucially size-limited, only resolving to fast code paths for types of 8 bytes or less. Using that with something like `std::chrono::time_point` isn't possible. But in this case as with many, marking a default-constructed `time_point` as special had no practical drawbacks and allowed what I wanted to do.


## TODO
- operator= overloads 5 and 6
- missing comparison operators
- or_else etc, c++23 interface
- specialize hash
- operator= to and from std::optional?
