#pragma once

#include "ftkConfigure.h"

#include "ftk/IO/ParseTo.h"

#include "ftk/Base/Meta.h"
#include <optional>

BEGIN_FTK_NAMESPACE

namespace io::parse
{
	namespace impl
	{
		template <typename T, class Value>
		inline T AsExtractor(const Value& value) {
			return value.template As<T>();
		}

		template <typename ContainerType, class Value, typename ExtractFunc>
		ContainerType ParseArray(const Value& value, ExtractFunc&& extract_func)
		{
			//value.CheckArrayOrNull();
			ContainerType result;
			auto inserter = std::inserter(result, end(result));

			for (size_t i = 0; i < value.size(); i++)
			{
				*inserter++ = extract_func(value[i]);
			}

			return result;
		}
	}

	template <typename T, typename Value>
	std::optional<decltype(Parse(std::declval<Value>(), To<T>{})) >
		Parse(const Value& value, To<std::optional<T>>)
	{
		if (!value.isValid())
		{
			return std::nullopt;
		}
		return value.template As<T>();
	}

	template <typename T, typename Value>
	std::enable_if_t<meta::kIsRange<T> && !meta::kIsMap<T>, T>
		Parse(const Value& value, To<T>)
	{
		return impl::ParseArray<T>(value, &impl::AsExtractor<meta::RangeValueType<T>, Value>);
	}
}

END_FTK_NAMESPACE
