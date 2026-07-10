#pragma once

#include "ftkConfigure.h"

#include "ftk/Base/Signal.h"
#include "ftk/Gui/ITreeViewModel.h"

#include <memory>
#include <unordered_map>
#include <vector>

BEGIN_FTK_NAMESPACE

class DocumentContext;
namespace model::data
{
	class Node;
}

class TreeViewModel : public gui::ITreeViewModel
{
public:
	TreeViewModel(DocumentContext& context);

	~TreeViewModel() override;

	NodeId root() const override;

	std::string getNodeName(NodeId node) const override;

	int getNodeStatus(NodeId node) const override;

	void reparentNode(NodeId node, NodeId parent) override;

	void renameNode(NodeId node, const std::string& name) override;

	void activateNode(NodeId node) override;

	std::vector<NodeId> selected() const override;

	void select(const std::vector<NodeId>& node) override;

	void clearSelection() override;

	bool isSelected(NodeId node) const override;

	base::Connection connectNodeAdded(std::function<void(NodeId)>) override;

	base::Connection connectNodeRemoved(std::function<void(NodeId)>) override;

	base::Connection connectNodeMoved(std::function<void(NodeId)>) override;

	base::Connection connectNodeChanged(std::function<void(NodeId)>) override;

	base::Connection connectSelectionChanged(std::function<void(NodeId)> fn) override;

	std::function<void(model::data::Node*)> onActivated;

protected:
	NodeId getViewNode(model::data::Node* node) const;

	model::data::Node* getModelNode(NodeId node) const;

	std::vector<base::Connection> m_connections;

	base::Signal<NodeId> m_nodeAdded;
	base::Signal<NodeId> m_nodeRemoved;
	base::Signal<NodeId> m_nodeMoved;
	base::Signal<NodeId> m_nodeChanged;
	base::Signal<NodeId> m_nodeSelectionChanged;

private:
	void buildTree();
	
	void subscribeToContext();

	DocumentContext& m_context;
	std::unique_ptr<gui::ITreeViewNode> m_root;
	std::unordered_map<model::data::Node*, NodeId> m_nodeMap;
};

END_FTK_NAMESPACE
