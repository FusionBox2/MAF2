#pragma once

#include "ftkConfigure.h"

#include "ftk/Core/Operation.h"

#include "DocumentContext.h"

BEGIN_FTK_NAMESPACE

class OperationCreateFactory : public core::Operation
{
public:
	OperationCreateFactory(const base::String& name, const base::String& typeName, DocumentContext& context, const base::String& nodeName);

	bool Do() override;

	bool Undo() override;

	PropertyList getProperties() override;

	bool IsConfigured() const override;

private:
	DocumentContext& m_context;
	std::shared_ptr<model::data::Node> m_node;
	base::String m_nodeName;
};

END_FTK_NAMESPACE
