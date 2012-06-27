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
#include "mafObserver.h"
#include "mafEventSender.h"
#include "mafUtility.h"
#include "mafStorable.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mafVMERoot;

/** utility class representing the MSF document. */
class mmuMSFDocument : public mafUtility, public mafStorable
{
public:
  mmuMSFDocument(mafVMERoot *root=NULL):m_Root(root) {}
  virtual ~mmuMSFDocument() {}
  virtual int InternalStore(mafStorageElement *node);
  virtual int InternalRestore(mafStorageElement *node);
protected:
  mafVMERoot *m_Root;
};

/** A storage class for MSF local files.
  This is a concrete implementation of storage object for storing MSF local files.
  @sa mafXMLStorage
  @todo
    - 
*/  
class MAF_EXPORT mafVMEStorage: public mafXMLStorage, public mafObserver, public mafEventSender
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

  virtual void SetURL(const char *name);

  /** process events coming from tree */
  virtual void OnEvent(mafEventBase *e);

protected:  
  /** Do not allow changing the file type from external objects. */
  void SetFileType(const char *filetype) {Superclass::SetFileType(filetype);}
  
  /** Do not allow changing the file version from external objects. */
  void SetVersion(const char *version) {Superclass::SetVersion(version);}

  mafVMERoot *m_Root; ///< the VME root node
};
#endif // _mafVMEStorage_h_
