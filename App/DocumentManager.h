#pragma once

#include "ftkConfigure.h"

#include "ftk/Core/FilePath.h"

#include <functional>
#include <memory>
#include <vector>

BEGIN_FTK_NAMESPACE

class DocumentContext;

class DocumentManager
{
public:
	static const size_t npos = static_cast<size_t>(-1);

	DocumentManager();

	DocumentManager(const DocumentManager&) = delete;

	DocumentManager& operator=(const DocumentManager&) = delete;

	DocumentManager(DocumentManager&&) = default;

	DocumentManager& operator=(DocumentManager&&) = default;

	~DocumentManager();

	size_t count() const;

	size_t create();

	DocumentContext& get(size_t index);

	bool open(const core::FilePath& url);

	bool open(const std::vector<core::FilePath>& urls);

	bool close(size_t index);

private:
	std::vector<std::unique_ptr<DocumentContext> > m_docs;
};

END_FTK_NAMESPACE
