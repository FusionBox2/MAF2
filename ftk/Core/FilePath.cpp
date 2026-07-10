#include "FilePath.h"

#include <algorithm>

BEGIN_FTK_NAMESPACE

namespace core
{
	FilePath::FilePath(const base::String& path)
		: m_filePath(path)
	{
		for (size_t i = 0; i < m_filePath.size(); i++)
		{
			if (m_filePath[i] == '\\')
			{
				m_filePath[i] = '/';
			}
		}
	}

	base::String FilePath::asString() const
	{
		return m_filePath;
	}

	FilePath& FilePath::operator/=(const FilePath& path)
	{
		if (m_filePath.ends_with(_R("/")))
		{
			m_filePath.erase(m_filePath.size() - 1);
		}
		if (path.m_filePath.starts_with(_R("/")))
		{
			m_filePath += path.m_filePath.substr(1);
		}
		else
		{
			m_filePath += path.m_filePath;
		}
		return *this;
	}

	bool FilePath::empty() const
	{
		return m_filePath.empty();
	}

	FilePath operator/(const FilePath& p1, const FilePath& p2)
	{
		FilePath result(p1);
		return result /= p2;
	}
}

END_FTK_NAMESPACE
