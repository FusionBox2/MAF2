/*=========================================================================

 Program: MAF2
 Module: vtkMAFDataPipe
 Authors: Marco Petrone
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------


#include "mafDecl.h"
#include "vtkMAFDataPipe.h"
#include "mafEventBase.h"

#include "mafDataPipeInterpolatorVTK.h"
#include "mafVME.h"

#include "vtkDataSet.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkErrorCode.h"
#include "vtkPolyData.h"
//------------------------------------------------------------------------------
vtkStandardNewMacro(vtkMAFDataPipe)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
vtkMAFDataPipe::vtkMAFDataPipe()
//------------------------------------------------------------------------------
{
  m_DataPipe = NULL;
  Output = nullptr;
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

//------------------------------------------------------------------------------
int vtkMAFDataPipe::FillInputPortInformation(int port, vtkInformation* info)
{
  info->Set(vtkAlgorithm::INPUT_IS_OPTIONAL(), 1);
  return Superclass::FillInputPortInformation(port, info);
}


int vtkMAFDataPipe::RequestDataObject(
    vtkInformation* request,
    vtkInformationVector** inputVector,
    vtkInformationVector* outputVector)
{
  vtkInformation* inInfo = inputVector[0]->GetInformationObject(0);
  vtkDataSet* input = Output;
  if (!input && inInfo)
  {
    input = vtkDataSet::SafeDownCast(inInfo->Get(vtkDataObject::DATA_OBJECT()));
  }

  if(!input)
  {
	  for (int i = 0; i < this->GetNumberOfOutputPorts(); ++i)
	  {
      vtkInformation* info = outputVector->GetInformationObject(i);
      vtkDataSet* output = vtkDataSet::SafeDownCast(info->Get(vtkDataObject::DATA_OBJECT()));
      if (!output || !output->IsA("vtkPolyData"))
      {
        vtkDataSet* newOutput = vtkPolyData::New();
        info->Set(vtkDataObject::DATA_OBJECT(), newOutput);
        newOutput->Delete();
      }
      vtkPolyData* obj = vtkPolyData::New();
	  	outputVector->GetInformationObject(i)->Set(vtkDataObject::DATA_OBJECT(), obj);
	  	obj->FastDelete();
	  }
  }
  else
  {
    // for each output
    for (int i = 0; i < this->GetNumberOfOutputPorts(); ++i)
    {
      vtkInformation* info = outputVector->GetInformationObject(i);
      vtkDataSet* output = vtkDataSet::SafeDownCast(info->Get(vtkDataObject::DATA_OBJECT()));

      if (!output || !output->IsA(input->GetClassName()))
      {
        vtkDataSet* newOutput = input->NewInstance();
        info->Set(vtkDataObject::DATA_OBJECT(), newOutput);
        newOutput->Delete();
      }
    }
  }
  return 1;
}

//------------------------------------------------------------------------------
int vtkMAFDataPipe::RequestInformation(
    vtkInformation* request,
    vtkInformationVector** inputVector,
    vtkInformationVector* outputVector)
//------------------------------------------------------------------------------
{
  static bool usePipe = true;
  // forward event to MAF data pipe
  if (usePipe && m_DataPipe)
    {m_DataPipe->RequestInformation();}
  return this->Superclass::RequestInformation(request, inputVector, outputVector);
}


//------------------------------------------------------------------------------
int vtkMAFDataPipe::RequestData(
    vtkInformation* request,
    vtkInformationVector** inputVector,
    vtkInformationVector* outputVector)
    //------------------------------------------------------------------------------
{
    vtkDataObject* input = Output;
    vtkInformation* inInfo = inputVector[0]->GetInformationObject(0);
    vtkInformation* outInfo = outputVector->GetInformationObject(0);
    if (!input&& inInfo)
    {
        input = inInfo->Get(vtkDataObject::DATA_OBJECT());
    }
        //get the info objects
    if (input && m_DataPipe->IsA("mafDataPipeCustom"))
        {m_DataPipe->RequestData();}
    //vtkDataObject* input = inInfo->Get(vtkDataObject::DATA_OBJECT());
    vtkDataObject* output = outInfo->Get(vtkDataObject::DATA_OBJECT());
    output->ShallowCopy(input);
    int res = Superclass::RequestData(request, inputVector, outputVector);
    if (input && !m_DataPipe->IsA("mafDataPipeCustom"))
        {m_DataPipe->RequestData();}
    return 1;
}

vtkDataSet* vtkMAFDataPipe::GetOutputOb()
{
  return Output;
}

void vtkMAFDataPipe::SetOutputOb(vtkDataSet* newOutput)
{
  vtkDataSet* oldOutput = this->Output;
  if (newOutput != oldOutput)
  {
    if (newOutput)
    {
      newOutput->Register(this);
    }
    this->Output = newOutput;
    if (oldOutput)
    {
      oldOutput->UnRegister(this);
    }
    this->Modified();
  }
}

