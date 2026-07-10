#pragma once

#include "ftkConfigure.h"

#include "TreeViewModel.h"

#include "IStatusController.h"

BEGIN_FTK_NAMESPACE

struct NodeActivated
{
	model::data::Node* node;
};

class TreeStatusViewModel : public TreeViewModel
{
public:
	TreeStatusViewModel(DocumentContext& context);

	~TreeStatusViewModel() override;

	void setStatusController(std::shared_ptr<IStatusController> status);

	int getNodeStatus(gui::ITreeViewNode* node) const override;

private:
	void emitForAll(NodeId root) const;

	std::shared_ptr<IStatusController> m_status;
	base::Connection m_statusConnection;
};

END_FTK_NAMESPACE
