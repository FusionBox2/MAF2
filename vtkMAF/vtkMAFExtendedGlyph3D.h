/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkMAFExtendedGlyph3D.h,v $
  Language:  C++
  Date:      $Date: 2008-07-03 11:27:45 $
  Version:   $Revision: 1.1 $

  Copyright (c) 1993-2002 Ken Martin, Will Schroeder, Bill Lorensen 
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkMAFExtendedGlyph3D - copy oriented and scaled glyph geometry to every input point
// .SECTION Description
// vtkMAFExtendedGlyph3D is an extended version of vtkGlyph3D supporting visibility by scalar, 
// i.e. the glyph is visualized only if scalar is != 0. Beware, if visibility is ON, scaling and 
// color by scalar are ignored.

// .SECTION See Also
// vtkTensorGlyph vtkGlyph3D

#ifndef __vtkMAFExtendedGlyph3D_h
#define __vtkMAFExtendedGlyph3D_h

#include "vtkGlyph3D.h"
#include "vtkMAFConfigure.h"

class VTK_vtkMAF_EXPORT vtkMAFExtendedGlyph3D : public vtkGlyph3D
{
public:
  vtkTypeMacro(vtkMAFExtendedGlyph3D,vtkGlyph3D);

  /**
  Construct object with scaling on, scaling mode is by scalar value, 
  scale factor = 1.0, the range is (0,1), orient geometry is on, and
  orientation is by vector. Clamping and indexing are turned off. No
  initial sources are defined.*/
  static vtkMAFExtendedGlyph3D *New();

  // Description:
  // This can be overwritten by subclass to return 0 when a point is
  // blanked. Default implementation is to always return 1;
  int IsPointVisible(vtkDataSet*, vtkIdType) override;

  /**
  Turn on/off scaling of source geometry.*/
  vtkSetMacro(ScalarVisibility,int);
  vtkBooleanMacro(ScalarVisibility,int);
  vtkGetMacro(ScalarVisibility,int);

protected:
  vtkMAFExtendedGlyph3D();
  ~vtkMAFExtendedGlyph3D() override;

  int ScalarVisibility;

private:
  vtkMAFExtendedGlyph3D(const vtkMAFExtendedGlyph3D&) = delete;
  void operator=(const vtkMAFExtendedGlyph3D&) = delete;
};

#endif
