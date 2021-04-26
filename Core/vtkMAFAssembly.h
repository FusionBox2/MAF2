/*=========================================================================

 Program: MAF2
 Module: vtkMAFAssembly
 Authors: Sivano Imboden
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __vtkMAFAssembly_h
#define __vtkMAFAssembly_h

#include "vtkAssembly.h"

//----------------------------------------------------------------------------
// forward declarations
//----------------------------------------------------------------------------
class mafNode;

/** create hierarchies of vtkProp3Ds (transformable props)
  vtkMAFAssembly same as vtkAssembly, but has an extra link 
  for client data.
  @sa vtkActor vtkAssembly vtkTransform vtkMapper vtkPolyDataMapper vtkPropAssembly
  */
class MAF_EXPORT vtkMAFAssembly : public vtkAssembly
{
public:
  static vtkMAFAssembly *New();

  vtkTypeRevisionMacro(vtkMAFAssembly,vtkAssembly);

  /**
  Get the VME associated to this Assembly */
  mafNode *  GetVme(){return m_Vme;};

  /**
  Set the VME associated to this Assembly */
  void      SetVme(mafNode *vme){m_Vme=vme;};

protected:
  vtkMAFAssembly();
  ~vtkMAFAssembly();

  // pointer to the VME that is represented by this Assembly
  mafNode      *m_Vme;

private:
  vtkMAFAssembly(const vtkMAFAssembly&);  // Not implemented.
  void operator=(const vtkMAFAssembly&);  // Not implemented.
};

#endif




