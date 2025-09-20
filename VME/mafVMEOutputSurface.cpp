/*=========================================================================

 Program: MAF2
 Module: mafVMEOutputSurface
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



#include "mafVMEOutputSurface.h"
#include "mafVME.h"
#include "mafIndent.h"
#include "mafDataPipe.h"
#include "mafGUI.h"
#include "mmaMaterial.h"

#include "vtkPolyData.h"
#include "vtkImageData.h"
#include "vtkAlgorithm.h"
#include "vtkAlgorithmOutput.h"

#include <assert.h>

//-------------------------------------------------------------------------
mafCxxTypeMacro(mafVMEOutputSurface)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
mafVMEOutputSurface::mafVMEOutputSurface()
//-------------------------------------------------------------------------
{
	m_Texture = NULL;
	m_Material = NULL;
	m_NumTriangles = _R("0");
}

//-------------------------------------------------------------------------
mafVMEOutputSurface::~mafVMEOutputSurface()
//-------------------------------------------------------------------------
{
	vtkDEL(m_Texture);
}

//-------------------------------------------------------------------------
vtkPolyData* mafVMEOutputSurface::GetSurfaceData()
//-------------------------------------------------------------------------
{
	return (vtkPolyData*)GetVTKData();
}

//-------------------------------------------------------------------------
void mafVMEOutputSurface::SetTexture(vtkImageData* tex)
//-------------------------------------------------------------------------
{
	if (m_Texture != tex)
	{
		m_Texture = tex;
		if (m_Texture)
			m_Texture->Register(NULL);
	}
}
//-------------------------------------------------------------------------
vtkImageData* mafVMEOutputSurface::GetTexture()
//-------------------------------------------------------------------------
{
	if (GetVTKData())
		GetVTKOutputPort()->GetProducer()->UpdateInformation();
	return m_Texture;
}

//-------------------------------------------------------------------------
std::shared_ptr<mmaMaterial> mafVMEOutputSurface::GetMaterial()
//-------------------------------------------------------------------------
{
	// if the VME set the material directly in the output return it
	if (m_Material)
		return  m_Material;

	// search for a material attribute in the VME connected to this output
	return GetVME() ? mmaMaterial::SafeDownCast(GetVME()->GetAttribute(mmaMaterial::GetAttributeName())) : nullptr;
}

//-------------------------------------------------------------------------
void mafVMEOutputSurface::SetMaterial(std::shared_ptr<mmaMaterial> material)
//-------------------------------------------------------------------------
{
	m_Material = material;
}
//-------------------------------------------------------------------------
mafGUI* mafVMEOutputSurface::CreateGui()
//-------------------------------------------------------------------------
{
	assert(!AccessGUI());
	auto gui = mafVMEOutput::CreateGui();
	this->Update();
	/*if (GetSurfaceData())
	{
	  this->Update();
	  int num = GetSurfaceData()->GetNumberOfPolys();
	  m_NumTriangles = num;
	}*/
	gui->Label(_L("triangles: "), &m_NumTriangles, true);
	gui->Divider();
	return gui;
}
//-------------------------------------------------------------------------
void mafVMEOutputSurface::Update()
//-------------------------------------------------------------------------
{
	assert(m_VME);
	m_VME->Update();
	if (GetSurfaceData())
	{
		//GetSurfaceData()->Modified();	//BES: 12.9.2012 - I do not see any reason for this except to make troubles during rendering since this forces rerender of everything
		//this->Update();
		int num = GetSurfaceData()->GetNumberOfPolys();
		m_NumTriangles = mafToString(num);
	}
	else
	{
		m_NumTriangles = _L("0");
	}
	UpdateGUI();
}
