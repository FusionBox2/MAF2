#include "Operation.h"

BEGIN_FTK_NAMESPACE

namespace core
{
	Operation::Operation(const base::String& name, bool canUndo)
		: m_operationName(name)
		, m_canUndo(canUndo)
	{
	}
}

END_FTK_NAMESPACE
