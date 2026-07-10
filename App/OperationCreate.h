#pragma once

#include "ftkConfigure.h"

#include "ftk/Core/Operation.h"

#include "DocumentContext.h"

BEGIN_FTK_NAMESPACE

template<typename T>
class OperationCreate : public core::Operation
{
public:
	OperationCreate(DocumentContext& context, std::shared_ptr<model::data::Node> parent, const base::String& nodeName)
		: Operation(true, _R("Create ") + base::String(_R(T::GetStaticTypeName())))
		, m_context(context)
		, m_parent(std::move(parent))
	{
		m_node = T::NewSPtr();
	}

	bool Do() override
	{
		if (!m_node || !m_parent)
		{
			return false;
		}
		m_context.getDocument()->addNode(m_node, m_parent);
		return true;
	}

	bool Undo() override
	{
		if (!m_node || !m_parent)
		{
			return false;
		}
		m_context.getDocument()->removeNode(m_node);
		return true;
	}

private:
	DocumentContext& m_context;
	std::shared_ptr<model::data::Node> m_node;
	std::shared_ptr<model::data::Node> m_parent;
};

END_FTK_NAMESPACE
