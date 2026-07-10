#pragma once

#include "ftkConfigure.h"

#include "ftk/Base/Connection.h"

#include <functional>

BEGIN_FTK_NAMESPACE

namespace model::data
{
	class Node;
}

class ISelectionController
{
public:
	virtual ~ISelectionController() = default;

	virtual std::vector<model::data::Node*> selected() const = 0;
	
	virtual void select(const std::vector<model::data::Node*>& nodes) = 0;

	virtual void clearSelection() = 0;

	virtual bool isSelected(model::data::Node* node) const = 0;

	virtual base::Connection connectSelectionChanged(std::function<void(model::data::Node*)> fn) = 0;
};

END_FTK_NAMESPACE
