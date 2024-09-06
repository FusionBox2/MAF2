/*=========================================================================

 Program: MAF2
 Module: mafPipeScalar
 Authors: Paolo Quadrani
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafPipeScalar_H__
#define __mafPipeScalar_H__

#include "mafPipeVTK.h"
//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------
class vtkCubeAxesActor2D;
class vtkActor;

//----------------------------------------------------------------------------
// mafPipeScalar :
//----------------------------------------------------------------------------
/** Visual pipe that visually represents the VTK representation of mafVMEScalar.*/
class MAF_EXPORT mafPipeScalar : public mafPipeVTK
{
public:
  mafTypeMacro(mafPipeScalar,mafPipeVTK);

               mafPipeScalar();
  ~mafPipeScalar () override;

  /** process events coming from Gui */
  void OnEvent(mafEventBase *maf_event) override;

  /** Create the VTK rendering pipeline*/
  void Create(mafNode *node, mafView *view) override;

  /** Manage the actor selection by showing the corner box around the actor when the corresponding VME is selected.*/
  void Select(bool select) override; 

  /** IDs for the GUI */
  enum PIPE_SCALAR_WIDGET_ID
  {
    ID_RADIUS = Superclass::ID_LAST,
    ID_LAST
  };

protected:
  vtkCubeAxesActor2D *m_CubeAxes;
  vtkActor *m_Actor;

  /** Update visual properties*/
  void UpdateProperty(bool fromTag = false) override;
  mafGUI  *CreateGui() override;
};  
#endif // __mafPipeScalar_H__
