#pragma once

#include "ftkConfigure.h"

#include "ftk/Base/Connection.h"

#include <functional>

BEGIN_FTK_NAMESPACE

namespace model::data
{
	class Node;
}

using NodeStatus = int;

class IStatusController
{
public:
	virtual ~IStatusController() = default;

	virtual NodeStatus getStatus(model::data::Node* node) const = 0;

	virtual base::Connection connectStatusChanged(std::function<void(model::data::Node*)> fn) = 0;
};

END_FTK_NAMESPACE
