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
#include "mafNodeManager.h"
#include "mafUtility.h"
#include "mafStorable.h"
#include "mafStorageElement.h"
#include "mmuIdFactory.h"
#include "mafEventIO.h"

//------------------------------------------------------------------------------
mafCxxTypeMacro(mafVMEStorage)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
mafVMEStorage::mafVMEStorage()
//------------------------------------------------------------------------------
{
  m_NodeManager = NULL;
  m_Parser->SetVersion(_R("2.2"));
  m_Parser->SetFileType(_R("MSF"));
  m_Parser->SetDocument(m_NodeManager); // create a MSF doc and set the root node
}

//------------------------------------------------------------------------------
mafVMEStorage::~mafVMEStorage()
//------------------------------------------------------------------------------
{
  SetManager(NULL);
  mafDEL(m_Parser);
}

//------------------------------------------------------------------------------
void mafVMEStorage::SetManager(mafNodeManager *manager)
//------------------------------------------------------------------------------
{
  if(m_NodeManager)
    m_NodeManager->SetListener(NULL);
  m_NodeManager = manager;
  if(m_NodeManager)
    m_NodeManager->SetListener(this);
  if(m_Parser)
    m_Parser->SetDocument(m_NodeManager);
}
//------------------------------------------------------------------------------
mafNodeManager *mafVMEStorage::GetManager()
//------------------------------------------------------------------------------
{
  return m_NodeManager;
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
