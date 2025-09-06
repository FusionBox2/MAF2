#pragma once

#include "ftkConfigure.h"

#include <ftk/Base/Meta.h>
#include "ftk/IO/SerializeTo.h"

BEGIN_FTK_NAMESPACE

namespace io::serialize
{
	template <typename ValueBuilder, typename T>
	std::enable_if_t<meta::isInteger<T> && !std::is_same_v<T, intmax_t> && !std::is_same_v<T, uintmax_t>, void> Serialize(ValueBuilder& b, T value, serialize::To<ValueBuilder>)
	{
		using IntT = std::conditional_t<std::is_signed_v<T>, intmax_t, uintmax_t>;
		Serialize(b, static_cast<IntT>(value), serialize::To<ValueBuilder>(b));
	}

	template <typename Value, typename T>
	using HasSerialize = decltype(Serialize(std::declval<Value&>(), std::declval<const T&>(), std::declval<serialize::To<Value>>()));

	template <class Value, class T>
	constexpr inline bool hasSerialize = meta::isDetected<HasSerialize, Value, T>;
}

END_FTK_NAMESPACE
