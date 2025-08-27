#pragma once

#include "ftkConfigure.h"

#include <type_traits>

BEGIN_FTK_NAMESPACE

namespace meta
{
	namespace impl {

		template <typename Default, typename AlwaysVoid, template <typename...> typename Trait, typename... Args>
		struct Detector {
			using value_t = std::false_type;
			using type = Default;
		};

		template <typename Default, template <typename...> typename Trait, typename... Args>
		struct Detector<Default, std::void_t<Trait<Args...>>, Trait, Args...> {
			using value_t = std::true_type;
			using type = Trait<Args...>;
		};

		template <template <typename...> typename Template, typename T>
		struct IsInstantiationOf : std::false_type {};

		template <template <typename...> typename Template, typename... Args>
		struct IsInstantiationOf<Template, Template<Args...>> : std::true_type {};

	}

	struct NotDetected {};

	template <template <typename...> typename Trait, typename... Args>
	inline constexpr bool isDetected = impl::Detector<NotDetected, void, Trait, Args...>::value_t::value;

	template <typename T>
	inline constexpr bool isCharacter =
		std::is_same_v<T, char> || std::is_same_v<T, wchar_t> || std::is_same_v<T, char16_t> || std::is_same_v<T, char32_t> || std::is_same_v<T, char8_t>;

	/// Returns `true` if the type is a true integer type (not `*char*` or `bool`)
	/// `signed char` and `unsigned char` are integer types
	template <typename T>
	inline constexpr bool isInteger = std::is_integral_v<T> && !isCharacter<T> && !std::is_same_v<T, bool>;
}

END_FTK_NAMESPACE
