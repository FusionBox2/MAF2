#pragma once

#include "ftkConfigure.h"

#include <ftk/Base/Meta.h>
#include "ftk/IO/ParseTo.h"

#include <exception>
#include <limits>

BEGIN_FTK_NAMESPACE

namespace io::parse
{
	template <typename T>
	void CheckInBounds(T x, T min, T max) {
		if (x < min || x > max) {
			throw std::range_error("Out of range");
		}
	}
	template <typename Dst, typename Src>
	Dst NarrowToInt(Src x) {
		static_assert(
			std::numeric_limits<Src>::min() <= std::numeric_limits<Dst>::min() &&
			std::numeric_limits<Src>::max() >= std::numeric_limits<Dst>::max(),
			"expanding cast requested"
			);
		CheckInBounds<Src>(x, std::numeric_limits<Dst>::min(), std::numeric_limits<Dst>::max());
		return static_cast<Dst>(x);
	}

	template <typename Value>
	float NarrowToFloat(double x, const Value& value) {
		CheckInBounds<double>(x, std::numeric_limits<float>::lowest(), std::numeric_limits<float>::max());
		return static_cast<float>(x);
	}

	template <typename Value, typename T>
	std::enable_if_t<meta::isInteger<T>, T> Parse(const Value& value, io::parse::To<T>) {
		using IntT = std::conditional_t<std::is_signed_v<T>, intmax_t, uintmax_t>;
		return NarrowToInt<T>(value.template As<IntT>());
	}

	template <typename Value>
	float Parse(const Value& value, io::parse::To<float>) {
		return NarrowToFloat(value.template As<double>(), value);
	}

	template <typename Value, typename T>
	using HasParse = decltype(Parse(std::declval<const Value&>(), parse::To<T>{}));

	template <class Value, class T>
	constexpr inline bool hasParse = meta::isDetected<HasParse, Value, T>;
}

END_FTK_NAMESPACE
