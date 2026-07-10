#pragma once

#include "ftkConfigure.h"

#include "ftk/Base/String.h"
#include "ftk/Core/Operation.h"

#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>

BEGIN_FTK_NAMESPACE

class DocumentContext;

class OperationsRegistry
{
public:
	enum class OperationType
	{
		Normal,
		Importer,
		Exporter,
		Edit
	};

	std::unique_ptr<core::Operation> createOperation(const base::String& name, DocumentContext& context) const;

	void registerOperation(const base::String& name, std::function<std::unique_ptr<core::Operation>(DocumentContext& context)> fn);

	std::vector<base::String> operations() const;

	OperationType type(const base::String& name) const;

	bool canExecute(const base::String& name, DocumentContext& context) const;

private:
	std::unordered_map<base::String, std::function<std::unique_ptr<core::Operation>(DocumentContext& context)> > m_registry;
};

END_FTK_NAMESPACE
