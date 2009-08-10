/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medOpVolumeMeasureTest.cpp,v $
Language:  C++
Date:      $Date: 2009-08-10 14:35:55 $
Version:   $Revision: 1.1.2.1 $
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

#include "medOpVolumeMeasureTest.h"
#include "medOpVolumeMeasure.h"

#include "mafString.h"
#include "mafVMESurface.h"
#include "vtkCubeSource.h"

#include <string>
#include <assert.h>

//-----------------------------------------------------------
void medOpVolumeMeasureTest::Test() 
//-----------------------------------------------------------
{
  vtkCubeSource *cube = vtkCubeSource::New();
  cube->SetXLength(10.0);
  cube->SetYLength(20.0);
  cube->SetZLength(30.0);
  cube->Update();
  	
  mafSmartPointer<mafVMESurface> vmeSurface;
  //mafNEW(vmeSurface);
  vmeSurface->SetData(cube->GetOutput(), 0);
  vmeSurface->Update();
  	
	medOpVolumeMeasure *volumeMeasureOp=new medOpVolumeMeasure("Volume Measure");
	volumeMeasureOp->TestModeOn();
  mafString result;
  volumeMeasureOp->VolumeCompute(vmeSurface);
  result = volumeMeasureOp->GetVolume();
  volumeMeasureOp->OpStop(MAF_OK);
	
  //printf("%s", result);
  CPPUNIT_ASSERT(result == mafString("6000"));

  
  vtkDEL(cube);
  mafDEL(volumeMeasureOp);
  
}
