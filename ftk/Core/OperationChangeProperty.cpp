#include "OperationChangeProperty.h"

BEGIN_FTK_NAMESPACE

namespace core
{
	OperationChangeProperty::OperationChangeProperty(const base::String& label, IProperty& property, std::any newValue)
		: Operation(label + property.id())
		, m_property(property)
		, m_newValue(std::move(newValue))
	{
	}

	OperationChangeProperty::OperationChangeProperty(const base::String& label, std::unique_ptr<IProperty> property, std::any newValue)
		: OperationChangeProperty(label, *property, std::move(newValue))
	{
		m_ownedProperty = std::move(property);
	}

	bool OperationChangeProperty::execute()
	{
		if (m_property.equals(m_newValue))
		{
			return false;
		}
		m_oldValue = m_property.get();
		m_property.set(m_newValue);
		return true;
	}

	bool OperationChangeProperty::undo()
	{
		m_property.set(m_oldValue);
		return true;
	}
}

END_FTK_NAMESPACE
