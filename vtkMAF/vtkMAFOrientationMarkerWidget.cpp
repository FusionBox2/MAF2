/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkMAFOrientationMarkerWidget.cpp,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkMAFOrientationMarkerWidget.h"

#include "vtkObjectFactory.h"
#include "vtkRenderer.h"

vtkCxxRevisionMacro(vtkMAFOrientationMarkerWidget, "$Revision: 1.1.2.3 $");
vtkStandardNewMacro(vtkMAFOrientationMarkerWidget);

//-------------------------------------------------------------------------
vtkMAFOrientationMarkerWidget::vtkMAFOrientationMarkerWidget()
{
  this->Renderer->SetLayer( 0 );
}

//-------------------------------------------------------------------------
vtkMAFOrientationMarkerWidget::~vtkMAFOrientationMarkerWidget()
{
}
