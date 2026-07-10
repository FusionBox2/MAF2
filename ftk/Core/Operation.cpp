#include "Operation.h"

BEGIN_FTK_NAMESPACE

namespace core
{
	Operation::Operation(const base::String& name, bool canUndo)
		: m_operationName(name)
		, m_canUndo(canUndo)
	{
	}

	WithProperties::PropertyList Operation::getProperties()
	{
		return {};
	}

	base::Connection Operation::connectOperationChanged(std::function<void()> fn)
	{
		return m_operationChanged.connect(std::move(fn));
	}
}

END_FTK_NAMESPACE
