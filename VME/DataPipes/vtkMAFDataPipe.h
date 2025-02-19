#pragma once

#include "ftkConfigure.h"

#include "vtkDataSetAlgorithm.h"

class mafDataPipe;
class vtkDataSet;

/** bridge class linking VTK pipeline to VME data pipe update mechanism.
  This object is a bridge between VTK pipeline update mechanism and MAF
  VME.
*/
class FTK_VME_EXPORT vtkMAFDataPipe : public vtkDataSetAlgorithm
{
public:
  vtkTypeMacro(vtkMAFDataPipe,vtkDataSetAlgorithm)

  static vtkMAFDataPipe *New();

  /** A bit of magic making this filter to take into consideration VME data pipe MTime */
  vtkMTimeType GetMTime() override;

  /** the data pipe it is linked to */
  void SetDataPipe(mafDataPipe *dpipe);

  /** return the data pipe connected to this object */
  mafDataPipe *GetDataPipe() {return m_DataPipe;}

protected:
  vtkMAFDataPipe();
  ~vtkMAFDataPipe() override;

  int RequestUpdateExtent(vtkInformation*, vtkInformationVector**, vtkInformationVector*) override;

  int RequestDataObject(vtkInformation*, vtkInformationVector**, vtkInformationVector*) override;

  int RequestInformation(vtkInformation*, vtkInformationVector**, vtkInformationVector*) override;

  int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  int RequestUpdateTime(vtkInformation*, vtkInformationVector**, vtkInformationVector*) override;

	mafDataPipe *m_DataPipe;

private:
  vtkMAFDataPipe(const vtkMAFDataPipe&) = delete;
  vtkMAFDataPipe& operator=(const vtkMAFDataPipe&) = delete;
};
