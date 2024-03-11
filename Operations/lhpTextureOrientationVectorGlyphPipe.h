/*========================================================================= 
Program:   Multimod Application Framework
Module:    $RCSfile: lhpTextureOrientationVectorGlyphPipe.h,v $
Language:  C++
Date:      $Date: 2009-01-29 16:36:58 $
Version:   $Revision: 1.1.2.1 $
Authors:   Nigel McFarlane
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __lhpTextureOrientationVectorGlyphPipe_H__
#define __lhpTextureOrientationVectorGlyphPipe_H__

#include "vtkRenderer.h"
#include "vtkActor.h"
#include "vtkPolyDataMapper.h"
#include "vtkPolyData.h"
#include "vtkSphereSource.h"
#include "vtkTransform.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkGlyph3D.h"

#include "mafVME.h"
#include "lhpTextureOrientationVectorGlyphPipe.h"
#include "lhpHistogramEqualizationFilter.h"
#include "lhpTextureOrientationFilter.h"
#include "lhpTextureOrientationCallbacks.h"

#include <ostream>



/*******************************************************************************
Visual pipeline which finds and displays texture orientation.
Constructs vtk objects (actor, mapper) for this pipeline and connects vme to renderer.
GetPolydata() method returns pointer to polydata which contains the orientation results.
*******************************************************************************/
class lhpTextureOrientationVectorGlyphPipe
{
public:
  lhpTextureOrientationVectorGlyphPipe(mafVME* vme, vtkRenderer *renderer) ;
  ~lhpTextureOrientationVectorGlyphPipe() ;
  vtkActor* GetActor() {return m_glyphActor ;}
  int GetVisibility() {return m_glyphActor->GetVisibility() ;}          ///< get visibility of pipeline  
  void SetVisibility(int visibility) ;                                  ///< set visibility of all actors in pipeline

  void SetTexWinSize(double winSize) {m_texFilter->SetTexWinSize(winSize) ;} ///< set size of texture window
  void SetTexWinStepSize(double winStep) {m_texFilter->SetTexWinStepSize(winStep) ;} ///< set step size of texture window

  ///< set the output format
  void SetOutputFormat(lhpTextureOrientation::OutputFormat outputFormat) 
  {m_texFilter->SetOutputFormat(outputFormat) ;}

  ///< add observer to texture filter to catch and rethrow user progress event
  ///< NB The calling program is still responsible for deleting its reference count to the callback
  void AddProgressObserver(int vtkEventID, lhpTextureOrientationProgressCallback *callback) ;

  vtkPolyData* GetPolydata() ;                                          ///< return polydata containing results
  void PrintSelf(std::ostream& os, vtkIndent indent) ;                  ///< print self
  void PrintResults(std::ostream& os) {m_texFilter->PrintResults(os) ;} ///< print orientation results
private:
  lhpHistogramEqualizationFilter *m_histEq ;
  lhpTextureOrientationFilter *m_texFilter ;
  vtkSphereSource *m_ellipsoid ;
  vtkTransform *m_transform ;
  vtkTransformPolyDataFilter *m_transformPD ;
  vtkGlyph3D *m_glyph3D ;
  vtkPolyDataMapper *m_glyphMapper ;
  vtkActor *m_glyphActor ;
};

#endif