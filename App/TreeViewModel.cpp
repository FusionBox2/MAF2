#include "TreeViewModel.h"

#include "ftk/Gui/ITreeViewNode.h"

#include "DocumentContext.h"
#include "ISelectionController.h"

#include "ftk/Core/Node.h"

#include "ftk/Core/OperationManager.h"

#include "ftk/Core/OperationChangeProperty.h"

#include "OpReparentNode.h"

#include <algorithm>

#include <deque>

BEGIN_FTK_NAMESPACE

namespace
{
	class TreeViewNode : public gui::ITreeViewNode
	{
	public:

		TreeViewNode(model::data::Node* node, TreeViewNode* parent)
			: m_model(node)
			, m_parent(parent)
		{
		}

		TreeViewNode* parent() const override
		{
			return m_parent;
		}

		std::vector<ITreeViewNode*> children() const override
		{
			std::vector<ITreeViewNode*> out;
			for (auto& c : m_children)
			{
				out.push_back(c.get());
			}
			return out;
		}

		model::data::Node* m_model = nullptr;
		TreeViewNode* m_parent = nullptr;
		std::vector<std::unique_ptr<TreeViewNode>> m_children;
	};

	void removeFromMapRecursive(TreeViewNode* node, std::unordered_map<model::data::Node*, gui::ITreeViewNode*>& nodeMap)
	{
		nodeMap.erase(node->m_model);
		for (auto& child : node->m_children)
		{
			removeFromMapRecursive(child.get(), nodeMap);
		}
	}
}

TreeViewModel::TreeViewModel(DocumentContext& context)
	: m_context(context)
{
	addTree(m_context.getDocument()->getRoot().get());
	subscribeToContext();
}

TreeViewModel::~TreeViewModel() = default;

TreeViewModel::NodeId TreeViewModel::root() const
{
	return m_root.get();
}

std::string TreeViewModel::getNodeName(NodeId node) const
{
	return mafStringToStd(getModelNode(node)->GetName());
}

int TreeViewModel::getNodeStatus(NodeId node) const
{
	return -1;
}

void TreeViewModel::renameNode(NodeId node, const std::string& name)
{
	auto nn = getModelNode(node);
	auto p =
		makeProperty
		(_R("name"), 
			[nn]() {return mafStringToStd(nn->GetName()); }, 
			[nn, this](const std::string& name) {nn->SetName(mafStdToString(name)); m_context.getDocument()->m_nodeChanged.emit({nn}); }
		);
	m_context.getOperationManager()->Submit(std::make_unique<core::OperationChangeProperty>(_R("Change property"), std::move(p), name));
}

void TreeViewModel::reparentNode(NodeId node, NodeId parent)
{
	m_context.getOperationManager()->Submit(std::make_unique<model::operations::ReparentNode>(_R("Reparent"), m_context, getModelNode(node)->SharedFromThis(), static_cast<TreeViewNode*>(parent)->m_model->SharedFromThis()));
}

void TreeViewModel::activateNode(NodeId node)
{
	if (onActivated)
	{
		onActivated(getModelNode(node));
	}
}

std::vector<gui::ITreeViewModel::NodeId> TreeViewModel::selected() const
{
	auto modelNodes = m_context.getSelectionController().selected();
	std::vector<NodeId> nodes;
	nodes.reserve(modelNodes.size());
	for (auto& modelNode : modelNodes)
	{
		nodes.push_back(getViewNode(modelNode));
	}
	return nodes;
}

void TreeViewModel::select(const std::vector<NodeId>& nodes)
{
	std::vector<model::data::Node*> modelNodes;
	modelNodes.reserve(nodes.size());
	for (auto& node : nodes)
	{
		modelNodes.push_back(getModelNode(node));
	}
	m_context.getSelectionController().select(modelNodes);
}

void TreeViewModel::clearSelection()
{
	m_context.getSelectionController().clearSelection();
}

bool TreeViewModel::isSelected(NodeId node) const
{
	return m_context.getSelectionController().isSelected(getModelNode(node));
}

base::Connection TreeViewModel::connectNodeAdded(std::function<void(NodeId)> fn)
{
	return m_nodeAdded.connect(fn);
}

base::Connection TreeViewModel::connectNodeRemoved(std::function<void(NodeId)> fn)
{
	return m_nodeRemoved.connect(fn);
}

base::Connection TreeViewModel::connectNodeMoved(std::function<void(NodeId)> fn)
{
	return m_nodeMoved.connect(fn);
}

base::Connection TreeViewModel::connectNodeChanged(std::function<void(NodeId)> fn)
{
	return m_nodeChanged.connect(fn);
}

gui::ITreeViewModel::NodeId TreeViewModel::getViewNode(model::data::Node* node) const
{
	if (auto it = m_nodeMap.find(node); it != m_nodeMap.end())
	{
		return it->second;
	}
	return nullptr;
}

model::data::Node* TreeViewModel::getModelNode(NodeId node) const
{
	return static_cast<TreeViewNode*>(node)->m_model;
}

void TreeViewModel::addNode(model::data::Node* node)
{
	if (!node)
	{
		return;
	}

	if (auto parentVN = static_cast<TreeViewNode*>(getViewNode(node->GetParent())))
	{
		parentVN->m_children.push_back(std::make_unique<TreeViewNode>(node, parentVN));
		m_nodeMap.emplace(node, parentVN->m_children.back().get());
	}
	else
	{
		m_root = std::make_unique<TreeViewNode>(node, nullptr);
		m_nodeMap.emplace(node, m_root.get());
	}
}

void TreeViewModel::addTree(model::data::Node* node)
{
	if (!node)
	{
		return;
	}

	for (auto queue = std::deque<model::data::Node*>(1, node); !queue.empty(); )
	{
		auto nextNode = queue.front();
		queue.pop_front();

		addNode(nextNode);

		for (size_t i = 0; i < nextNode->GetNumberOfChildren(); i++)
		{
			queue.push_back(nextNode->GetChild(i).get());
		}
	}
}

void TreeViewModel::subscribeToContext()
{
	m_connections.push_back(m_context.getDocument()->connectTreeAdded(
		[this](const NodeAdded& e)
		{
			addTree(e.node);
			for (auto queue = std::deque<NodeId>(1, getViewNode(e.node)); !queue.empty(); )
			{
				auto nextNode = queue.front();
				queue.pop_front();
				m_nodeAdded.emit(nextNode);
				for (auto& child : nextNode->children())
				{
					queue.push_back(child);
				}
			}
		}));

	m_connections.push_back(m_context.getDocument()->connectTreeRemoved(
		[this](const NodeRemoved& e)
		{
			auto vn = static_cast<TreeViewNode*>(getViewNode(e.node));
			removeFromMapRecursive(vn, m_nodeMap);
			if (auto parentVN = vn->parent())//vn != m_root.get()
			{
				auto it = std::find_if(begin(parentVN->m_children), end(parentVN->m_children), [vn](auto& p) {return p.get() == vn; });
				auto extracted = std::move(*it);//retain it till notification handled
				parentVN->m_children.erase(it);
			}
			else
			{
				m_root.reset();
			}
			m_nodeRemoved.emit(vn);
		}));

	m_connections.push_back(m_context.getDocument()->connectNodeAdded(
		[this](const NodeAdded& e)
		{
			addTree(e.node);
			m_nodeAdded.emit(getViewNode(e.node));
		}));

	m_connections.push_back(m_context.getDocument()->connectNodeRemoved(
		[this](const NodeRemoved& e)
		{
			auto vn = static_cast<TreeViewNode*>(getViewNode(e.node));
			m_nodeMap.erase(e.node);
			if (auto parentVN = vn->parent())//vn != m_root.get()
			{
				auto it = std::find_if(begin(parentVN->m_children), end(parentVN->m_children), [vn](auto& p) {return p.get() == vn; });
				auto extracted = std::move(*it);//retain it till notification handled
				parentVN->m_children.erase(it);
			}
			else
			{
				m_root.reset();
			}
			m_nodeRemoved.emit(vn);
		}));

	m_connections.push_back(m_context.getDocument()->connectTreeMoved(
		[this](const NodeMoved& e)
		{
			auto vn = static_cast<TreeViewNode*>(getViewNode(e.node));
			auto oldParentVN = vn->parent();
			auto newParentVN = static_cast<TreeViewNode*>(getViewNode(e.node->GetParent()));

			auto it = std::find_if(begin(oldParentVN->m_children), end(oldParentVN->m_children), [vn](auto& p) {return p.get() == vn; });

			newParentVN->m_children.push_back(std::move(*it));
			oldParentVN->m_children.erase(it);
			auto raw = newParentVN->m_children.back().get();
			raw->m_parent = newParentVN;
			m_nodeMoved.emit(raw);
		}));

	m_connections.push_back(m_context.getDocument()->connectNodeChanged(
		[this](const NodeChanged& e)
		{
			auto vn = static_cast<TreeViewNode*>(getViewNode(e.node));
			m_nodeChanged.emit(vn);
		}));

	m_connections.push_back(m_context.connectStatusChanged(
		[this](const NodeStatusChanged& e)
		{
			auto vn = static_cast<TreeViewNode*>(getViewNode(e.node));
			m_nodeChanged.emit(vn);
		}));

	m_connections.push_back(m_context.getSelectionController().connectSelectionChanged(
		[this](model::data::Node* node)
		{
			if (auto vn = static_cast<TreeViewNode*>(getViewNode(node)))
			{
			}
		}));
}

END_FTK_NAMESPACE
