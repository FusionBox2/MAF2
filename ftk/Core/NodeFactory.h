#pragma once

#include "ftkConfigure.h"

#include "mafNode.h"
#include "mafPics.h"

#include <map>
#include <string>

#define mafPlugNodeMacro(node_type,descr) \
  NodeFactory::RegisterNewNode(node_type::GetStaticTypeName(), descr, node_type::NewObject);\
  if (mafPictureFactory::GetPicsInitialized()) \
    mafPictureFactory::GetPictureFactory()->AddVmePic(_R(node_type::GetStaticTypeName()),node_type::GetIcon());

using NodeCreateType = mafObject*;
using CreateNodeFunction = NodeCreateType(*)();

BEGIN_FTK_NAMESPACE

class FTK_CORE_EXPORT NodeFactory
{
public:
  static int Initialize();

  static NodeCreateType CreateNode(const char* ClassName);

  static const char* GetDescription(const char* ClassName);

  static void RegisterNewNode(const char* ClassName, const char* description, CreateNodeFunction createFunction);

private:
  NodeFactory() = default;

  static NodeFactory& GetFactory();

  struct CreateInformation
  {
    std::string        m_Description;
    CreateNodeFunction m_CreateObject;
  };
  std::map<std::string, CreateInformation, std::less<> > m_creatorsMap;
};

template <class T>
class mafPlugNode
{
public:
  mafPlugNode(const char* description);

};

template <class T>
mafPlugNode<T>::mafPlugNode(const char *description)
{ 
  NodeFactory::RegisterNewNode(T::GetStaticTypeName(), description, T::NewObject);
  {
    // here plug node's icon inside picture factory
    if (mafPictureFactory::GetPicsInitialized())
      mafPictureFactory::GetPictureFactory()->AddVmePic(_R(T::GetStaticTypeName()),T::GetIcon());
  }
}

END_FTK_NAMESPACE
