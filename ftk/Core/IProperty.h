#pragma once

#include "ftkConfigure.h"

#include "ftk/Base/Meta.h"
#include "ftk/Base/String.h"

#include <any>
#include <functional>
#include <limits>
#include <memory>
#include <optional>
#include <typeindex>
#include <utility>

BEGIN_FTK_NAMESPACE

namespace PropertyMetadataKeys
{
	inline constexpr auto Min = "min";
	inline constexpr auto Max = "max";
	inline constexpr auto Step = "step";
	inline constexpr auto Category = "category";
	inline constexpr auto ReadOnly = "readOnly";
	inline constexpr auto Unit = "unit";
	inline constexpr auto Description = "description";
}

class PropertyMetadata
{
public:

	using Entry = std::pair<base::String, std::any>;

	PropertyMetadata() = default;

	PropertyMetadata(std::initializer_list<Entry> values)
	{
		for (const auto& [key, value] : values)
		{
			m_values.emplace(key, value);
		}
	}

	template<typename T>
	std::optional<T> get(const base::String& key) const
	{
		if (auto it = m_values.find(key); it != m_values.end())
		{
			try
			{
				return std::any_cast<T>(it->second);
			}
			catch (const std::bad_any_cast&)
			{
			}
		}
		return std::nullopt;
	}

	template<typename T>
	void set(base::String key, T value)
	{
		m_values[std::move(key)] = std::move(value);
	}

	bool contains(const base::String& key) const
	{
		return m_values.count(key) != 0;
	}

private:
	std::unordered_map<base::String, std::any> m_values;
};

class IProperty
{
public:
	IProperty(base::String id, PropertyMetadata metadata = {})
		: m_id(std::move(id))
		, m_metadata(std::move(metadata))
	{}

	virtual ~IProperty() = default;

	const base::String& id() const
	{
		return m_id;
	}

	virtual const PropertyMetadata& metadata() const
	{
		return m_metadata;
	}

	virtual std::type_index type() const = 0;

	virtual std::any get() const = 0;

	virtual bool equals(const std::any& value) const = 0;

	virtual void set(const std::any& value) = 0;

private:
	base::String m_id;
	PropertyMetadata m_metadata;
};

template<typename T, typename Getter, typename Setter>
class Property : public IProperty
{
public:
	using ValueType = T;

	using GetterResult = std::invoke_result_t<Getter>;

	Property(base::String id, Getter getter, Setter setter, PropertyMetadata metadata = {})
		: IProperty(std::move(id), std::move(metadata))
		, m_getter(std::move(getter))
		, m_setter(std::move(setter))
	{
	}

	std::type_index type() const override
	{
		return typeid(T);
	}

	std::any get() const override
	{
		return getValue();
	}

	bool equals(const std::any& value) const override
	{
		return getValue() == std::any_cast<T>(value);
	}

	void set(const std::any& value) override
	{
		setValue(std::any_cast<T>(value));
	}

	decltype(auto) getValue() const
	{
		return std::invoke(m_getter);
	}

	template<typename TValue>
	void setValue(TValue&& value)
	{
		std::invoke(m_setter, std::forward<TValue>(value));
	}

private:
	Getter m_getter;
	Setter m_setter;
};

namespace property::details
{
	template <typename T>
	using ValueType = std::remove_cvref_t<decltype(std::declval<const T&>().getValue())>;
}

template <typename T>
using PropertyValueType = meta::DetectedType<property::details::ValueType, T>;

template <typename T>
inline constexpr bool kIsPropertyTyped = meta::isDetected<property::details::ValueType, T>;


template<typename Getter, typename Setter, typename T = std::remove_cvref_t<std::invoke_result_t<Getter>>>
auto makeProperty(base::String id, Getter getter, Setter setter, PropertyMetadata metadata = {})
{
	return std::make_unique<Property<T, Getter, Setter>>(std::move(id), std::move(getter), std::move(setter), std::move(metadata));
}

template<class TObject, class T>
auto makeProperty(base::String id, TObject& object, T TObject::* member, PropertyMetadata metadata = {})
{
	return makeProperty(std::move(id), [&object, member]() -> decltype(auto) {return (object.*member); }, [&object, member](const T& v) {object.*member = v; }, std::move(metadata));
}

template<typename TObject, typename Getter, typename Setter>
auto makeProperty(base::String id, TObject& object, Getter getter, Setter setter, PropertyMetadata metadata = {})
{
	using T = std::remove_cvref_t<std::invoke_result_t<Getter, TObject&>>;
	return makeProperty(std::move(id), [&object, getter]() ->decltype(auto){return std::invoke(getter, object); }, [&object, setter](const T& v) {std::invoke(setter, object, v); }, std::move(metadata));
}

template<typename Getter, typename Setter, typename T = std::remove_cvref_t<std::invoke_result_t<Getter>>>
auto makeNumericProperty(base::String id, Getter getter, Setter setter, std::optional<T> minValue, std::optional<T> maxValue, PropertyMetadata metadata = {})
{
	PropertyMetadata numericMetadata = std::move(metadata);
	numericMetadata.set(PropertyMetadataKeys::Min, minValue.value_or(std::numeric_limits<T>::lowest()));
	numericMetadata.set(PropertyMetadataKeys::Max, maxValue.value_or(std::numeric_limits<T>::max()));
	return makeProperty(std::move(id), std::move(getter), std::move(setter), std::move(numericMetadata));
}

template<typename Getter, typename Setter, typename T = std::remove_cvref_t<std::invoke_result_t<Getter>>>
auto makeNumericProperty(base::String id, Getter getter, Setter setter, PropertyMetadata metadata = {})
{
	return makeProperty<Getter, Setter, T>(id, id, getter, setter, std::move(metadata));
}

END_FTK_NAMESPACE
