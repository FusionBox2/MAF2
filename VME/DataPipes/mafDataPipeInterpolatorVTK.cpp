#include "mafDataPipeInterpolatorVTK.h"

#include "mafVME.h"
#include "mafVMEItemVTK.h"
#include "mafVMEGeneric.h"
#include "vtkMAFDataPipe.h"
#include "vtkDataSet.h"
#include "vtkTrivialProducer.h"
#include "vtkStreamingDemandDrivenPipeline.h"

//------------------------------------------------------------------------------
mafDataPipeInterpolatorVTK::mafDataPipeInterpolatorVTK()
//------------------------------------------------------------------------------
{
  m_VTKDataPipe = vtkNew<vtkMAFDataPipe>();
  vtkNew<vtkStreamingDemandDrivenPipeline> sddp;
  m_VTKDataPipe->SetExecutive(sddp);
  //vtkNew<vtkTrivialProducer> prod;
  //m_VTKDataPipe->SetInputConnection(prod->GetOutputPort());
  m_VTKDataPipe->SetDataPipe(this);
}

//------------------------------------------------------------------------------
mafDataPipeInterpolatorVTK::~mafDataPipeInterpolatorVTK() = default;
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
bool mafDataPipeInterpolatorVTK::Accept(mafVME *vme)
//------------------------------------------------------------------------------
{
  return Superclass::Accept(vme)&&vme->IsA(mafVMEGeneric::GetStaticTypeId());
}

//------------------------------------------------------------------------------
vtkAlgorithmOutput *mafDataPipeInterpolatorVTK::GetVTKOutputPort()
//------------------------------------------------------------------------------
{
  m_VTKDataPipe->UpdateInformation();
  vtkDataSet *data = vtkDataSet::SafeDownCast(m_VTKDataPipe->GetInput());
  return (data != NULL) ? m_VTKDataPipe->GetOutputPort() : NULL;
}

//----------------------------------------------------------------------------
void mafDataPipeInterpolatorVTK::Update()
//----------------------------------------------------------------------------
{
  m_VTKDataPipe->Update();
}

//------------------------------------------------------------------------------
void mafDataPipeInterpolatorVTK::Execute()
//------------------------------------------------------------------------------
{
  Superclass::Execute();

  auto mtime=this->GetMTime();

  // if the current item is changed set the data inside new item as input for the interpolator
  // more specialized interpolators could redefine this to have more inputs (e.g. when 
  // interpolating different items)
  if ( m_CurrentItem && (m_CurrentItem.get()!=m_OldItem || \
    mtime>m_UpdateTime.GetMTime() || \
    //mtime>m_VTKDataPipe->GetInformationTime() ||
    !m_CurrentItem->IsDataPresent()))
  {
    vtkDataSet *data = GetCurrentItemVTK()->GetData();
    if (data != NULL)
    {
      if (auto producer = vtkTrivialProducer::SafeDownCast(m_VTKDataPipe->GetInputAlgorithm()))
        producer->SetOutput(data);
      else
        m_VTKDataPipe->SetInputData(data);
      //m_VTKDataPipe->SetInputData(data);
      m_UpdateTime.Modified();
    }
  } 
}
