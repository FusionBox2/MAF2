/*=========================================================================

 Program: MAF2
 Module: mafOpAddLandmark
 Authors: Paolo Quadrani
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "mafOpAddLandmark.h"

#include "mafDecl.h"

#include "mafInteractorPicker.h"
#include "mafInteractor.h"
#include "mafGUI.h"

//dictionary
#include "mafGUIHolder.h"
#include "mafGUISplittedPanel.h"
#include "mafGUINamedPanel.h"
#include "mafGUIDictionaryWidget.h"
#include "mafOpExplodeCollapse.h"
#include "mafSmartPointer.h"

#include "mafVME.h"
#include "mafVMELandmarkCloud.h"
#include "mafVMELandmark.h"
#include "mafVMESurface.h"
#include "mafVMEVolumeGray.h"
#include "mafVMESurfaceParametric.h"

#include "vtkDataSet.h"
#include "vtkPoints.h"


//----------------------------------------------------------------------------
mafCxxTypeMacro(mafOpAddLandmark);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafOpAddLandmark::mafOpAddLandmark(const wxString &label) :
mafOp(label)
//----------------------------------------------------------------------------
{
	m_OpType			= OPTYPE_OP;
	m_Canundo			= true;

  m_PickedVme			  = NULL;
  m_Cloud				    = NULL;
  m_LandmarkPicker	= NULL;
  m_OldBehavior     = NULL;
  m_LandmarkAdded.clear();

  m_LandmarkName        = "new_landmark";
	m_CloudCreatedFlag    = false;
	m_PickingActiveFlag   = false;
	m_LandmarkPosition[0] = m_LandmarkPosition[1] = m_LandmarkPosition[2] = 0;
	m_AddToCurrentTime    = 1;
}
//----------------------------------------------------------------------------
mafOpAddLandmark::~mafOpAddLandmark()
//----------------------------------------------------------------------------
{
  for (int i=0;i<m_LandmarkAdded.size();i++)
  {
    mafDEL(m_LandmarkAdded[i]);
  }
	m_LandmarkAdded.clear();
  mafDEL(m_LandmarkPicker);
	if(m_CloudCreatedFlag) 
    mafDEL(m_Cloud);
}
//----------------------------------------------------------------------------
mafOp* mafOpAddLandmark::Copy()   
//----------------------------------------------------------------------------
{
	mafOpAddLandmark *op = new mafOpAddLandmark(m_Label);
  op->m_OpType = m_OpType;
	op->m_Canundo = m_Canundo;
	op->m_PickingActiveFlag = m_PickingActiveFlag;
  return op;
}
//----------------------------------------------------------------------------
bool mafOpAddLandmark::Accept(mafNode *node)
//----------------------------------------------------------------------------
{
	if (!node) return false;
	
  if( node->IsMAFType(mafVMESurface) || node->IsMAFType(mafVMEVolumeGray) || node->IsMAFType(mafVMESurfaceParametric)) 
	{
		m_PickingActiveFlag = true;
    return true;
	}

  if( node->IsMAFType(mafVMELandmarkCloud) && 
		  node->GetParent() && 
		  (node->GetParent()->IsMAFType(mafVMESurface) || node->GetParent()->IsMAFType(mafVMEVolumeGray) || node->GetParent()->IsMAFType(mafVMESurfaceParametric))) 
	{
		  m_PickingActiveFlag = true;
			return true;
	}

  if( node->IsMAFType(mafVMELandmark) && 
		  node->GetParent() && 
			node->GetParent()->IsMAFType(mafVMELandmarkCloud) &&
			node->GetParent()->GetParent() && 
			(node->GetParent()->GetParent()->IsMAFType(mafVMESurface) || node->GetParent()->GetParent()->IsMAFType(mafVMEVolumeGray) || node->GetParent()->GetParent()->IsMAFType(mafVMESurfaceParametric))) 
	{
		m_PickingActiveFlag = true;
		return true;
	}

	m_PickingActiveFlag = false;
  return true; //accept all other vmes
}
//----------------------------------------------------------------------------
// Widgets ID's
//----------------------------------------------------------------------------
enum ADD_LANDMARK_ID
{
  ID_LM_NAME = MINID,
  ID_ADD_TO_CURRENT_TIME,
  ID_LOAD,
	ID_CHANGE_POSITION,
	ID_ADD_LANDMARK,
};
//----------------------------------------------------------------------------
void mafOpAddLandmark::OpRun()
//----------------------------------------------------------------------------
{
	if(m_PickingActiveFlag == true)
	{
		if(m_Input->IsMAFType(mafVMESurface) || m_Input->IsMAFType(mafVMEVolumeGray) || m_Input->IsMAFType(mafVMESurfaceParametric))
		{
			m_PickedVme = mafVME::SafeDownCast(m_Input);
			mafNEW(m_Cloud);

			if (m_TestMode == true)
			{
				m_Cloud->TestModeOn();
			}

			m_Cloud->Open();
			m_Cloud->SetName(_("new landmark cloud"));
			m_Cloud->SetRadius(m_PickedVme->GetOutput()->GetVTKData()->GetLength()/60.0);
			m_Cloud->ReparentTo(m_PickedVme);
			mafEventMacro(mafEvent(this,VME_SHOW,m_Cloud,true));
			m_CloudCreatedFlag = true;
		}
		else if(m_Input->IsMAFType(mafVMELandmark))
		{
			// add a new landmark as brother of this one
			m_Cloud   = (mafVMELandmarkCloud *) m_Input->GetParent();
			m_PickedVme = mafVME::SafeDownCast(m_Input->GetParent()->GetParent());
		}
		else if(m_Input->IsMAFType(mafVMELandmarkCloud))
		{
			m_Cloud   = (mafVMELandmarkCloud*)m_Input;
			m_PickedVme = mafVME::SafeDownCast(m_Input->GetParent());
		}
		else
		{
			assert(false); 
		}
		// customize m_PickedVme behavior
		m_LandmarkPicker = mafInteractorPicker::New();
		m_LandmarkPicker->SetListener(this);

		m_OldBehavior = m_PickedVme->GetBehavior();
		m_PickedVme->SetBehavior(m_LandmarkPicker);

		
	}
	else
	{
		if(m_Input->IsMAFType(mafVMELandmark))
		{
			// add a new landmark as brother of this one
			m_Cloud = (mafVMELandmarkCloud *) m_Input->GetParent();
		}
		else if(m_Input->IsMAFType(mafVMELandmarkCloud))
		{
			m_Cloud = (mafVMELandmarkCloud*) m_Input;
		}
		else
		{
			mafNEW(m_Cloud);
			m_Cloud->Open();
			m_Cloud->SetName(_("new landmark cloud"));
			m_Cloud->ReparentTo(m_Input);
			mafEventMacro(mafEvent(this,VME_SHOW,m_Cloud,true));
			m_CloudCreatedFlag = true;
		}
	}
  
  if (!GetTestMode())
  {
	  mafString tooltip(_("If checked, add the landmark to the current time. \nOtherwise add the landmark at time = 0"));
	
	  // setup gui_panel
	  m_GuiPanel = new mafGUINamedPanel(mafGetFrame(),-1);
	  m_GuiPanel->SetTitle(_("Add Landmark:"));
	
	  // setup splitter
	  mafGUISplittedPanel *sp = new mafGUISplittedPanel(m_GuiPanel,-1);
	  m_GuiPanel->Add(sp,1,wxEXPAND);
	
	  // setup dictionary
	  m_Dict = new mafGUIDictionaryWidget(sp,-1);
	  m_Dict->SetListener(this);
	  m_Dict->SetCloud(m_Cloud);
	  sp->PutOnTop(m_Dict->GetWidget());
	
	  // setup GuiHolder
	  m_Guih = new mafGUIHolder(sp,-1,false,true);
	  
	
	  // setup Gui
	  m_Gui = new mafGUI(this);
	  m_Gui->SetListener(this);
	  m_Gui->Button(ID_LOAD,_("load dictionary"));
	  m_Gui->Divider();
	  m_Gui->Label(_("landmark name"));
	  m_Gui->String(ID_LM_NAME,"",&m_LandmarkName);
	  m_Gui->Divider();
	  m_Gui->Button(ID_ADD_LANDMARK,_("add landmark"));
	  m_Gui->Divider();
	  m_Gui->Bool(ID_ADD_TO_CURRENT_TIME, _("current time"),&m_AddToCurrentTime,1,tooltip);
	  m_Gui->Divider();
	  m_Gui->Label(_("choose a name from the dictionary"));
	  m_Gui->Label(_("and place landmark by"));
	  m_Gui->Label(_("clicking on the parent surface"));
	  m_Gui->Divider();
	  m_Gui->Vector(ID_CHANGE_POSITION, _("Position"), m_LandmarkPosition,MINFLOAT,MAXFLOAT,2,_("landmark position"));
	  m_Gui->OkCancel();
	  m_Gui->Enable(wxOK, false);
	
	  // Show Gui
	  m_Guih->Put(m_Gui);
	
	  sp->PutOnBottom(m_Guih);
	  mafEventMacro(mafEvent(this,OP_SHOW_GUI,(wxWindow *)m_GuiPanel));
  }

}
//----------------------------------------------------------------------------
void mafOpAddLandmark::OpDo()
//----------------------------------------------------------------------------
{
  AddLandmark();
}
//----------------------------------------------------------------------------
void mafOpAddLandmark::OpUndo()
//----------------------------------------------------------------------------
{
  RemoveLandmark();
}
//----------------------------------------------------------------------------
void mafOpAddLandmark::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  vtkPoints *pts = NULL; 
  if(mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {
      case ID_LOAD:
		  m_Dict->LoadDictionary();
		  break;
      
      case ITEM_SELECTED:
		  m_LandmarkName = *(e->GetString());
			if(this->m_Cloud && this->m_Cloud->FindInTreeByName(m_LandmarkName.GetCStr()))
			{
        ExistingLandmarkMessage();
				m_LandmarkName = "";
			}
			m_Gui->Update();
		  break;

      case VME_PICKED:
        if(this->m_Cloud && this->m_Cloud->FindInTreeByName(m_LandmarkName.GetCStr()))
        {
          ExistingLandmarkMessage();
          m_LandmarkName = "";
          m_Gui->Update();
        }
        else
        {
          pts = vtkPoints::SafeDownCast(e->GetVtkObj());
          if (pts)
          {
            pts->GetPoint(0,m_LandmarkPosition);
            AddLandmark(m_LandmarkPosition);
            m_Dict->SetCloud(m_Cloud);
            m_Gui->Update();
          }
        }
      break;
      case ID_CHANGE_POSITION:
      {
        mafVMELandmark *landmark = mafVMELandmark::SafeDownCast(this->m_Cloud->FindInTreeByName(m_LandmarkName.GetCStr()));
		    if(this->m_Cloud && landmark)
		    {
			    landmark->SetAbsPose(m_LandmarkPosition[0],m_LandmarkPosition[1],m_LandmarkPosition[2],0,0,0);
			    m_Gui->Update();
			    mafEventMacro(mafEvent(this,CAMERA_UPDATE));
		    }
      }
      break;
			case ID_ADD_LANDMARK:
		  if(this->m_Cloud && !this->m_Cloud->FindInTreeByName(m_LandmarkName.GetCStr()))
			{
				AddLandmark(m_LandmarkPosition);
				m_Gui->Update();
			}
			else
			{
        ExistingLandmarkMessage();
				m_LandmarkName = "";
				m_Gui->Update();
			}
			break;
      case wxOK:
        OpStop(OP_RUN_OK);
      break;
      case wxCANCEL:
        RemoveLandmark();
        OpStop(OP_RUN_CANCEL);
      break;
      case ID_LM_NAME:
      if(this->m_Cloud && this->m_Cloud->FindInTreeByName(m_LandmarkName.GetCStr()))
      {
        ExistingLandmarkMessage();
        m_LandmarkName = "";
        m_Gui->Update();
      }
      else if(wxString(m_LandmarkName).Find(" ") != -1)
      {
        wxString msg(_("Landmark has character space in his name, substitute those space!"));
        wxMessageBox(msg, _("Warning"), wxOK|wxICON_WARNING , NULL);
        m_LandmarkName = "";
        m_Gui->Update();
      }
      break;
      default:
        mafEventMacro(*e);
      break; 
    }
  }
}
//----------------------------------------------------------------------------
void mafOpAddLandmark::ExistingLandmarkMessage()
//----------------------------------------------------------------------------
{
  wxString existing_lm_msg(_("Landmark with that name already exist, Please change it!"));
  wxMessageBox(existing_lm_msg, _("Warning"), wxOK|wxICON_WARNING , NULL);
}
//----------------------------------------------------------------------------
void mafOpAddLandmark::OpStop(int result)
//----------------------------------------------------------------------------
{
	if(m_PickingActiveFlag == true)
  {
		mafEventMacro(mafEvent(this,OP_HIDE_GUI,(wxWindow *)m_GuiPanel));
		cppDEL(m_Dict);
		cppDEL(m_GuiPanel);
    mafDEL(m_LandmarkPicker);
    m_PickedVme->SetBehavior(m_OldBehavior);
	}
	else
	{
	  HideGui();
	}

	mafEventMacro(mafEvent(this,result));
}
//----------------------------------------------------------------------------
void mafOpAddLandmark::AddLandmark(double pos[3])
//----------------------------------------------------------------------------
{
  m_LandmarkPosition[0] = pos[0];
  m_LandmarkPosition[1] = pos[1];
  m_LandmarkPosition[2] = pos[2];

  bool cloud_was_open = m_Cloud->IsOpen();
  if (!cloud_was_open)
  {
    m_Cloud->Open();
  }
  
  mafSmartPointer<mafVMELandmark> landmark;
  landmark->SetName(m_LandmarkName.GetCStr());
  landmark->ReparentTo(m_Cloud);
  if(NULL != m_PickedVme)
     landmark->SetTimeStamp(m_PickedVme->GetTimeStamp());
  landmark->Update();
  if(m_AddToCurrentTime)
    landmark->SetAbsPose(m_LandmarkPosition[0],m_LandmarkPosition[1],m_LandmarkPosition[2],0,0,0);
  else
    landmark->SetAbsPose(m_LandmarkPosition[0],m_LandmarkPosition[1],m_LandmarkPosition[2],0,0,0,0);

  mafEventMacro(mafEvent(this,VME_SHOW,landmark.GetPointer(),true));
  mafEventMacro(mafEvent(this,CAMERA_UPDATE));

  m_LandmarkAdded.push_back(landmark);
  m_LandmarkAdded[m_LandmarkAdded.size()-1]->Register(NULL);

  if (!cloud_was_open)
  {
    m_Cloud->Close();
  }

  if (m_Gui && !GetTestMode())
  {
  	m_Gui->Enable(wxOK, true);
  }
}
//----------------------------------------------------------------------------
void mafOpAddLandmark::AddLandmark()
//----------------------------------------------------------------------------
{
  int reparent_result = MAF_OK;
  if(m_CloudCreatedFlag)
  {
		if(m_PickingActiveFlag == true)
			reparent_result = m_Cloud->ReparentTo(m_PickedVme);
		else
		  reparent_result = m_Cloud->ReparentTo(m_Input);
    if (reparent_result == MAF_OK)
    {
      mafEventMacro(mafEvent(this,VME_SHOW,m_Cloud,true));
    }
  }
  else
  {
    bool cloud_was_open = m_Cloud->IsOpen();
    if (!cloud_was_open)
    {
      m_Cloud->Open();
    }
    for (int l=0; l < m_LandmarkAdded.size(); l++)
    {
      reparent_result = m_LandmarkAdded[l]->ReparentTo(m_Cloud);
      if (reparent_result == MAF_OK)
      {
        mafEventMacro(mafEvent(this,VME_SHOW,m_LandmarkAdded[l],true));
      }
    }
    if (!cloud_was_open)
    {
      m_Cloud->Close();
    }
  }
  mafEventMacro(mafEvent(this,CAMERA_UPDATE));
}
//----------------------------------------------------------------------------
void mafOpAddLandmark::RemoveLandmark()
//----------------------------------------------------------------------------
{
  if(m_CloudCreatedFlag)
  {
    mafEventMacro(mafEvent(this,VME_REMOVE,m_Cloud));
  }
  else
  {
    for (int l=0; l < m_LandmarkAdded.size(); l++)
    {
      mafEventMacro(mafEvent(this,VME_REMOVE,m_LandmarkAdded[l]));
    }
  }
  mafEventMacro(mafEvent(this,CAMERA_UPDATE));
}
