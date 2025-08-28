#include "mafIndent.h"

#include <ostream>

namespace
{
	constexpr int MAF_STD_INDENT = 2;
	constexpr int MAF_NUMBER_OF_TABS = 20;

	constexpr char blanks[MAF_NUMBER_OF_TABS * MAF_STD_INDENT + 1] = "                                        ";
}

BEGIN_FTK_NAMESPACE

namespace utilities
{
	Indent::Indent(int indent) : m_Indent(indent) {}

	Indent Indent::GetNextIndent() const
	{
		return std::min(m_Indent + 1, MAF_NUMBER_OF_TABS);
	}

	std::ostream& operator<<(std::ostream& os, const Indent& indent)
	{
		os << blanks + (MAF_NUMBER_OF_TABS - indent.GetIndent()) * MAF_STD_INDENT;
		return os;
	}
}

END_FTK_NAMESPACE
