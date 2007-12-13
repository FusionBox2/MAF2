/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: lhpFactoryTagHandlerTest.cpp,v $
Language:  C++
Date:      $Date: 2007-12-13 14:55:29 $
Version:   $Revision: 1.1 $
Authors:   Stefano Perticoni
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
#include "lhpFactoryTagHandlerTest.h"

#include "mafVME.h"
#include "mafTagArray.h"
#include "mafTransform.h"
#include "lhpFactoryTagHandler.h"
#include "mafVMEOutputNULL.h"
#include "mafVersion.h"
#include <iostream>
#include "mafString.h"

#include "vtkMAFSmartPointer.h"
#include "vtkSphereSource.h"
#include "vtkDataSet.h"

// dummy tag concrete handler
class lhpTagHandler_L0000: public lhpTagHandler
{
  public:
    mafTypeMacro(lhpTagHandler_L0000, lhpTagHandler)
    
    void FillVMETag(mafVME *vme) 
    {
      // tag name from type
      mafString tagName = this->GetTypeName();
      int endPos = tagName.FindFirst("_");
      tagName.Erase(0, endPos);

      mafString message = "Hooray!!! I can handle ";
      message.Append(tagName.GetCStr());

      mafLogMessage(message.GetCStr());

      // tag handling code
      vme->GetTagArray()->SetTag(tagName.GetCStr(), "pippo value");
    };
};

mafCxxTypeMacro(lhpTagHandler_L0000);
// end dummy concrete tag handler


void lhpFactoryTagHandlerTest::setUp()
{
  this->CreateTestData();
}

void lhpFactoryTagHandlerTest::tearDown()
{
  m_SphereSurfaceVME->Delete()  ;
}

void lhpFactoryTagHandlerTest::CreateTestData()
{
  m_SphereSurfaceVME = NULL;  
  m_SphereSurfaceVME = mafVMESurface::New();
  vtkMAFSmartPointer<vtkSphereSource> sphereSource;

  m_SphereSurfaceVME->SetData(sphereSource->GetOutput(),-1);
  
  // since this is not enough ie test on data is failing..
  // m_SphereSurfaceVME->Update();
  // I have have to use the MAF Magic update!
  m_SphereSurfaceVME->GetOutput()->GetVTKData()->Update();

  CPPUNIT_ASSERT(m_SphereSurfaceVME->GetOutput()->GetVTKData()->GetNumberOfCells() > 0);
}

void lhpFactoryTagHandlerTest::TestCreateTagHandlerInstance()
{
  lhpFactoryTagHandler *tagHandlersFactory = lhpFactoryTagHandler::GetInstance();
  CPPUNIT_ASSERT(tagHandlersFactory!=NULL);

  std::list<mafObjectFactory *> list=mafObjectFactory::GetRegisteredFactories();
  CPPUNIT_ASSERT(list.size()==1);

  // plug a handler in the main tag handler factory
  lhpPlugTagHandler<lhpTagHandler_L0000>("L0000 tag handler"); 

  lhpTagHandler *tagHandlerNE = NULL;
  tagHandlerNE  = lhpFactoryTagHandler::CreateInstance("lhpTagHandler_Not_Existing");
  
  lhpTagHandler *tagHandlerL0000 = NULL;
  tagHandlerL0000 = lhpFactoryTagHandler::CreateInstance("lhpTagHandler_L0000");
  

  CPPUNIT_ASSERT(tagHandlerNE == NULL);
  CPPUNIT_ASSERT(tagHandlerL0000!=NULL);

  CPPUNIT_ASSERT(tagHandlerL0000->IsMAFType(lhpTagHandler_L0000));
  
  CPPUNIT_ASSERT(m_SphereSurfaceVME->GetTagArray()->IsTagPresent("L0000") == false);
  
  tagHandlerL0000->FillVMETag(m_SphereSurfaceVME);
  
  CPPUNIT_ASSERT(m_SphereSurfaceVME->GetTagArray()->IsTagPresent("L0000") == true);
  
  mafString value = m_SphereSurfaceVME->GetTagArray()->GetTag("L0000")->GetValue();
  CPPUNIT_ASSERT(value == "pippo value");


  // test factory contents
  const std::vector<std::string> &tagNamesList = tagHandlersFactory->GetTagHandlerNames();
  int s = tagNamesList.size();
  CPPUNIT_ASSERT(s==1); 

  bool foundNE=false;
  bool foundL0000=false;
  for (int i=0;i<tagNamesList.size();i++)
  {
    if (tagNamesList[i]=="lhpTagHandler_Not_Existing")
      foundNE=true;
    if (tagNamesList[i]=="lhpTagHandler_L0000")
      foundL0000=true;
  }
  
  CPPUNIT_ASSERT_EQUAL( false, foundNE);
  CPPUNIT_ASSERT_EQUAL( true, foundL0000);
 
  // cleanup factory products
  tagHandlerL0000->Delete();
  
}

  
