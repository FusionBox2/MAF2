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
  ~lhpVisualPipeSurfaceScalar () override;

  /** process events coming from Gui */
  void OnEvent(mafEventBase *maf_event) override;

  /** IDs for the GUI */
  enum PIPE_SURFACESCALAR_WIDGET_ID
  {
    ID_LUT = Superclass::ID_LAST,
    ID_LAST
  };

  /** Create the VTK rendering pipeline*/
  void Create(mafNode *node, mafView *view) override;

  /** Manage the actor selection by showing the corner box around the actor when the corresponding VME is selected.*/
  void Select(bool select) override; 

  void UpdateProperty(bool fromTag = false) override;

protected:
  /** Internally used to create a new instance of the GUI.*/
  mafGUI *CreateGui() override;

  vtkPolyDataMapper *m_Mapper;
  vtkActor *m_Actor;
  vtkActor *m_OutlineActor;
  std::shared_ptr<mmaMaterial> m_Material;
};  
#endif // __lhpVisualPipeSurfaceScalar_H__
