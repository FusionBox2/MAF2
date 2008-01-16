/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: lhpOpImporterC3DTest.h,v $
Language:  C++
Date:      $Date: 2008-01-16 15:41:59 $
Version:   $Revision: 1.1 $
Authors:   Daniele Giunchi
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __CPP_UNIT_lhpOpImporterC3DTEST_H__
#define __CPP_UNIT_lhpOpImporterC3DTEST_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

#include "lhpTagHandler.h"

class lhpOpImporterC3D;

class lhpOpImporterC3DTest : public CPPUNIT_NS::TestFixture
{
  public:
    // CPPUNIT fixture: executed before each test
    void setUp();

    // CPPUNIT fixture: executed after each test
    void tearDown();

    CPPUNIT_TEST_SUITE( lhpOpImporterC3DTest );	
    CPPUNIT_TEST( Test);
    CPPUNIT_TEST_SUITE_END();

  protected:
		void Test();

  private:
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
  runner.addTest( lhpOpImporterC3DTest::suite());
  runner.run( controller );

  // Print test in a compiler compatible format.
  CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
  outputter.write(); 

  return result.wasSuccessful() ? 0 : 1;
}

#endif
