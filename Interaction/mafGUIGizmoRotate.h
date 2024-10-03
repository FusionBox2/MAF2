/*=========================================================================

 Program: MAF2
 Module: mafGUIGizmoRotate
 Authors: Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafGizmoGuiRotate_H__
#define __mafGizmoGuiRotate_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafEvent.h"
#include "mafBaseEventHandler.h"
#include "mafGUIGizmoInterface.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafGUIButton;
class mafMatrix;

//----------------------------------------------------------------------------
/** gui class for rotation gizmo

  @sa
 
  @todo
*/
class MAF_EXPORT mafGUIGizmoRotate : public mafGUIGizmoInterface
{
public:

  mafGUIGizmoRotate(mafBaseEventHandler *listener, bool testMode = false);
  ~mafGUIGizmoRotate() override; 

  void OnEvent(mafEventBase *maf_event) override;

  /** Enable-Disable the GUI's widgets */
  void EnableWidgets(bool enable) override;

//----------------------------------------------------------------------------
//gui constants: 
//----------------------------------------------------------------------------

// this constants must be visible from the owner gizmo  
enum GIZMO_ROTATE_ID
{
    ID_ROTATE_X = MINID,
    ID_ROTATE_Y,
    ID_ROTATE_Z,
};

  /**
  Set RotAngles ivar, only rotation part is copied from pose*/
  void SetAbsOrientation(std::shared_ptr<mafMatrix> pose);
  
protected:

  /** Create the GUI*/
  void CreateGui() override;

  /**
  Gui rot angles*/
  double m_Orientation[3];

  /** 
  Send position to listener as vtkMatrix */
  void SendAbsOrientation(mafEventBase *sourceEvent);

  /** friend test */
  friend class mafGUIGizmoRotateTest;

};
#endif
