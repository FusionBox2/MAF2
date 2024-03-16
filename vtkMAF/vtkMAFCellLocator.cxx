/*=========================================================================

 Program: MAF2Medical
 Module: vtkMAFCellLocator
 Authors: Josef Kohout (Josef.Kohout *AT* beds.ac.uk)
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*//*========================================================================= 
Program: Multimod Application Framework RELOADED 
Module: $RCSfile: vtkMAFCellLocator.cxx,v $ 
Language: C++ 
Date: $Date: 2011-02-16 21:15:02 $ 
Version: $Revision: 1.1.2.1 $ 
Authors: Josef Kohout (Josef.Kohout *AT* beds.ac.uk)
========================================================================== 
Copyright (c) 2008 University of Bedfordshire (www.beds.ac.uk)
See the COPYINGS file for license details 
=========================================================================
*/

#include "vtkMAFCellLocator.h"

#include "vtkCellArray.h"
#include "vtkGenericCell.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"
#include "vtkPolyData.h"
#include "vtkBox.h"

#include <math.h>

vtkStandardNewMacro(vtkMAFCellLocator);

//----------------------------------------------------------------------------
vtkMAFCellLocator::vtkMAFCellLocator()
{
}
