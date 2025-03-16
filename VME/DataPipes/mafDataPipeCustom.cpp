#include "mafDataPipeCustom.h"

#include "mafVME.h"
#include "vtkMAFDataPipe.h"
#include "vtkDataSet.h"

//------------------------------------------------------------------------------
mafDataPipeCustom::mafDataPipeCustom()
//------------------------------------------------------------------------------
{
  m_VTKDataPipe = vtkSmartPointer<vtkMAFDataPipe>::New();
  m_VTKDataPipe->SetDataPipe(this);
}

//------------------------------------------------------------------------------
mafDataPipeCustom::~mafDataPipeCustom() = default;
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
vtkAlgorithmOutput *mafDataPipeCustom::GetVTKOutputPort()
//------------------------------------------------------------------------------
{
  m_VTKDataPipe->UpdateInformation();
  return m_VTKDataPipe->GetOutputPort();
}

//----------------------------------------------------------------------------
void mafDataPipeCustom::Update()
//----------------------------------------------------------------------------
{
  m_VTKDataPipe->Update();
}

//------------------------------------------------------------------------------
void mafDataPipeCustom::UpdateBounds()
//------------------------------------------------------------------------------
{
  if (m_VTKDataPipe->GetOutput())
  {
	  m_VTKDataPipe->Update();
	  m_VTKDataPipe->GetOutput()->ComputeBounds();
	  m_Bounds.DeepCopy(m_VTKDataPipe->GetOutput()->GetBounds());
  }
}
//------------------------------------------------------------------------------
void mafDataPipeCustom::SetInputConnection(vtkAlgorithmOutput *input)
//------------------------------------------------------------------------------
{
  m_VTKDataPipe->SetInputConnection(0, input);
}

//------------------------------------------------------------------------------
void mafDataPipeCustom::SetInputData(vtkDataSet *input_dataset)
//------------------------------------------------------------------------------
{
  m_VTKDataPipe->SetInputData(0, input_dataset);
}
