/*=========================================================================

 Program: MAF2
 Module: vtkMAFTextActorMeter
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*//*=========================================================================

  Program:   Multimod Fundation Library
  Module:    $RCSfile: vtkMAFTextActorMeter.h,v $
  Language:  C++
  Date:      $Date: 2009-06-08 14:54:55 $
  Version:   $Revision: 1.1.2.2 $
  Authors:   Daniele Giunchi
  Project:   MultiMod Project (www.ior.it/multimod)

==========================================================================
  Copyright (c) 2002/2003 
  CINECA - Interuniversity Consortium (www.cineca.it)
  v. Magnanelli 6/3
  40033 Casalecchio di Reno (BO)
  Italy
  ph. +39-051-6171411 (90 lines) - Fax +39-051-6132198
========================================================================= */

#ifndef __vtkMAFTextActorMeter_h
#define __vtkMAFTextActorMeter_h

//------------------------------------------------------------------------------
// Includes:
//------------------------------------------------------------------------------
#include "vtkMAFConfigure.h" //??
#include "vtkActor.h"
#include "vtkTextActor.h"

//------------------------------------------------------------------------------
// Forward declarations
//------------------------------------------------------------------------------

/**
class vtkMAFTextActorMeter:
  An actor displaying world X and Y axes as an Actor2D.
  Camera must be in ParallelProjection and aligned with world axis.
*/
//-----------------------------------------------------------------------------
class VTK_vtkMAF_EXPORT vtkMAFTextActorMeter : public vtkTextActor
//-----------------------------------------------------------------------------
{
 public:
  /** RTTI macro. */
  vtkTypeMacro(vtkMAFTextActorMeter,vtkTextActor);
  /** Create an instance of the object. */
  static	vtkMAFTextActorMeter *New();
  
  /** Method is intended for rendering. */
  int	 RenderOverlay(vtkViewport *viewport) override;

  /** Retrieve text actor input. */
  char*   GetText(){return GetInput();}
  /** Modify text actor input. */
  void 	  SetText(const char * inputString){SetInput(inputString);}
  /** Modify text actor position. */
  void    SetTextPosition(double pos[3]) { TextPosition[0] = pos[0]; TextPosition[1] = pos[1]; TextPosition[2] = pos[2]; TextPosition[3] = 1; Modified(); }
  /** Retrieve text actor position. */
  double* GetTextPosition();

  /** Modify text actor color. */
  void SetColor(double r,double g,double b);
	
protected:
  /** constructor. */
	vtkMAFTextActorMeter();
  /** destructor. */
	~vtkMAFTextActorMeter() override;
	
  //variables
  double TextPosition[4];
private:
  /** Copy Constructor Not implemented. */
  vtkMAFTextActorMeter(const vtkMAFTextActorMeter&);
  /** Assign Operator Not implemented. */
  void operator=(const vtkMAFTextActorMeter&);
};
#endif
