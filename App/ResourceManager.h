#pragma once

#include "ftkConfigure.h"

#include "Resource.h"
#include "ResourceId.h"
#include "ResourceInfo.h"

#include <memory>
#include <unordered_map>

BEGIN_FTK_NAMESPACE

class ResourceManager
{
public:

	void registerResource(ResourceInfo resource);

	void unregisterResource(const ResourceInfo& resource);

	const ResourceInfo* resource(const ResourceId& id) const;

	Resource* data(const ResourceId& id);

private:
	Resource* loadData(const ResourceInfo& info) const;

	std::unordered_map<ResourceId, ResourceInfo> m_resources;
	std::unordered_map<ResourceId, std::unique_ptr<Resource>> m_loadedResources;
};

END_FTK_NAMESPACE
