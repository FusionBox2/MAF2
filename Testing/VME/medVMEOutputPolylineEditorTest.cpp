/*=========================================================================

 Program: MAF2Medical
 Module: medVMEOutputPolylineEditorTest
 Authors: Daniele Giunchi
 
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

#include <cppunit/config/SourcePrefix.h>
#include "medVMEOutputPolylineEditorTest.h"

#include "medVMEOutputPolylineEditor.h"

#include "medVMEPolylineEditor.h"
#include "mafMatrix.h"
#include "vtkMAFSmartPointer.h"

#include "vtkPolyData.h"
#include "vtkCellArray.h"
#include "vtkPoints.h"
#include "vtkPointData.h"

#include <iostream>

#define TEST_RESULT CPPUNIT_ASSERT(m_Result)
#define TOLERANCE 1.0e-3

//----------------------------------------------------------------------------
void medVMEOutputPolylineEditorTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void medVMEOutputPolylineEditorTest::setUp()
//----------------------------------------------------------------------------
{
	m_Result = false;
}
//----------------------------------------------------------------------------
void medVMEOutputPolylineEditorTest::tearDown()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void medVMEOutputPolylineEditorTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
  medVMEOutputPolylineEditor outputSurface;
}
//----------------------------------------------------------------------------
void medVMEOutputPolylineEditorTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  medVMEOutputPolylineEditor *outputSurface = new medVMEOutputPolylineEditor();
  cppDEL(outputSurface);
}
//----------------------------------------------------------------------------
void medVMEOutputPolylineEditorTest::TestGetPolylineData_Update()
//----------------------------------------------------------------------------
{
  //create a parametric surface
  medVMEPolylineEditor *vmePolylineEditor;
  mafNEW(vmePolylineEditor);
  
  vtkMAFSmartPointer<vtkPoints> pts;
  pts->InsertNextPoint(0.0,0.0,0.0);
  pts->InsertNextPoint(1.0,1.0,1.0);

  vtkMAFSmartPointer<vtkCellArray> cells;
  vtkIdType lineids[2];
  lineids[0] = 0;
  lineids[1] = 1;
  cells->InsertNextCell(2,lineids);

  vtkMAFSmartPointer<vtkPolyData> polyline;
  polyline->SetPoints(pts);
  polyline->SetLines(cells);
  vmePolylineEditor->SetData(polyline, 0.0);

  medVMEOutputPolylineEditor *outputPolylineEditor = NULL;
  outputPolylineEditor = medVMEOutputPolylineEditor::SafeDownCast(vmePolylineEditor->GetOutput());
  outputPolylineEditor->Update();
  m_Result = outputPolylineEditor != NULL && 
             vmePolylineEditor->GetOutput()->GetVTKData() == outputPolylineEditor->GetPolylineData();

  TEST_RESULT;

  mafDEL(vmePolylineEditor);
}