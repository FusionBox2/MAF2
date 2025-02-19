#pragma once

#include "mafDataPipeInterpolator.h"
#include "mafVMEItemVTK.h"

class vtkMAFDataPipe;

/** data interpolator specialized for mafVMEGenericVTK (for VTK data).
  This interpolator is specialized for VTK datasets. By default selects the 
  right VMEItem, extracts the inner VTK dataset and set it as input of the 
  vtkMAFDataPipe .

  @sa vtkMAFDataPipe mafVMEGenericVTK
  
  @todo
  -
*/
class MAF_EXPORT mafDataPipeInterpolatorVTK : public mafDataPipeInterpolator
{
public:
  mafTypeMacro(mafDataPipeInterpolatorVTK,mafDataPipeInterpolator);

  /** This DataPipe accepts only VME's with internal DataArray. */
  bool Accept(mafVME *vme) override;

  /**
    Get the MTime: this is the bit of magic that makes everything work.*/
  //virtual unsigned long GetMTime();

   /**  Get the output of the interpolator item*/
  std::shared_ptr<mafVMEItemVTK> GetCurrentItemVTK() { return std::static_pointer_cast<mafVMEItemVTK>(m_CurrentItem); }

  /** return the VTK dataset generated as output to this data pipe */
  vtkAlgorithmOutput *GetVTKOutputPort() override;

  /** update the data pipe output */
  void Update() override;

protected:
  mafDataPipeInterpolatorVTK();
  ~mafDataPipeInterpolatorVTK() override;

  void PreExecute1() override;
  void PreExecute2() override;
  void Execute() override {}

  vtkSmartPointer<vtkMAFDataPipe> m_VTKDataPipe; ///< VTK pipeline bridge component

private:
  mafDataPipeInterpolatorVTK(const mafDataPipeInterpolatorVTK&) = delete;
  void operator=(const mafDataPipeInterpolatorVTK&) = delete;
};
