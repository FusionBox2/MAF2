/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: lhpTagHandlerDICOMContainerTest.h,v $
Language:  C++
Date:      $Date: 2008-01-18 15:31:02 $
Version:   $Revision: 1.1 $
Authors:   Daniele Giunchi
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __CPP_UNIT_lhpTagHandlerDICOMContainerTest_H__
#define __CPP_UNIT_lhpTagHandlerDICOMContainerTest_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

#include "lhpTagHandler.h"

class lhpFactoryTagHandler;
class mafVMESurface;


class lhpTagHandlerDICOMContainerTest : public CPPUNIT_NS::TestFixture
{
  public:
    // CPPUNIT fixture: executed before each test
    void setUp();

    // CPPUNIT fixture: executed after each test
    void tearDown();

    CPPUNIT_TEST_SUITE( lhpTagHandlerDICOMContainerTest );
    CPPUNIT_TEST( TestL0000_resource_data_Attributes_SourceAttributes_SourceType_SourceDir_Type_StudyDate );
    

    CPPUNIT_TEST_SUITE_END();

  protected:
		void TestL0000_resource_data_Attributes_SourceAttributes_SourceType_SourceDir_Type_StudyDate();
		
    void CreateTestData();

  private:
    lhpFactoryTagHandler *m_TagHandlersFactory;
    lhpTagHandlerInputOutputParametersCargo *m_ParametersCargo;
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
  runner.addTest( lhpTagHandlerDICOMContainerTest::suite());
  runner.run( controller );

  // Print test in a compiler compatible format.
  CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
  outputter.write(); 

  return result.wasSuccessful() ? 0 : 1;
}

#endif
