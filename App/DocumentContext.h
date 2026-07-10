#pragma once

#include "ftkConfigure.h"

#include "Document.h"
#include "ResourceManager.h"

#include "ftk/Base/Signal.h"
#include "ftk/Core/FilePath.h"

#include <functional>
#include <memory>
#include <string>

BEGIN_FTK_NAMESPACE

namespace core
{
	class OperationManager;
}
class ISelectionController;

using ViewId = uint64_t;

struct NodeStatusChanged
{
	model::data::Node* node;
	//ViewId view;
};

class DocumentContext
{
public:
	using GetSaveAsFileFn = std::function<std::string()>;

	DocumentContext();

	DocumentContext(const DocumentContext&) = delete;

	DocumentContext& operator=(const DocumentContext&) = delete;

	DocumentContext(DocumentContext&&) = delete;

	DocumentContext& operator=(DocumentContext&&) = delete;

	~DocumentContext();

	bool create();

	bool load(const core::FilePath& url);

	bool save(const core::FilePath& url);

	std::shared_ptr<Document> getDocument();

	const core::FilePath& getURL() const;

	core::OperationManager* getOperationManager() const;

	ISelectionController& getSelectionController() const;

	base::Connection connectStatusChanged(std::function<void(const NodeStatusChanged&)> fn);

private:
	std::shared_ptr<Document> m_model;
	std::unique_ptr<ResourceManager> m_resourceManager;
	std::unique_ptr<ISelectionController> m_selection;
	std::unique_ptr<core::OperationManager> m_operationManager;
	core::FilePath m_url;
	GetSaveAsFileFn m_requestSaveAs;
	base::Signal<const NodeStatusChanged&> m_statusChanged;
};

END_FTK_NAMESPACE
