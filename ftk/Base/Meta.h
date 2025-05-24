#pragma once

#include "ftkConfigure.h"

#include <type_traits>

BEGIN_FTK_NAMESPACE

template <typename T>
inline constexpr bool kIsCharacter =
std::is_same_v<T, char> || std::is_same_v<T, wchar_t> || std::is_same_v<T, char16_t> || std::is_same_v<T, char32_t> || std::is_same_v<T, char8_t>;

/// Returns `true` if the type is a true integer type (not `*char*` or `bool`)
/// `signed char` and `unsigned char` are integer types
template <typename T>
inline constexpr bool kIsInteger = std::is_integral_v<T> && !kIsCharacter<T> && !std::is_same_v<T, bool>;

END_FTK_NAMESPACE
