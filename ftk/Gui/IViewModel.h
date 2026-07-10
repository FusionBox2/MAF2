#pragma once

#include "ftkConfigure.h"

#include "ftk/Core/WithProperties.h"

BEGIN_FTK_NAMESPACE

namespace gui
{
	class IViewModel : public core::WithProperties
	{
	public:
		~IViewModel() override;
	};
}

END_FTK_NAMESPACE
