/*=========================================================================

 Program: MAF2
 Module: vtkMAFPolyDataNormals.cxx
 Authors: Josef Kohout (besoft@kiv.zcu.cz)
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABo
 ILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "vtkMAFPolyDataNormals.h"
#include "vtkSetGet.h"
#include "vtkPolydata.h"
#include "vtkPointData.h"
#include "vtkCellData.h"

vtkStandardNewMacro(vtkMAFPolyDataNormals);

vtkMAFPolyDataNormals::vtkMAFPolyDataNormals()
{
}

