#include "Document.h"

#include "ftk/Core/NodeManager.h"
#include "ftk/Core/Node.h"
#include "mafStorage.h"

BEGIN_FTK_NAMESPACE

Document::Document(std::unique_ptr<model::data::NodeManager> nodeManager, std::unique_ptr<mafStorage> storage)
	: m_manager(std::move(nodeManager))
	, m_storage(std::move(storage))
{
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
	m_connections.emplace(p, p->connectNodeChanged([this, p]() {m_nodeChanged.emit({ p }); }));
	m_nodeAdded.emit({ node.get() });
}

void Document::removeNode(std::shared_ptr<model::data::Node> node)
{
	model::data::Node::ReparentTo(node, nullptr);
	m_nodeRemoved.emit({ node.get() });
}

void Document::moveNode(std::shared_ptr<model::data::Node> node, std::shared_ptr<model::data::Node> newParent)
{
	model::data::Node::ReparentTo(node, newParent.get());
	m_nodeMoved.emit({ node.get() });
}

void Document::postLoad()
{
	for (auto& n : *getRoot())
	{
		auto p = &n;
		m_connections.emplace(&n, n.connectNodeChanged([this, p]() {m_nodeChanged.emit({p}); }));
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

base::Connection Document::connectNodeMoved(std::function<void(const NodeMoved&)> fn)
{
	return m_nodeMoved.connect(fn);
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

END_FTK_NAMESPACE
