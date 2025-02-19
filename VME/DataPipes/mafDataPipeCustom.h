#pragma once


#include "mafDataPipeInterpolator.h"

class vtkMAFDataPipe;

/** a data pipe which simply forwards VTK update events to the VME.
  This data pipe creates an internal vtkSource objects and links to it to 
  receive update events and forward them to the VME.
  @sa vtkMAFDataPipe
*/
class MAF_EXPORT mafDataPipeCustom : public mafDataPipe
{
public:
  mafTypeMacro(mafDataPipeCustom,mafDataPipe);

  /** return the VTK dataset generated as output to this data pipe */
  vtkAlgorithmOutput *GetVTKOutputPort() override;

  /** return the bridge object between VTK datapipe and MAF update mechanism */
  vtkMAFDataPipe *GetVTKDataPipe();

  /** update the data pipe output */
  void Update() override;

  /** update bounds of the data pipe copying from VTK filter output bounds */
  void UpdateBounds() override;

  /** sets the first input of the datapipe */
  void SetInputConnection(vtkAlgorithmOutput *input_dataset);

  /** sets the first input of the datapipe */
  void SetInputData(vtkDataSet *input_dataset);

protected:
  mafDataPipeCustom();
  ~mafDataPipeCustom() override;

  vtkMAFDataPipe *m_VTKDataPipe; ///< VTK pipeline bridge component

private:
  mafDataPipeCustom(const mafDataPipeCustom&) = delete;
  void operator=(const mafDataPipeCustom&) = delete;
};
