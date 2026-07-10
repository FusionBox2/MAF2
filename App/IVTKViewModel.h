#pragma once

#include "ftkConfigure.h"

#include "ftk/Gui/IViewModel.h"

#include "ftk/Base/Connection.h"

#include <array>

BEGIN_FTK_NAMESPACE

class IVTKViewNode;

class IVTKViewModel : public gui::IViewModel
{
public:
	using NodeId = IVTKViewNode*;

	virtual NodeId root() const = 0;

	virtual base::Connection connectSceneUpdated(std::function<void()> fn) = 0;

	virtual base::Connection connectSceneReset(std::function<void(const std::array<double, 6>&)> fn) = 0;
};

END_FTK_NAMESPACE
