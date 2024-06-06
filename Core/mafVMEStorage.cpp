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
#include "mafStorageElement.h"
#include "mmuIdFactory.h"
#include "mafEventIO.h"

//------------------------------------------------------------------------------
mafVMEStorage::mafVMEStorage()
	: mafStorage(_R("MSF"), _R("2.2"))
    , m_Document(nullptr)
//------------------------------------------------------------------------------
{
}

//------------------------------------------------------------------------------
mafVMEStorage::~mafVMEStorage()
//------------------------------------------------------------------------------
{
  SetManager(NULL);
}

//------------------------------------------------------------------------------
void mafVMEStorage::SetManager(mafNodeManager *manager)
//------------------------------------------------------------------------------
{
  if(m_Document)
    m_Document->SetListener(NULL);
  m_Document = manager;
  if(m_Document)
    manager->SetListener(this);
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
//------------------------------------------------------------------------------
int mafVMEStorage::InternalStore(const mafString& filename)
//------------------------------------------------------------------------------
{
    mafXMLWriter writer(m_FileType, m_Version);
    m_Document->Store(writer.GetRoot());
    return writer.Save(filename);
}
//------------------------------------------------------------------------------
int mafVMEStorage::InternalRestore(const mafString& filename)
//------------------------------------------------------------------------------
{
    mafXMLReader reader(m_FileType, m_Version);
    reader.Load(filename);
    m_Document->Restore(reader.GetRoot());
    return MAF_OK;
}

