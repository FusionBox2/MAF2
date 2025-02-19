#include "mafDecl.h"
#include "vtkMAFDataPipe.h"

#include "mafDataPipe.h"
#include "mafVME.h"

#include "vtkDataSet.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkErrorCode.h"

//------------------------------------------------------------------------------
vtkStandardNewMacro(vtkMAFDataPipe)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
vtkMAFDataPipe::vtkMAFDataPipe()
//------------------------------------------------------------------------------
{
  m_DataPipe = nullptr;
}

//------------------------------------------------------------------------------
vtkMAFDataPipe::~vtkMAFDataPipe()
//------------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void vtkMAFDataPipe::SetDataPipe(mafDataPipe *dpipe)
//----------------------------------------------------------------------------
{
  m_DataPipe = dpipe;
}

//----------------------------------------------------------------------------
// Get the MTime. Take in consideration also modifications to the MAF data pipe
vtkMTimeType vtkMAFDataPipe::GetMTime()
//------------------------------------------------------------------------------
{
  auto mtime = this->Superclass::GetMTime();

  if (m_DataPipe)
  {
    mtime = (std::max)(mtime, m_DataPipe->GetMTime());
  }

  return mtime;
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
    // forward event to MAF data pipe
    if (m_DataPipe)
        m_DataPipe->OnPreUpdate1();
    return Superclass::RequestDataObject(request, inputVector, outputVector);
}

//------------------------------------------------------------------------------
int vtkMAFDataPipe::RequestInformation(
    vtkInformation* request,
    vtkInformationVector** inputVector,
    vtkInformationVector* outputVector)
//------------------------------------------------------------------------------
{
  if (m_DataPipe)
    m_DataPipe->OnPreUpdate2();
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

	if (m_DataPipe && !m_DataPipe->IsA("mafDataPipeCustom"))
    m_DataPipe->OnUpdate();

	return 1;
}
