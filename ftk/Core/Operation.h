#pragma once

#include "ftkConfigure.h"

#include <ftk/Base/String.h>

BEGIN_FTK_NAMESPACE

namespace core
{
	class Operation
	{
	public:
		Operation(const base::String& name = _R(""), bool canUndo = false);

		virtual ~Operation() = default;

		virtual bool Do() = 0;

		virtual bool Undo() = 0;

		virtual bool CanDo() const { return true; }

		virtual bool CanUndo() const { return m_canUndo; }

		const base::String& GetName() const { return m_operationName; }

	protected:
		base::String m_operationName;
		bool m_canUndo;
	};
}

END_FTK_NAMESPACE
