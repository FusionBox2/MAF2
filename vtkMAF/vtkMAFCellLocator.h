/*========================================================================= 
  Program: Multimod Application Framework RELOADED 
  Module: $RCSfile: vtkMAFCellLocator.h,v $ 
  Language: C++ 
  Date: $Date: 2011-05-26 08:33:31 $ 
  Version: $Revision: 1.1.2.2 $ 
  Authors: Simone Brazzale
  ========================================================================== 
  Copyright (c) 2008 CINECA
  See the COPYINGS file for license details 
  =========================================================================

  vtkMAFCellLocator is equivalent to the standard vtkCellLocator
  but it implements a new function imported from VTK 5.2.
*/
#ifndef vtkMAFCellLocator_h__
#define vtkMAFCellLocator_h__

#include "vtkMAFConfigure.h"
#include "vtkCellLocator.h"
#include "vtkMAFConfigure.h"

/**
    class name: vtkMAFCellLocator
    Spatial search object to quickly locate cells in 3D.
*/

class VTK_vtkMAF_EXPORT vtkMAFCellLocator : public vtkCellLocator
{

public:
  /** create object  instance. */
  static vtkMAFCellLocator *New();
  /**  RTTI Macro. */
  vtkTypeMacro(vtkMAFCellLocator, vtkCellLocator);	

protected:
  vtkMAFCellLocator();
  
private:
  vtkMAFCellLocator(const vtkMAFCellLocator&);  // Not implemented.
  void operator=(const vtkMAFCellLocator&);  // Not implemented.
};

#endif // vtkMAFCellLocator_h__