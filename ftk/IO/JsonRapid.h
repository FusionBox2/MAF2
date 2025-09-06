#pragma once

#include "ftkConfigure.h"

#include "Parse.h"
#include "Serialize.h"

#include <memory>
#include <string_view>
#include <string>

BEGIN_FTK_NAMESPACE

namespace io::jsonrapid
{
	namespace parse
	{
	}

	class ValueBuilder;
	class Value final
	{
	public:
		struct DefaultConstructed {};

		using Builder = ValueBuilder;

		Value();

		Value(const Value& value);
		Value(Value&& value) noexcept;

		~Value();

		Value& operator=(const Value& value);
		Value& operator=(Value&& value) noexcept;

		Value operator()(std::string_view key) const;
		Value operator[](std::string_view key) const;
		Value operator[](std::size_t index) const;

		std::size_t size() const;

		bool isValid() const;
		bool isMissing() const;
		bool isNull() const;

		template<typename T>
		auto As() const;

		template <typename T, typename First, typename... Rest>
		auto As(First&& default_arg, Rest&&... more_default_args) const;

		/// @brief Returns value of *this converted to T or T() if this->IsMissing().
		/// @throw Anything derived from std::exception.
		/// @note Use as `value.As<T>({})`
		template <typename T>
		auto As(DefaultConstructed) const;

		static Value FromFile(std::string_view url);
		void Store(std::string_view url);

		friend std::string Parse(const Value& value, io::parse::To<std::string>);
		friend class ValueBuilder;

	private:
		class NativeHolder;

		Value(void*, std::shared_ptr<NativeHolder>);

		std::shared_ptr<NativeHolder> m_holder;
		void* m_current = nullptr;
	};

	template<typename T>
	auto Value::As() const
	{
		static_assert(
			io::parse::hasParse<Value, T>,
			"There is no `Parse(const Value&, io::parse::To<T>)` "
			"in namespace of `T` or `io::parse`. "
			"Probably you have not provided a `Parse` function overload."
			);

		return Parse(*this, io::parse::To<T>{});
	}

	template <typename T, typename First, typename... Rest>
	auto Value::As(First&& default_arg, Rest&&... more_default_args) const {
		if (isMissing() || isNull())
		{
			// intended raw ctor call, sometimes casts
			// NOLINTNEXTLINE(google-readability-casting)
			return decltype(As<T>())(std::forward<First>(default_arg), std::forward<Rest>(more_default_args)...);
		}
		return As<T>();
	}

	template <typename T>
	auto Value::As(Value::DefaultConstructed) const {
		return (isMissing() || isNull()) ? decltype(As<T>())() : As<T>();
	}

	intmax_t Parse(const Value&, io::parse::To<intmax_t>);
	uintmax_t Parse(const Value&, io::parse::To<uintmax_t>);
	bool Parse(const Value&, io::parse::To<bool>);
	double Parse(const Value&, io::parse::To<double>);
	std::string Parse(const Value&, io::parse::To<std::string>);
}

namespace io::jsonrapid
{
	namespace serialize
	{

	}
	class ValueBuilder final
	{
	public:
		static const size_t npos = size_t(-1);

		struct DefaultConstructed {};
		using Value = Value;

		ValueBuilder();

		ValueBuilder(const Value& other);
		ValueBuilder(Value&& other) noexcept;

		ValueBuilder(const ValueBuilder& value);
		ValueBuilder(ValueBuilder&& value) noexcept;

		ValueBuilder& operator=(const ValueBuilder& value);
		ValueBuilder& operator=(ValueBuilder&& value) noexcept;

		template<typename T>
		ValueBuilder& operator=(const T&);

		ValueBuilder operator()(std::string_view key);
		ValueBuilder operator[](std::string_view key);
		ValueBuilder operator[](std::size_t index);

		void setValue(const std::string& s);

		void setValue(const char* s);
		void setValue(char* s);
		void setValue(std::string_view s);

		template<typename T>
		void setValue(const T&);

		std::size_t size() const;

		bool isValid() const;

		Value extractValue();

	private:

		Value m_value;
	};

	template<typename T>
	ValueBuilder& ValueBuilder::operator=(const T& t)
	{
		setValue(t);
		return *this;
	}

	template <typename T>
	void ValueBuilder::setValue(const T& t)
	{
		static_assert(
			io::serialize::hasSerialize<ValueBuilder, T>,
			"There is no `Serialize(xmlxerces::ValueBuilder&, const T&, io::serialize::To<xmlxerces::ValueBuilder>)` "
			"in namespace of `T` or `io::serialize`. "
			"Probably you have not provided a `Serialize` function overload."
			);
		Serialize(*this, t, io::serialize::To<ValueBuilder>(*this));
	}

	void Serialize(ValueBuilder& b, int i, io::serialize::To<ValueBuilder>);
}

END_FTK_NAMESPACE
