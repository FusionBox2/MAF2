/*==============================================================================

Program:   Multimod Application Framework
Module:    $RCSfile: lhpOpMultiscaleExploreTest.cpp,v $
Language:  C++
Date:      $Date: 2008-01-28 16:36:59 $
Version:   $Revision: 1.2 $
Authors:   Nigel McFarlane

================================================================================
Copyright (c) 2007 University of Bedfordshire, UK (www.beds.ac.uk)
All rights reserved.
===============================================================================*/


//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------
#include "mafDefines.h" 

#include <cppunit/config/SourcePrefix.h>

#include "lhpOpMultiscaleExplore.h"
#include "lhpOpMultiscaleExploreTest.h"

#include "mafConfigure.h"

#include "vtkCubeSource.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "mafRWI.h"
#include "mafVMESurface.h"
#include "mafEvent.h"



static const bool renderingOn = false ;  // switch interactive rendering on

//------------------------------------------------------------------------------
void lhpOpMultiscaleExploreTest::setUp()
//------------------------------------------------------------------------------
{
  // Create 2 synthetic surface vme's
  m_cuboid1 = vtkCubeSource::New() ;
  m_cuboid1->SetXLength(1.0) ;
  m_cuboid1->SetYLength(1.0) ;
  m_cuboid1->SetZLength(5.0) ;
  m_cuboid1->SetCenter(0, 0, 600) ;

  m_cuboid2 = vtkCubeSource::New() ;
  m_cuboid2->SetXLength(500.0) ;
  m_cuboid2->SetYLength(100.0) ;
  m_cuboid2->SetZLength(100.0) ;
  m_cuboid2->SetCenter(0,0,0) ;

  mafNEW(m_vme1);
  m_vme1->SetName("vme1");
  m_vme1->SetDataByReference(m_cuboid1->GetOutput(), 0) ;

  mafNEW(m_vme2);
  m_vme1->SetName("vme2");
  m_vme1->SetDataByReference(m_cuboid2->GetOutput(), 0) ;


  // Instantiate renderer, window and interactor
  m_renderer = vtkRenderer::New() ;
  m_renWin = vtkRenderWindow::New() ;
  m_rwi = vtkRenderWindowInteractor::New() ;
  m_renWin->AddRenderer(m_renderer) ;
  m_renWin->SetInteractor(m_rwi) ;

}


//------------------------------------------------------------------------------
void lhpOpMultiscaleExploreTest::tearDown()
//------------------------------------------------------------------------------
{
  m_cuboid1->Delete() ;
  m_cuboid2->Delete() ;

  mafDEL(m_vme1) ;
  mafDEL(m_vme2) ;

  m_renderer->Delete() ;
  m_renWin->Delete() ;
  m_rwi->Delete() ;
}

//------------------------------------------------------------------------------
void lhpOpMultiscaleExploreTest::TestFixture()
//------------------------------------------------------------------------------
{
}


//------------------------------------------------------------------------------
// Test Constructor method
void lhpOpMultiscaleExploreTest::TestConstructor()
//------------------------------------------------------------------------------
{
  lhpOpMultiscaleExplore* op = new lhpOpMultiscaleExplore ;
  delete op ;
}

//------------------------------------------------------------------------------
// Test AddVMEtoScene method
void lhpOpMultiscaleExploreTest::TestAddVMEToScene()
//------------------------------------------------------------------------------
{
  // Create op without a dialog
  lhpOpMultiscaleExplore* op = new lhpOpMultiscaleExplore ;
  op->m_Input = m_vme1 ;

  op->CreateOpWithoutDialog(m_renderer);

  // add two vme's to scene
  op->AddVmeToScene(m_vme1) ;
  op->AddVmeToScene(m_vme2) ;

  delete op ;
}



