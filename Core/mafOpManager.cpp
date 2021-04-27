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


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------


#include "mafOpManager.h"
#include <wx/datetime.h>
#include "mmuIdFactory.h"
#include "mafDecl.h"
#include "mafOp.h"
#include "mafUser.h"
#include "mafGUI.h"
#include "mafGUISettings.h"
#include "mafOpStack.h"
#include "mafOpContextStack.h"
#include "mafOpSelect.h"
#include "mafGUISettingsDialog.h"
#include "mafAttributeTraceability.h"

#include "mafStorage.h"
#include "mafEventIO.h"
#include "mafDataVector.h"
#include "mafTagArray.h"
#include "mafTagItem.h"
#include "mafNode.h"
#include "mafVMEGenericAbstract.h"

//------------------------------------------------------------------------------
// Events
//------------------------------------------------------------------------------
MAF_ID_IMP(mafOpManager::OPERATION_INTERFACE_EVENT)
MAF_ID_IMP(mafOpManager::RUN_OPERATION_EVENT)

/**
There are 4 paths in OpProcessor

1) OpRun -> run the op's GUI, ...... in asynchronous way can call OpDo
used into operations op,imp,exp

2) OpExec -> run OpDo in synchronous way
used by nested select,transform,cut,copy,paste, op

3) OpUndo -> do pop e run in synchronous way op->OpUndo
used by undo

4) OpRedo -> do pop e run in synchronous way op->OpRedo
used by redo

During the Run an operation can run nested operations,
the nested operations can be cancelled individually.

the nested operations send OP_RUN_STARTING and OP_RUN_TERMINATED to the caller operation.
instead to Logic - in Logic don't need any more running_op_counter, in theory.
In practice it needs for View-Settings.
No is dangerous - if the operations don't manage OP_RUN_STARTING and OP_RUN_TERMINATED 
these return and reach logic - is better to avoid.

During the Exec, can be run nested operations
but could not be done the undo.
*/

//----------------------------------------------------------------------------
mafOpManager::mafOpManager()
//----------------------------------------------------------------------------
{
  m_RemoteListener = NULL;
  m_RunningOp      = NULL;
	m_Selected	     = NULL;
  m_NaturalNode    = NULL;
  m_User           = NULL;
  m_Warn           = true;
  m_FromRemote     = false;

  m_OpParameters = NULL;
  

  m_CollaborateStatus = false;

  m_OpList.clear();
}
//----------------------------------------------------------------------------
mafOpManager::~mafOpManager()
//----------------------------------------------------------------------------
{
  // clear clipboard to avoid crash if some VME is still into the clipboard.
  m_Context.Clear();

  for(int i = 0; i < m_OpList.size(); i++)
  {
    mafGUISettings *s = m_OpList[i]->GetSetting();
    if (s != NULL)
    {
      delete s;
    }
    delete m_OpList[i];
  }
  m_OpList.clear();

  cppDEL(m_User);
}
//----------------------------------------------------------------------------
void mafOpManager::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  mafOp* o = NULL; 

  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch (e->GetId())
    {
      case OP_EXEC:
        o = e->GetOp();
        if(o) 
          OpExec(o);
      break; 
      case OP_RUN:
        o = e->GetOp();
        if(o) 
          OpRun(o);
      break; 
      case OP_RUN_OK:
        o = (mafOp*) e->GetSender();
        assert(o == m_Context.Caller());
        OpRunOk(o);
      break; 
      case OP_RUN_CANCEL:
        o = (mafOp*) e->GetSender();
        assert(o == m_Context.Caller());
        OpRunCancel(o);
      break;
      case REMOTE_PARAMETER:
        if(m_CollaborateStatus && m_RemoteListener)
        {
          // Send the event to synchronize the remote application in case of collaboration modality
          long w_id = e->GetArg();
          WidgetDataType w_data;
          w_data.dValue  = 0.0;
          w_data.fValue  = 0.0;
          w_data.iValue  = 0;
          w_data.sValue  = "";
          w_data.dType = NULL_DATA;
          m_RunningOp->GetGui()->GetWidgetValue(w_id, w_data);
          mafEvent ev(this,OPERATION_INTERFACE_EVENT,w_data,w_id);
          ev.SetChannel(REMOTE_COMMAND_CHANNEL);
          m_RemoteListener->OnEvent(&ev);
        }
      break;
      default:
        mafEventMacro(*e);
      break;
    }
  }
}
//----------------------------------------------------------------------------
long mafOpManager::OpAdd(mafOp *op, wxString menuPath, bool can_undo, mafGUISettings *setting)
//----------------------------------------------------------------------------
{
  m_OpList.push_back(op);
	op->m_Id = m_OpList.size() - 1;
  op->SetListener(this);
  op->SetCanundo(can_undo);
  if (setting != NULL)
  {
    op->SetSetting(setting);
  }
  return m_OpList.size() - 1;
}
//----------------------------------------------------------------------------
void mafOpManager::FillSettingDialog(mafGUISettingsDialog *settingDialog)
//----------------------------------------------------------------------------
{
  for(int i=0; i<m_OpList.size(); i++)
  {
    mafOp *o = m_OpList[i];
    mafGUISettings *setting = o->GetSetting();
    if (setting != NULL)
    {
      settingDialog->AddPage(setting->GetGui(), setting->GetLabel());
    }
  }
}
//----------------------------------------------------------------------------
void mafOpManager::VmeSelected(mafNode* v)   
//----------------------------------------------------------------------------
{
  m_Selected = v;
}
//----------------------------------------------------------------------------
mafNode* mafOpManager::GetSelectedVme()
//----------------------------------------------------------------------------
{
	return m_Selected;
}
//----------------------------------------------------------------------------
void mafOpManager::ClearUndoStack()
//----------------------------------------------------------------------------
{
   m_Context.Clear();
}
//----------------------------------------------------------------------------
bool mafOpManager::WarnUser(mafOp *op)
//----------------------------------------------------------------------------
{
	bool go = true;
	if(m_Warn)
	{
    bool show_message = (op == NULL) || !op->CanUndo();
    if (show_message)
    {
      wxMessageDialog dialog(
        mafGetFrame(),
        _("This operation can not UnDo. Continue?"),
        _("Warning"),
        wxYES_NO|wxYES_DEFAULT
        );
      if(dialog.ShowModal() == wxID_NO) go = false;
    }
	}
  return go;
}
//----------------------------------------------------------------------------
void mafOpManager::OpRun(mafString &op_type, void *op_param)
//----------------------------------------------------------------------------
{
  int i;
  for (i=0; i< m_OpList.size(); i++)
  {
    if (op_type.Equals(_R(m_OpList[i]->GetTypeName())))
    {
      OpRun(m_OpList[i], op_param);
      break;
    }
  }
  if (i==m_OpList.size())
    mafLogMessage(_M(_R("Error Op:\"") + op_type + _R("\" not found")));
}
//----------------------------------------------------------------------------
void mafOpManager::OpRun(int op_id, void *op_param)
//----------------------------------------------------------------------------
{
  if(op_id >=0 && op_id < m_OpList.size()) 
    OpRun(GetOperationById(op_id), op_param);
}
//----------------------------------------------------------------------------
void mafOpManager::OpRun(mafOp *op, void *op_param)
//----------------------------------------------------------------------------
{
  if(!WarnUser(op))
  {
    return;
  }

	//Code to manage operation's Input Preserving
  mafString tag_nature;
  if(const mafTagItem *ti = m_Selected->GetTagArray()->GetTag(_R("VME_NATURE")))
  {
    tag_nature = ti->GetValue();
  }
  
	if(!tag_nature.IsEmpty() && tag_nature.Equals(_R("NATURAL")) && !op->IsInputPreserving())
	{
		wxString warning_msg = _("The operation do not preserve input VME integrity, a copy is required! \nThis should require a lot of memory and time depending on data dimension. \nDo you want to continue?");
		wxMessageDialog dialog(mafGetFrame(),warning_msg, _("Warning"), wxYES_NO | wxYES_DEFAULT);
		if(dialog.ShowModal() == wxID_YES)
    {
      mafString synthetic_name = _R("Copied ");
      mafAutoPointer<mafNode> synthetic_vme = m_Selected->MakeCopy();
      synthetic_vme->ReparentTo(m_Selected->GetParent());
      synthetic_name.Append(m_Selected->GetName());
      synthetic_vme->SetName(synthetic_name);
      if(mafTagItem *ti = synthetic_vme->GetTagArray()->GetTag(_R("VME_NATURE")))
      	ti->SetValue(_R("SYNTHETIC"));
      else
        synthetic_vme->GetTagArray()->SetTag(mafTagItem(_R("VME_NATURE"), _R("SYNTHETIC")));
      mafEventMacro(mafEvent(this,VME_SHOW,m_Selected,false));
      m_NaturalNode = m_Selected;
      mafEventMacro(mafEvent(this,VME_SELECT,synthetic_vme,true));
      mafEventMacro(mafEvent(this,VME_SHOW,synthetic_vme,true));
    }
    else
    {
      m_RunningOp = NULL;
      return;
    }
	}


	m_RunningOp = op->Copy();
  m_RunningOp->m_Id = op->m_Id;    //Paolo 15/09/2004 The operation ID is not copied from the Copy() method.
	m_RunningOp->SetSetting(op->GetSetting());
  m_RunningOp->SetListener(this);
	m_RunningOp->SetInput(m_Selected);
  m_RunningOp->Collaborate(m_CollaborateStatus);
  if (op_param != NULL)
  {
    m_RunningOp->SetParameters(op_param);
  }
  mafGUISettings *settings = m_RunningOp->GetSetting();
  if (settings != NULL)
  {
    settings->SetListener(m_RunningOp);
  }

  Notify(OP_RUN_STARTING);  //SIL. 17-9-2004: - moved here in order to notify which op is started

  m_Context.Push(m_RunningOp);
  m_RunningOp->OpRun();
}
//----------------------------------------------------------------------------
void mafOpManager::OpRunOk(mafOp *op)
//----------------------------------------------------------------------------
{	
  m_Context.Pop();

  m_RunningOp = NULL;
  m_NaturalNode = NULL;
	OpDo(op);
  //Notify success with "arg" paramerter
  Notify(OP_RUN_TERMINATED,true);
}
//----------------------------------------------------------------------------
void mafOpManager::OpRunCancel(mafOp *op)
//----------------------------------------------------------------------------
{
  m_Context.Pop();

	m_RunningOp = NULL;
	delete op;

  if (m_NaturalNode != NULL)
  {
    m_Selected->ReparentTo(NULL);
    mafEventMacro(mafEvent(this,VME_SELECT,m_NaturalNode));
    mafEventMacro(mafEvent(this,VME_SHOW,m_NaturalNode,true));
    m_NaturalNode = NULL;
  }

  //Notify cancel with "arg" paramerter
  Notify(OP_RUN_TERMINATED,false);
}
//----------------------------------------------------------------------------
void mafOpManager::OpExec(mafOp *op, void *op_param)
/** call this to exec an operation without user interaction but with undo/redo services */
//----------------------------------------------------------------------------
{
	assert(op);
	if(op->Accept(m_Selected))
  {
		mafOp *o = op->Copy();
		o->SetListener(this);
		o->SetInput(m_Selected);
		OpDo(o);
  }
}
//----------------------------------------------------------------------------
void mafOpManager::OpDo(mafOp *op)
//----------------------------------------------------------------------------
{
  m_Context.Redo_Clear();
  op->OpDo();
  mafNode *in_node = op->GetInput();
  mafNode *out_node = op->GetOutput();

  if (in_node != NULL)
  {
    mafLogMessage(_M(_R("executed operation '" )+ op->GetLabel() + _R("' on input data: ") + in_node->GetName()));
  }
  else
  {
    mafLogMessage(_M(_R("executed operation '") + op->GetLabel() + _R("'")));
  }
  if (out_node != NULL)
  {
    mafLogMessage(_M(_R("operation '") + op->GetLabel() + _R("' generate ")+ out_node->GetName() + _R(" as output")));
  }

  if (op->GetType() != OPTYPE_EDIT)
      FillTraceabilityAttribute(op, in_node, out_node);

  if(op->CanUndo()) 
  {
	  m_Context.Undo_Push(op);
  }
  else
  {
	  m_Context.Undo_Clear();
    delete op;  
  }
}

//----------------------------------------------------------------------------
void mafOpManager::SetMafUser(mafUser *user)
//----------------------------------------------------------------------------
{
  m_User = user;
}

//----------------------------------------------------------------------------
void mafOpManager::FillTraceabilityAttribute(mafOp *op, mafNode *in_node, mafNode *out_node)
//----------------------------------------------------------------------------
{
  mafString trialEvent = _R("Modify");
  mafString operationName;
  mafString parameters;
  mafString appStamp;
  mafString userID;
  mafString isNatural = _R("false");
  mafString revision;
  mafString dateAndTime;

  operationName = _R(op->GetTypeName());
  parameters = op->GetParameters();

  wxDateTime time = wxDateTime::UNow();
  dateAndTime  = mafString::Format(_R("%02d/%02d/%02d %02d:%02d:%02d"),time.GetDay(), time.GetMonth()+1, time.GetYear(), time.GetHour(), time.GetMinute(),time.GetSecond());

  if (m_User != NULL && m_User->IsAuthenticated())
      userID = m_User->GetName();

  if (in_node != NULL)
  {
    mafAttributeTraceability *traceability = (mafAttributeTraceability *)in_node->GetAttribute(_R("TrialAttribute"));
    if (traceability == NULL)
    {
      traceability = mafAttributeTraceability::New();
      traceability->SetName(_R("TrialAttribute"));
      in_node->SetAttribute(_R("TrialAttribute"), traceability);
    }

    if(mafTagItem *ti = in_node->GetRoot()->GetTagArray()->GetTag(_R("APP_STAMP")))
      appStamp = ti->GetValue();


#ifdef _WIN32
    mafString regKeyPath = _R("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\");
    regKeyPath.Append(appStamp);

    wxRegKey RegKey(regKeyPath.toWx());
    if(RegKey.Exists())
    {
        wxString revisionWx;
      RegKey.Create();
      RegKey.QueryValue(wxT("DisplayVersion"), revisionWx);
      revision = mafWxToString(revisionWx);
    }
    appStamp.Append(_R(" "));
    appStamp.Append(revision);
#endif
   
    if(mafTagItem *ti = in_node->GetTagArray()->GetTag(_R("VME_NATURE")))
    {
      isNatural = ti->GetValue();
      if (isNatural.Compare(_R("NATURAL")) == 0)
        isNatural = _R("true");
      else
        isNatural = _R("false");
    }
    traceability->AddTraceabilityEvent(trialEvent, operationName, parameters, dateAndTime, appStamp, userID, isNatural);
  }

  if (out_node != NULL)
  {
    int c = 0; //counter not to write single parameter on first VME which is a group
    wxString singleParameter = parameters.toWx();
    mafNodeIterator *iter = out_node->NewIterator();
    for (mafNode *node = iter->GetFirstNode(); node; node = iter->GetNextNode())
    {
      if (node != NULL)
      {
        c++;
        mafAttributeTraceability *traceability = (mafAttributeTraceability *)node->GetAttribute(_R("TrialAttribute"));
        if (traceability == NULL)
        {
          trialEvent = _R("Create");
          traceability = mafAttributeTraceability::New();
          traceability->SetName(_R("TrialAttribute"));
          node->SetAttribute(_R("TrialAttribute"), traceability);
        }
        else
        {
          mafString trial = traceability->GetLastTrialEvent();
          if (trial.IsEmpty())
            trialEvent = _R("Create");
        }

        if(mafTagItem *ti = in_node->GetRoot()->GetTagArray()->GetTag(_R("APP_STAMP")))
          appStamp = ti->GetValue();

        if(mafTagItem *ti = in_node->GetTagArray()->GetTag(_R("VME_NATURE")))
        {
          isNatural = ti->GetValue();
          if (isNatural.Compare(_R("NATURAL")) == 0 )
            isNatural = _R("true");
          else
            isNatural = _R("false");
        }

        if (out_node->GetNumberOfChildren() == 0 || c == 1)
        {
           traceability->AddTraceabilityEvent(trialEvent, operationName, parameters, dateAndTime, appStamp, userID, isNatural);
        }
        else
        {
          int count = singleParameter.find_first_of('=');
          wxString par = singleParameter.Mid(0, count);
          singleParameter = singleParameter.AfterFirst('=');
          count = singleParameter.Find(par.c_str());
          par.Append("=");
          par.Append(singleParameter.substr(0, count-2));
          singleParameter = singleParameter.Mid(count);
          traceability->AddTraceabilityEvent(trialEvent, operationName, mafWxToString(par), dateAndTime, appStamp, userID, isNatural);
        }
      }
    }
    iter->Delete();
  }
}
//----------------------------------------------------------------------------
void mafOpManager::OpUndo()
//----------------------------------------------------------------------------
{
  if( m_Context.Undo_IsEmpty()) 
  {
    mafLogMessage(_M(mafString(_L("empty undo stack"))));
    return;
  }

	mafOp* op = m_Context.Undo_Pop();
  mafNode *in_node = op->GetInput();
  mafNode *out_node = op->GetOutput();
  if (in_node != NULL)
  {
    mafLogMessage(_M(_R("undo = ") + op->GetLabel() + _R(" on input data: ") + in_node->GetName()));
    mafAttributeTraceability *traceability = (mafAttributeTraceability *)in_node->GetAttribute(_R("TrialAttribute"));
    if (traceability != NULL)
    {
      traceability->RemoveTraceabilityEvent();
      mafString trial = traceability->GetLastTrialEvent();
      if (trial.IsEmpty())
        in_node->RemoveAttribute(_R("TrialAttribute"));
    }
  }
  else
  {
    mafLogMessage(_M(_R("undo = ") + op->GetLabel()));
  }

  if (out_node != NULL)
  {
    mafNodeIterator *iter = out_node->NewIterator();
    for (mafNode *node = iter->GetFirstNode(); node; node = iter->GetNextNode())
    {
      if (node != NULL)
      {
        mafAttributeTraceability *traceability = (mafAttributeTraceability *)node->GetAttribute(_R("TrialAttribute"));
        if (traceability != NULL)
        {
          traceability->RemoveTraceabilityEvent();
        }
      }
    }
    iter->Delete();
  }

	op->OpUndo();
	m_Context.Redo_Push(op);

}
//----------------------------------------------------------------------------
void mafOpManager::OpRedo()   
//----------------------------------------------------------------------------
{
  if( m_Context.Redo_IsEmpty())
  {
    mafLogMessage(_M("empty redo stack"));
    return;
  }

	mafOp* op = m_Context.Redo_Pop();
  mafNode *in_node = op->GetInput();
  mafNode *out_node = op->GetOutput();
  mafString parameters = op->GetParameters();
  if (in_node != NULL)
  {
    mafLogMessage(_M(_R("redo = ") + op->GetLabel() + _R(" on input data: ") + in_node->GetName()));
  }
  else
  {
    mafLogMessage(_M(_R("redo = ") + op->GetLabel()));
  }
	op->OpDo();
  
	m_Context.Undo_Push(op);

  FillTraceabilityAttribute(op, in_node, out_node);

}
//----------------------------------------------------------------------------
void mafOpManager::Notify(int msg, long arg)   
//----------------------------------------------------------------------------
{
	if(m_Context.Caller() == NULL)
	// not a nested operation - notify logic
		mafEventMacro(mafEvent(this,msg,m_RunningOp,arg));   //SIL. 17-9-2004: added the m_RunningOp at the event (may be NULL)
	//else
	// nested operation - notify caller
		    // m_Context.Caller()->OnEvent(mafEvent(this,msg));   
        // NO - it is dangerous - if the caller don't handle the msg,
				// msg will be bounced and then forwarded to Logic
				// better wait until this feature is really needed 
}
//----------------------------------------------------------------------------
bool mafOpManager::StopCurrentOperation()
//----------------------------------------------------------------------------
{
//  assert(false); //SIL. 2-7-2004: -- Seems that no-one is using this -- but tell me if the program stop here -- thanks
  mafOp *prev_running_op = m_RunningOp;
	
	while(m_RunningOp) //loop danger !!!
	{
    if(m_RunningOp->OkEnabled())
		  m_RunningOp->ForceStopWithOk();
    else
		  m_RunningOp->ForceStopWithCancel();

    //if(prev_running_op == m_RunningOp)
		//{
      //previous try failed, be more bad
			//OpRunCancel(m_RunningOp); - wrong: op-gui will be still plugged
		//}

    if(prev_running_op == m_RunningOp)
		{
			break; //second try failed, give it up and break to prevent loops
		}
	}
  return (m_RunningOp == NULL);
}
//----------------------------------------------------------------------------
mafOp *mafOpManager::GetRunningOperation()
//----------------------------------------------------------------------------
{
  return m_RunningOp;
}
//----------------------------------------------------------------------------
bool mafOpManager::ForceStopWithOk()
//----------------------------------------------------------------------------
{
  if(!m_RunningOp) 
    return false;
  mafOp *to_be_killed = m_RunningOp;

  if(m_RunningOp->OkEnabled())
    m_RunningOp->ForceStopWithOk();
  
  return m_RunningOp != to_be_killed;
}
//----------------------------------------------------------------------------
bool mafOpManager::ForceStopWithCancel()
//----------------------------------------------------------------------------
{
  if(!m_RunningOp) 
    return false;
  mafOp *to_be_killed = m_RunningOp;

  m_RunningOp->ForceStopWithCancel();

  return m_RunningOp != to_be_killed;
}
//----------------------------------------------------------------------------
void mafOpManager::Collaborate(bool status)
//----------------------------------------------------------------------------
{
  m_CollaborateStatus = status;

  for(int index = 0; index < m_OpList.size(); index++)
    m_OpList[index]->Collaborate(status);
}
//----------------------------------------------------------------------------
mafOp *mafOpManager::GetOperationById(int id)
//----------------------------------------------------------------------------
{
  std::vector<mafOp *>::iterator it = m_OpList.begin();
  for(int i=0;i<m_OpList.size(); i++)
  {
    if(m_OpList[i]->m_Id == id)
      return m_OpList[i];
  }

  return NULL;
}
