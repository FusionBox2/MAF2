#pragma once

#include "ftkConfigure.h"

#include "ftk/Base/Signal.h"

#include <memory>
#include <unordered_map>

BEGIN_FTK_NAMESPACE

namespace model::data
{
	class Node;
	class NodeManager;
}
class mafStorage;

struct NodeAdded
{
	//std::shared_ptr<model::data::Node> parent;
	model::data::Node* node;
};

struct NodeRemoved
{
	//std::shared_ptr<model::data::Node> parent;
	model::data::Node* node;
};

struct NodeMoved
{
	model::data::Node* node;
	//std::shared_ptr<model::data::Node> oldParent;
	//std::shared_ptr<model::data::Node> newParent;
};

struct NodeChanged
{
	model::data::Node* node;
};


class Document
{
public:
	Document(std::unique_ptr<model::data::NodeManager> node, std::unique_ptr<mafStorage> storage);

	Document(const Document&) = default;

	Document& operator=(const Document&) = default;

	Document(Document&&) = default;

	Document& operator=(Document&&) = default;

	~Document();

	std::shared_ptr<model::data::Node> getRoot();

	model::data::NodeManager* getManager();

	void addNode(std::shared_ptr<model::data::Node> node, std::shared_ptr<model::data::Node> parent);

	void removeNode(std::shared_ptr<model::data::Node> node);

	void moveNode(std::shared_ptr<model::data::Node> node, std::shared_ptr<model::data::Node> newParent);

	void postLoad();

	base::Connection connectNodeAdded(std::function<void(const NodeAdded&)> fn);

	base::Connection connectNodeRemoved(std::function<void(const NodeRemoved&)> fn);

	base::Connection connectNodeMoved(std::function<void(const NodeMoved&)> fn);

	base::Connection connectNodeChanged(std::function<void(const NodeChanged&)> fn);

	//Connection connectNodeAdded(std::function<void(model::data::Node*)> fn);

	//Connection connectNodeRemoved(std::function<void(model::data::Node*)> fn);

	//Connection connectNodeMoved(std::function<void(model::data::Node*)> fn);

	//Connection connectNodeChanged(std::function<void(model::data::Node*)> fn);

//private:
	base::Signal<const NodeAdded&> m_nodeAdded;
	base::Signal<const NodeRemoved&> m_nodeRemoved;
	base::Signal<const NodeMoved&> m_nodeMoved;
	base::Signal<const NodeChanged&> m_nodeChanged;
	std::unordered_map<model::data::Node*, base::Connection> m_connections;
	std::unique_ptr<model::data::NodeManager> m_manager;
	std::unique_ptr<mafStorage> m_storage;
};

END_FTK_NAMESPACE
