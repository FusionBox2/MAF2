#pragma once

#include "ftkConfigure.h"

#include "ftk/Core/Operation.h"

#include "ftk/Core/IProperty.h"

#include <memory>

BEGIN_FTK_NAMESPACE

namespace core
{
	class OperationChangeProperty : public core::Operation
	{
	public:
		OperationChangeProperty(const base::String& label, IProperty& property, std::any newValue);

		OperationChangeProperty(const base::String& label, std::unique_ptr<IProperty> property, std::any newValue);

		bool execute() override;

		bool undo() override;

	private:
		std::unique_ptr<IProperty> m_ownedProperty;
		IProperty& m_property;
		std::any m_oldValue;
		std::any m_newValue;
	};

	template<typename TProperty>
	class OperationChangePropertyT : public core::Operation
	{
	public:
		using T = TProperty::ValueType;

		//OpChangePropertyT(IProperty& property, T&& newValue, const std::string& label = "Change ")
		OperationChangePropertyT(IProperty& property, T newValue, const base::String& label = _R("Change "))
			: core::Operation(label + property.id())
			, m_property(property)
			, m_newValue(std::move(newValue))
		{
		}

		OperationChangePropertyT(std::unique_ptr<IProperty> property, T newValue, const base::String& label = _R("Change "))
			: OperationChangePropertyT(*property, std::move(newValue), label)
		{
			m_ownedProperty = std::move(property);
		}

		bool execute() override
		{
			m_oldValue = m_property.getValue();
			m_property.setValue(m_newValue);
			return true;
		}

		bool undo() override
		{
			m_property.setValue(m_oldValue);
			return true;
		}

	private:
		std::unique_ptr<TProperty> m_ownedProperty;
		TProperty& m_property;
		std::any m_oldValue;
		std::any m_newValue;
	};
}

END_FTK_NAMESPACE
