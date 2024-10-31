#include "AttributeFactory.h"

#include "mmaMaterial.h"
#include "mmaVolumeMaterial.h"
#include "mmaApplicationLayout.h"
#include "mafAttributeTraceability.h"
#include "mmaMeter.h"
#include "mafTagArray.h"
#include "medAttributeSegmentationVolume.h"

int AttributeFactory::Initialize()
{
  mafPlugAttributeMacro(mafTagArray, "a basic kind of attribute used to store key-value pairs");
  mafPlugAttributeMacro(mmaMeter, "Meter attributes");
  mafPlugAttributeMacro(mmaMaterial, "Material attributes");
  mafPlugAttributeMacro(mmaVolumeMaterial, "Volume material attributes");
  mafPlugAttributeMacro(mmaApplicationLayout, "Application layout attributes");
  mafPlugAttributeMacro(mafAttributeTraceability, "Trial event attributes");

  mafPlugAttributeMacro(medAttributeSegmentationVolume, "Segmentation Volume attributes");
  return MAF_OK;
}

AttributeCreateType AttributeFactory::CreateAttribute(const char* ClassName)
{
  auto pos = GetFactory().m_creatorsMap.find(ClassName);
  if (pos != GetFactory().m_creatorsMap.end())
  {
    return pos->second.m_CreateObject();
  }
  return nullptr;
}

const char* AttributeFactory::GetDescription(const char* ClassName)
{
  auto pos = GetFactory().m_creatorsMap.find(ClassName);
  if (pos != GetFactory().m_creatorsMap.end())
  {
    return pos->second.m_Description.c_str();
  }
  return nullptr;
}

void AttributeFactory::RegisterNewAttribute(const char* ClassName, const char* description, CreateAttributeFunction createFunction)
{
  CreateInformation info;
  info.m_Description = description;
  info.m_CreateObject = createFunction;
  GetFactory().m_creatorsMap[ClassName] = info;
}

AttributeFactory& AttributeFactory::GetFactory()
{
  static AttributeFactory instance;
  return instance;
}
