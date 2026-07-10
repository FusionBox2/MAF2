#pragma once

#include "ftkConfigure.h"

#include <memory>
#include <vector>

BEGIN_FTK_NAMESPACE

class IProperty;

class IPropertiesBuilder
{
public:
	virtual ~IPropertiesBuilder() = default;

	virtual std::vector<std::unique_ptr<IProperty>> getProperties() const = 0;
};

END_FTK_NAMESPACE
