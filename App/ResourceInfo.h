#pragma once

#include "ftkConfigure.h"

#include "ResourceId.h"

#include <string>

BEGIN_FTK_NAMESPACE

struct ResourceInfo
{
	ResourceId id;

	std::string type;

	std::string location;
};

END_FTK_NAMESPACE
