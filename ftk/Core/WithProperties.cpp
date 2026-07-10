#include "WithProperties.h"

BEGIN_FTK_NAMESPACE

namespace core
{
	base::Connection WithProperties::connectPropertiesChanged(std::function<void()> fn)
	{
		return m_propertiesChanged.connect(std::move(fn));
	}
}

END_FTK_NAMESPACE
