#pragma once

#include "ftkConfigure.h"

#include "ResourceId.h"

BEGIN_FTK_NAMESPACE

class Resource
{
public:
	virtual ~Resource();

	const ResourceId& id() const
	{
		return m_id;
	}

protected:
	explicit Resource(ResourceId id);

private:
	ResourceId m_id;
};

END_FTK_NAMESPACE
