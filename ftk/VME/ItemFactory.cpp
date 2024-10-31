#include "ftk/VME/ItemFactory.h"

#include "mafVMEItemVTK.h"
#include "mafVMEItemScalarMatrix.h"

int ItemFactory::Initialize()
{
  mafPlugItemMacro(mafVMEItemVTK, "a VME item storing VTK datasets");
#ifdef MAF_USE_ITK
  mafPlugItemMacro(mafVMEItemScalarMatrix, "a VME item storing matrix scalar data through vnl matrix");
#endif
  return MAF_OK;
}

ItemCreateType ItemFactory::CreateItem(const char* ClassName)
{
  auto pos = GetFactory().m_creatorsMap.find(ClassName);
  if (pos != GetFactory().m_creatorsMap.end())
  {
    return pos->second.m_CreateObject();
  }
  return nullptr;
}

const char* ItemFactory::GetDescription(const char* ClassName)
{
  auto pos = GetFactory().m_creatorsMap.find(ClassName);
  if (pos != GetFactory().m_creatorsMap.end())
  {
    return pos->second.m_Description.c_str();
  }
  return nullptr;
}

void ItemFactory::RegisterNewItem(const char* ClassName, const char* description, CreateItemFunction createFunction)
{
  CreateInformation info;
  info.m_Description = description;
  info.m_CreateObject = createFunction;
  GetFactory().m_creatorsMap[ClassName] = info;
}

ItemFactory& ItemFactory::GetFactory()
{
  static ItemFactory instance;
  return instance;
}
