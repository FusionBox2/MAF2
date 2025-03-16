#pragma once 

#include "mafDataPipe.h"
#include "vtkSmartPointer.h"

class vtkMAFDataPipe;
class vtkDataSet;

/** a data pipe which simply forwards VTK update events to the VME.
  This data pipe creates an internal vtkSource objects and links to it to 
  receive update events and forward them to the VME.
  @sa vtkMAFDataPipe
*/
class MAF_EXPORT mafDataPipeCustom : public mafDataPipe
{
public:
  mafTypeMacroN(mafDataPipeCustom);

  /** return the VTK dataset generated as output to this data pipe */
  vtkAlgorithmOutput *GetVTKOutputPort() override;

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
  mafDataPipeCustom(const mafDataPipeCustom&) = delete;
  mafDataPipeCustom(mafDataPipeCustom&&) = delete;
  mafDataPipeCustom& operator=(const mafDataPipeCustom&) = delete;
  mafDataPipeCustom& operator=(mafDataPipeCustom&&) = delete;
  ~mafDataPipeCustom() override;

  vtkSmartPointer<vtkMAFDataPipe> m_VTKDataPipe; ///< VTK pipeline bridge component
};
