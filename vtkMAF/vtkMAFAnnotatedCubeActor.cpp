/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkMAFAnnotatedCubeActor.cpp,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "vtkMAFAnnotatedCubeActor.h"
#include "vtkObject.h"
#include "vtkObjectFactory.h"

vtkCxxRevisionMacro(vtkMAFAnnotatedCubeActor, "$Revision: 1.1.2.3 $");
vtkStandardNewMacro(vtkMAFAnnotatedCubeActor);

//-------------------------------------------------------------------------
vtkMAFAnnotatedCubeActor::vtkMAFAnnotatedCubeActor()
{
  this->SetXPlusFaceText ( "L" );
  this->SetXMinusFaceText( "R" );
  this->SetYPlusFaceText ( "P" );
  this->SetYMinusFaceText( "A" );
  this->SetZPlusFaceText ( "S" );
  this->SetZMinusFaceText( "I" );
}

//-------------------------------------------------------------------------
vtkMAFAnnotatedCubeActor::~vtkMAFAnnotatedCubeActor()
{
}
