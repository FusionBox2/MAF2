#pragma once

#include "ftkConfigure.h"

#include <ostream>

BEGIN_FTK_NAMESPACE

namespace utilities
{
	class FTK_BASE_EXPORT Indent final
	{
	public:
		Indent(int indent = 0);

		Indent GetNextIndent() const;

		int GetIndent() const { return m_Indent; }

		operator int() const { return GetIndent(); }
	private:
		int m_Indent = 0;
	};

	FTK_BASE_EXPORT std::ostream& operator<<(std::ostream& os, const Indent& indent);
}

using mafIndent = utilities::Indent;

END_FTK_NAMESPACE
