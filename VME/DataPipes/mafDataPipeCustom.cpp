/*=========================================================================

 Program: MAF2
 Module: mafDataPipeCustom
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


#include "mafDataPipeCustom.h"

#include "mafVME.h"
#include "vtkMAFDataPipe.h"
#include "mafEventBase.h"
#include "vtkDataSet.h"

#include <assert.h>

//------------------------------------------------------------------------------
mafCxxTypeMacro(mafDataPipeCustom)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
mafDataPipeCustom::mafDataPipeCustom()
//------------------------------------------------------------------------------
{
  vtkNEW(m_VTKDataPipe);
  m_VTKDataPipe->SetDataPipe(this);
}

//------------------------------------------------------------------------------
mafDataPipeCustom::~mafDataPipeCustom()
//------------------------------------------------------------------------------
{
  vtkDEL(m_VTKDataPipe);
} 

//------------------------------------------------------------------------------
vtkAlgorithmOutput *mafDataPipeCustom::GetVTKOutputPort()
//------------------------------------------------------------------------------
{
  m_VTKDataPipe->UpdateInformation();
  return m_VTKDataPipe->GetOutputPort();
}

//----------------------------------------------------------------------------
vtkMAFDataPipe *mafDataPipeCustom::GetVTKDataPipe()
//----------------------------------------------------------------------------
{
  return m_VTKDataPipe;
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
    if (auto dataSet = vtkDataSet::SafeDownCast(m_VTKDataPipe->GetOutput()))
    {
      dataSet->ComputeBounds();
      m_Bounds.DeepCopy(dataSet->GetBounds());
    }
  }
}
void mafDataPipeCustom::RequestDataObject()
{
  Superclass::RequestDataObject();
}
void mafDataPipeCustom::RequestInformation()
{
  Superclass::RequestInformation();
}
void mafDataPipeCustom::RequestData()
{
  Superclass::RequestData();
}

//------------------------------------------------------------------------------
void mafDataPipeCustom::SetInputConnection(vtkAlgorithmOutput *input_dataset)
//------------------------------------------------------------------------------
{
  GetVTKDataPipe()->SetInputConnection(0,input_dataset);
}

//------------------------------------------------------------------------------
void mafDataPipeCustom::SetInputData(vtkDataSet *input_dataset)
//------------------------------------------------------------------------------
{
  GetVTKDataPipe()->SetInputData(0,input_dataset);
}
