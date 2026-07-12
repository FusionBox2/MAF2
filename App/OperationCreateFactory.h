#pragma once

#include "ftkConfigure.h"

#include "ftk/Core/Operation.h"

#include "DocumentContext.h"

BEGIN_FTK_NAMESPACE

class OperationCreateFactory : public core::Operation
{
public:
	OperationCreateFactory(const base::String& name, const base::String& typeName, DocumentContext& context, const base::String& nodeName);

	bool execute() override;

	bool undo() override;

	PropertyList getProperties() override;

	bool isConfigured() const override;

private:
	DocumentContext& m_context;
	std::shared_ptr<model::data::Node> m_node;
};

END_FTK_NAMESPACE
