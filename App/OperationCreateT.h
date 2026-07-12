#pragma once

#include "ftkConfigure.h"

#include "ftk/Core/Operation.h"

#include "DocumentContext.h"
#include "ISelectionController.h"

BEGIN_FTK_NAMESPACE

template<typename T>
class OperationCreateT : public core::Operation
{
public:
	OperationCreateT(const base::String& name, const base::String& typeName, DocumentContext& context, const base::String& nodeName)
		: Operation(name)
		, m_context(context)
	{
		m_node = T::NewSPtr();
		m_node->SetName(nodeName);
	}

	bool execute() override
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

	bool undo() override
	{
		if (!m_node)
		{
			return false;
		}
		m_context.getDocument()->removeNode(m_node);
		return true;
	}

	PropertyList getProperties() override
	{
		auto result = Operation::getProperties();
		result.push_back(makeProperty(_R("Node name"), [this]() {return mafStringToStd(m_node->GetName()); }, [this](const std::string& v) {if (auto nv = mafStdToString(v); nv != m_node->GetName()) { m_node->SetName(nv); m_valuesChanged.emit(); }}));
		return result;
	}

	bool isConfigured() const override
	{
		return !m_node->GetName().empty();
	}

private:
	DocumentContext& m_context;
	std::shared_ptr<model::data::Node> m_node;
};

END_FTK_NAMESPACE
