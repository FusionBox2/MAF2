/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkMAFExtendedGlyph3D.cxx,v $
  Language:  C++
  Date:      $Date: 2010-04-06 15:06:49 $
  Version:   $Revision: 1.1.2.1 $

  Copyright (c) 1993-2002 Ken Martin, Will Schroeder, Bill Lorensen 
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkMAFExtendedGlyph3D.h"

#include "vtkCell.h"
#include "vtkDataSet.h"
#include "vtkFloatArray.h"
#include "vtkIdList.h"
#include "vtkIdTypeArray.h"
#include "vtkMath.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkPolyData.h"
#include "vtkTransform.h"
#include "vtkUnsignedCharArray.h"

vtkStandardNewMacro(vtkMAFExtendedGlyph3D);

// Construct object with scaling on, scaling mode is by scalar value,
// scale factor = 1.0, the range is (0,1), orient geometry is on, and
// orientation is by vector. Clamping and indexing are turned off. No
// initial sources are defined.
vtkMAFExtendedGlyph3D::vtkMAFExtendedGlyph3D()
{
  this->ScalarVisibility = 0;
}

vtkMAFExtendedGlyph3D::~vtkMAFExtendedGlyph3D()
{
}

int vtkMAFExtendedGlyph3D::IsPointVisible(vtkDataSet *input, vtkIdType id)
{
    if(GetScalarVisibility())
    {
        if(vtkDataArray* inScalars = this->GetInputArrayToProcess(0, input))
        {
            return inScalars->GetComponent(id, 0) == 0 ? 0 : 1;
        }
    }
    return Superclass::IsPointVisible(input, id);
}
