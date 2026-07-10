#include "Resource.h"

BEGIN_FTK_NAMESPACE

Resource::~Resource() = default;

Resource::Resource(ResourceId id)
	: m_id(std::move(id))
{
}

END_FTK_NAMESPACE
