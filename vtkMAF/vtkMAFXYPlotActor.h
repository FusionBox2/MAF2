/*=========================================================================

 Program: MAF2
 Module: vtkMAFXYPlotActor
 Authors: Gianluigi Crimi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __vtkMAFXYPlotActor_h
#define __vtkMAFXYPlotActor_h

#include "vtkMAFConfigure.h"

#include "vtkXYPlotActor.h"


//----------------------------------------------------------------------------
// vtkMAFXYPlotActor :
//----------------------------------------------------------------------------
/** This class is a modification of vtkXYPlotActor whit some bug correction about plot rendering.
    The original vtk class have some problems when a line intersect the render area, but both
    ending points are outside that area. In this case the original class will render bad lines.
*/
class VTK_vtkMAF_EXPORT vtkMAFXYPlotActor : public vtkXYPlotActor
{
public:
  vtkTypeMacro(vtkMAFXYPlotActor,vtkXYPlotActor);

  // Description:
  // Instantiate object with autorange computation; bold, italic, and shadows
  // on; arial font family; the number of labels set to 5 for the x and y
  // axes; a label format of "%-#6.3g"; and x coordinates computed from point
  // ids.
  static vtkMAFXYPlotActor *New();

protected:
  vtkMAFXYPlotActor();
  
private:
  vtkMAFXYPlotActor(const vtkMAFXYPlotActor&);  // Not implemented.
  void operator=(const vtkMAFXYPlotActor&);  // Not implemented.
};


#endif

