/*=========================================================================

 Program: MAF2
 Module: mafVMEItemVTK
 Authors: Marco Petrone
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __mafVMEItemVTK_h
#define __mafVMEItemVTK_h
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafVMEItem.h"
#include "ftk/Base/String.h"
#include "ftk/Base/MTime.h"
#include "vtkSmartPointer.h"
//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mafVME;
class mafOBB;
class mafStorageElement;
class mafTagArray;
class vtkDataSet;
class vtkDataReader;
class vtkDataSetWriter;
class vtkCallbackCommand;
class vtkObject;


/** mafVMEItemVTK - store the single VTK dataset stored into a mafDataVector
  mafVMEItemVTK is an object that stores the single time stamped dataset of a
  mafVME. This class associates a Time stamp and a Tagged list to an internally 
  stored dataset. The type of dataset is not defined by base class, and subclasses
  should define a GetData() and SetData() functions. 
  A mafVMEItemVTK item has also a the following member variables:
 
  <B>m_DataType<\B> to store a "textual description of data".
  <B>m_Id<\B> to store a numeric unique Id (for internal use) see mafVMERoot::GetNextItemId)
  <B>m_URL<\B> internally used to store the name of the file where data is stored

  @sa mafVMEGenericVTK mafDataVector mafTagArray

  @todo
  - Add a link to the VME object
  - Implement DeepCopy and SmartCopy functions
  - build a test
  - implement read from TmpFile
  - read from memory
  - implement catching of VTK I/O errors
*/
class MAF_EXPORT mafVMEItemVTK : public mafVMEItem
{
public:  
  mafTypeMacro(mafVMEItemVTK,mafVMEItem);

  void Print(std::ostream& os, const int indent=0) const override;

  /** copy data from another dataset */
  void DeepCopy(mafVMEItem *a) override;

  /** copy large data from another dataset and release it.*/
  void DeepCopyVmeLarge(mafVMEItem *a) override;

  /** reference another dataset's internal data */
  void ShallowCopy(mafVMEItem *a) override;

  /** 
    return data converted into VTK format. (supported only if MAF is compiled
    with VTK support) */
  virtual vtkDataSet *GetData();

  /** set the VTK dataset */
  virtual void SetData(vtkDataSet *data);

  /**
    Compare two datasets. Two datasets are considered equivalent if they store
    the same type of data, have the same TimeStamp and equivalent TagArray.
    Id and URL are not considered for the comparison.
    To force compare the dataset internal data, use SetGlobalCompareDataOn()*/
  bool Equals(mafVMEItem *a) override;

  /** Read the data file and update the item's data.*/
  int ReadData(mafString &filename, int resolvedURL = MAF_OK) override;

  /**
  Return true if data is !=NULL. Currently this doesn't ensure data is the same on 
  the file. IsDataModified() can be used to know if data has been changed with respect
  to file.*/
  bool IsDataPresent() override {return m_Data!=NULL;}

  /**  UpdateBounds for this data. GetBounds automatically call this function...*/
  void UpdateBounds() override;

  /** Internally used to update data, automatically called by GetData() */
  void UpdateData() override;

  /** return "vtk" file extension */
  const char * GetDataFileExtension() override;

  /** destroy VTK reader and unregister VTK dataset */
  void ReleaseData() override;

  /** return pointer to memory to be used as input. This must be set with SetInputMemory(). */
  void GetOutputMemory(const char *&out_str, int &size) override;

  /** 
    Release memory where data has been written. This should
    be used in MEMORY mode where data is written into memory */
  void ReleaseOutputMemory() override;

  /** Serialize the data into the compressed archive.*/
  bool StoreToArchive(wxZipOutputStream &zip) override;

protected:
  mafVMEItemVTK(); // to be allocated with New()
  ~mafVMEItemVTK() override; // to be deleted with Delete()

  /** Check that stored file is valid and has the same dimension of that one present in memory.*/
  int CheckFile(const char *filename) override;

  /** Check that stored string is valid.*/
  int CheckFile(const char *input_string, int input_len) override;
  
  /** Update the vtk reader to read from memory or file from disk (encrypted or not).*/
  int UpdateReader(vtkDataReader *reader, mafString &filename);

  /**
    Restore data stored in this object. This function asks the storage
    for the filename corresponding to the URL.
    This method is automatically called by GetData().*/
  int InternalRestoreData() override;
  /**
    Store data stored in this object. This function asks the storage object
    for a tmp filename for saving and then call ... to store the tmp file
    into the URL. This method is called by Store().*/
  int InternalStoreData(const char *url) override;

  static void InternalProcessUpdateEvents(vtkObject* sender, unsigned long channel, void* clientdata, void* calldata);

  vtkSmartPointer<vtkDataSet> m_Data;       ///< pointer to VTK dataset
  vtkDataReader *             m_DataReader; ///< the reader used to read VTK data
  vtkDataSetWriter *          m_DataWriter; ///< the writer used to write VTK data
  int                         m_IOStatus;   ///< internally used to store the IO status
  vtkCallbackCommand*         m_UpdateEventRouter;

private:
  mafVMEItemVTK(const mafVMEItemVTK&);
  void operator=(const mafVMEItemVTK&);
};

#endif
