#pragma once

#include "ftkConfigure.h"

#include "ftk/Core/IProperty.h"

#include "ftk/Core/OperationChangeProperty.h"

#include "ftk/Core/OperationManager.h"

#include <memory>

BEGIN_FTK_NAMESPACE

namespace core
{
	class OperationManager;

	template<typename TProperty>
	class OperationPropertyT : public IProperty
	{
	public:

		OperationPropertyT(OperationManager& manager, std::unique_ptr<TProperty> property)
			: IProperty(property->id())
			, m_manager(manager)
			, m_internalProperty(std::move(property))
		{
		}

		const PropertyMetadata& metadata() const override
		{
			return m_internalProperty->metadata();
		}

		std::type_index type() const override
		{
			return m_internalProperty->type();
		}

		std::any get() const override
		{
			return m_internalProperty->get();
		}

		bool equals(const std::any& value) const override
		{
			return m_internalProperty->equals(value);
		}

		void set(const std::any& value) override
		{
			m_manager.submit(std::make_unique<OperationChangeProperty>(_R("Change property"), *m_internalProperty, value));
		}

		decltype(auto) getValue() const
		{
			return m_internalProperty->getValue();
		}

		template<typename TValue>
		void setValue(TValue&& value)
		{
			m_manager.submit(std::make_unique<OperationChangePropertyT<TProperty>>(_R("Change property"), *m_internalProperty, std::forward<TValue>(value)));
		}

	private:
		OperationManager& m_manager;
		std::unique_ptr<TProperty> m_internalProperty;
	};

	template<typename TProperty>
	auto makeOperationProperty(OperationManager& manager, std::unique_ptr<TProperty> property)
	{
		return std::make_unique<OperationPropertyT<TProperty>>(manager, std::move(property));
	}

	template<typename... Args>
	auto makeOperationProperty(OperationManager& manager, Args&&... args)
	{
		auto property = makeProperty(std::forward<Args>(args)...);

		using PropertyType = std::remove_reference_t<decltype(*property)>;

		return std::make_unique<OperationPropertyT<PropertyType>>(manager, std::move(property));
	}
}

END_FTK_NAMESPACE
