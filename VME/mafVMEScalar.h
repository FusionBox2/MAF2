#pragma once

#include "ftkConfigure.h"

#include "mafVMEGenericAbstract.h"

BEGIN_FTK_NAMESPACE

class mafVMEOutputScalar;
class mafScalarVector;
class vtkDataSet;

/** mafVMEScalar */
class FTK_VME_EXPORT mafVMEScalar : public mafVMEGenericAbstract
{
public:
  mafTypeMacroN(mafVMEScalar);

  /** Set the data for the given timestamp. 
  This function automatically creates a VMEItem for the data to be stored.
  Return MAF_OK if succeeded, MAF_ERROR if they kind of data is not accepted by
  this type of VME. */
  virtual int SetData(double data, mafTimeStamp t);

  /** Set the time for this VME.
  It updates also the VTK representation for the scalar data.*/
  void SetTimeStamp(mafTimeStamp t) override;

  /** Return true is this VME has more than one time stamp, either  for data or matrices */
  bool IsAnimated() override;

  /** Get the pointer to the array of Scalar's*/
  mafScalarVector *GetScalarVector() {return m_ScalarVector.get();}

  /** Return the list of time stamps of the data scalar array stored in this VME. */
  void GetDataTimeStamps(std::vector<mafTimeStamp> &kframes) override;

  /**
  Return the list of timestamps for this VME. Timestamps list is 
  obtained merging timestamps for matrices and VME scalar data*/
  void GetLocalTimeStamps(std::vector<mafTimeStamp> &kframes) override;

  /** Set the time bounds for the time varying VME based on scalar data and matrix vector.*/
  void GetLocalTimeBounds(mafTimeStamp tbounds[2]) override;

  /** print a dump of this object */
  void Print(std::ostream& os, const int tabs=0) override;

  /** Copy the contents of another mafVMEScalar into this one. */
  int DeepCopy(mafNode *a) override;

  /** Compare with another mafVMEScalar. */
  bool Equals(mafVME *vme) override;

  /** return the right type of output.*/  
  mafVMEOutputScalar *GetScalarOutput() {return (mafVMEOutputScalar *)GetOutput();}

  /** return the right type of output */
  mafVMEOutput *GetOutput() override;

  /** return icon */
  static const char** GetIcon();

  /** Return the suggested pipe-typename for the visualization of this vme */
  mafString GetVisualPipe() override {return mafString(_R("mafPipeScalar"));};

protected:
  mafVMEScalar();
  ~mafVMEScalar() override;

  void InternalPreUpdate() override;

  void InternalStore(mafStorageElementBuilder& parent) override;
  void InternalRestore(const mafStorageElement& node) override;

  std::unique_ptr<mafScalarVector> m_ScalarVector;

private:
  mafVMEScalar(const mafVMEScalar&); // Not implemented
  void operator=(const mafVMEScalar&); // Not implemented
};

END_FTK_NAMESPACE
