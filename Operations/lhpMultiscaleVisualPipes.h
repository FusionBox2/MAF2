/*========================================================================= 
Program:   Multimod Application Framework
Module:    $RCSfile: lhpMultiscaleVisualPipes.h,v $
Language:  C++
Date:      $Date: 2007-11-29 16:16:07 $
Version:   $Revision: 1.2 $
Authors:   Nigel McFarlane
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __lhpMultiscaleVisualPipes_H__
#define __lhpMultiscaleVisualPipes_H__

#include "mafVME.h"
#include "vtkRenderer.h"
#include "vtkActor.h"
#include "vtkPolyDataMapper.h"
#include "vtkCubeSource.h"



/*******************************************************************************
lhpMultiscaleSurfacePipeline: vtk visual pipeline for surface data
Constructs vtk objects (actor, mapper) for this pipeline and connects vme to renderer.
*******************************************************************************/
class lhpMultiscaleSurfacePipeline
{
public:
  lhpMultiscaleSurfacePipeline(mafVME* vme, vtkRenderer *renderer) ;
  ~lhpMultiscaleSurfacePipeline() ;
  virtual vtkActor* GetActor() {return m_actor ;}
  virtual vtkPolyDataMapper* GetMapper() {return m_mapper ;}
private:
  vtkActor* m_actor ;
  vtkPolyDataMapper* m_mapper ;
};


/*******************************************************************************
lhpMultiscaleTokenPipeline: vtk visual pipeline for multiscale token
Constructs vtk objects (source, actor, mapper etc) and plugs pipe into renderer.
Note that the token pipeline has to be given a color.
*******************************************************************************/
class lhpMultiscaleTokenPipeline
{
public:
  lhpMultiscaleTokenPipeline(vtkRenderer *renderer, int colorId) ;
  ~lhpMultiscaleTokenPipeline() ;
  virtual vtkActor* GetActor() {return m_actor ;}
  virtual vtkPolyDataMapper* GetMapper() {return m_mapper ;}
private:
  virtual void CalculateColor(double *a) ;
  int m_colorId ;
  vtkCubeSource* m_tokenSource ;
  vtkActor* m_actor ;
  vtkPolyDataMapper* m_mapper ;
};

#endif