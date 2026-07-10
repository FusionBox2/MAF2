#pragma once

#include "ftkConfigure.h"

#include <vector>

BEGIN_FTK_NAMESPACE

namespace gui
{
	class ITreeViewNode
	{
	public:
		virtual ~ITreeViewNode() = default;

		virtual ITreeViewNode* parent() const = 0;

		virtual std::vector<ITreeViewNode*> children() const = 0;
	};
}

END_FTK_NAMESPACE
