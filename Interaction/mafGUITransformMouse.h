/*=========================================================================

 Program: MAF2
 Module: mafGUITransformMouse
 Authors: Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafGUITransformMouse_H__
#define __mafGUITransformMouse_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafBaseEventHandler.h"
#include "mafGUITransformInterface.h"
#include "mafInteractorCompositorMouse.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafGUI;
class mafGUIButton;
class mafInteractorGenericMouse;
class mafInteractor;
class mafVME;

//----------------------------------------------------------------------------
/** transform gui

  @sa
 
  @todo

*/
class MAF_EXPORT mafGUITransformMouse : public mafGUITransformInterface
{
public:
  mafGUITransformMouse(mafVME *input, mafBaseEventHandler *listener = NULL, bool testMode = false);
	~mafGUITransformMouse() override; 

  // constraints enum
  enum TRANSFORM_MOUSE_WIDGET_ID
  {
    X_AXIS = 0,
    Y_AXIS, 
    Z_AXIS,
    VIEW_PLANE, 
    NORMAL_VIEW_PLANE,
    XY_PLANE, 
    XZ_PLANE, 
    YZ_PLANE,
    SURFACE_SNAP,
    NORMAL_SURFACE,
  };

  void OnEvent(mafEventBase *maf_event) override;

  /** Override superclass */
	void EnableWidgets(bool enable) override;

  //----------------------------------------------------------------------------
  //gui constants: 
  //----------------------------------------------------------------------------

  // this constants must be visible from the owner object  
  enum GUI_TRANSFORM_MOUSE_ID
  {
      ID_ROTATION_AXES = MINID,
      ID_TRASLATION_AXES,
  };

  /** Start and stop interaction through this object isa */
  void AttachInteractorToVme();
  void DetachInteractorFromVme();

  void SetRotationConstraintId(int value){m_RotationConstraintId = value;};
  void SetTranslationConstraintId(int value){m_TranslationConstraintId = value;};

  mafInteractorGenericMouse* CreateBehavior(MMI_ACTIVATOR activator);

  mafInteractorGenericMouse* GetIsaRotate();

protected:  
  mafInteractor* m_OldInteractor;
   
  /** Create interactors */
  void CreateISA();

  /** Override superclass */
  void CreateGui() override;

  mafInteractorCompositorMouse *m_IsaCompositor;

  mafInteractorGenericMouse *m_IsaRotate;
  mafInteractorGenericMouse *m_IsaTranslate;
  mafInteractorGenericMouse *m_IsaRoll;

  int m_RotationConstraintId;
	int m_TranslationConstraintId;
  
  /** Override superclass */;
  void RefSysVmeChanged() override;

  friend class mafGUITransformMouseTest;
  
};
#endif
