/*=========================================================================

 Program: MAF2
 Module: mafVMEOutputScalar
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



#include "mafVMEOutputScalar.h"
#include "mafGUI.h"

#include "mafVMEScalar.h"
#include "mafScalarVector.h"
#include "mafIndent.h"

#ifdef MAF_USE_VTK
#include "vtkMAFSmartPointer.h"
#include "vtkPoints.h"
#include "vtkCellArray.h"
#include "vtkDoubleArray.h"
#include "vtkPointData.h"
#include "vtkPolyData.h"
#endif

#include <assert.h>

//-------------------------------------------------------------------------
mafCxxTypeMacro(mafVMEOutputScalar)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
mafVMEOutputScalar::mafVMEOutputScalar()
//-------------------------------------------------------------------------
{
#ifdef MAF_USE_VTK
  vtkNEW(m_Polydata);
#endif

  m_ScalarDataString = _R("");
}

//-------------------------------------------------------------------------
mafVMEOutputScalar::~mafVMEOutputScalar()
//-------------------------------------------------------------------------
{
#ifdef MAF_USE_VTK
  vtkDEL(m_Polydata);
#endif
}

//-------------------------------------------------------------------------
double mafVMEOutputScalar::GetScalarData()
//-------------------------------------------------------------------------
{
  assert(m_VME);
  mafScalarVector *scalarVector = ((mafVMEScalar *)m_VME)->GetScalarVector();
  return scalarVector->GetItemBefore(GetTimeStamp());
  //return scalarVector->GetNearestScalar(GetTimeStamp());  // Also this method could be used; depend on interpolation
                                                            // we want to use.
}

#ifdef MAF_USE_VTK
//-------------------------------------------------------------------------
vtkDataSet *mafVMEOutputScalar::GetVTKData()
//-------------------------------------------------------------------------
{
  UpdateVTKRepresentation();
  return m_Polydata;
}
//-------------------------------------------------------------------------
void mafVMEOutputScalar::UpdateVTKRepresentation()
//-------------------------------------------------------------------------
{
  double data = GetScalarData();

  int pointId[2];
  double time_point = GetTimeStamp();
  vtkMAFSmartPointer<vtkPoints> points;
  vtkMAFSmartPointer<vtkCellArray> verts;
  vtkMAFSmartPointer<vtkDoubleArray> scalars;
  scalars->SetNumberOfValues(1);
  scalars->SetNumberOfComponents(1);
  scalars->FillComponent(0,data);
  points->InsertPoint(0, 0.0, 0.0, 0.0);
  pointId[0] = 0;
  pointId[1] = 0;
  verts->InsertNextCell(2,pointId);

  m_Polydata->SetPoints(points);
  m_Polydata->SetLines(verts);
  m_Polydata->GetPointData()->SetScalars(scalars);
  m_Polydata->Modified();
}
#endif

//-------------------------------------------------------------------------
mafGUI* mafVMEOutputScalar::CreateGui()
//-------------------------------------------------------------------------
{
  assert(m_Gui == NULL);
  m_Gui = mafVMEOutput::CreateGui();

  if (m_VME)
  {
    this->Update();
  }
  /*double data = GetScalarData();
  m_ScalarDataString = "";
  m_ScalarDataString << data;*/
  m_Gui->Label(_L("data: "),&m_ScalarDataString);
	m_Gui->Divider(); 
	return m_Gui;
}

//-------------------------------------------------------------------------
void mafVMEOutputScalar::Update()
//-------------------------------------------------------------------------
{
  double data = GetScalarData();
  m_ScalarDataString = _R("");
  m_ScalarDataString += mafToString(data);
  if (m_Gui)
  {
    m_Gui->Update();
  }
}
