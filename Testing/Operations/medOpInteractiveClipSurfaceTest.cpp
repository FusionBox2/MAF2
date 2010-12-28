/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medOpInteractiveClipSurfaceTest.cpp,v $
Language:  C++
Date:      $Date: 2010-12-28 15:52:25 $
Version:   $Revision: 1.1.2.2 $
Authors:   Simone Brazzale
==========================================================================
Copyright (c) 2007
CINECA - Interuniversity Consortium (www.cineca.it)
SCS s.r.l. - BioComputing Competence Centre (www.scsolutions.it - www.b3c.it)

MafMedical Library use license agreement

The software named MafMedical Library and any accompanying documentation, 
manuals or data (hereafter collectively "SOFTWARE") is property of the SCS s.r.l.
This is an open-source copyright as follows:
Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:
* Redistributions of source code must retain the above copyright notice, 
this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice, 
this list of conditions and the following disclaimer in the documentation and/or 
other materials provided with the distribution.
* Modified source versions must be plainly marked as such, and must not be misrepresented 
as being the original software.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 'AS IS' 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
ARE DISCLAIMED. IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE FOR 
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND 
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

MafMedical is partially based on OpenMAF.
=========================================================================*/

#include "mafDefines.h"
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "medOpInteractiveClipSurfaceTest.h"
#include "medOpInteractiveClipSurface.h"

#include "mafVMESurface.h"
#include "mafAbsMatrixPipe.h"

#include "vtkSphereSource.h"
#include "vtkAppendPolyData.h"
#include "vtkDataSet.h"
#include "vtkPolyData.h"

#include <string>
#include <assert.h>

//-----------------------------------------------------------
void medOpInteractiveClipSurfaceTest::TestDynamicAllocation() 
//-----------------------------------------------------------
{
  medOpInteractiveClipSurface *clip = new medOpInteractiveClipSurface();
  mafDEL(clip);
}
//-----------------------------------------------------------
void medOpInteractiveClipSurfaceTest::TestOpRun() 
//-----------------------------------------------------------
{
  
  vtkSphereSource *sphere = vtkSphereSource::New();
  sphere->Update();
  vtkAppendPolyData *append = vtkAppendPolyData::New();
  append->SetInput(sphere->GetOutput());
  append->Update();

  mafVMESurface *surfaceInput;
  mafNEW(surfaceInput);
  surfaceInput->SetData(append->GetOutput(),0.0);
  surfaceInput->GetOutput()->GetVTKData()->Update();
  surfaceInput->GetOutput()->Update();
  surfaceInput->Update();

  int numberOfPointsBefore=append->GetOutput()->GetNumberOfPoints();
  CPPUNIT_ASSERT(numberOfPointsBefore==50);

  medOpInteractiveClipSurface *clip = new medOpInteractiveClipSurface();
  clip->TestModeOn();
  clip->SurfaceAccept(surfaceInput);
  clip->SetInput(surfaceInput);
  clip->OpRun();
  int result = clip->Clip();
  CPPUNIT_ASSERT(result==MAF_OK);

  surfaceInput->GetOutput()->GetVTKData()->Update();
  surfaceInput->GetOutput()->Update();
  surfaceInput->Update();

  int numberOfPointsAfter=surfaceInput->GetOutput()->GetVTKData()->GetNumberOfPoints();
  CPPUNIT_ASSERT(numberOfPointsAfter==57);

  mafDEL(surfaceInput);

  append->Delete();
  sphere->Delete();

  mafDEL(clip);

  delete wxLog::SetActiveTarget(NULL);
}