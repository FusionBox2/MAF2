#include "Operation.h"

BEGIN_FTK_NAMESPACE

namespace core
{
	Operation::Operation(const base::String& name, bool canUndo)
		: m_operationName(name)
		, m_canUndo(canUndo)
	{
	}

	bool Operation::CanUndo()
	{
		return static_cast<const Operation*>(this)->CanUndo();
	}
}

END_FTK_NAMESPACE
