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

bool OperationCreateFactory::execute()
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

bool OperationCreateFactory::undo()
{
	if (!m_node)
	{
		return false;
	}
	m_context.getDocument()->removeNode(m_node);
	return true;
}

core::WithProperties::PropertyList OperationCreateFactory::getProperties()
{
	auto result = Operation::getProperties();
	result.push_back(makeProperty(_R("Node name"), [this]() {return mafStringToStd(m_node->GetName()); }, [this](const std::string& v) {if (auto nv = mafStdToString(v); nv != m_node->GetName()) { m_node->SetName(nv); m_valuesChanged.emit(); }}));
	return result;
}

bool OperationCreateFactory::isConfigured() const
{
	return !m_node->GetName().empty();
}

END_FTK_NAMESPACE
