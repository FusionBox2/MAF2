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
//------------------------------------------------------------------------------
vtkStandardNewMacro(vtkMAFDataPipe)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
vtkMAFDataPipe::vtkMAFDataPipe()
//------------------------------------------------------------------------------
{
  m_DataPipe = NULL;
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
  unsigned long mtime = this->Superclass::GetMTime();

  if (m_DataPipe)
  {
    unsigned long dpipeMTime = m_DataPipe->GetMTime();
    if (dpipeMTime > mtime)
    {
      mtime = dpipeMTime;
    }
  }

  return mtime;
}

void vtkMAFDataPipe::UpdateInformation()
{
    if (m_DataPipe)
        {mafEventBase evUnq(this, VME_OUTPUT_DATA_PREUPDATE); m_DataPipe->OnEvent(&evUnq);}
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
    // forward event to MAF data pipe
    if (m_DataPipe)
        {mafEventBase evUnq(this, VME_OUTPUT_DATA_PREUPDATE); m_DataPipe->OnEvent(&evUnq);}
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
        {mafEventBase evUnq(this, VME_OUTPUT_DATA_PREUPDATE); m_DataPipe->OnEvent(&evUnq);}
  return this->Superclass::RequestInformation(request, inputVector, outputVector);
}


//------------------------------------------------------------------------------
int vtkMAFDataPipe::RequestData(
    vtkInformation* request,
    vtkInformationVector** inputVector,
    vtkInformationVector* outputVector)
    //------------------------------------------------------------------------------
{
    vtkDataObject* input = nullptr;
    vtkInformation* inInfo = inputVector[0]->GetInformationObject(0);
    vtkInformation* outInfo = outputVector->GetInformationObject(0);
    if (inInfo)
    {
        input = inInfo->Get(vtkDataObject::DATA_OBJECT());
    }
        //get the info objects
    if (input && m_DataPipe->IsA("mafDataPipeCustom"))
        {mafEventBase evUnq(this, VME_OUTPUT_DATA_UPDATE); m_DataPipe->OnEvent(&evUnq);}
    //vtkDataObject* input = inInfo->Get(vtkDataObject::DATA_OBJECT());
    vtkDataObject* output = outInfo->Get(vtkDataObject::DATA_OBJECT());
    output->ShallowCopy(input);
    int res = Superclass::RequestData(request, inputVector, outputVector);
    if (input && !m_DataPipe->IsA("mafDataPipeCustom"))
        {mafEventBase evUnq(this, VME_OUTPUT_DATA_UPDATE); m_DataPipe->OnEvent(&evUnq);}
    return 1;
}
