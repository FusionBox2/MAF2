#include "mafAttribute.h"

#include "mafDefines.h" 

#include "ftk/Core/AttributeFactory.h"
#include "mafStorageElement.h"
#include "mafIndent.h"
#include "assert.h"

#include <ostream>

//-------------------------------------------------------------------------
mafAttribute& mafAttribute::operator=(const mafAttribute &a)
//-------------------------------------------------------------------------
{
  DeepCopy(&a);
  return *this;
}

//-------------------------------------------------------------------------
bool mafAttribute::operator==(const mafAttribute &a) const
//-------------------------------------------------------------------------
{
  return Equals(&a);
}

//-------------------------------------------------------------------------
void mafAttribute::DeepCopy(const mafAttribute *a)
//-------------------------------------------------------------------------
{
  assert(a);
  assert(a->IsA(GetTypeId()));
  m_Name=a->GetName();
}

//-------------------------------------------------------------------------
std::shared_ptr<mafAttribute> mafAttribute::MakeCopy()
//-------------------------------------------------------------------------
{
  std::shared_ptr<mafAttribute> new_attr(NewInstance());
  assert(new_attr);
  new_attr->DeepCopy(this);
  return new_attr;
}

//-------------------------------------------------------------------------
bool mafAttribute::Equals(const mafAttribute *a) const
//-------------------------------------------------------------------------
{
  return a->IsA(GetTypeId()) && m_Name==a->GetName();
}

//-------------------------------------------------------------------------
void mafAttribute::SetName(const mafString& name)
//-------------------------------------------------------------------------
{
  m_Name=name;
}
//-------------------------------------------------------------------------
const mafString& mafAttribute::GetName() const
//-------------------------------------------------------------------------
{
  return m_Name;
}

//-------------------------------------------------------------------------
void mafAttribute::InternalStore(mafStorageElementBuilder& parent)
//-------------------------------------------------------------------------
{
  parent[_R("Name")].SetValue(m_Name);
}

//-------------------------------------------------------------------------
void mafAttribute::InternalRestore(const mafStorageElement& node)
//-------------------------------------------------------------------------
{
  m_Name = node[_R("Name")].As<mafString>();
}
//-------------------------------------------------------------------------
void mafAttribute::Print(std::ostream& os, const int tabs) const
//-------------------------------------------------------------------------
{
  mafIndent indent(tabs);

  os << indent << "Attribute Type Name: " << GetTypeName() << std::endl;

  os << indent << "Name: " << m_Name.GetCStr() << std::endl;
}

mafAttribute* mafAttribute::Create(const char* AttributeType)
{
  return AttributeFactory::CreateInstance(AttributeType);
}
