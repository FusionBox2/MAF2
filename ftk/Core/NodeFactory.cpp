#include "ftk/Core/NodeFactory.h"

#include "mafNodeLayout.h"

int NodeFactory::Initialize()
{
  mafPlugNodeMacro(mafNodeLayout,"node for layout list");
  return MAF_OK;
}

NodeCreateType NodeFactory::CreateNode(const char* ClassName)
{
  auto pos = GetFactory().m_creatorsMap.find(ClassName);
  if (pos != GetFactory().m_creatorsMap.end())
  {
    return pos->second.m_CreateObject();
  }
  return nullptr;
}

const char* NodeFactory::GetDescription(const char* ClassName)
{
  auto pos = GetFactory().m_creatorsMap.find(ClassName);
  if (pos != GetFactory().m_creatorsMap.end())
  {
    return pos->second.m_Description.c_str();
  }
  return nullptr;
}

void NodeFactory::RegisterNewNode(const char* ClassName, const char* description, CreateNodeFunction createFunction)
{
  CreateInformation info;
  info.m_Description = description;
  info.m_CreateObject = createFunction;
  GetFactory().m_creatorsMap[ClassName] = info;
}

NodeFactory& NodeFactory::GetFactory()
{
  static NodeFactory instance;
  return instance;
}
