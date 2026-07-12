#include "Document.h"

#include "ftk/Core/NodeManager.h"
#include "ftk/Core/Node.h"
#include "mafStorage.h"


#include "Gui/mafEvent.h"

BEGIN_FTK_NAMESPACE

void Document::OnEvent(mafEventBase* event)
{
	if (auto e = mafEvent::SafeDownCast(event))
	{
		mafID eventId = e->GetId();
		if (VME_ADDED == eventId)
		{
			m_nodeAdded.emit({ e->GetVme() });
		}
		if (VME_REMOVING == eventId)
		{
			m_nodeRemoved.emit({e->GetVme()});
		}
	}
}

Document::Document(std::unique_ptr<model::data::NodeManager> nodeManager, std::unique_ptr<mafStorage> storage)
	: m_manager(std::move(nodeManager))
	, m_storage(std::move(storage))
{
	if (m_storage)
		m_storage->SetListener(this);
	else
		m_manager->SetListener(this);
}

Document::~Document() = default;

std::shared_ptr<model::data::Node> Document::getRoot()
{
	return m_manager->GetRoot();
}

model::data::NodeManager* Document::getManager()
{
	return m_manager.get();
}

void Document::addNode(std::shared_ptr<model::data::Node> node, std::shared_ptr<model::data::Node> parent)
{
	model::data::Node::ReparentTo(node, parent.get());
	auto p = node.get();
	m_connections.emplace(p, p->connectValuesChanged([this, p]() {m_nodeChanged.emit({ p }); }));
	recursiveEmitAdded(node.get());
	//m_treeAdded.emit({ node.get() });
}

void Document::removeNode(std::shared_ptr<model::data::Node> node)
{
	model::data::Node::ReparentTo(node, nullptr);
	recursiveEmitRemoved(node.get());
	//m_treeRemoved.emit({ node.get() });
}

void Document::moveNode(std::shared_ptr<model::data::Node> node, std::shared_ptr<model::data::Node> newParent)
{
	model::data::Node::ReparentTo(node, newParent.get());
	m_treeMoved.emit({ node.get() });
}

void Document::postLoad()
{
	for (auto& n : *getRoot())
	{
		auto p = &n;
		m_connections.emplace(&n, n.connectValuesChanged([this, p]() {m_nodeChanged.emit({p}); }));
	}
}

base::Connection Document::connectNodeAdded(std::function<void(const NodeAdded&)> fn)
{
	return m_nodeAdded.connect(fn);
}

base::Connection Document::connectNodeRemoved(std::function<void(const NodeRemoved&)> fn)
{
	return m_nodeRemoved.connect(fn);
}

base::Connection Document::connectTreeAdded(std::function<void(const NodeAdded&)> fn)
{
	return m_treeAdded.connect(fn);
}

base::Connection Document::connectTreeRemoved(std::function<void(const NodeRemoved&)> fn)
{
	return m_treeRemoved.connect(fn);
}

base::Connection Document::connectTreeMoved(std::function<void(const NodeMoved&)> fn)
{
	return m_treeMoved.connect(fn);
}

base::Connection Document::connectNodeChanged(std::function<void(const NodeChanged&)> fn)
{
	return m_nodeChanged.connect(fn);
}

/*Connection Document::connectNodeAdded(std::function<void(model::data::Node*)> fn)
{
	return m_nodeAdded.connect(fn);
}

Connection Document::connectNodeRemoved(std::function<void(model::data::Node*)> fn)
{
	return m_nodeRemoved.connect(fn);
}

Connection Document::connectNodeMoved(std::function<void(model::data::Node*)> fn)
{
	return m_nodeMoved.connect(fn);
}

Connection Document::connectNodeChanged(std::function<void(model::data::Node*)> fn)
{
	return m_nodeChanged.connect(fn);
}*/

void Document::recursiveEmitAdded(model::data::Node* node)
{
	m_nodeAdded.emit({ node });

	for (size_t i = 0, numChildren = node->GetNumberOfChildren(); i < numChildren; ++i)
	{
		recursiveEmitAdded(node->GetChild(i).get());
	}
}

void Document::recursiveEmitRemoved(model::data::Node* node)
{
	for (size_t i = 0, numChildren = node->GetNumberOfChildren(); i < numChildren; ++i)
	{
		recursiveEmitAdded(node->GetChild(numChildren - i - 1).get());
	}

	m_nodeRemoved.emit({ node });
}

END_FTK_NAMESPACE
