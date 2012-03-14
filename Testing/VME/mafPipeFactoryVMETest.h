/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafPipeFactoryVMETest.h,v $
Language:  C++
Date:      $Date: 2009-02-16 10:52:05 $
Version:   $Revision: 1.1.2.1 $
Authors:   Daniele Giunchi
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __CPP_UNIT_mafPipeFactoryVMETEST_H__
#define __CPP_UNIT_mafPipeFactoryVMETEST_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>



class mafPipeFactoryVMETest : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( mafPipeFactoryVMETest );
  CPPUNIT_TEST( TestGetInstance_Initialize );
	CPPUNIT_TEST( TestGetMAFSourceVersion );
	CPPUNIT_TEST( TestGetDescription );
  CPPUNIT_TEST_SUITE_END();

  protected:
    void TestGetInstance_Initialize();
		void TestGetMAFSourceVersion();
		void TestGetDescription();
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
  runner.addTest( mafPipeFactoryVMETest::suite());
  runner.run( controller );

  // Print test in a compiler compatible format.
  CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
  outputter.write(); 

  return result.wasSuccessful() ? 0 : 1;
}

#endif
