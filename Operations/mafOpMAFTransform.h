/*=========================================================================

 Program: MAF2
 Module: mafOpMAFTransform
 Authors: Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafOpMAFTransform_H__
#define __mafOpMAFTransform_H__

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
// mafOpMAFTransform :
//----------------------------------------------------------------------------
/** 
Transform a vme using constrains 
*/

class MAF_EXPORT mafOpMAFTransform : public mafOpTransformInterface
{
public:
  mafOpMAFTransform(const mafString& label = _R("Transform  \tCtrl+T"));
 ~mafOpMAFTransform() override;
  void OnEvent(mafEventBase *maf_event) override;
  
  mafTypeMacro(mafOpMAFTransform, mafOpTransformInterface);

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
  
  void OnEventThis(mafEventBase *maf_event) override;  
  void OnEventGizmoTranslate(mafEventBase *maf_event) override;
  void OnEventGizmoRotate(mafEventBase *maf_event) override;
  void OnEventGizmoScale(mafEventBase *maf_event);
  void OnEventGuiTransform(mafEventBase *maf_event);
  void OnEventGuiSaveRestorePose(mafEventBase *maf_event);
  void OnEventGuiTransformTextEntries(mafEventBase *maf_event);

  void OpStop(int result) override;

  /** 
  plugged objects */
  mafGizmoTranslate           *m_GizmoTranslate;  
  mafGizmoRotate              *m_GizmoRotate;
  mafGizmoScale               *m_GizmoScale;
  mafGUITransformMouse        *m_GuiTransform;
  mafGUISaveRestorePose       *m_GuiSaveRestorePose;
  mafGUITransformTextEntries  *m_GuiTransformTextEntries;

  double m_RotationStep;
  double m_TranslationStep;
  int    m_EnableStep;

  /** 
  Override superclass*/
  void RefSysVmeChanged() override;

  /** test friend */
  friend class mafOpMAFTransformTest;
};
#endif
  


