/*=========================================================================

 Program: MAF2
 Module: mafVMEItemScalarMatrix
 Authors: Paolo Quadrani
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __mafVMEItemScalarMatrix_h
#define __mafVMEItemScalarMatrix_h
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafVMEItem.h"
#include "mafString.h"
#include "ftk/Base/MTime.h"

#include <vnl/vnl_matrix.h>

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mafVME;
class mafOBB;
class mafStorageElement;
class mafTagArray;

//template class MAF_EXPORT vnl_matrix<double>;

/** mafVMEItemScalarMatrix - store time-varying scalar information into a vnl_vector
  mafVMEItemScalarMatrix is an object that stores the single time stamped scalar array of a
  mafVME. This class associates a Time stamp and a Tagged list to an internally 
  stored scalar array. 
  A mafVMEItemScalarMatrix item has also a the following member variables:

  <B>m_DataType<\B> to store a "textual description of data".
  <B>m_Id<\B> to store a numeric unique Id (for internal use) see mafVMERoot::GetNextItemId)
  <B>m_URL<\B> internally used to store the name of the file where data is stored

  @sa mafVMEScalarMatrix mafTagArray

  @todo
  - Add a link to the VME object
  - Implement DeepCopy and SmartCopy functions
  - build a test
  - implement read from TmpFile
  - read from memory
  - Complete the implementation of reading crypted files.
*/
class MAF_EXPORT mafVMEItemScalarMatrix : public mafVMEItem
{
public:  
  mafTypeMacro(mafVMEItemScalarMatrix,mafVMEItem);

  void Print(std::ostream& os, const int indent=0) const override;

  /** Copy data from another scalar item*/
  void DeepCopy(mafVMEItem *a) override;

  /** Reference another scalar item's internal data*/
  void ShallowCopy(mafVMEItem *a) override;

  /** Return data scalar. (Supported only if MAF is compiled
    with ITK support which include VNL library) */
  virtual vnl_matrix<double> &GetData();

  /** Set the scalar data */
  virtual void SetData(vnl_matrix<double> &data);

  /** Compare two scalar items. Two scalar items are considered equivalent if they store
    the same type of data, have the same TimeStamp and equivalent TagArray.
    Id and URL are not considered for the comparison.
    To force compare the scalar items internal data, use SetGlobalCompareDataOn()*/
  bool Equals(mafVMEItem *a) override;

  /** Read the data file and update the item's data.*/
  int ReadData(mafString &filename, int resolvedURL = MAF_OK) override;

  /** Return true if scalar data is not empty. Currently this doesn't ensure data is the same on 
  the file. IsDataModified() can be used to know if data has been changed with respect
  to file.*/
  bool IsDataPresent() override {return !m_Data.empty();}
  
  /** UpdateBounds for this data. GetBounds automatically call this function...*/
  void UpdateBounds() override;

  /** Internally used to update data, automatically called by GetData() */
  void UpdateData() override;

  /** Return "sca" file extension */
  const char * GetDataFileExtension() override;

  /** Clear scalar data.*/
  void ReleaseData() override;

  /** Return pointer to memory to be used as input. This must be set with SetInputMemory(). */
  void GetOutputMemory(const char *&out_str, int &size) override;

  /** Release memory where data has been written. This should
    be used in MEMORY mode where data is written into memory */
  void ReleaseOutputMemory() override;

  /** Serialize the data into the compressed archive.*/
  bool StoreToArchive(wxZipOutputStream &zip) override;

protected:
  mafVMEItemScalarMatrix(); // to be allocated with New()
  ~mafVMEItemScalarMatrix() override; // to be deleted with Delete()

  /** Check that stored file is valid and has the same dimension of that one present in memory.*/
  int CheckFile(const char *filename) override {return MAF_OK;};

  /** Check that stored string is valid.*/
  int CheckFile(const char *input_string, int input_len) override {return MAF_OK;};

  /** Update the vnl reader to read from memory or file from disk (encrypted or not).*/
  int UpdateReader(mafString &filename);

  /** Restore data stored in this object. This function asks the storage
    for the filename corresponding to the URL.
    This method is automatically called by GetData().*/
  int InternalRestoreData() override;
  
  /** Store scalar item's data. This function asks the storage object
    for a tmp filename for saving and then call ... to store the tmp file
    into the URL. This method is called by Store().*/
  int InternalStoreData(const char *url) override;

  vnl_matrix<double>  m_Data;       ///< pointer to scalar data
  mafString           m_DataString; 
  int                 m_IOStatus;   ///< internally used to store the IO status
  double              m_ScalarBouns[2];
  
private:
  mafVMEItemScalarMatrix(const mafVMEItemScalarMatrix&);
  void operator=(const mafVMEItemScalarMatrix&);
};
#endif
