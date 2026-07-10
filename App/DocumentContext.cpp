#include "DocumentContext.h"

#include "Document.h"

#include "DocumentSelection.h"
#include "ftk/Core/OperationManager.h"

#include "ftk/IO/StorageElement.h"
#include "ftk/Core/NodeManager.h"
#include "ftk/Core/Node.h"
#include "mafStorage.h"

BEGIN_FTK_NAMESPACE

DocumentContext::DocumentContext()
{
	m_operationManager = std::make_unique<core::OperationManager>();
}

DocumentContext::~DocumentContext() = default;

bool DocumentContext::create()
{
	if (m_model || !m_url.empty())
	{
		return false;
	}
	m_model = std::make_shared<Document>(std::make_unique<model::data::NodeManager>(), nullptr);
	m_resourceManager = std::make_unique<ResourceManager>();
	m_selection = std::make_unique<DocumentSelection>(*this);
	return true;
}

bool DocumentContext::load(const core::FilePath& url)
{
	if (m_model || !m_url.empty())
	{
		return false;
	}
	auto storage = std::make_unique<mafStorage>();
	auto nm = std::make_unique<model::data::NodeManager>();
	storage->SetManager(nm.get());
	storage->SetURL(url.asString());
	int res = storage->Restore();

	io::Reader reader(_R("MSF"), _R("2.2"));
	if (res == MAF_OK || res == mafStorage::IO_WRONG_OBJECT_TYPE)
	{
		m_model = std::make_shared<Document>(std::move(nm), std::move(storage));
		m_model->postLoad();
		m_selection = std::make_unique<DocumentSelection>(*this);
		return true;
	}
	return false;
}

bool DocumentContext::save(const core::FilePath& url)
{
	if (!m_model)
	{
		return false;
	}
	io::Writer writer(_R("MSF"), _R("2.2"));
	auto root = writer.GetRoot();
	m_model->getManager()->Store(root);
	writer.Save(url.asString());
	m_url = url;
	m_operationManager->MarkAsSaved();
	return true;
}

std::shared_ptr<Document> DocumentContext::getDocument()
{
	return m_model;
}

const core::FilePath& DocumentContext::getURL() const
{
	return m_url;
}

core::OperationManager* DocumentContext::getOperationManager() const
{
	return m_operationManager.get();
}

ISelectionController& DocumentContext::getSelectionController() const
{
	return *m_selection;
}

base::Connection DocumentContext::connectStatusChanged(std::function<void(const NodeStatusChanged&)> fn)
{
	return m_statusChanged.connect(fn);
}

END_FTK_NAMESPACE
