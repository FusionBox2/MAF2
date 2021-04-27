/*=========================================================================

 Program: MAF2
 Module: mafVMEOutputMesh
 Authors: Stefano Perticoni
 
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



#include "mafVMEOutputMesh.h"
#include "mafVME.h"
#include "mafIndent.h"
#include "mafDataPipe.h"
#include "mafGUI.h"
#include "mmaMaterial.h"

#include "vtkUnstructuredGrid.h"
#include "vtkImageData.h"

#include <assert.h>

//-------------------------------------------------------------------------
mafCxxTypeMacro(mafVMEOutputMesh)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
mafVMEOutputMesh::mafVMEOutputMesh()
//-------------------------------------------------------------------------
{
  m_NumCells  = _R("0");
	m_Material = NULL;
}

//-------------------------------------------------------------------------
mafVMEOutputMesh::~mafVMEOutputMesh()
//-------------------------------------------------------------------------
{
}

//-------------------------------------------------------------------------
vtkUnstructuredGrid *mafVMEOutputMesh::GetUnstructuredGridData()
//-------------------------------------------------------------------------
{
  return vtkUnstructuredGrid::SafeDownCast(GetVTKData());
}

//-------------------------------------------------------------------------
mafGUI* mafVMEOutputMesh::CreateGui()
//-------------------------------------------------------------------------
{
  assert(m_Gui == NULL);
  m_Gui = mafVMEOutput::CreateGui();
  if (GetUnstructuredGridData())
  {
    this->Update();
    int num = GetUnstructuredGridData()->GetNumberOfCells();
    m_NumCells = mafToString(num);
  }
  m_Gui->Label(_L("cells: "), &m_NumCells, true);
  m_Gui->Divider();
	return m_Gui;
}
//-------------------------------------------------------------------------
void mafVMEOutputMesh::Update()
//-------------------------------------------------------------------------
{
  assert(m_VME);
  m_VME->Update();
  if (GetUnstructuredGridData())
  {
    int num = GetUnstructuredGridData()->GetNumberOfCells();
    m_NumCells = mafToString(num);
  }
  else
  {
    m_NumCells = _L("0");
  }
  if (m_Gui)
  {
    m_Gui->Update();
  }
}
//-------------------------------------------------------------------------
mmaMaterial *mafVMEOutputMesh::GetMaterial()
//-------------------------------------------------------------------------
{
	// if the VME set the material directly in the output return it
	if (m_Material)
		return  m_Material;

	// search for a material attribute in the VME connected to this output
	return GetVME() ? mmaMaterial::SafeDownCast(GetVME()->GetAttribute(_R("MaterialAttributes"))) : NULL;
}

//-------------------------------------------------------------------------
void mafVMEOutputMesh::SetMaterial(mmaMaterial *material)
//-------------------------------------------------------------------------
{
	m_Material = material;
}
