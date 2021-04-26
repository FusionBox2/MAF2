/*=========================================================================

 Program: MAF2
 Module: mafGUITransformTextEntries
 Authors: Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------


#include "mafGUITransformTextEntries.h"
#include "mafDecl.h"
#include "mafSmartPointer.h"
#include "mafTransformFrame.h"

#include "mafGUI.h"
#include "mafGUIButton.h"

#include "mafInteractorGenericMouse.h"
#include "mafInteractorCompositorMouse.h"

#include "mafMatrix.h"
#include "mafTransform.h"
#include "mafVME.h"
#include "mafVMEOutput.h"

#include "vtkMatrix4x4.h"

//----------------------------------------------------------------------------
mafGUITransformTextEntries::mafGUITransformTextEntries(mafVME *input, mafBaseEventHandler *listener, bool enableScaling, bool testMode)
//----------------------------------------------------------------------------
{
  assert(input);

  m_EnableScaling = enableScaling;
  m_CurrentTime = -1;
  SetListener(listener);
  m_InputVME = input;
  m_Gui = NULL;
  
  m_RefSysVME = m_InputVME;

  m_Position[0] = m_Position[1] = m_Position[2] = 0;
  m_Orientation[0] = m_Orientation[1] = m_Orientation[2] = 0;
  m_Scaling[0] = m_Scaling[1] = m_Scaling[2] = 1; 

  m_CurrentTime = m_InputVME->GetTimeStamp();

  m_TestMode = testMode;

  if (m_TestMode == false)
  {
    CreateGui();
  }

  SetAbsPose(m_InputVME->GetOutput()->GetAbsMatrix());
}
//----------------------------------------------------------------------------
mafGUITransformTextEntries::~mafGUITransformTextEntries() 
//----------------------------------------------------------------------------
{   
  
}

//----------------------------------------------------------------------------
void mafGUITransformTextEntries::CreateGui()
//----------------------------------------------------------------------------
{
  m_Gui = new mafGUI(this);
  m_Gui->SetListener(this);

  m_Gui->Divider(2);
  m_Gui->Label(_R("vme pose"), true);
  m_Gui->Double(ID_TRANSLATE_X, _R("Translate X"), &m_Position[0]);
  m_Gui->Double(ID_TRANSLATE_Y, _R("Translate Y"), &m_Position[1]);
  m_Gui->Double(ID_TRANSLATE_Z, _R("Translate Z"), &m_Position[2]);
  m_Gui->Double(ID_ROTATE_X, _R("Rotate X"), &m_Orientation[0]);
  m_Gui->Double(ID_ROTATE_Y, _R("Rotate Y"), &m_Orientation[1]);
  m_Gui->Double(ID_ROTATE_Z, _R("Rotate Z"), &m_Orientation[2]);

  if (m_EnableScaling == true)
  {
    m_Gui->Double(ID_SCALE_X, _R("Scale X"), &m_Scaling[0], 0);
    m_Gui->Double(ID_SCALE_Y, _R("Scale Y"), &m_Scaling[1], 0);
    m_Gui->Double(ID_SCALE_Z, _R("Scale Z"), &m_Scaling[2], 0);
  }
  else
  {
    mafLogMessage(_M("scaling not enabled, not building scaling gui"));
  }

  m_Gui->Divider();

  m_Gui->Update();
}

//----------------------------------------------------------------------------
void mafGUITransformTextEntries::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {
      case ID_TRANSLATE_X:
      case ID_TRANSLATE_Y:
      case ID_TRANSLATE_Z:
      case ID_ROTATE_X:
      case ID_ROTATE_Y:
      case ID_ROTATE_Z:
      case ID_SCALE_X:
      case ID_SCALE_Y:
      case ID_SCALE_Z:
      {
        TextEntriesChanged();
      }
      break;

      default:
        mafEventMacro(*e);
      break;
    }
  }
}

//----------------------------------------------------------------------------
void mafGUITransformTextEntries::EnableWidgets(bool enable)
//----------------------------------------------------------------------------
{
  m_Gui->Enable(ID_TRANSLATE_X, enable);
  m_Gui->Enable(ID_TRANSLATE_Y, enable);
  m_Gui->Enable(ID_TRANSLATE_Z, enable);
  m_Gui->Enable(ID_ROTATE_X, enable);
  m_Gui->Enable(ID_ROTATE_Y, enable);
  m_Gui->Enable(ID_ROTATE_Z, enable);
  m_Gui->Enable(ID_SCALE_X, enable);
  m_Gui->Enable(ID_SCALE_Y, enable);
  m_Gui->Enable(ID_SCALE_Z, enable);
}

//----------------------------------------------------------------------------
void mafGUITransformTextEntries::Reset()
//----------------------------------------------------------------------------
{
  SetRefSys(m_InputVME);
  SetAbsPose(m_InputVME->GetOutput()->GetAbsMatrix());
  m_Scaling[0] = m_Scaling[1] = m_Scaling[2] = 1;
  m_Gui->Update();
}

//----------------------------------------------------------------------------
void mafGUITransformTextEntries::RefSysVmeChanged()
//----------------------------------------------------------------------------
{
  this->SetAbsPose(m_InputVME->GetOutput()->GetAbsMatrix());
}

//----------------------------------------------------------------------------
void mafGUITransformTextEntries::TextEntriesChanged()
//----------------------------------------------------------------------------
{
  // build the matrix to be applied to vme:

  /*
  - scale
  - rotate around Y, than X, then Z
  - translate 
  */

  mafSmartPointer<mafTransform> tran;
  tran->Scale(m_Scaling[0], m_Scaling[1], m_Scaling[2],POST_MULTIPLY);
  tran->RotateY(m_Orientation[1], POST_MULTIPLY);
  tran->RotateX(m_Orientation[0], POST_MULTIPLY);
  tran->RotateZ(m_Orientation[2], POST_MULTIPLY);
  tran->SetPosition(m_Position);

  // premultiply to ref sys abs matrix
  tran->Concatenate(m_RefSysVME->GetOutput()->GetAbsTransform(), POST_MULTIPLY);
  m_InputVME->SetAbsMatrix(tran->GetMatrix(), m_CurrentTime);
  
  this->SetAbsPose(m_InputVME->GetOutput()->GetAbsMatrix());
  
  // notify the listener about the new abs pose
  mafEvent e2s;
  e2s.SetSender(this);
  e2s.SetMatrix(tran->GetMatrixPointer());
  e2s.SetId(ID_TRANSFORM);
  mafEventMacro(e2s);
}

#define TOLERANCE 1.0e-02
//----------------------------------------------------------------------------
void mafGUITransformTextEntries::SetAbsPose(mafMatrix* absPose, mafTimeStamp timeStamp)
//----------------------------------------------------------------------------
{
  // express absPose in RefSysVME refsys
  mafTransformFrame *mflTr;
  mafNEW(mflTr);
  mflTr->SetInput(absPose);
  mflTr->SetTargetFrame(m_RefSysVME->GetOutput()->GetAbsMatrix());
  mflTr->Update();
  
  // update gui with new pose: Position, Orientation, Scale
  mafTransform::GetPosition(mflTr->GetMatrix(), m_Position);
  mafTransform::GetOrientation(mflTr->GetMatrix(), m_Orientation);
  mafTransform::GetScale(mflTr->GetMatrix(), m_Scaling);

  // bug #2754: Text boxes show a bad approxiamation of the inserted value 
  // if the Position or/and Orientation is near to zero but not zero, 
  // we approximate to zero the new Position or/and Orientation
  for (int i=0; i<3; i++)
  {
    if (m_Position[i]!=0.0 && (m_Position[i] < TOLERANCE && m_Position[i] > - TOLERANCE))
      m_Position[i] = 0.0;
    
    if (m_Orientation[i]!=0.0 && (m_Orientation[i] < TOLERANCE && m_Orientation[i] > - TOLERANCE))
      m_Orientation[i] = 0.0;  
  }

  if (m_TestMode == false)
  {
    assert(m_Gui);
    m_Gui->Update();
  }

  mafDEL(mflTr);
}
