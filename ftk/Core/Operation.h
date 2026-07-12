#pragma once

#include "ftkConfigure.h"

#include <ftk/Base/String.h>
#include "ftk/Core/WithProperties.h"

BEGIN_FTK_NAMESPACE

namespace core
{
	class Operation : public WithProperties
	{
	public:
		Operation(const base::String& name = _R(""), bool canUndo = true);

		~Operation() override = default;

		virtual bool execute() = 0;

		virtual bool undo() = 0;

		virtual bool canExecute() const { return true; }

		virtual bool canUndo() const { return m_canUndo; }

		virtual bool isConfigured() const { return true; }

		const base::String& getName() const { return m_operationName; }

	protected:
		base::String m_operationName;
		bool m_canUndo;
	};
}

END_FTK_NAMESPACE
