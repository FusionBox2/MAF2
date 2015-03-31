/*=========================================================================

 Program: MAF2
 Module: mafVMEStorage
 Authors: Marco Petrone m.petrone@cineca.it
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "mafIncludeWX.h" // to be removed

#include "mafVMEStorage.h"
#include "mafVMERoot.h"
#include "mafUtility.h"
#include "mafStorable.h"
#include "mafStorageElement.h"
#include "mmuIdFactory.h"
#include "mafEventIO.h"

//------------------------------------------------------------------------------
// mmuMSFDocument
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
mafVMERoot *mmuMSFDocument::GetRoot()
//------------------------------------------------------------------------------
{
  return m_Root;
}
//------------------------------------------------------------------------------
void mmuMSFDocument::SetRoot(mafVMERoot *root)
//------------------------------------------------------------------------------
{
  m_Root = root;
}
//------------------------------------------------------------------------------
int mmuMSFDocument::InternalStore(mafStorageElement *node)
//------------------------------------------------------------------------------
{
  // here should write elements specific for the document
  if(!m_Root)
    return MAF_ERROR;
  mafStorageElement *root_elem=node->StoreObject("Root",m_Root);
  return root_elem?MAF_OK:MAF_ERROR;
}

//------------------------------------------------------------------------------
int mmuMSFDocument::InternalRestore(mafStorageElement *node)
//-------------------------------------------------------
{
  // here should restore elements specific for the document
  SetRoot(NULL);
  mafObject *obj = node->RestoreObject("Root");
  if(!obj)
    return MAF_ERROR; 
  mafReferenceCounted *rc = mafReferenceCounted::SafeDownCast(obj);
  if(!rc)
  {
    obj->Delete();
    return MAF_ERROR;
  }
  mafAutoPointer<mafReferenceCounted> arc = rc;
  mafVMERoot *root = mafVMERoot::SafeDownCast(obj);
  m_Root = root;
  if(root)
    return m_Root->Initialize();
  return MAF_ERROR; 
}

//------------------------------------------------------------------------------
// mafVMEStorage
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
mafCxxTypeMacro(mafVMEStorage)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
mafVMEStorage::mafVMEStorage()
//------------------------------------------------------------------------------
{
  m_Parser->SetVersion("2.2");
  m_Parser->SetFileType("MSF");
  SetDocument(&m_MSFDoc); // create a MSF doc and set the root node
}

//------------------------------------------------------------------------------
mafVMEStorage::~mafVMEStorage()
//------------------------------------------------------------------------------
{
}

//------------------------------------------------------------------------------
void mafVMEStorage::SetRoot(mafVMERoot *root)
//------------------------------------------------------------------------------
{
  m_MSFDoc.SetRoot(root);
  if(!root)
    return;
  root->Initialize();
}
//------------------------------------------------------------------------------
mafVMERoot *mafVMEStorage::GetRoot()
//------------------------------------------------------------------------------
{
  return m_MSFDoc.GetRoot();
}

//------------------------------------------------------------------------------
void mafVMEStorage::OnEvent(mafEventBase *e)
//------------------------------------------------------------------------------
{
  // default forward events to 
  if (e->GetChannel()==MCH_UP)
  {
    if (e->GetId()==NODE_GET_STORAGE)
    {
      // return the storage pointer: here the hypothesis sis the root node listener is a storage.
      mafEventIO *io_event=mafEventIO::SafeDownCast(e);
      io_event->SetStorage(this);
      return;
    }
    // by default send events to listener
    InvokeEvent(e);
  }
}
