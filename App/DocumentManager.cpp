#include "DocumentManager.h"
#include "DocumentContext.h"

#include "ftk/Core/FilePath.h"
#include "ftk/Core/OperationManager.h"

BEGIN_FTK_NAMESPACE

DocumentManager::DocumentManager() = default;

DocumentManager::~DocumentManager() = default;

size_t DocumentManager::create()
{
	auto context = std::make_unique<DocumentContext>();
	if (context->create())
	{
		m_docs.push_back(std::move(context));
		return m_docs.size();
	}
	return npos;
}

size_t DocumentManager::count() const
{
	return m_docs.size();
}

DocumentContext& DocumentManager::get(size_t index)
{
	return *m_docs.at(index);
}

bool DocumentManager::open(const core::FilePath& url)
{
	auto context = std::make_unique<DocumentContext>();
	if (context->load(url))
	{
		m_docs.push_back(std::move(context));
		return true;
	}
	return false;
}

bool DocumentManager::open(const std::vector<core::FilePath>& urls)
{
	if (urls.empty())
		return false;
	for (auto& url : urls)
	{
		if (!open(url))
		{
			return false;
		}
	}
	return true;
}

bool DocumentManager::close(size_t index)
{
	m_docs.erase(m_docs.begin() + index);
	return true;
}

END_FTK_NAMESPACE
