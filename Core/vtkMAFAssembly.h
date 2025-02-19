#pragma once

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

  vtkTypeMacro(vtkMAFAssembly,vtkAssembly);

  /**
  Get the VME associated to this Assembly */
  mafNode *  GetVme(){return m_Vme;};

  /**
  Set the VME associated to this Assembly */
  void      SetVme(mafNode *vme){m_Vme=vme;};

protected:
  vtkMAFAssembly();
  ~vtkMAFAssembly() override;

  // pointer to the VME that is represented by this Assembly
  mafNode      *m_Vme;

private:
  vtkMAFAssembly(const vtkMAFAssembly&);  // Not implemented.
  void operator=(const vtkMAFAssembly&);  // Not implemented.
};
