#pragma once

#include "mafDataPipeInterpolator.h"
#include <vnl/vnl_matrix.h>

//----------------------------------------------------------------------------
// forward declarations
//----------------------------------------------------------------------------
class mafVMEItemScalarMatrix;

/**
  Class Name: mafScalarMatrixInterpolator.
  Data interpolator specialized for mafVMEScalarMatrix (for VNL matrix data).
  This interpolator is specialized for scalar data. By default selects the 
  right VMEItem, extracts the inner VNL matrix and set it as input of the 
  DataPipe.

  @sa mafVMEScalarMatrix
  
  @todo
  -
*/
class MAF_EXPORT mafDataPipeInterpolatorScalarMatrix : public mafDataPipeInterpolator
{
public:
  /** type macro for RTTI and instance creation*/
  mafTypeMacroN(mafDataPipeInterpolatorScalarMatrix);

  /** This DataPipe accepts only VME's with internal DataArray. */
  bool Accept(mafVME *vme) override;

  /**
    Get the MTime: this is the bit of magic that makes everything work.*/
  //virtual unsigned long GetMTime();

   /**  Get the output of the interpolator item*/
  std::shared_ptr<mafVMEItemScalarMatrix> GetCurrentItemScalarMatrix() {return std::static_pointer_cast<mafVMEItemScalarMatrix>(m_CurrentItem);}

  /** return the vnl_matrix data generated as output to this data pipe */
  virtual vnl_matrix<double> &GetScalarData();

protected:
  mafDataPipeInterpolatorScalarMatrix();
  mafDataPipeInterpolatorScalarMatrix(const mafDataPipeInterpolatorScalarMatrix&) = delete;
  mafDataPipeInterpolatorScalarMatrix(mafDataPipeInterpolatorScalarMatrix&&) = delete;
  mafDataPipeInterpolatorScalarMatrix& operator=(const mafDataPipeInterpolatorScalarMatrix&) = delete;
  mafDataPipeInterpolatorScalarMatrix& operator=(mafDataPipeInterpolatorScalarMatrix&&) = delete;
  ~mafDataPipeInterpolatorScalarMatrix() override;

  /** Set m_ScalarData to current item data*/
  void Execute() override;

  vnl_matrix<double> m_ScalarData;
};
