#pragma once

#include "ftkConfigure.h"

#include <type_traits>
#include <iterator>

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
namespace meta
{
	namespace impl {

		template <typename T>
		using KeyType = typename T::key_type;

		template <typename T>
		using MappedType = typename T::mapped_type;

		/// Helps in definitions of traits for utils::meta::IsDetected
		template <typename T, typename U>
		using ExpectSame = std::enable_if_t<std::is_same_v<T, U>>;
		template <typename T>
		using IsRange =
			ExpectSame<std::decay_t<decltype(begin(std::declval<T&>()))>, std::decay_t<decltype(end(std::declval<T&>()))>>;
		template <typename T>
		using SubscriptOperatorResult = decltype(std::declval<T>()[std::declval<typename T::key_type>()]);
		template <typename T>
		using IteratorType = std::enable_if_t<isDetected<IsRange, T>, decltype(begin(std::declval<T&>()))>;
		template <template <typename...> typename Trait, typename... Args>
		using DetectedType = typename impl::Detector<NotDetected, void, Trait, Args...>::type;
		template <typename T>
		using RangeValueType = typename std::iterator_traits<DetectedType<IteratorType, T>>::value_type;
	}
	template <template <typename...> typename Trait, typename... Args>
	using DetectedType = typename impl::Detector<NotDetected, void, Trait, Args...>::type;

	template <typename T>
	using MapKeyType = DetectedType<impl::KeyType, T>;

	template <typename T>
	using MapValueType = DetectedType<impl::MappedType, T>;

	template <typename T>
	using RangeValueType = DetectedType<impl::RangeValueType, T>;


	template <typename T>
	inline constexpr bool kIsRange = isDetected<impl::IsRange, T>;
	template <typename T>
	inline constexpr bool kIsMap =
		isDetected<impl::IsRange, T>&& isDetected<impl::KeyType, T>&& isDetected<impl::MappedType, T>;

	/// Returns true if T is a map (but not a multimap!)
	template <typename T>
	inline constexpr bool kIsUniqueMap = kIsMap<T> && isDetected<
		impl::SubscriptOperatorResult,
		T>;  // no operator[] in multimaps


}

END_FTK_NAMESPACE
