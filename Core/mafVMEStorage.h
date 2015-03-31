/*=========================================================================

 Program: MAF2
 Module: mafVMEStorage
 Authors: Marco Petrone
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __mafVMEStorage_h__
#define __mafVMEStorage_h__

#include "mafStorage.h"
#include "mafParser.h"
#include "mafBaseEventHandler.h"
#include "mafEventSender.h"
#include "mafStorable.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mafNodeManager;

/** A storage class for MSF local files.
  This is a concrete implementation of storage object for storing MSF local files.
  @sa mafXMLStorage
  @todo
    - 
*/  
class MAF_EXPORT mafVMEStorage: public mafStorage, public mafBaseEventHandler, public mafEventSender
{
public:
  mafTypeMacro(mafVMEStorage,mafStorage)

  mafVMEStorage();
  virtual ~mafVMEStorage();
  
  mafNodeManager *GetManager();

  void SetManager(mafNodeManager *manager);

  /** process events coming from tree */
  virtual void OnEvent(mafEventBase *e);

protected:  
  mafNodeManager    *m_NodeManager; ///< the VME root node
};
#endif // _mafVMEStorage_h_
