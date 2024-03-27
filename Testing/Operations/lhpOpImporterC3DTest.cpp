/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: lhpOpImporterC3DTest.cpp,v $
Language:  C++
Date:      $Date: 2008-06-18 11:14:40 $
Version:   $Revision: 1.3 $
Authors:   Daniele Giunchi
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafDefines.h" 
#include "mafDefines.h"
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include <cppunit/config/SourcePrefix.h>
#include "lhpOpImporterC3DTest.h"
#include "lhpOpImporterC3D.h"
#include "mafVMELandmark.h"
#include "mafVMEOutputScalarMatrix.h"

#include "mafVMELandmarkCloud.h"
#include "medVMEAnalog.h"
#include "mafVMESurface.h"
#include "mafVMEVector.h"
#include "vtkDataset.h"

#include "vnl/vnl_matrix.h"
#include "vtkMath.h"

#define EPSILON 0.01
#define DIFF_MINOR_THAN_EPSILON(a,b) fabs(a - b)<=EPSILON?true:false

//----------------------------------------------------
void lhpOpImporterC3DTest::setUp()
//----------------------------------------------------
{
  
}
//----------------------------------------------------
void lhpOpImporterC3DTest::tearDown()
//----------------------------------------------------
{
  
}

//----------------------------------------------------------------------------------------
void lhpOpImporterC3DTest::Test()
//----------------------------------------------------------------------------------------
{
  lhpOpImporterC3D *importer=new lhpOpImporterC3D("importer");
  importer->TestModeOn();
  mafString filename=LHP_DATA_ROOT;
  filename<<"/C3D/C3Dtest.c3d";
  importer->SetC3DFileName(filename.GetCStr());
  importer->Import();
  
  //TRAJECTORIES
  mafVMELandmarkCloud *cloud=(mafVMELandmarkCloud *)importer->GetLandmarkCloudVME();
 
  int num = cloud->GetNumberOfLandmarks();
  CPPUNIT_ASSERT(num == 46); //total landmarks

  
  //sample point ( RGT at time stamp  2000 ms)
  double result[3];
  double pos[3] = {266.377197,	-136.802872,	789.745300};
  double time = 2;

  cloud->Open();
  
  cloud->GetLandmark("RGT")->GetPoint(result, time);
  //mafLogMessage("P:%f, %f ,%f", result[0], result[1], result[2]);
  
  
  CPPUNIT_ASSERT(DIFF_MINOR_THAN_EPSILON(result[0],pos[0]) && 
                 DIFF_MINOR_THAN_EPSILON(result[1],pos[1]) &&
                 DIFF_MINOR_THAN_EPSILON(result[2],pos[2]) );

  //ANALOG
  medVMEAnalog *analog=(medVMEAnalog *)importer->GetAnalogVME();
  CPPUNIT_ASSERT(analog->GetNumberOfTimeStamps() == 7420);
  vnl_matrix<double> mat = analog->GetScalarOutput()->GetScalarData();
  double timestamp = mat.get(0,5001); //it means 5002 because analog signals start from 1 and not from 0
  double value = mat.get(1,5001);
  //mafLogMessage("Analog %f %f", timestamp , value);
  CPPUNIT_ASSERT(DIFF_MINOR_THAN_EPSILON(value,0.798992));

  //FORCE PLATE
  mafVMESurface *platform0=(mafVMESurface *)importer->GetPlatformVME();
  platform0->GetOutput()->GetVTKData()->Update();
  CPPUNIT_ASSERT(platform0->GetOutput()->GetVTKData()->GetNumberOfPoints() == 24);
  
  double boundsPlat0[6];
  platform0->GetOutput()->GetVTKData()->GetBounds(boundsPlat0);
  //mafLogMessage("b:%f, %f ,%f, %f, %f ,%f", boundsPlat0[0], boundsPlat0[1], boundsPlat0[2],boundsPlat0[3],boundsPlat0[4],boundsPlat0[5]);
  CPPUNIT_ASSERT(boundsPlat0[0] == 0 && boundsPlat0[1] == 400 && 
                 boundsPlat0[2] == 0 && boundsPlat0[3] == 600 && 
                 boundsPlat0[4] == -5 && boundsPlat0[5] == 0);

  mafVMEVector *force0=(mafVMEVector *)importer->GetForceVME();
  force0->GetOutput()->GetVTKData()->Update();
  CPPUNIT_ASSERT(force0->GetOutput()->GetVTKData()->GetNumberOfPoints() == 2);
  double forceV1[3];
  double forceV2[3];
  force0->SetTimeStamp(2.4999); 
  force0->Update();
  force0->GetOutput()->GetVTKData()->GetPoint(0, forceV1);
  force0->GetOutput()->GetVTKData()->GetPoint(1, forceV2);
  //mafLogMessage("%f, %f, % f", forceV1[0], forceV1[1],forceV1[2]);
  //mafLogMessage("%f, %f, % f", forceV2[0], forceV2[1],forceV2[2]);
  //control cop
  CPPUNIT_ASSERT(DIFF_MINOR_THAN_EPSILON(forceV1[0],174.370056) && 
                 DIFF_MINOR_THAN_EPSILON(forceV1[1],430.864624) &&
                 DIFF_MINOR_THAN_EPSILON(forceV1[2],0) );
  
  CPPUNIT_ASSERT(DIFF_MINOR_THAN_EPSILON(forceV2[0]-174.370056,-62.286217) && 
                 DIFF_MINOR_THAN_EPSILON(forceV2[1]-430.864624,63.659505) &&
                 DIFF_MINOR_THAN_EPSILON(forceV2[2],734.070362));



  mafVMEVector *moment0=(mafVMEVector *)importer->GetMomentVME();
  moment0->GetOutput()->GetVTKData()->Update();
  CPPUNIT_ASSERT(moment0->GetOutput()->GetVTKData()->GetNumberOfPoints() == 2);
  double momentV1[3];
  double momentV2[3];
  moment0->SetTimeStamp(2.4999);
  moment0->Update();
  moment0->GetOutput()->GetVTKData()->GetPoint(0, momentV1);
  moment0->GetOutput()->GetVTKData()->GetPoint(1, momentV2);
  //mafLogMessage("%f, %f, % f", momentV1[0], momentV1[1],momentV1[2]);
  //mafLogMessage("%f, %f, % f", momentV2[0], momentV2[1],momentV2[2]);
  CPPUNIT_ASSERT(DIFF_MINOR_THAN_EPSILON(momentV2[0]-174.370056,96063.838879) && 
                 DIFF_MINOR_THAN_EPSILON(momentV2[1]-430.864624,18814.179313) &&
                 DIFF_MINOR_THAN_EPSILON(momentV2[2],10035.374665));

  //second platform
  mafVMESurface *platform1=(mafVMESurface *)importer->GetPlatformVME(1);
  platform1->GetOutput()->GetVTKData()->Update();
  CPPUNIT_ASSERT(platform1->GetOutput()->GetVTKData()->GetNumberOfPoints() == 24);

  double boundsPlat1[6];
  platform1->GetOutput()->GetVTKData()->GetBounds(boundsPlat1);
  //mafLogMessage("%f, %f ,%f, %f, %f ,%f", boundsPlat1[0], boundsPlat1[1], boundsPlat1[2],boundsPlat1[3],boundsPlat1[4],boundsPlat1[5]);
  CPPUNIT_ASSERT(boundsPlat1[0] == -265 && boundsPlat1[1] == 135 && 
    boundsPlat1[2] == 605 && boundsPlat1[3] == 1205 && 
    boundsPlat1[4] == -5 && boundsPlat1[5] == 0);
  
  mafVMEVector *force1=(mafVMEVector *)importer->GetForceVME(1);
  force1->GetOutput()->GetVTKData()->Update();
  CPPUNIT_ASSERT(force1->GetOutput()->GetVTKData()->GetNumberOfPoints() == 2);
  force1->SetTimeStamp(2.4999); //5000 ts in validation document
  force1->Update();
  
  force1->GetOutput()->GetVTKData()->GetPoint(0, forceV1);
  force1->GetOutput()->GetVTKData()->GetPoint(1, forceV2);
  //mafLogMessage("%f, %f, % f", forceV1[0], forceV1[1],forceV1[2]);
  //mafLogMessage("%f, %f, % f", forceV2[0], forceV2[1],forceV2[2]);
  CPPUNIT_ASSERT(DIFF_MINOR_THAN_EPSILON(forceV1[0],-65.000) && 
                 DIFF_MINOR_THAN_EPSILON(forceV1[1],905.000) &&
                 DIFF_MINOR_THAN_EPSILON(forceV1[2],0) );

  CPPUNIT_ASSERT(DIFF_MINOR_THAN_EPSILON(forceV2[0]+65.000,-0.487190) && 
                 DIFF_MINOR_THAN_EPSILON(forceV2[1]-905.000,0.000000) &&
                 DIFF_MINOR_THAN_EPSILON(forceV2[2],3.187216) );
  
  mafVMEVector *moment1=(mafVMEVector *)importer->GetMomentVME(1);
  moment1->GetOutput()->GetVTKData()->Update();
  CPPUNIT_ASSERT(moment1->GetOutput()->GetVTKData()->GetNumberOfPoints() == 2);
  moment1->SetTimeStamp(2.4999);
  moment1->Update();
  moment1->GetOutput()->GetVTKData()->GetPoint(0, momentV1);
  moment1->GetOutput()->GetVTKData()->GetPoint(1, momentV2);
  //mafLogMessage("%f, %f, % f", momentV1[0], momentV1[1],momentV1[2]);
  //mafLogMessage("%f, %f, % f", momentV2[0], momentV2[1],momentV2[2]);
  CPPUNIT_ASSERT(DIFF_MINOR_THAN_EPSILON(momentV2[0]+65.000,318.721592) && 
                 DIFF_MINOR_THAN_EPSILON(momentV2[1]-905.000,336.665700) &&
                 DIFF_MINOR_THAN_EPSILON(momentV2[2],-12.959033));

  cppDEL(importer);
}