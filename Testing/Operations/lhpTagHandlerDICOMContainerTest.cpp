/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: lhpTagHandlerDICOMContainerTest.cpp,v $
Language:  C++
Date:      $Date: 2008-01-18 15:31:02 $
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
#include "lhpTagHandlerDICOMContainerTest.h"
#include "lhpTagHandlerDICOMContainer.h"

#include "lhpFactoryTagHandler.h"
#include "mafVMESurface.h"
#include "mafTagArray.h"

#include "vtkSmartPointer.h"
#include "vtkSphereSource.h"

//----------------------------------------------------
void lhpTagHandlerDICOMContainerTest::setUp()
//----------------------------------------------------
{
  m_ParametersCargo = lhpTagHandlerInputOutputParametersCargo::New();
  m_TagHandlersFactory = lhpFactoryTagHandler::GetInstance();
  CreateTestData();
}
//----------------------------------------------------
void lhpTagHandlerDICOMContainerTest::tearDown()
//----------------------------------------------------
{
  m_ParametersCargo->Delete();
	m_TestVMESurface->Delete();
}
//----------------------------------------------------
void lhpTagHandlerDICOMContainerTest::CreateTestData()
//----------------------------------------------------
{
  m_TestVMESurface = NULL;  
  m_TestVMESurface = mafVMESurface::New();
  vtkNew<vtkSphereSource> sphereSource;

  m_TestVMESurface->SetData(sphereSource->GetOutput(),-1);
  //dicom
  m_TestVMESurface->GetTagArray()->SetTag("StudyDate", "2008-01-01");

  
  // since this is not enough ie test on data is failing..
  // m_SphereSurfaceVME->Update();
  // I have have to use the MAF Magic update!

  m_TestVMESurface->Modified();
  m_TestVMESurface->Update();
  
  m_ParametersCargo->SetInputVme(m_TestVMESurface);

  
}

//----------------------------------------------------------------------------------------
void lhpTagHandlerDICOMContainerTest::TestL0000_resource_data_Attributes_SourceAttributes_SourceType_SourceDir_Type_StudyDate()
//----------------------------------------------------------------------------------------
{
	lhpTagHandler *tag = NULL;
	tag = m_TagHandlersFactory->CreateTagHandlerInstance("lhpTagHandler_L0000_resource_data_Attributes_SourceAttributes_SourceType_SourceDir_Type_StudyDate");

	CPPUNIT_ASSERT(tag!=NULL);
	CPPUNIT_ASSERT(tag->IsMAFType(lhpTagHandler_L0000_resource_data_Attributes_SourceAttributes_SourceType_SourceDir_Type_StudyDate));
	CPPUNIT_ASSERT(m_TestVMESurface->GetTagArray()->IsTagPresent(tag->GetTagName()) == false);

	tag->HandleAutoTag(m_ParametersCargo);

	mafString value = m_ParametersCargo->GetTagHandlerGeneratedString();
  mafString control;
  m_TestVMESurface->GetTagArray()->GetTag("StudyDate")->GetValueAsSingleString(control);
	CPPUNIT_ASSERT(value == control);
	tag->Delete();
}