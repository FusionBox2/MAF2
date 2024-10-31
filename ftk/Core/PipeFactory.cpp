#include "ftk/Core/PipeFactory.h"

#ifdef MAF_USE_VTK
#include "mafPipeBox.h"
#endif

int PipeFactory::Initialize()
{
  mafPlugPipeMacro(mafPipeBox, "General pipe to show box for vtk data");
  return MAF_OK;
}
PipeCreateType PipeFactory::CreatePipe(const char* ClassName)
{
  auto pos = GetFactory().m_creatorsMap.find(ClassName);
  if (pos != GetFactory().m_creatorsMap.end())
  {
    return pos->second.m_CreateObject();
  }
  return nullptr;
}

const char* PipeFactory::GetDescription(const char* ClassName)
{
  auto pos = GetFactory().m_creatorsMap.find(ClassName);
  if (pos != GetFactory().m_creatorsMap.end())
  {
    return pos->second.m_Description.c_str();
  }
  return nullptr;
}

void PipeFactory::RegisterNewPipe(const char* ClassName, const char* description, CreatePipeFunction createFunction)
{
  CreateInformation info;
  info.m_Description = description;
  info.m_CreateObject = createFunction;
  GetFactory().m_creatorsMap[ClassName] = info;
}

PipeFactory& PipeFactory::GetFactory()
{
  static PipeFactory instance;
  return instance;
}
