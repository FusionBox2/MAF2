/*=========================================================================

 Program: MAF2
 Module: mafOpManager
 Authors: Silvano Imboden
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafOpManager_H__
#define __mafOpManager_H__

//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "mafEvent.h"
#include "mafEventSender.h"
#include "mafBaseEventHandler.h"
#include "mafOpContextStack.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafNode;
class mafOp;
class mafDeviceButtonsPadMouse;
class mafGUISettings;
class mafGUISettingsDialog;
class mafUser;

#ifdef MAF_EXPORTS
#include "mafDllMacros.h"
EXPORT_STL_VECTOR(MAF_EXPORT,mafOp*);
#endif

//----------------------------------------------------------------------------
// mafOpManager :
//----------------------------------------------------------------------------
/**  */
class MAF_EXPORT mafOpManager: public mafBaseEventHandler, public mafEventSender
{
public:
	         mafOpManager();
	virtual ~mafOpManager(); 
  virtual void SetRemoteListener(mafBaseEventHandler *Listener) {m_RemoteListener = Listener;};
	virtual void OnEvent(mafEventBase *maf_event);

  /** Event IDs used in collaborative modality.*/
  MAF_ID_DEC(OPERATION_INTERFACE_EVENT)
  MAF_ID_DEC(RUN_OPERATION_EVENT)

	/** Add the operation 'op' to the list of available operations. */
	virtual long OpAdd(mafOp *op, wxString menuPath = "", bool can_undo = true, mafGUISettings *setting = NULL);

  /** Fill the setting dialog with the settings associated to the plugged operations.*/
  void FillSettingDialog(mafGUISettingsDialog *settingDialog);

	/** Record the selected vme and enable the menu_entries relative to the compatible operations. */
	virtual void VmeSelected(mafNode* v);
	
	/** Return the selected vme. */
	virtual mafNode* GetSelectedVme();

	bool UndoAvailable(){return !m_Context.Undo_IsEmpty();}
  bool RedoAvailable(){return !m_Context.Redo_IsEmpty();}

	/** Run the operation by id. */
  virtual void OpRun(int op_id, void *op_param = NULL);
	
	/** Call this to exec an operation with user interaction and undo/redo services. */
  virtual void OpRun(mafOp *op, void *op_param = NULL);

  /** Run the operation by searching it from its type name.*/
  virtual void OpRun(mafString &op_type, void *op_param = NULL);

	/** Execute the operation 'op' and warn the user if the operation is undoable. */
  virtual void OpExec(mafOp *op, void *op_param = NULL);

	/** Set the flag for warning the user if the operation is undoable. */
  virtual void WarningIfCantUndo (bool warn) {m_Warn = warn;};

	/** return true if there is a running operation. */
  virtual bool Running()								 {return m_Context.Caller() != NULL;};

	/** Clear the stack of executed operation. */
  virtual void ClearUndoStack(); 

	/** Try to Stop the Current operation with OK condition. */
	virtual bool ForceStopWithOk();

	/** Try to Stop the Current Stop operation with CANCEL condition. */
	virtual bool ForceStopWithCancel();

	/** Stop the current operation in any case - to be used as last resort. */
  virtual bool StopCurrentOperation();

  /** Return the current running operation. Return NULL if no operation is running.*/
  mafOp *GetRunningOperation();

  /** Return an instance of operation from id.*/
  mafOp *GetOperationById(int id);

  /** 
  Initialize the action for the mouse device. */
  void SetMouse(mafDeviceButtonsPadMouse *mouse);


  /** Turn On/Off the collaboration status. */
  void Collaborate(bool status);

  bool m_FromRemote; ///< Flag used to check if a command comes from local or remote application.

	/** Execute the 'UnDo' method of the operation. */
  virtual void OpUndo();

	/** Execute the 'Do' method of the operation. */
	virtual void OpRedo();

  /** Set MafUser */
  void SetMafUser(mafUser *user);

  /** Return the current maf user */
  mafUser* GetMafUser(){return m_User;};

protected:
	/** Execute the current operation. */
  virtual void OpDo(mafOp *op);

	/** This method is called if the operation must be executed. */
  virtual void OpRunOk(mafOp *op);

	/** The operation is not executed and is deleted. */
  virtual void OpRunCancel(mafOp *op);

	/** Warn the user if the operation is undoable. */
  virtual bool WarnUser(mafOp *op);

	/** Sent a string message to the listener. */
  virtual void Notify(int msg, long arg=0L);

  /** Fill the attribute for traceability events*/
  void FillTraceabilityAttribute(mafOp *op, mafNode *in_node, mafNode *out_node);


  mafDeviceButtonsPadMouse          *m_Mouse; ///< Pointer to the mouse devices.
  bool               m_Warn; ///< Flag to warn the user when an operation that can not undo is starting.
	mafOpContextStack  m_Context;
  mafOp             *m_RunningOp; ///< Pointer to the current running operation.
  mafNode						*m_Selected; ///< Pointer to the current selected node.
  mafNode           *m_NaturalNode; ///< Pointer to the NATURAL node on which is running a non-input preserving operation.

  mafUser           *m_User; ///<User credentials

  std::vector<mafOp *> m_OpList; ///< List of pointer of plugged operations.

  void *m_OpParameters; ///< Pointer to the operation's parameter list.


  bool m_CollaborateStatus;  ///< Flag set to know if the application is in collaborative mode or no.

  mafBaseEventHandler       *m_RemoteListener; ///< Listener used to send messages to remote applications

  /** test friend */
  friend class mafOpManagerTest;
};
#endif
