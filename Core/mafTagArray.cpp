/*=========================================================================

 Program: MAF2
 Module: mafTagArray
 Authors: Marco Petrone
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------


#include "mafTagArray.h"
#include "mafStorageElement.h"
#include "mafIndent.h"
#include <assert.h>

mafCxxTypeMacro(mafTagArray)

//-------------------------------------------------------------------------
mafTagArray::mafTagArray()
//-------------------------------------------------------------------------
{
}

//-------------------------------------------------------------------------
mafTagArray::~mafTagArray()
//-------------------------------------------------------------------------
{
}

//-------------------------------------------------------------------------
void mafTagArray::operator=(const mafTagArray &a)
//-------------------------------------------------------------------------
{
  Superclass::operator =(a);
}

//-------------------------------------------------------------------------
void mafTagArray::DeepCopy(const mafTagArray *a)
//-------------------------------------------------------------------------
{
  Superclass::DeepCopy(a);
  mmuTagsMap::const_iterator it;
  for (it=a->m_Tags.begin();it!=a->m_Tags.end();it++)
  {
    const mafTagItem &titem=it->second;
    SetTag(titem);
  }
}

//-------------------------------------------------------------------------
void mafTagArray::DeepCopy(const mafAttribute *a)
//-------------------------------------------------------------------------
{
  if (a->IsMAFType(mafTagArray))
  {
    DeepCopy((const mafTagArray *)a);
  }
}

//-------------------------------------------------------------------------
const mafTagItem *mafTagArray::GetTag(const mafString& name) const
//-------------------------------------------------------------------------
{
  mmuTagsMap::const_iterator it=m_Tags.find(name);
  if (it!=m_Tags.end())
    return &(it->second);
  return NULL;
}

//-------------------------------------------------------------------------
mafTagItem *mafTagArray::GetTag(const mafString& name)
//-------------------------------------------------------------------------
{
  mmuTagsMap::iterator it=m_Tags.find(name);
  if (it!=m_Tags.end())
    return &(it->second);
  return NULL;
}

//-------------------------------------------------------------------------
bool mafTagArray::GetTag(const mafString& name,mafTagItem &item) const
//-------------------------------------------------------------------------
{
  const mafTagItem *tmp_item=GetTag(name);
  if (tmp_item)
  {
    item=*tmp_item;

    return true;
  }

  return false;
}

//-------------------------------------------------------------------------
void mafTagArray::SetTag(const mafTagItem &value)
//-------------------------------------------------------------------------
{
  m_Tags[value.GetName()] = value;
}

//-------------------------------------------------------------------------
void mafTagArray::DeleteTag(const mafString& name)
//-------------------------------------------------------------------------
{
  mmuTagsMap::iterator it=m_Tags.find(name);
  if (it!=m_Tags.end())
    m_Tags.erase(it);
}

//-------------------------------------------------------------------------
void mafTagArray::GetTagList(std::vector<mafString> &list) const
//-------------------------------------------------------------------------
{
  list.clear();
  list.resize(GetNumberOfTags());
  int i = 0;
  for (mmuTagsMap::const_iterator it=m_Tags.begin();it!=m_Tags.end();it++)
  {
    list[i++] = it->second.GetName();
  }
}

//-------------------------------------------------------------------------
bool mafTagArray::operator==(const mafTagArray &a) const
//-------------------------------------------------------------------------
{
  return Equals(&a);
}

//-------------------------------------------------------------------------
bool mafTagArray::Equals(const mafTagArray *array) const
//-------------------------------------------------------------------------
{
  assert(array);
  if (GetNumberOfTags()!=array->GetNumberOfTags())
    return false;

  mmuTagsMap::const_iterator it=m_Tags.begin();
  mmuTagsMap::const_iterator it2=array->m_Tags.begin();
  int i=0;
  for (;it!=m_Tags.end();it++,it2++,i++)
  {
    if (it->second!=it2->second)
      return false;
  }

  return true;
}

//-------------------------------------------------------------------------
/*void mafTagArray::GetTagsByType(int type, std::vector<mafTagItem *> &array)
//-------------------------------------------------------------------------
{
  array.clear();
  mmuTagsMap::iterator it=m_Tags.begin();
  for (;it!=m_Tags.end();it++)
  {
    if (it->second.GetType()==type)
    {
      array.push_back(&(it->second));
    }
  }
}*/
//-------------------------------------------------------------------------
int mafTagArray::GetNumberOfTags() const
//-------------------------------------------------------------------------
{
  return m_Tags.size();
}

//-------------------------------------------------------------------------
void mafTagArray::InternalStore(mafStorageElementBuilder& parent)
//-------------------------------------------------------------------------
{
  Superclass::InternalStore(parent);
  parent(_R("NumberOfTags")).SetValue(GetNumberOfTags());
  for (auto& item : m_Tags)
  {
	{auto titem = parent[_R("TItem")]; item.second.Store(titem);}
  }
}

//-------------------------------------------------------------------------
void mafTagArray::InternalRestore(const mafStorageElement& node)
//-------------------------------------------------------------------------
{
  Superclass::InternalRestore(node);// == MAF_OK)
  mafID numAttrs = node(_R("NumberOfTags")).As<mafID>();

  auto children = node[_R("TItem")];
  int idx = 0;
  for (int i = 0; (idx < numAttrs) && (i < children.GetNumItems()); i++)
  {
    mafTagItem new_titem;
    new_titem.Restore(children[i]);
    SetTag(new_titem);
    idx++;
  }
  if (idx < numAttrs)
  {
    mafErrorMacro("Error Restoring TagArray: wrong number of restored items, should be " << numAttrs << ", found " << children.GetNumItems());
  }
}

//-------------------------------------------------------------------------
void mafTagArray::Print(std::ostream& os, const int tabs) const
//-------------------------------------------------------------------------
{
  Superclass::Print(os,tabs);
  
  mafIndent indent(tabs);
  os << indent << "Tags:"<<std::endl;
  mafIndent next_indent(indent.GetNextIndent());

  mafTagItem item;
  for (mmuTagsMap::const_iterator it=m_Tags.begin();it!=m_Tags.end();it++)
  {
    item = it->second;
    item.Print(os,next_indent);
  }
}