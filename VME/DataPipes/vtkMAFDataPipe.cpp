#include "vtkMAFDataPipe.h"

#include "mafDataPipe.h"
#include "mafVME.h"

#include "vtkDataSet.h"
#include "vtkPolyData.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkErrorCode.h"
//------------------------------------------------------------------------------
vtkStandardNewMacro(vtkMAFDataPipe)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
vtkMAFDataPipe::vtkMAFDataPipe() = default;
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
vtkMAFDataPipe::~vtkMAFDataPipe() = default;
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/*vtkDataSet* vtkMAFDataPipe::GetOutput()
{
  return this->GetOutput(0);
}

//------------------------------------------------------------------------------
vtkDataSet* vtkMAFDataPipe::GetOutput(int port)
{
  return vtkDataSet::SafeDownCast(this->GetOutputDataObject(port));
}*/


//----------------------------------------------------------------------------
void vtkMAFDataPipe::SetDataPipe(mafDataPipe *dpipe)
//----------------------------------------------------------------------------
{
  m_DataPipe=dpipe;
}

//----------------------------------------------------------------------------
// Get the MTime. Take in consideration also modifications to the MAF data pipe
vtkMTimeType vtkMAFDataPipe::GetMTime()
//------------------------------------------------------------------------------
{
  auto mtime = this->Superclass::GetMTime();

  if (m_DataPipe)
  {
    mtime = (std::max)(mtime, (vtkMTimeType)m_DataPipe->GetMTime());
  }

  return mtime;
}

void vtkMAFDataPipe::Update()
{
  Superclass::Update();
}

void vtkMAFDataPipe::Update(int port)
{
  Superclass::Update(port);
}

void vtkMAFDataPipe::UpdateInformation()
{
  Superclass::UpdateInformation();
}

int vtkMAFDataPipe::RequestUpdateExtent(
    vtkInformation* request,
    vtkInformationVector** inputVector,
    vtkInformationVector* outputVector)
{
  return Superclass::RequestUpdateExtent(request, inputVector, outputVector);
}

int vtkMAFDataPipe::RequestDataObject(
    vtkInformation* request,
    vtkInformationVector** inputVector,
    vtkInformationVector* outputVector)
{
  /*if (this->GetNumberOfInputPorts() != 0 && inputVector[0]->GetInformationObject(0) == nullptr)
  {
    for (int i = 0; i < this->GetNumberOfOutputPorts(); ++i)
    {
      vtkPolyData* obj = vtkPolyData::New();
      outputVector->GetInformationObject(i)->Set(vtkDataObject::DATA_OBJECT(), obj);
      obj->FastDelete();
    }
    return 1;
  }*/

    // forward event to MAF data pipe
    static bool usePreupdate = true;
    if (usePreupdate && m_DataPipe)
      m_DataPipe->OnPreUpdate();
    return Superclass::RequestDataObject(request, inputVector, outputVector);
}

//------------------------------------------------------------------------------
int vtkMAFDataPipe::RequestInformation(
    vtkInformation* request,
    vtkInformationVector** inputVector,
    vtkInformationVector* outputVector)
//------------------------------------------------------------------------------
{
  // forward event to MAF data pipe
  if (m_DataPipe)
    m_DataPipe->OnPreUpdate();
  return this->Superclass::RequestInformation(request, inputVector, outputVector);
}

int vtkMAFDataPipe::RequestUpdateTime(
  vtkInformation* request,
  vtkInformationVector** inputVector,
  vtkInformationVector* outputVector)
{
  return Superclass::RequestUpdateTime(request, inputVector, outputVector);
}

//------------------------------------------------------------------------------
int vtkMAFDataPipe::RequestData(
    vtkInformation* request,
    vtkInformationVector** inputVector,
    vtkInformationVector* outputVector)
    //------------------------------------------------------------------------------
{
  if (m_DataPipe && m_DataPipe->IsA("mafDataPipeCustom"))
    m_DataPipe->OnUpdate();

  vtkInformation* inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation* outInfo = outputVector->GetInformationObject(0);

	if (!inInfo)
  {
    return 0;//switch to 1 if allow empty output
  }

	//get the info objects
  vtkDataObject* input = inInfo->Get(vtkDataObject::DATA_OBJECT());
  vtkDataObject* output = outInfo->Get(vtkDataObject::DATA_OBJECT());

	output->ShallowCopy(input);

  static bool doInfo = true;
  if (doInfo)
  {
    vtkInformation* inputInfo = inputVector[0]->GetInformationObject(0);
    vtkDataObject* inputObject = inputInfo->Get(vtkDataObject::DATA_OBJECT());
    vtkInformation* outputInfo = outputVector->GetInformationObject(0);
    vtkInformation* dataInfo = inputObject->GetInformation();
    if (dataInfo->Get(vtkDataObject::DATA_EXTENT_TYPE()) == VTK_3D_EXTENT)
    {
      int extent[6];
      dataInfo->Get(vtkDataObject::DATA_EXTENT(), extent);
      outputInfo->Set(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(), extent, 6);
    }
  }
	if (m_DataPipe && !m_DataPipe->IsA("mafDataPipeCustom"))
    m_DataPipe->OnUpdate();

	return 1;
}

int vtkMAFDataPipe::FillOutputPortInformation(int port, vtkInformation* info)
{
  return Superclass::FillOutputPortInformation(port, info);
}

int vtkMAFDataPipe::FillInputPortInformation(int port, vtkInformation* info)
{
  info->Set(vtkAlgorithm::INPUT_IS_OPTIONAL(), 1);
  return Superclass::FillInputPortInformation(port, info);
}
