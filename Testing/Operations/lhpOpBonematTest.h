/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: lhpOpBonematTest.h,v $
Language:  C++
Date:      $Date: 2008-06-30 08:45:02 $
Version:   $Revision: 1.5 $
Authors:   Stefano Perticoni
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __CPP_UNIT_lhpOpBonematTest_H__
#define __CPP_UNIT_lhpOpBonematTest_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

#include "lhpOpBonemat.h"

class lhpOpImporterC3D;

class lhpOpBonematTest : public CPPUNIT_NS::TestFixture
{
  public:
    // CPPUNIT fixture: executed before each test
    void setUp();

    // CPPUNIT fixture: executed after each test
    void tearDown();

    CPPUNIT_TEST_SUITE(lhpOpBonematTest);	
    CPPUNIT_TEST(TestConstructorDestructor);
    CPPUNIT_TEST(TestPrintSelf);
    CPPUNIT_TEST(TestSaveLoadConfigurationFile);
    CPPUNIT_TEST(TestBonematEIntegration3Intervals);
    CPPUNIT_TEST(TestBonematEIntegrationOneInterval);
    CPPUNIT_TEST(TestBonematHUIntegrationOneInterval);
    CPPUNIT_TEST(TestBonematHUIntegration3Intervals);
    CPPUNIT_TEST_SUITE_END();

  protected:
		void TestPrintSelf();
    void TestConstructorDestructor();
    void TestSaveLoadConfigurationFile();
    void TestBonematEIntegration3Intervals();
    void TestBonematFromConfigFileOk();
    void TestBonematEIntegrationOneInterval();
    void TestBonematHUIntegrationOneInterval();
    void TestBonematHUIntegration3Intervals();

    void TestCase(const char *inputConfFile, const char *inputDataDir, 
      const char *inputVTKMesh,
      const char *inputVTKVolume, const char *outputDataCorrectDir,
      const char *outputFrequencyFile
      );
    
    
    
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
  runner.addTest( lhpOpBonematTest::suite());
  runner.run( controller );

  // Print test in a compiler compatible format.
  CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
  outputter.write(); 

  return result.wasSuccessful() ? 0 : 1;
}
#endif
