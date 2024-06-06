/*=========================================================================

 Program: MAF2
 Module: mafRoot
 Authors: Marco Petrone
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "mafRoot.h"
#include "mafObject.h"
#include "mafStorageElement.h"
#include "mafIndent.h"

//-------------------------------------------------------------------------
mafRoot::mafRoot()
//-------------------------------------------------------------------------
{
  m_MaxNodeId = 0;
}

//-------------------------------------------------------------------------
mafRoot::~mafRoot()
//-------------------------------------------------------------------------
{
}
//-------------------------------------------------------------------------
mafRoot* mafRoot::SafeDownCast(mafObject *o)
//-------------------------------------------------------------------------
{
  try 
  { 
    return dynamic_cast<mafRoot *>(o);
  } 
  catch (std::bad_cast) 
  { 
    return NULL;
  }
}

//-------------------------------------------------------------------------
void mafRoot::StoreRoot(mafStorageElementBuilder& parent)
//-------------------------------------------------------------------------
{
  parent(_R("MaxNodeId")).SetValue(mafToString(m_MaxNodeId));
}
//-------------------------------------------------------------------------
void mafRoot::RestoreRoot(const mafStorageElement& node)
//-------------------------------------------------------------------------
{
  SetMaxNodeId(node(_R("MaxNodeId")).As<mafID>());
}

//-------------------------------------------------------------------------
void mafRoot::Print(std::ostream& os, const int tabs)// const 
//-------------------------------------------------------------------------
{
  os << mafIndent(tabs) << "MaxNodeId: " << m_MaxNodeId << "\n";
}
