/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: lhpTagHandlerContainerTest.cpp,v $
Language:  C++
Date:      $Date: 2007-12-20 07:25:20 $
Version:   $Revision: 1.5 $
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

#include "lhpFactoryTagHandler.h"
#include "mafVME.h"
#include "mafTagArray.h"
#include "mafTransform.h"
#include "mafVMEOutputNULL.h"
#include "mafVersion.h"
#include <iostream>
#include "mafString.h"
#include "mafVMESurface.h"
#include "mafVMERoot.h"
#include "mafMatrixVector.h"

#include "vtkMAFSmartPointer.h"
#include "vtkSphereSource.h"
#include "vtkDataSet.h"


//----------------------------------------------------
void lhpTagHandlerContainerTest::setUp()
//----------------------------------------------------
{
  m_ParametersCargo = lhpTagHandlerInputOutputParametersCargo::New();
  m_TagHandlersFactory = lhpFactoryTagHandler::GetInstance();
  CreateTestData();
}
//----------------------------------------------------
void lhpTagHandlerContainerTest::tearDown()
//----------------------------------------------------
{
  m_ParametersCargo->Delete();;
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

  m_TestVMESurface->SetAbsPose(0,0,0,0,0,0, 5); //insert temporal instant 5
  m_TestVMESurface->GetMatrixVector()->Modified();
  m_TestVMESurface->Modified();
  m_TestVMESurface->Update();
  
  m_ParametersCargo->SetInputVme(m_TestVMESurface);

  CPPUNIT_ASSERT(m_TestVMESurface->GetOutput()->GetVTKData()->GetNumberOfCells() > 0);
}
//----------------------------------------------------------------------------------------
void lhpTagHandlerContainerTest::TestlhpTagHandler_L0000_resource_data_Type_Timevarying()
//----------------------------------------------------------------------------------------
{
  lhpTagHandler *tag = NULL;
  tag = m_TagHandlersFactory->CreateTagHandlerInstance("lhpTagHandler_L0000_resource_data_Type_Timevarying");

  CPPUNIT_ASSERT(tag!=NULL);
  CPPUNIT_ASSERT(tag->IsMAFType(lhpTagHandler_L0000_resource_data_Type_Timevarying));
  CPPUNIT_ASSERT(m_TestVMESurface->GetTagArray()->IsTagPresent(tag->GetTagName()) == false);

  tag->HandleAutoTag(m_ParametersCargo);

  mafString value = m_ParametersCargo->GetTagHandlerGeneratedString();
  CPPUNIT_ASSERT(value == mafString(m_TestVMESurface->IsAnimated()? "1" : "0"));
  tag->Delete();

}

//----------------------------------------------------------------------------------------
void lhpTagHandlerContainerTest::TestlhpTagHandler_L0000_resource_data_Size_TimeFramesCount()
//----------------------------------------------------------------------------------------
{
  lhpTagHandler *tag = NULL;
  tag = m_TagHandlersFactory->CreateTagHandlerInstance("lhpTagHandler_L0000_resource_data_Size_TimeFramesCount");
  CPPUNIT_ASSERT(tag!=NULL);
  CPPUNIT_ASSERT(tag->IsMAFType(lhpTagHandler_L0000_resource_data_Size_TimeFramesCount));
  CPPUNIT_ASSERT(m_TestVMESurface->GetTagArray()->IsTagPresent(tag->GetTagName()) == false);

  tag->HandleAutoTag(m_ParametersCargo);
  

  mafString value = m_ParametersCargo->GetTagHandlerGeneratedString();
  mafString result;
  result << (long) m_TestVMESurface->GetNumberOfTimeStamps();
  CPPUNIT_ASSERT(value == result);
  tag->Delete();

}

//----------------------------------------------------------------------------------------
void lhpTagHandlerContainerTest::TestlhpTagHandler_L0000_resource_data_Dataset_FileType_Encryption()
//----------------------------------------------------------------------------------------
{
  lhpTagHandler *tag = NULL;
  tag = m_TagHandlersFactory->CreateTagHandlerInstance("lhpTagHandler_L0000_resource_data_Dataset_FileType_Encryption");
  m_TestVMESurface->SetCrypting(true);

  CPPUNIT_ASSERT(tag!=NULL);
  CPPUNIT_ASSERT(tag->IsMAFType(lhpTagHandler_L0000_resource_data_Dataset_FileType_Encryption));
  CPPUNIT_ASSERT(m_TestVMESurface->GetTagArray()->IsTagPresent(tag->GetTagName()) == false);

  tag->HandleAutoTag(m_ParametersCargo);
  

  mafString value = m_ParametersCargo->GetTagHandlerGeneratedString();
  CPPUNIT_ASSERT(value == mafString(m_TestVMESurface->GetCrypting() ? "1" : "0"));
  tag->Delete();

}
//----------------------------------------------------------------------------------------
void lhpTagHandlerContainerTest::TestlhpTagHandler_L0000_resource_data_TimeSpace_TimeStampVector()
//----------------------------------------------------------------------------------------
{
  lhpTagHandler *tag = NULL;
  tag = m_TagHandlersFactory->CreateTagHandlerInstance("lhpTagHandler_L0000_resource_data_TimeSpace_TimeStampVector");
  m_TestVMESurface->SetCrypting(true);

  CPPUNIT_ASSERT(tag!=NULL);
  CPPUNIT_ASSERT(tag->IsMAFType(lhpTagHandler_L0000_resource_data_TimeSpace_TimeStampVector));
  CPPUNIT_ASSERT(m_TestVMESurface->GetTagArray()->IsTagPresent(tag->GetTagName()) == false);

  tag->HandleAutoTag(m_ParametersCargo);
  
  mafString value = m_ParametersCargo->GetTagHandlerGeneratedString();

  mafString result;
  std::vector<mafTimeStamp> timeStamps;
  m_TestVMESurface->GetAbsTimeStamps(timeStamps);
  long timeCount;
  for(timeCount = 0; timeCount < timeStamps.size(); timeCount++)
  {
    result << timeStamps[timeCount];
    if(timeCount < timeStamps.size() - 1 )
    {
      result << " ";
    }
  }

  CPPUNIT_ASSERT(value == result);
  tag->Delete();

}
//----------------------------------------------------------------------------------------
void lhpTagHandlerContainerTest::TestlhpTagHandler_L0000_resource_data_TreeInfo_VmeRootName()
//----------------------------------------------------------------------------------------
{
  mafVMERoot *root;
  mafNEW(root);
  root->SetName("TestName");
  root->AddChild(m_TestVMESurface);
  root->Modified();
  root->Update();

  lhpTagHandler *tag = NULL;
  tag = m_TagHandlersFactory->CreateTagHandlerInstance("lhpTagHandler_L0000_resource_data_TreeInfo_VmeRootName");

  CPPUNIT_ASSERT(tag!=NULL);
  CPPUNIT_ASSERT(tag->IsMAFType(lhpTagHandler_L0000_resource_data_TreeInfo_VmeRootName));
  CPPUNIT_ASSERT(m_TestVMESurface->GetTagArray()->IsTagPresent(tag->GetTagName()) == false);

  tag->HandleAutoTag(m_ParametersCargo);
  
  mafString value = m_ParametersCargo->GetTagHandlerGeneratedString();
  CPPUNIT_ASSERT(value == root->GetName());
  tag->Delete();

  mafDEL(root);

  delete wxLogStderr::SetActiveTarget(NULL);
}