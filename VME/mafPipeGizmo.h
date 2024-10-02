/*=========================================================================

 Program: MAF2
 Module: mafPipeGizmo
 Authors: Paolo Quadrani
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafPipeGizmo_H__
#define __mafPipeGizmo_H__

#include "mafPipeVTK.h"
#include "mafOBB.h"

//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------
class vtkActor;
class vtkCaptionActor2D;
class vtkPolyDataMapper;
class mafGizmoAutoscaleHelper;
class mafGizmoInterface;

//----------------------------------------------------------------------------
// mafPipeGizmo :
//----------------------------------------------------------------------------
/** Visual pipe used to render a mafVMEGizmo. This show simply the polydata present into the VME.*/
class MAF_EXPORT mafPipeGizmo : public mafPipeVTK
{
public:
  mafTypeMacro(mafPipeGizmo,mafPipeVTK);

               mafPipeGizmo();
  ~mafPipeGizmo () override;

  /** Create the VTK rendering pipeline*/
  void Create(mafNode *node, mafView *view) override;

  /** Manage the actor selection by showing the corner box around the actor when the corresponding VME is selected.*/
  void Select(bool select) override;

  /** process events coming from gui */
  void OnEvent(mafEventBase *maf_event) override;

  /** Get assembly front/back */
  vtkMAFAssembly *GetAssemblyFront() override {return m_AssemblyFront;};
  vtkMAFAssembly *GetAssemblyBack() override {return m_AssemblyBack;};


protected:

  std::shared_ptr<mafGizmoAutoscaleHelper> m_GizmoAutoscaleHelper;

  /** the master gizmo acting as mediator between its parts : see mafGizmoInterface*/
  mafGizmoInterface *m_Mediator;

  mafOBB m_OBB;

  void UpdatePipe();

  mafString m_Caption;

  vtkPolyDataMapper *m_Mapper;
  vtkActor *m_GizmoActor;
  vtkActor *m_OutlineActor;
  vtkCaptionActor2D *m_CaptionActor;
};  
#endif // __mafPipeGizmo_H__
