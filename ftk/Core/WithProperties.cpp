#include "WithProperties.h"

BEGIN_FTK_NAMESPACE

namespace core
{
	WithProperties::PropertyList WithProperties::getProperties()
	{
		return {};
	}

	base::Connection WithProperties::connectPropertiesChanged(std::function<void()> fn)
	{
		return m_propertiesChanged.connect(std::move(fn));
	}

	base::Connection WithProperties::connectValuesChanged(std::function<void()> fn)
	{
		return m_valuesChanged.connect(std::move(fn));
	}
}

END_FTK_NAMESPACE
