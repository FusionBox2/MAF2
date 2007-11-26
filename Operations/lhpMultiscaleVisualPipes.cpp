/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: lhpMultiscaleVisualPipes.cpp,v $
Language:  C++
Date:      $Date: 2007-11-26 12:39:56 $
Version:   $Revision: 1.1 $
Authors:   Nigel McFarlane
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

#include "mafVME.h"
#include "vtkRenderer.h"
#include "vtkPolyData.h"
#include "vtkActor.h"
#include "vtkPolyDataMapper.h"
#include "vtkSphereSource.h"
#include "vtkProperty.h"
#include "lhpMultiscaleVisualPipes.h"

#include <cstdlib>


//------------------------------------------------------------------------------
// Surface pipeline constructor
lhpMultiscaleSurfacePipeline::lhpMultiscaleSurfacePipeline(mafVME* vme, vtkRenderer *renderer)
//------------------------------------------------------------------------------
{
  // get polydata from vme
  vtkPolyData* polydata = vtkPolyData::SafeDownCast(vme->GetOutput()->GetVTKData());

  m_mapper = vtkPolyDataMapper::New() ;
  m_mapper->SetInput(polydata);
  m_mapper->ScalarVisibilityOn();

  m_actor = vtkActor::New() ;
  m_actor->SetMapper(m_mapper);

  renderer->AddActor(m_actor) ;
}


//------------------------------------------------------------------------------
// Surface pipeline destructor
lhpMultiscaleSurfacePipeline::~lhpMultiscaleSurfacePipeline()
//------------------------------------------------------------------------------
{
  m_actor->Delete() ;
  m_mapper->Delete() ;
}


//------------------------------------------------------------------------------
// Token pipeline constructor
lhpMultiscaleTokenPipeline::lhpMultiscaleTokenPipeline(vtkRenderer *renderer, int colorId) : m_colorId(colorId)
//------------------------------------------------------------------------------
{
  // Set up source for sphere polydata
  m_sphereSource = vtkSphereSource::New() ;
  m_sphereSource->SetRadius(1.0) ;
  m_sphereSource->SetThetaResolution(10) ;
  m_sphereSource->SetPhiResolution(10) ;

  m_mapper	= vtkPolyDataMapper::New();
  m_mapper->SetInput(m_sphereSource->GetOutput());
  m_mapper->ScalarVisibilityOn();

  m_actor = vtkActor::New();
  m_actor->SetMapper(m_mapper);

  // set visibility and color
  m_actor->SetVisibility(0) ;

  double a[3] ;
  CalculateColor(a) ;
  m_actor->GetProperty()->SetColor(a) ;

  renderer->AddActor(m_actor) ;
}


//------------------------------------------------------------------------------
// Token pipeline destructor
lhpMultiscaleTokenPipeline::~lhpMultiscaleTokenPipeline()
//------------------------------------------------------------------------------
{
  m_actor->Delete() ;
  m_mapper->Delete() ;
  m_sphereSource->Delete() ;
}



//---------------------------------------------------------------------
// Calculate color
void lhpMultiscaleTokenPipeline::CalculateColor(double *a)
//------------------------------------------------------------------------------
{
  static double lut[15][3] =
  {
    {1,0,0}, {1,1,0}, {0,1,0}, {0,1,1}, {0,0,1}, {1,0,1}, 
    {1, 0.5, 0}, {0.5, 1, 0}, {0, 1, 0.5}, {0, 0.5, 1}, {0.5, 0, 1}, {1, 0, 0.5},
    {1, 0.5, 0.5}, {0.5, 1, 0.5},  {0.5, 0.5, 1}
  } ;

  if (m_colorId < 15){
    // look up color in lut
    a[0] = lut[m_colorId][0] ;
    a[1] = lut[m_colorId][1] ;
    a[2] = lut[m_colorId][2] ;
  }
  else{
    // ran out of lut values, so set to random colour
    a[0] = 0.5 * (double)std::rand() / (double)(RAND_MAX+1) ;
    a[1] = 0.5 * (double)std::rand() / (double)(RAND_MAX+1) ;
    a[2] = 0.5 * (double)std::rand() / (double)(RAND_MAX+1) ;

    // set max component to 1.0
    if (a[0] >= a[1] && a[0] >= a[2])
      a[0] = 1.0 ;
    else if (a[1] > a[0] && a[1] >= a[2])
      a[1] = 1.0 ;
    else
      a[2] = 1.0 ;
  }
}
