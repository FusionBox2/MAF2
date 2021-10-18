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

=========================================================================*/

#include "vtkMAFTextActorMeter.h"

#include "vtkRenderer.h"
#include "vtkCamera.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"
#include "vtkProperty.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"

#include "vtkActor.h"
#include "vtkTextActor.h"
#include "vtkTextProperty.h"



vtkStandardNewMacro(vtkMAFTextActorMeter);
//------------------------------------------------------------------------------
vtkMAFTextActorMeter::vtkMAFTextActorMeter()
//------------------------------------------------------------------------------
{
  SetInput("");
  SetPosition(-30, -30);
}
//------------------------------------------------------------------------------
vtkMAFTextActorMeter::~vtkMAFTextActorMeter()
//------------------------------------------------------------------------------
{
}
//------------------------------------------------------------------------------
int vtkMAFTextActorMeter::RenderOverlay(vtkViewport *viewport)
//------------------------------------------------------------------------------
{
  vtkRenderer *ren = static_cast<vtkRenderer *>(viewport);

  vtkCamera *cam = ren->GetActiveCamera();
  if(!cam->GetParallelProjection()) return 0;

  double dp[3];
  viewport->SetWorldPoint(TextPosition);
  viewport->WorldToDisplay();
  viewport->GetDisplayPoint(dp);
  GetPositionCoordinate()->SetCoordinateSystemToDisplay();
  SetPosition(dp[0], dp[1]);
  return Superclass::RenderOverlay(viewport);
}
//----------------------------------------------------------------------------
double *vtkMAFTextActorMeter::GetTextPosition()
//----------------------------------------------------------------------------
{
	return TextPosition;
}
//----------------------------------------------------------------------------
void vtkMAFTextActorMeter::SetColor(double r,double g,double b)
//----------------------------------------------------------------------------
{
  GetTextProperty()->SetColor(r,g,b);
}
