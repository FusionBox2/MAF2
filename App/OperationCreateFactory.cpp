#include "OperationCreateFactory.h"

#include "ISelectionController.h"
#include "ftk/Core/NodeFactory.h"

BEGIN_FTK_NAMESPACE

OperationCreateFactory::OperationCreateFactory(const base::String& name, const base::String& typeName, DocumentContext& context, const base::String& nodeName)
	: Operation(name)
	, m_context(context)
{
	m_node = NodeFactory::CreateNode(base::StringToStd(typeName).c_str());
	m_node->SetName(nodeName);
}

bool OperationCreateFactory::Do()
{
	if (!m_node)
	{
		return false;
	}
	if (auto sel = m_context.getSelectionController().selected(); sel.size() == 1)
	{
		m_context.getDocument()->addNode(m_node, sel[0]->SharedFromThis());
		return true;
	}
	return false;
}

bool OperationCreateFactory::Undo()
{
	if (!m_node)
	{
		return false;
	}
	m_context.getDocument()->removeNode(m_node);
	return true;
}

END_FTK_NAMESPACE
