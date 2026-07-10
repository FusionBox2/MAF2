#include "ResourceManager.h"

BEGIN_FTK_NAMESPACE

void ResourceManager::registerResource(ResourceInfo resource)
{
	auto id = resource.id;
	m_resources[id] = std::move(resource);
}

void ResourceManager::unregisterResource(const ResourceInfo& resource)
{
	m_resources.erase(resource.id);
}

const ResourceInfo* ResourceManager::resource(const ResourceId& id) const
{
	if (auto it = m_resources.find(id); it != m_resources.end())
	{
		return &it->second;
	}
	return nullptr;
}

Resource* ResourceManager::loadData(const ResourceInfo& info) const
{
	return nullptr;
}

END_FTK_NAMESPACE
