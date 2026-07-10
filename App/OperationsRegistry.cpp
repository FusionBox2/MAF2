#include "OperationsRegistry.h"

#include "DocumentContext.h"
#include "ISelectionController.h"
#include "mafOp.h"

BEGIN_FTK_NAMESPACE

std::unique_ptr<core::Operation> OperationsRegistry::createOperation(const base::String& name, DocumentContext& context) const
{
	if (auto it = m_registry.find(name); it != end(m_registry))
	{
		return it->second(context);
	}
	return nullptr;
}

void OperationsRegistry::registerOperation(const base::String& name, std::function<std::unique_ptr<core::Operation>(DocumentContext& context)> fn)
{
	m_registry.emplace(name, std::move(fn));
}

std::vector<base::String> OperationsRegistry::operations() const
{
	std::vector<base::String> result;
	for (auto& entry : m_registry)
	{
		result.push_back(entry.first);
	}
	return result;
}

OperationsRegistry::OperationType OperationsRegistry::type(const base::String& name) const
{
	DocumentContext context;
	context.create();
	if (auto op = createOperation(name, context))
	{
		if (auto mo = dynamic_cast<mafOp*>(op.get()))
		{
			if (mo->GetType() == OPTYPE_IMPORTER)
			{
				return OperationType::Importer;
			}
				
			if (mo->GetType() == OPTYPE_EXPORTER)
			{
				return OperationType::Exporter;
			}

			if (mo->GetType() == OPTYPE_EDIT)
			{
				return OperationType::Edit;
			}
		}
	}
	return OperationType::Normal;
}

bool OperationsRegistry::canExecute(const base::String& name, DocumentContext& context) const
{
	if (auto op = createOperation(name, context))
	{
		if (auto mo = dynamic_cast<mafOp*>(op.get()))
		{
			if (auto sel = context.getSelectionController().selected(); sel.size() == 1)
			{
				return mo->Accept(sel[0]);
			}
			return false;
		}
		return op->CanDo();
	}
	return false;
}

END_FTK_NAMESPACE
