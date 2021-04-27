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
int mafRoot::StoreRoot(mafStorageElement *parent)
//-------------------------------------------------------------------------
{
  parent->SetAttribute(_R("MaxNodeId"),mafToString(m_MaxNodeId));
  return MAF_OK;
}
//-------------------------------------------------------------------------
int mafRoot::RestoreRoot(mafStorageElement *element)
//-------------------------------------------------------------------------
{
  mafID max_id;
  if (!element->GetAttributeAsInteger(_R("MaxNodeId"),max_id))
    return MAF_ERROR;

  SetMaxNodeId(max_id);

  return MAF_OK;
}

//-------------------------------------------------------------------------
void mafRoot::Print(std::ostream& os, const int tabs)// const 
//-------------------------------------------------------------------------
{
  os << mafIndent(tabs) << "MaxNodeId: " << m_MaxNodeId << "\n";
}
