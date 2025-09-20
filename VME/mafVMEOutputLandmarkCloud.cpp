/*=========================================================================

 Program: MAF2
 Module: mafVMEOutputLandmarkCloud
 Authors: Paolo Quadrani

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

#include "mafVMEOutputLandmarkCloud.h"
#include "mafGUI.h"
#include "mafDataPipe.h"

#include "mafVME.h"
#include "mafVMELandmarkCloud.h"

#include "vtkDataSet.h"

#include <assert.h>

//-------------------------------------------------------------------------
mafVMEOutputLandmarkCloud::mafVMEOutputLandmarkCloud()
//-------------------------------------------------------------------------
{
	m_NumLandmarks = _R("0");
}

mafVMEOutputLandmarkCloud::~mafVMEOutputLandmarkCloud() = default;

//-------------------------------------------------------------------------
vtkAlgorithmOutput* mafVMEOutputLandmarkCloud::GetVTKOutputPort()
//-------------------------------------------------------------------------
{
	assert(m_VME);
	assert(mafVMELandmarkCloud::SafeDownCast(m_VME));
	if (mafVMELandmarkCloud::SafeDownCast(m_VME)->IsOpen())
	{
		return NULL;
	}
	else
	{
		return Superclass::GetVTKOutputPort();
	}
}

//-------------------------------------------------------------------------
mafGUI* mafVMEOutputLandmarkCloud::CreateGui()
//-------------------------------------------------------------------------
{
	assert(!AccessGUI());
	auto gui = mafVMEOutput::CreateGui();
	m_NumLandmarks = mafToString(mafVMELandmarkCloud::SafeDownCast(m_VME)->GetNumberOfLandmarks());
	gui->Label(_L("points: "), &m_NumLandmarks, true);
	gui->Divider();
	return gui;
}
//-------------------------------------------------------------------------
void mafVMEOutputLandmarkCloud::Update()
//-------------------------------------------------------------------------
{
	assert(m_VME);
	m_VME->Update();
	m_NumLandmarks = mafToString(mafVMELandmarkCloud::SafeDownCast(m_VME)->GetNumberOfLandmarks());
	UpdateGUI();
}
