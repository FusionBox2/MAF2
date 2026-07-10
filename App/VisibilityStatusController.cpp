#include "VisibilityStatusController.h"

#include "VTKViewModel.h"

BEGIN_FTK_NAMESPACE

VisibilityStatusController::VisibilityStatusController(std::shared_ptr<VTKViewModel> viewModel)
	: m_viewModel(std::move(viewModel))
{
	if (m_viewModel)
	{
		m_connection = m_viewModel->connectVisibilityChanged([this](model::data::Node* node) {m_statusChanged.emit(node); });
	}
}

NodeStatus VisibilityStatusController::getStatus(model::data::Node* node) const
{
	if (m_viewModel)
	{
		switch (m_viewModel->getVisibility(node))
		{
		case VTKViewModel::NodeVisibility::NODE_NON_VISIBLE:
			return 0;
		case VTKViewModel::NodeVisibility::NODE_VISIBLE_OFF:
			return 1;
		case VTKViewModel::NodeVisibility::NODE_VISIBLE_ON:
			return 2;
		case VTKViewModel::NodeVisibility::NODE_MUTEX_OFF:
			return 3;
		case VTKViewModel::NodeVisibility::NODE_MUTEX_ON:
			return 4;
		}
	}
	return -1;
}

base::Connection VisibilityStatusController::connectStatusChanged(std::function<void(model::data::Node*)> fn)
{
	return m_statusChanged.connect(fn);
}

END_FTK_NAMESPACE
