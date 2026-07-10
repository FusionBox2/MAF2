#include "TreeStatusViewModel.h"

#include "ftk/Gui/ITreeViewNode.h"

BEGIN_FTK_NAMESPACE

TreeStatusViewModel::TreeStatusViewModel(DocumentContext& context)
	: TreeViewModel(context)
{
}

TreeStatusViewModel::~TreeStatusViewModel() = default;

void TreeStatusViewModel::setStatusController(std::shared_ptr<IStatusController> status)
{
	m_statusConnection = !status ? base::Connection() : status->connectStatusChanged([this](model::data::Node* node)
		{
			m_nodeChanged.emit(getViewNode(node));
		});
	m_status = std::move(status);
	emitForAll(root());
}

int TreeStatusViewModel::getNodeStatus(gui::ITreeViewNode* node) const
{
	return m_status ? m_status->getStatus(getModelNode(node)) : -1;
}

void TreeStatusViewModel::emitForAll(NodeId root) const
{
	m_nodeChanged.emit(root);
	for (auto& child : root->children())
	{
		emitForAll(child);
	}
}

END_FTK_NAMESPACE
