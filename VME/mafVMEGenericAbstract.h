/*=========================================================================

 Program: MAF2
 Module: mafVMEGenericAbstract
 Authors: Marco Petrone
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __mafVMEGenericAbstract_h
#define __mafVMEGenericAbstract_h
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafVME.h"
//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mafDataVector;
class mafMatrixVector;
class mafNode;
class mafGUI;

/** mafVMEGenericAbstract - a VME featuring an internal array for matrices and data.
  mafVMEGenericAbstract is a node featuring a standard way for managing 4D data, featuring
  an array for pose matrixes (MatrixVector) and an array for internally stored
  data (DataSetVector).
  @sa mafVME mafMatrixVector mafDataVector

  @todo
  - 
  */
class MAF_EXPORT mafVMEGenericAbstract : public mafVME
{
public:
  mafTypeMacro(mafVMEGenericAbstract,mafVME);

  enum VME_GUI_ID
  {
    ID_VME_CRYPTING = Superclass::ID_LAST,
    ID_LAST
  };

  /** print a dump of this object */
  void Print(std::ostream& os, const int tabs=0) override;// const;

  /**
    Copy the contents of another VME into this one. Notice that subtrees
    are not copied, i.e. copy is not recursive!
    Concrete class should reimplement this function to verify admitted
    conversion. */
  int DeepCopy(mafNode *a) override;

  /**
    Allow to perform a DeepCopy with data very large.
    Release memory of the input mafNode. */
  virtual int DeepCopyVmeLarge(mafNode *a);

  /** 
    perform a copy by simply referencing the copied VME's data array. 
    Beware: This can allow to save memory when doing special tasks, but
    can be very dangerous making one of the VME inconsistent. Some VMEs
    do not support such a function! */
  int ShallowCopy(mafVME *a) override;

  /**
    Compare two VME. Two VME are considered equivalent if they have equivalent 
    items, TagArrays, MatrixVectors, Name and Type. */
  bool Equals(mafVME *vme) override;

  /**
    Return the matrix vector associated with this VME. Matrix vector is an array of
    time stamped 4x4 matrices, used to generate the output VME pose matrix. The matrix
    vector is made persistent by saving it in the MSF-XML file (or other kind of storage
    for metadata).
    This array can be NULL for VMEs generating the output matrix procedurally starting from from
    different sources. */
  mafMatrixVector *GetMatrixVector() {return m_MatrixVector;}

  /**
    Set the Pose matrix of the VME. This function modifies the MatrixVector. You can
    set or get the Pose for a specified time. When setting, if the time does not exist
    the MatrixVector creates a new KeyMatrix on the fly. When getting, the matrix vector
    interpolates on the fly according to the matrix interpolator.*/
  void SetMatrix(const mafMatrix &mat) override;

  /** Get the pointer to the array of VMEItem's*/
  mafDataVector *GetDataVector() {return m_DataVector;}

  /** Return the list of time stamps of the VMEItemArray stored in this VME. */
  virtual void GetDataTimeStamps(std::vector<mafTimeStamp> &kframes);

  /** Return the list of time stamps of the MatrixVector stored in this VME. */
  void GetMatrixTimeStamps(std::vector<mafTimeStamp> &kframes);

  /**
    Return the list of timestamps for this VME. Timestamps list is 
    obtained merging timestamps for matrices and VME items*/
  void GetLocalTimeStamps(std::vector<mafTimeStamp> &kframes) override;

  /** Set the time bounds for the time varying VME based on data and matrix vector.*/
  void GetLocalTimeBounds(mafTimeStamp tbounds[2]) override;

  /** 
    return true is this VME has more than one time stamp, either  for
    data or matrices */
  bool IsAnimated() override;

  /** Return true if the data associated with the VME is present and updated at the current time.*/
  bool IsDataAvailable() override;

  /** return an xpm-icon that can be used to represent this node */
  static const char **GetIcon();   //SIL. 11-4-2005:

  void OnEvent(mafEventBase *maf_event) override;

  bool            m_EnableModifiedEvent = true;
protected:
  mafVMEGenericAbstract();
  ~mafVMEGenericAbstract() override;

  void InternalStore(mafStorageElementBuilder& parent) override;
  void InternalRestore(const mafStorageElement& node) override;

  /** Create GUI for the VME */
  mafGUI *CreateGui() override;

  mafMatrixVector *m_MatrixVector;
  mafDataVector   *m_DataVector;
private:
  mafVMEGenericAbstract(const mafVMEGenericAbstract&); // Not implemented
  void operator=(const mafVMEGenericAbstract&); // Not implemented
};
#endif
