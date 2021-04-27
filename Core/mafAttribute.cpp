/*=========================================================================

 Program: MAF2
 Module: mafAttribute
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



#include "mafAttribute.h"
#include "mafStorageElement.h"
#include "mafIndent.h"
#include "assert.h"

#include <ostream>

mafCxxAbstractTypeMacro(mafAttribute);

//-------------------------------------------------------------------------
void mafAttribute::operator=(const mafAttribute &a)
//-------------------------------------------------------------------------
{
  DeepCopy(&a);
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
mafAttribute *mafAttribute::MakeCopy()
//-------------------------------------------------------------------------
{
  mafAttribute *new_attr=NewInstance();
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
int mafAttribute::InternalStore(mafStorageElement *parent)
//-------------------------------------------------------------------------
{
  return parent->StoreText(_R("Name"),m_Name);
}

//-------------------------------------------------------------------------
int mafAttribute::InternalRestore(mafStorageElement *node)
//-------------------------------------------------------------------------
{
  return node->RestoreText(_R("Name"),m_Name);
}
//-------------------------------------------------------------------------
void mafAttribute::Print(std::ostream& os, const int tabs) const
//-------------------------------------------------------------------------
{
  mafIndent indent(tabs);

  os << indent << "Name: " << m_Name.GetCStr() << std::endl;
}
