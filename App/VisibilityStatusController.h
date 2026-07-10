#pragma once

#include "ftkConfigure.h"

#include "ftk/Base/Signal.h"

#include "IStatusController.h"

#include <functional>

BEGIN_FTK_NAMESPACE

namespace model::data
{
	class Node;
}
class VTKViewModel;

class VisibilityStatusController : public IStatusController
{
public:
	VisibilityStatusController(std::shared_ptr<VTKViewModel> viewModel);

	virtual ~VisibilityStatusController() = default;

	virtual NodeStatus getStatus(model::data::Node* node) const;

	virtual base::Connection connectStatusChanged(std::function<void(model::data::Node*)> fn);

protected:
	std::shared_ptr<VTKViewModel> m_viewModel;
	base::Connection m_connection;
	base::Signal<model::data::Node*> m_statusChanged;
};

END_FTK_NAMESPACE
