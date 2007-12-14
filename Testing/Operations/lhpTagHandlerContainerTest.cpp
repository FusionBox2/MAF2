/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: lhpTagHandlerContainerTest.cpp,v $
Language:  C++
Date:      $Date: 2007-12-14 14:48:17 $
Version:   $Revision: 1.1 $
Authors:   Daniele Giunchi
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include <cppunit/config/SourcePrefix.h>
#include "lhpTagHandlerContainerTest.h"

#include "lhpTagHandlerContainer.h"
#include "lhpFactoryTagHandler.h"
#include "mafVME.h"
#include "mafTagArray.h"
#include "mafTransform.h"
#include "mafVMEOutputNULL.h"
#include "mafVersion.h"
#include <iostream>
#include "mafString.h"
#include "mafVMESurface.h"

#include "vtkMAFSmartPointer.h"
#include "vtkSphereSource.h"
#include "vtkDataSet.h"


//----------------------------------------------------
void lhpTagHandlerContainerTest::setUp()
//----------------------------------------------------
{
  m_TagHandlersFactory = lhpFactoryTagHandler::GetInstance();
  CreateTestData();
}
//----------------------------------------------------
void lhpTagHandlerContainerTest::tearDown()
//----------------------------------------------------
{
  m_TestVMESurface->Delete();
}
//----------------------------------------------------
void lhpTagHandlerContainerTest::CreateTestData()
//----------------------------------------------------
{
  m_TestVMESurface = NULL;  
  m_TestVMESurface = mafVMESurface::New();
  vtkMAFSmartPointer<vtkSphereSource> sphereSource;

  m_TestVMESurface->SetData(sphereSource->GetOutput(),-1);

  // since this is not enough ie test on data is failing..
  // m_SphereSurfaceVME->Update();
  // I have have to use the MAF Magic update!
  m_TestVMESurface->GetOutput()->GetVTKData()->Update();

  CPPUNIT_ASSERT(m_TestVMESurface->GetOutput()->GetVTKData()->GetNumberOfCells() > 0);
}

void lhpTagHandlerContainerTest::TestlhpTagHandler_L0000_resource_data_Type_Timevarying()
{
  lhpTagHandler *tag = NULL;
  tag = m_TagHandlersFactory->CreateInstance("lhpTagHandler_L0000_resource_data_Type_Timevarying");

  CPPUNIT_ASSERT(tag!=NULL);
  CPPUNIT_ASSERT(tag->IsMAFType(lhpTagHandler_L0000_resource_data_Type_Timevarying));
  CPPUNIT_ASSERT(m_TestVMESurface->GetTagArray()->IsTagPresent(tag->GetTagName()) == false);

  tag->FillVMETag(m_TestVMESurface);
  CPPUNIT_ASSERT(m_TestVMESurface->GetTagArray()->IsTagPresent(tag->GetTagName()) == true);

  mafString value = m_TestVMESurface->GetTagArray()->GetTag(tag->GetTagName())->GetValue();
  CPPUNIT_ASSERT(value == "False");
  tag->Delete();

}

  
