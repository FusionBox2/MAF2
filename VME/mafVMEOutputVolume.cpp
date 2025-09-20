/*=========================================================================

 Program: MAF2
 Module: mafVMEOutputVolume
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


#include "mafVMEOutputVolume.h"
#include "mafVMEOutputVTK.h"
#include "mafVME.h"
#include "mafVMEVolumeGray.h"
#include "mafDataPipe.h"
#include "mafGUI.h"
#include "mmaVolumeMaterial.h"

#include "vtkImageData.h"
#include "vtkRectilinearGrid.h"
#include "vtkUnstructuredGrid.h"

#include <assert.h>

#define NULL_STRING_DATA _R("--")

//-------------------------------------------------------------------------
mafCxxTypeMacro(mafVMEOutputVolume)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
mafVMEOutputVolume::mafVMEOutputVolume()
//-------------------------------------------------------------------------
{
	m_Material = NULL;
}

//-------------------------------------------------------------------------
mafVMEOutputVolume::~mafVMEOutputVolume()
//-------------------------------------------------------------------------
{
}

//-------------------------------------------------------------------------
vtkImageData* mafVMEOutputVolume::GetStructuredData()
//-------------------------------------------------------------------------
{
	return vtkImageData::SafeDownCast(GetVTKData());
}

//-------------------------------------------------------------------------
vtkRectilinearGrid* mafVMEOutputVolume::GetRectilinearData()
//-------------------------------------------------------------------------
{
	return vtkRectilinearGrid::SafeDownCast(GetVTKData());
}
//-------------------------------------------------------------------------
vtkUnstructuredGrid* mafVMEOutputVolume::GetUnstructuredData()
//-------------------------------------------------------------------------
{
	return vtkUnstructuredGrid::SafeDownCast(GetVTKData());
}
//-------------------------------------------------------------------------
std::shared_ptr<mmaVolumeMaterial> mafVMEOutputVolume::GetMaterial()
//-------------------------------------------------------------------------
{
	// if the VME set the material directly in the output return it
	if (m_Material)
		return  m_Material;

	// search for a material attribute in the VME connected to this output
	return GetVME() ? mmaVolumeMaterial::SafeDownCast(GetVME()->GetAttribute(mmaVolumeMaterial::GetAttributeName())) : nullptr;
}
//-------------------------------------------------------------------------
void mafVMEOutputVolume::SetMaterial(std::shared_ptr<mmaVolumeMaterial> material)
//-------------------------------------------------------------------------
{
	m_Material = material;
}

//-------------------------------------------------------------------------
mafGUI* mafVMEOutputVolume::CreateGui()
//-------------------------------------------------------------------------
{
	assert(!AccessGUI());
	auto gui = mafVMEOutput::CreateGui();

	if (GetVTKData())
	{
		this->Update();
	}
	m_VtkDataType += NULL_STRING_DATA;

	vtkDataSet* data = this->GetVTKData();
	if (data != NULL)
	{
		m_VtkDataType.clear();
		m_VtkDataType += _R(this->GetVTKData()->GetClassName());
	}

	gui->Label(_L("vtk type: "), &m_VtkDataType);
	gui->Label(_L(" bounds: "), true);
	gui->Label(&m_VolumeBounds[0]);
	gui->Label(&m_VolumeBounds[1]);
	gui->Label(&m_VolumeBounds[2]);
	gui->Label(_L("scalar range:"), true);
	gui->Label(&m_ScaralRangeString);
	gui->Divider();
	return gui;
}
//-------------------------------------------------------------------------
void mafVMEOutputVolume::Update()
//-------------------------------------------------------------------------
{
	assert(m_VME);
	m_VME->Update();
	if (GetVTKData())
	{
		m_VtkDataType = _R("");
		m_VtkDataType += _R(this->GetVTKData()->GetClassName());
		double b[6];
		m_VME->GetOutput()->GetVMELocalBounds(b);
		m_VolumeBounds[0] = _R(" xmin: ") + mafString::Format(_R("%g"), RoundValue(b[0])) + _R("   xmax: ") + mafString::Format(_R("%g"), RoundValue(b[1]));
		m_VolumeBounds[1] = _R(" ymin: ") + mafString::Format(_R("%g"), RoundValue(b[2])) + _R("   ymax: ") + mafString::Format(_R("%g"), RoundValue(b[3]));
		m_VolumeBounds[2] = _R(" zmin: ") + mafString::Format(_R("%g"), RoundValue(b[4])) + _R("   zmax: ") + mafString::Format(_R("%g"), RoundValue(b[5]));
		double srange[2];
		//this->GetVTKData()->Update();
		this->GetVTKData()->GetScalarRange(srange);
		m_ScaralRangeString = mafString::Format(_R("min: %6.2f max: %6.2f"), srange[0], srange[1]);;
		//m_ScaralRangeString << " min: " << srange[0] << "    max: " << srange[1];
	}

	UpdateGUI();
}
