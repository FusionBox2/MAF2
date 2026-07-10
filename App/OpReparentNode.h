#pragma once

#include "ftkConfigure.h"

#include "ftk/Core/Operation.h"

#include "DocumentContext.h"

BEGIN_FTK_NAMESPACE

namespace model::operations
{

	class ReparentNode : public core::Operation
	{
	public:
		ReparentNode(const base::String& name, DocumentContext& context, std::shared_ptr<model::data::Node> node, std::shared_ptr<model::data::Node> newParent);

		bool Do() override;

		bool Undo() override;

	private:
		DocumentContext& m_context;
		std::shared_ptr<model::data::Node> m_node;
		std::shared_ptr<model::data::Node> m_newParent;
		std::shared_ptr<model::data::Node> m_oldParent;
	};
}
END_FTK_NAMESPACE
