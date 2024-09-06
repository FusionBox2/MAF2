/*=========================================================================

 Program: MAF2Medical
 Module: medOpMove
 Authors: Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __medOpMove_H__
#define __medOpMove_H__

//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------

#include "mafOpTransformInterface.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafGizmoTranslate;
class mafGizmoRotate;
class mafGizmoScale;
class mafGUITransformMouse;
class mafGUISaveRestorePose;
class mafGUITransformTextEntries;
class mafNode;

//----------------------------------------------------------------------------
// medOpMove :
//----------------------------------------------------------------------------
/** 
Transform a vme using constrains 
*/

class MED_OPERATION_EXPORT medOpMove : public mafOpTransformInterface
{
public:
  medOpMove(const mafString& label = _R("Move\tCtrl+T"));
 ~medOpMove() override;
  void OnEvent(mafEventBase *maf_event) override;
  
  mafTypeMacro(medOpMove, mafOpTransformInterface);

  mafOp* Copy() override;

  /** Return true for the acceptable vme type. */
  bool Accept(mafNode* vme) override;

  /** Builds operation's interface. */
  void OpRun() override;
  
  /** Execute the operation. */
  void OpDo() override;
  
  /** Makes the undo for the operation. */
  void OpUndo() override;
	
  void Reset();

protected:
  /** Create the gui */
  void CreateGui() override;
  
  void GetDelta(mafMatrix& delta);
  void OnEventThis(mafEventBase *maf_event) override;  
  void OnEventGizmoTranslate(mafEventBase *maf_event) override;
  void OnEventGizmoRotate(mafEventBase *maf_event) override;
  void OnEventGuiTransformMouse(mafEventBase *maf_event);
  void OnEventGuiSaveRestorePose(mafEventBase *maf_event);
  void OnEventGuiTransformTextEntries(mafEventBase *maf_event);

  void OpStop(int result) override;

  /** 
  plugged objects */
  mafGizmoTranslate           *m_GizmoTranslate;  
  mafGizmoRotate              *m_GizmoRotate;
  mafGUITransformMouse        *m_GuiTransformMouse;
  mafGUISaveRestorePose       *m_GuiSaveRestorePose;
  mafGUITransformTextEntries  *m_GuiTransformTextEntries;

  double                      m_TransfTranslation[3];
  double                      m_TransfRotation[3];
  double                      m_TransfScaling[3];

  double m_RotationStep;
  double m_TranslationStep;
  int    m_EnableStep;

  /** 
  Override superclass*/
  void RefSysVmeChanged() override;
};
#endif
  


