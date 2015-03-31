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

#include "mafXMLStorage.h"
#include "mafBaseEventHandler.h"
#include "mafEventSender.h"
#include "mafUtility.h"
#include "mafStorable.h"
#include "mafVMERoot.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------

/** utility class representing the MSF document. */
class mmuMSFDocument : public mafUtility, public mafStorable
{
public:
  mmuMSFDocument(mafVMERoot *root=NULL):m_Root(root) {}
  virtual ~mmuMSFDocument() {}
  virtual int InternalStore(mafStorageElement *node);
  virtual int InternalRestore(mafStorageElement *node);
  void SetRoot(mafVMERoot *root);
  mafVMERoot *GetRoot();
protected:
  mafAutoPointer<mafVMERoot> m_Root;
};

/** A storage class for MSF local files.
  This is a concrete implementation of storage object for storing MSF local files.
  @sa mafXMLStorage
  @todo
    - 
*/  
class MAF_EXPORT mafVMEStorage: public mafXMLStorage, public mafBaseEventHandler, public mafEventSender
{
public:
  MAF_ID_DEC(MSF_FILENAME_CHANGED);
  mafTypeMacro(mafVMEStorage,mafXMLStorage)

  mafVMEStorage();
  virtual ~mafVMEStorage();
  
  /** return the root node attached to this tree */
  mafVMERoot *GetRoot();

  /** 
    Set the root to this Storage. This is usually not necessary
    since storage creates the root on its own */
  void SetRoot (mafVMERoot *root);

  /** process events coming from tree */
  virtual void OnEvent(mafEventBase *e);

protected:  
  /** Do not allow changing the file type from external objects. */
  void SetFileType(const char *filetype) {Superclass::SetFileType(filetype);}
  
  /** Do not allow changing the file version from external objects. */
  void SetVersion(const char *version) {Superclass::SetVersion(version);}

  mmuMSFDocument m_MSFDoc; ///< the VME root node
};
#endif // _mafVMEStorage_h_
