#pragma once

#include "ftkConfigure.h"

#include "vtkPassThrough.h"

#include "vtkDataSetAlgorithm.h"

class mafDataPipe;
class vtkDataSet;

/** bridge class linking VTK pipeline to VME data pipe update mechanism.
  This object is a bridge between VTK pipeline update mechanism and MAF
  VME.
*/
using BaseBridgeType = vtkDataSetAlgorithm; //vtkPassThrough;
class MAF_EXPORT vtkMAFDataPipe : public BaseBridgeType
{
public:
  vtkTypeMacro(vtkMAFDataPipe,BaseBridgeType)

  static vtkMAFDataPipe *New();

  //vtkDataSet* GetOutput();
  //vtkDataSet* GetOutput(int);

  /** A bit of magic making this filter to take into consideration VME data pipe MTime */
  vtkMTimeType GetMTime() override;

  /** the data pipe it is linked to */
  void SetDataPipe(mafDataPipe *dpipe);

  mafDataPipe *GetDataPipe() const {return m_DataPipe;}

  void Update() override;

  void Update(int port) override;

  void UpdateInformation() override;

protected:
  vtkMAFDataPipe();
  vtkMAFDataPipe(const vtkMAFDataPipe&) = delete;
  vtkMAFDataPipe(vtkMAFDataPipe&&) = delete;
  vtkMAFDataPipe& operator=(const vtkMAFDataPipe&) = delete;
  vtkMAFDataPipe& operator=(vtkMAFDataPipe&&) = delete;
  ~vtkMAFDataPipe() override;

  int RequestUpdateExtent(vtkInformation*, vtkInformationVector**, vtkInformationVector*) override;

  int RequestDataObject(vtkInformation*, vtkInformationVector**, vtkInformationVector*) override;

  int RequestInformation(vtkInformation*, vtkInformationVector**, vtkInformationVector*) override;

  int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  int RequestUpdateTime(vtkInformation*, vtkInformationVector**, vtkInformationVector*) override;

  int FillOutputPortInformation(int port, vtkInformation* info) override;

	int FillInputPortInformation(int port, vtkInformation* info) override;

	mafDataPipe *m_DataPipe = nullptr;
};
