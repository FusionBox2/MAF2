/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafVMEOutputVolumeTest.h,v $
Language:  C++
Date:      $Date: 2009-04-27 10:40:24 $
Version:   $Revision: 1.1.2.1 $
Authors:   Daniele Giunchi
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __CPP_UNIT_mafVMEOutputVolumeTest_H__
#define __CPP_UNIT_mafVMEOutputVolumeTest_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

/** Test for mafMatrix; Use this suite to trace memory problems */
class mafVMEOutputVolumeTest : public CPPUNIT_NS::TestFixture
{
public: 
  // CPPUNIT fixture: executed before each test
  void setUp();

  // CPPUNIT fixture: executed after each test
  void tearDown();

  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( mafVMEOutputVolumeTest );
  CPPUNIT_TEST( TestFixture ); // just to test that the fixture has no leaks
  CPPUNIT_TEST( TestStaticAllocation );
  CPPUNIT_TEST( TestDynamicAllocation );
  CPPUNIT_TEST( TestGetStructuredData );
  CPPUNIT_TEST( TestGetRectilinearData );
  CPPUNIT_TEST( TestGetUnstructuredData );
  CPPUNIT_TEST( TestSetGetVolumeMaterial );
  CPPUNIT_TEST( TestGetVTKDataTypeAsString_Update );
  CPPUNIT_TEST_SUITE_END();

private:
  void TestFixture();
  void TestStaticAllocation();
  void TestDynamicAllocation();
  void TestGetStructuredData();
  void TestGetRectilinearData();
  void TestGetUnstructuredData();
  void TestSetGetVolumeMaterial();
  void TestGetVTKDataTypeAsString_Update();

	bool m_Result;
};

int
main( int argc, char* argv[] )
{
  // Create the event manager and test controller
  CPPUNIT_NS::TestResult controller;

  // Add a listener that collects test result
  CPPUNIT_NS::TestResultCollector result;
  controller.addListener( &result );        

  // Add a listener that print dots as test run.
  CPPUNIT_NS::BriefTestProgressListener progress;
  controller.addListener( &progress );      

  // Add the top suite to the test runner
  CPPUNIT_NS::TestRunner runner;
  runner.addTest( mafVMEOutputVolumeTest::suite());
  runner.run( controller );

  // Print test in a compiler compatible format.
  CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
  outputter.write(); 

  return result.wasSuccessful() ? 0 : 1;
}
#endif
