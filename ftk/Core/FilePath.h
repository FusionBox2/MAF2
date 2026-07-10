#pragma once

#include "ftkConfigure.h"

#include "ftk/Base/String.h"

BEGIN_FTK_NAMESPACE

namespace core
{
	class FilePath
	{
	public:
		FilePath() = default;

		explicit FilePath(const base::String& path);

		FilePath& operator/=(const FilePath& path);

		base::String asString() const;

		bool empty() const;

	private:
		base::String m_filePath;
	};

	FilePath operator/(const FilePath& p1, const FilePath& p2);
}

END_FTK_NAMESPACE
