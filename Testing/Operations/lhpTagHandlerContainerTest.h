/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: lhpTagHandlerContainerTest.h,v $
Language:  C++
Date:      $Date: 2007-12-17 13:40:19 $
Version:   $Revision: 1.2 $
Authors:   Daniele Giunchi
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __CPP_UNIT_lhpFactoryTagHandlerTEST_H__
#define __CPP_UNIT_lhpFactoryTagHandlerTEST_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

class lhpFactoryTagHandler;
class mafVMESurface;

class lhpTagHandlerContainerTest : public CPPUNIT_NS::TestFixture
{
  public:
    // CPPUNIT fixture: executed before each test
    void setUp();

    // CPPUNIT fixture: executed after each test
    void tearDown();

    CPPUNIT_TEST_SUITE( lhpTagHandlerContainerTest );
    CPPUNIT_TEST( TestlhpTagHandler_L0000_resource_data_Type_Timevarying );
    CPPUNIT_TEST( TestlhpTagHandler_L0000_resource_data_Size_TimeFramesCount );
    CPPUNIT_TEST( TestlhpTagHandler_L0000_resource_data_Dataset_FileType_Encryption );  
    CPPUNIT_TEST( TestlhpTagHandler_L0000_resource_data_TimeSpace_TimeStampVector );
    CPPUNIT_TEST( TestlhpTagHandler_L0000_resource_data_TreeInfo_VmeRootName );
    CPPUNIT_TEST_SUITE_END();

  protected:
    void TestlhpTagHandler_L0000_resource_data_Type_Timevarying();
    void TestlhpTagHandler_L0000_resource_data_Size_TimeFramesCount();
    void TestlhpTagHandler_L0000_resource_data_Dataset_FileType_Encryption();
    void TestlhpTagHandler_L0000_resource_data_TimeSpace_TimeStampVector();
    void TestlhpTagHandler_L0000_resource_data_TreeInfo_VmeRootName();  
    void CreateTestData();

  private:
    lhpFactoryTagHandler *m_TagHandlersFactory;
    mafVMESurface *m_TestVMESurface;

};


int
main( int argc, char* argv[] )
{
  // Create the event manager and test controller
  CPPUNIT_NS::TestResult controller;

  // Add a listener that colllects test result
  CPPUNIT_NS::TestResultCollector result;
  controller.addListener( &result );        

  // Add a listener that print dots as test run.
  CPPUNIT_NS::BriefTestProgressListener progress;
  controller.addListener( &progress );      

  // Add the top suite to the test runner
  CPPUNIT_NS::TestRunner runner;
  runner.addTest( lhpTagHandlerContainerTest::suite());
  runner.run( controller );

  // Print test in a compiler compatible format.
  CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
  outputter.write(); 

  return result.wasSuccessful() ? 0 : 1;
}

#endif
