/*=========================================================================

 Program: MAF2
 Module: vtkMAFCellsFilterTest
 Authors: Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include <cppunit/config/SourcePrefix.h>

#include "vtkSmartPointer.h"
#include "vtkSphereSource.h"
#include "vtkMAFCellsFilter.h"
#include "vtkMAFCellsFilterTest.h"

#include <assert.h>

void vtkMAFCellsFilterTest::setUp()
{
 
}

void vtkMAFCellsFilterTest::tearDown()
{

}

void vtkMAFCellsFilterTest::TestFixture()
{

}

void vtkMAFCellsFilterTest::TestMarkCell()
{  
  vtkNew<vtkSphereSource> sphere;
  sphere->Update();

	vtkNew<vtkMAFCellsFilter> mc;
  mc->SetInput(sphere->GetOutput());
  mc->Update();

  int nc = sphere->GetOutput()->GetNumberOfCells();
  assert(nc != 0);
 
  const int markedCellsNumber = 3;
  int markedCellsIDs[markedCellsNumber] = {0, 10, 20} ;
  
  for (int i = 0;i < markedCellsNumber; i++)
  {
    mc->MarkCell(markedCellsIDs[i]);
  }

  mc->Update();

  CPPUNIT_ASSERT(mc->GetNumberOfMarkedCells() == 3);
}



void vtkMAFCellsFilterTest::TestUnmarkCell()
{  
  vtkNew<vtkSphereSource> sphere;
  sphere->Update();

  vtkNew<vtkMAFCellsFilter> mc;
  mc->SetInput(sphere->GetOutput());
  mc->Update();

  int nc = sphere->GetOutput()->GetNumberOfCells();
  assert(nc != 0);

  const int markedCellsNumber = 3;
  int markedCellsIDs[markedCellsNumber] = {0, 10, 20} ;

  for (int i = 0;i < markedCellsNumber; i++)
  {
    mc->MarkCell(markedCellsIDs[i]);
  }

  mc->Update();

  CPPUNIT_ASSERT(mc->GetNumberOfMarkedCells() == 3);
  
  for (int i = 0;i < markedCellsNumber; i++)
  {
    mc->UnmarkCell(markedCellsIDs[i]);
  }

  mc->Update();

  CPPUNIT_ASSERT(mc->GetNumberOfMarkedCells() == 0);

}

void vtkMAFCellsFilterTest::TestToggleCell()
{  
  vtkNew<vtkSphereSource> sphere;
  sphere->Update();

  vtkNew<vtkMAFCellsFilter> mc;
  mc->SetInput(sphere->GetOutput());
  mc->Update();

  int nc = sphere->GetOutput()->GetNumberOfCells();
  assert(nc != 0);

  const int markedCellsNumber = 3;
  int markedCellsIDs[markedCellsNumber] = {0, 10, 20} ;

  for (int i = 0;i < markedCellsNumber; i++)
  {
    mc->MarkCell(markedCellsIDs[i]);
  }

  mc->Update();

  CPPUNIT_ASSERT(mc->GetNumberOfMarkedCells() == 3);

  for (int i = 0;i < markedCellsNumber; i++)
  {
    mc->ToggleCell(markedCellsIDs[i]);
  }

  mc->Update();

  CPPUNIT_ASSERT(mc->GetNumberOfMarkedCells() == 0);

}

void vtkMAFCellsFilterTest::TestUndoMarks()
{  
  vtkNew<vtkSphereSource> sphere;
  sphere->Update();

  vtkNew<vtkMAFCellsFilter> mc;
  mc->SetInput(sphere->GetOutput());
  mc->Update();

  int nc = sphere->GetOutput()->GetNumberOfCells();
  assert(nc != 0);

  const int markedCellsNumber = 3;
  int markedCellsIDs[markedCellsNumber] = {0, 10, 20} ;

  for (int i = 0;i < markedCellsNumber; i++)
  {
    mc->MarkCell(markedCellsIDs[i]);
  }

  mc->Update();

  CPPUNIT_ASSERT(mc->GetNumberOfMarkedCells() == 3);

  mc->UndoMarks();
  mc->Update();

  CPPUNIT_ASSERT(mc->GetNumberOfMarkedCells() == 0);

}

void vtkMAFCellsFilterTest::TestGetNumberOfMarkedCells()
{  
  vtkNew<vtkSphereSource> sphere;
  sphere->Update();

  vtkNew<vtkMAFCellsFilter> mc;
  mc->SetInput(sphere->GetOutput());
  mc->Update();
  
  CPPUNIT_ASSERT(mc->GetNumberOfMarkedCells() == 0);

  int nc = sphere->GetOutput()->GetNumberOfCells();
  assert(nc != 0);

  const int markedCellsNumber = 5;
  int markedCellsIDs[markedCellsNumber] = {0, 10, 20, 30, 40} ;

  for (int i = 0;i < markedCellsNumber; i++)
  {
    mc->MarkCell(markedCellsIDs[i]);
  }

  mc->Update();

  CPPUNIT_ASSERT(mc->GetNumberOfMarkedCells() == 5);
}

void vtkMAFCellsFilterTest::TestGetIdMarkedCell()
{  
  vtkNew<vtkSphereSource> sphere;
  sphere->Update();

  vtkNew<vtkMAFCellsFilter> mc;
  mc->SetInput(sphere->GetOutput());
  mc->Update();

  int nc = sphere->GetOutput()->GetNumberOfCells();
  assert(nc != 0);

  const int markedCellsNumber = 3;
  int markedCellsIDs[markedCellsNumber] = {0, 10, 20} ;

  for (int i = 0;i < markedCellsNumber; i++)
  {
    mc->MarkCell(markedCellsIDs[i]);
  }

  mc->Update();

  CPPUNIT_ASSERT(mc->GetNumberOfMarkedCells() == 3);
  
  for (int i = 0;i < markedCellsNumber; i++)
  {
    CPPUNIT_ASSERT_EQUAL(mc->GetIdMarkedCell(i) , markedCellsIDs[i]);
  }

}
