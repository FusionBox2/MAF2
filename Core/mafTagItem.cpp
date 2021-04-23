/*=========================================================================

 Program: MAF2
 Module: mafTagItem
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


#include "mafTagItem.h"
#include "mafString.h"
#include "mafIndent.h"
#include "mafStorageElement.h"
#include <vector>
#include <assert.h>

//-------------------------------------------------------------------------
mafTagItem::mafTagItem()
//-------------------------------------------------------------------------
{
  Initialize();
}

//-------------------------------------------------------------------------
mafTagItem::~mafTagItem()
//-------------------------------------------------------------------------
{
}

//-------------------------------------------------------------------------
mafTagItem::mafTagItem(const mafString& name, const mafString& value, int t)
//-------------------------------------------------------------------------
{
  Initialize();
  SetValue(value);
  SetName(name);
  SetType(t);
}

//-------------------------------------------------------------------------
mafTagItem::mafTagItem(const mafString& name, const mafString *values, int numcomp, int t)
//-------------------------------------------------------------------------
{
  Initialize();
  SetName(name);
  SetValues(values,numcomp);
  SetType(t);
}

//-------------------------------------------------------------------------
mafTagItem::mafTagItem(const mafString& name, const std::vector<mafString>& values, int t)
//-------------------------------------------------------------------------
{
  Initialize();
  SetName(name);
  SetValues(values);
  SetType(t);
}

//-------------------------------------------------------------------------
mafTagItem::mafTagItem(const mafString& name, double value)
//-------------------------------------------------------------------------
{
  Initialize();
  SetName(name);
  SetValue(value);
}

//-------------------------------------------------------------------------
mafTagItem::mafTagItem(const mafString& name, const double *value, int numcomp)
//-------------------------------------------------------------------------
{
  Initialize();
  SetName(name);
  SetValues(value,numcomp);

}

//-------------------------------------------------------------------------
mafTagItem::mafTagItem(const mafString& name, const std::vector<double>& values)
//-------------------------------------------------------------------------
{
  Initialize();
  SetName(name);
  SetValues(values);
  SetType(MAF_NUMERIC_TAG);
}

//-------------------------------------------------------------------------
void mafTagItem::DeepCopy(const mafTagItem *item)
//-------------------------------------------------------------------------
{
  assert(item);
  SetName(item->GetName());
  SetComponents(item->GetComponents());
  SetType(item->GetType());
}
//-------------------------------------------------------------------------
void mafTagItem::operator=(const mafTagItem& p)
//-------------------------------------------------------------------------
{
  DeepCopy(&p);
}

//-------------------------------------------------------------------------
mafTagItem::mafTagItem(const mafTagItem& p)
//-------------------------------------------------------------------------
{
  Initialize();
  *this=p;
}

//-------------------------------------------------------------------------
bool mafTagItem::operator==(const mafTagItem& p) const
//-------------------------------------------------------------------------
{
  return Equals(&p);
}

//-------------------------------------------------------------------------
bool mafTagItem::operator!=(const mafTagItem& p) const
//-------------------------------------------------------------------------
{
  return !Equals(&p);
}

//-------------------------------------------------------------------------
void mafTagItem::SetName(const mafString& name)
//-------------------------------------------------------------------------
{
  m_Name=name;
}

//-------------------------------------------------------------------------
const mafString& mafTagItem::GetName() const
//-------------------------------------------------------------------------
{
  return m_Name;
}

//-------------------------------------------------------------------------
void mafTagItem::SetValue(double value,int component)
//-------------------------------------------------------------------------
{
  mafString tmp = mafToString(value);
  SetValue(tmp,component);
  SetType(MAF_NUMERIC_TAG);
}

//-------------------------------------------------------------------------
void mafTagItem::SetComponent(const mafString& value,int component)
//-------------------------------------------------------------------------
{
  SetValue(value,component);
}

//-------------------------------------------------------------------------
void mafTagItem::SetComponent(double value , int component)
//-------------------------------------------------------------------------
{
  SetValue(value,component);
}

//-------------------------------------------------------------------------
void mafTagItem::SetValue(const mafString& value,int component)
//-------------------------------------------------------------------------
{
  if (component>=GetNumberOfComponents())
    SetNumberOfComponents(component+1);
  m_Components[component]=value;
  SetType(MAF_STRING_TAG);
}

//-------------------------------------------------------------------------
void mafTagItem::SetValues(const mafString *values, int numcomp)
//-------------------------------------------------------------------------
{
  // if the number of component differs, reallocate memory
  if (numcomp!=GetNumberOfComponents())
  {
    this->SetNumberOfComponents(numcomp);
  }

  // copy data
  for (int i=0;i<numcomp;i++)
  {
    this->SetValue(values[i],i);
  }
}

//-------------------------------------------------------------------------
void mafTagItem::SetValues(const std::vector<mafString>& values)
//-------------------------------------------------------------------------
{
  m_Components = values;
}

//-------------------------------------------------------------------------
void mafTagItem::SetValues(const double *values, int numcomp)
//-------------------------------------------------------------------------
{
  // if the number of component differs, reallocate memory
  if (numcomp!=GetNumberOfComponents())
  {
    this->SetNumberOfComponents(numcomp);
  }

  // copy data
  for (int i=0;i<numcomp;i++)
  {
    this->SetValue(values[i],i);
  }
}

//-------------------------------------------------------------------------
void mafTagItem::SetValues(const std::vector<double>& values)
//-------------------------------------------------------------------------
{
  // if the number of component differs, reallocate memory
  if (values.size()!=GetNumberOfComponents())
  {
    this->SetNumberOfComponents(values.size());
  }

  // copy data
  for (int i=0;i<values.size();i++)
  {
    this->SetValue(values[i],i);
  }
}

//-------------------------------------------------------------------------
void mafTagItem::SetComponents(const mafString *values, int numcomp)
//-------------------------------------------------------------------------
{
  SetValues(values,numcomp);
}

//-------------------------------------------------------------------------
void mafTagItem::SetComponents(const std::vector<mafString>& components)
//-------------------------------------------------------------------------
{
  SetValues(components);
}

//-------------------------------------------------------------------------
void mafTagItem::SetComponents(const double *components, int numcomp)
//-------------------------------------------------------------------------
{
  SetValues(components,numcomp);
}

//-------------------------------------------------------------------------
void mafTagItem::SetComponents(const std::vector<double>& components)
//-------------------------------------------------------------------------
{
  SetValues(components);
}

//-------------------------------------------------------------------------
const mafString& mafTagItem::GetValue(int component) const
//-------------------------------------------------------------------------
{
  static mafString empty;
  if (GetNumberOfComponents()>component)
    return m_Components[component];
  return empty;
}

//-------------------------------------------------------------------------
void mafTagItem::RemoveValue(int component)
//-------------------------------------------------------------------------
{
  if (GetNumberOfComponents()>component)
    m_Components.erase(m_Components.begin() + component);
}

//-------------------------------------------------------------------------
const mafString& mafTagItem::GetComponent(int comp) const
//-------------------------------------------------------------------------
{
  return GetValue(comp);
}

//-------------------------------------------------------------------------
double mafTagItem::GetValueAsDouble(int component) const
//-------------------------------------------------------------------------
{
  if (GetNumberOfComponents()<=component)
    return 0;
  return atof(GetValue(component).GetCStr());
}

//-------------------------------------------------------------------------
double mafTagItem::GetComponentAsDouble(int comp) const
//-------------------------------------------------------------------------
{
  return GetValueAsDouble(comp);
}

//-------------------------------------------------------------------------
void mafTagItem::Initialize()
//-------------------------------------------------------------------------
{
  m_Name=_R("");
  m_Type=MAF_MISSING_TAG;
  m_Components.clear();
}

//-------------------------------------------------------------------------
int mafTagItem::GetNumberOfComponents() const
//-------------------------------------------------------------------------
{
  return m_Components.size();
}

//-------------------------------------------------------------------------
void mafTagItem::SetNumberOfComponents(int n)
//-------------------------------------------------------------------------
{
  m_Components.resize(n);  
}

//-------------------------------------------------------------------------
void mafTagItem::GetValueAsSingleString(mafString &str) const
//-------------------------------------------------------------------------
{
  if (GetNumberOfComponents()>0)
  {
    int i;
    // the size of the single string is given by the sum of 
    // the sizes of the single components, plus 1 char for
    // "," for each component, plus 2 chars for brackets and
    // one for the trailing "\0".

    // leading parenthesis
    str=_R("(");

    for (i=0;i<GetNumberOfComponents();i++)
    {
      // colon separator
      if (i>0) str+=_R(",");

      str+=_R("\"") + m_Components[i]+_R("\"");    
    }

    // tailing parenthesis
    str+=_R(")");
  }
  else
  {
    // no components
    str.Clear();
  }

}

//-------------------------------------------------------------------------
void mafTagItem::GetTypeAsString(mafString &str) const
//-------------------------------------------------------------------------
{
  switch (m_Type)
  {
  case (MAF_MISSING_TAG):
    str = _R("MIS");
    break;
  case (MAF_NUMERIC_TAG):
    str = _R("NUM");
    break;
  case (MAF_STRING_TAG):
    str = _R("STR");
    break;
  default:
    str = _R("UNK");
  }
}

//-------------------------------------------------------------------------
bool mafTagItem::Equals(const mafTagItem *item) const
//-------------------------------------------------------------------------
{
  if (!item)
    return false;

  if (m_Name!=item->m_Name || \
    GetNumberOfComponents()!=item->GetNumberOfComponents() || \
    m_Type!=item->m_Type)
  {
    return false;
  }
  
  // compare single components 
  for (int i=0;i<this->GetNumberOfComponents();i++)
  {
    if (m_Components[i].IsEmpty()&&item->m_Components[i].IsEmpty())
      continue;

    if (m_Components[i]!=item->m_Components[i])
      return false;
  }

  return true;
}

//-------------------------------------------------------------------------
void mafTagItem::Print(std::ostream& os, const int tabs) const
//-------------------------------------------------------------------------
{
  mafIndent indent(tabs);

  os << indent <<"Name: \"" << (m_Name.GetCStr()?m_Name.GetCStr():_R("(NULL)")) <<"\"";

  int t=this->GetType();
  char *tstr;

  switch (t)
  {
  case (MAF_MISSING_TAG):
    tstr="MISSING";
    break;
  case (MAF_NUMERIC_TAG):
    tstr="NUMERIC";
    break;
  case (MAF_STRING_TAG):
    tstr="STRING";
    break;
  default:
    tstr="UNKNOWN";
  }


  os << " Type: " << tstr << " (" << t << ") ";

  os << " Components: ";

  if (GetNumberOfComponents()>0)
  {
    mafString tmp;
    GetValueAsSingleString(tmp);
    os  << tmp.GetCStr();
    os  <<" NumComp: " << GetNumberOfComponents();
  }

  os << std::endl; // end of single line printing
}

//-------------------------------------------------------------------------
int mafTagItem::InternalStore(mafStorageElement *parent)
//-------------------------------------------------------------------------
{
  parent->SetAttribute(_R("Name"), GetName());
  parent->SetAttribute(_R("Mult"),mafToString(GetNumberOfComponents()));
  mafString type;
  GetTypeAsString(type);
  parent->SetAttribute(_R("Type"),type);

  if (parent->StoreVectorN(_R("TItem"),m_Components,GetNumberOfComponents(),_R("TC"))==MAF_ERROR)
    return MAF_ERROR;

  return MAF_OK;
}

//-------------------------------------------------------------------------
int mafTagItem::InternalRestore(mafStorageElement *node)
//-------------------------------------------------------------------------
{
  if (!node->GetAttribute(_R("Name"),m_Name))
    return MAF_ERROR;

  mafString type;
  if (!node->GetAttribute(_R("Type"),type))
    return MAF_ERROR;
  
  if (type==_R("NUM"))
  {
    SetType(MAF_NUMERIC_TAG);
  }
  else if (type==_R("STR"))
  {
    SetType(MAF_STRING_TAG);
  }
  else if (type==_R("MIS"))
  {
    SetType(MAF_MISSING_TAG);
  }
  else
  {
    SetType(atof(type.GetCStr()));
  }

  mafID num;
  if (!node->GetAttributeAsInteger(_R("Mult"),num))
    return MAF_ERROR;
  
  SetNumberOfComponents(num);
  
  return node->RestoreVectorN(_R("TItem"),m_Components,GetNumberOfComponents(),_R("TC"));
}

