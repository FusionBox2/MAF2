/*=========================================================================

 Program: MAF2
 Module: mafGUIGizmoInterface
 Authors: Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafGizmoGuiInterface_H__
#define __mafGizmoGuiInterface_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafEvent.h"
#include "mafEventSender.h"
#include "mafBaseEventHandler.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mflMatrix;
class mafGUI;

//---------------------------------------------------------------------------- 
/** abstract base class for gizmo gui

  @sa
 
  @todo
  - add an ivar to register widgets enabling status: should also refactor dependencies to use this flag.
  - provide a barebone implementation for functions doing nothing
  - enforce superclass implementation by using pure virtuals
*/

class MAF_EXPORT mafGUIGizmoInterface : public mafBaseEventHandler, public mafEventSender
{
public:

  virtual ~mafGUIGizmoInterface(); 

  virtual void OnEvent(mafEventBase *maf_event) {};

  /** Return the gui to be plugged*/
  mafGUI *GetGui() {return m_Gui;};

  /** Enable-Disable the GUI's widgets */
  virtual void EnableWidgets(bool enable) {};
  
  /** Turn On m_TestMode flag. 
  The m_TestMode flag is used to exclude the execution of splash screen or wxBusyInfo that conflicts with test machine.*/
  void TestModeOn() {m_TestMode = true;};

  /** Used to turn off m_TestMode flag.*/
  void TestModeOff() {m_TestMode = false;};

  /** Get TestMode*/
  bool GetTestMode(){return m_TestMode;};

protected:
  virtual void CreateGui() {};
  
  mafGUIGizmoInterface(mafBaseEventHandler *listener = NULL);

  mafGUI      *m_Gui;    
  bool m_TestMode;

  /** Test friend */
  friend class mafGUIGizmoInterfaceTest;
};
#endif
