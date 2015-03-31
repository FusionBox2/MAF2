/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpVisualPipeSurfaceScalar.h,v $
  Language:  C++
  Date:      $Date: 2008-07-25 12:15:29 $
  Version:   $Revision: 1.3 $
  Authors:   Paolo Quadrani
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __lhpVisualPipeSurfaceScalar_H__
#define __lhpVisualPipeSurfaceScalar_H__

#include "mafPipeVTK.h"
//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------
class vtkActor;
class vtkPolyDataMapper;
class mmaMaterial;

//----------------------------------------------------------------------------
// lhpVisualPipeSurfaceScalar :
//----------------------------------------------------------------------------
/** Visual pipe used to render a lhpVMESurfaceScalarVarying. 
This show simply the polydata present into the VME with its associated scalar. The visual pipe offers also the possibility
to change the LUT of the VME through the Gui.
@sa lhpVMESurfaceScalarVarying*/
class lhpVisualPipeSurfaceScalar : public mafPipeVTK
{
public:
  mafTypeMacro(lhpVisualPipeSurfaceScalar,mafPipeVTK);

               lhpVisualPipeSurfaceScalar();
  virtual     ~lhpVisualPipeSurfaceScalar ();

  /** process events coming from Gui */
  virtual void OnEvent(mafEventBase *maf_event);

  /** IDs for the GUI */
  enum PIPE_SURFACESCALAR_WIDGET_ID
  {
    ID_LUT = Superclass::ID_LAST,
    ID_LAST
  };

  /** Create the VTK rendering pipeline*/
  virtual void Create(mafSceneNode *n);

  /** Manage the actor selection by showing the corner box around the actor when the corresponding VME is selected.*/
  virtual void Select(bool select); 

  void UpdateProperty(bool fromTag = false);

protected:
  /** Internally used to create a new instance of the GUI.*/
  virtual mafGUI *CreateGui();

  vtkPolyDataMapper *m_Mapper;
  vtkActor *m_Actor;
  vtkActor *m_OutlineActor;
  mmaMaterial *m_Material;
};  
#endif // __lhpVisualPipeSurfaceScalar_H__
