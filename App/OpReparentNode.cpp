#include "OpReparentNode.h"

#include "ftk/Core/Node.h"

BEGIN_FTK_NAMESPACE

namespace model::operations
{
	ReparentNode::ReparentNode(const base::String& name, DocumentContext& context, std::shared_ptr<model::data::Node> node, std::shared_ptr<model::data::Node> newParent)
		: Operation(name)
		, m_context(context)
		, m_node(std::move(node))
		, m_newParent(std::move(newParent))
	{
	}

	bool ReparentNode::Do()
	{
		m_oldParent = m_node->GetParent()->SharedFromThis();
		m_context.getDocument()->moveNode(m_node, m_newParent);
		return true;
	}

	bool ReparentNode::Undo()
	{
		m_context.getDocument()->moveNode(m_node, m_oldParent);
		return true;
	}
}

END_FTK_NAMESPACE
