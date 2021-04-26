/*=========================================================================

 Program: MAF2
 Module: mafVMEOutputPointSet
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

#include "mafVMEOutputPointSet.h"
#include "mafVME.h"
#include "mafGUI.h"
#include "mafIndent.h"
#include "mmaMaterial.h"

#include "vtkPolyData.h"
#include "vtkDataSet.h"

#include <assert.h>

//-------------------------------------------------------------------------
mafCxxTypeMacro(mafVMEOutputPointSet)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
mafVMEOutputPointSet::mafVMEOutputPointSet()
//-------------------------------------------------------------------------
{
  m_NumPoints = _R("0");
  m_Material = NULL;
}

//-------------------------------------------------------------------------
mafVMEOutputPointSet::~mafVMEOutputPointSet()
//-------------------------------------------------------------------------
{
}

//-------------------------------------------------------------------------
vtkPolyData *mafVMEOutputPointSet::GetPointSetData()
//-------------------------------------------------------------------------
{
  return (vtkPolyData *)GetVTKData();
}

//-------------------------------------------------------------------------
mmaMaterial *mafVMEOutputPointSet::GetMaterial()
//-------------------------------------------------------------------------
{
  // if the VME set the material directly in the output return it
  if (m_Material)
    return  m_Material;

  // search for a material attribute in the VME connected to this output
  return GetVME() ? mmaMaterial::SafeDownCast(GetVME()->GetAttribute(_R("MaterialAttributes"))) : NULL;
}

//-------------------------------------------------------------------------
void mafVMEOutputPointSet::SetMaterial(mmaMaterial *material)
//-------------------------------------------------------------------------
{
  m_Material = material;
}

//-------------------------------------------------------------------------
mafGUI* mafVMEOutputPointSet::CreateGui()
//-------------------------------------------------------------------------
{
  assert(m_Gui == NULL);
  m_Gui = mafVMEOutput::CreateGui();
  if (GetPointSetData())
  {
    this->Update();
    int num = GetPointSetData()->GetNumberOfVerts();
    m_NumPoints = mafToString(num);
  }
  m_Gui->Label(_L("points: "), &m_NumPoints, true);
	m_Gui->Divider();
  return m_Gui;
}
//-------------------------------------------------------------------------
void mafVMEOutputPointSet::Update()
//-------------------------------------------------------------------------
{
  assert(m_VME);
  m_VME->Update();
  if (GetPointSetData())
  {
    int num = GetPointSetData()->GetNumberOfVerts();
    m_NumPoints = mafToString(num);
  }
  else
  {
    m_NumPoints = _R("0");
  }
  if (m_Gui)
  {
    m_Gui->Update();
  }
}
