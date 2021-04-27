/*=========================================================================

 Program: MAF2
 Module: mafOpApplyTrajectory
 Authors: Roberto Mucci, Simone Brazzale
 
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

#include "mafOpApplyTrajectory.h"
#include <wx/busyinfo.h>
#include <wx/txtstrm.h>
#include <wx/tokenzr.h>
#include <wx/wfstream.h>

#include "mafDecl.h"

#include "mafGUI.h" 
#include "mafVMEGenericAbstract.h"
#include "mafTransform.h"
#include "mafMatrix.h"
#include "mafMatrixVector.h"
#include "mafSmartPointer.h"

#include <iostream>
#include <fstream>
using namespace std;

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafOpApplyTrajectory);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Constants :
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafOpApplyTrajectory::mafOpApplyTrajectory(const mafString& label) : Superclass(label)
//----------------------------------------------------------------------------
{
	m_OpType	= OPTYPE_OP;
	m_Canundo	= true;
	m_File		= _R("");
	m_FileDir = mafGetApplicationDirectory() + _R("/Data/");
  m_OriginalMatrix = NULL;
  m_VME = NULL;
}
//----------------------------------------------------------------------------
mafOpApplyTrajectory::~mafOpApplyTrajectory()
//----------------------------------------------------------------------------
{
  if (m_OriginalMatrix != NULL)
  {
    mafDEL(m_OriginalMatrix);
  }
}
//----------------------------------------------------------------------------
mafOp* mafOpApplyTrajectory::Copy()   
//----------------------------------------------------------------------------
{
	mafOpApplyTrajectory *cp = new mafOpApplyTrajectory(GetLabel());
	cp->m_Canundo = m_Canundo;
	cp->m_OpType = m_OpType;
  cp->SetListener(GetListener());
	cp->m_Next = NULL;

	cp->m_File = m_File;
	return cp;
}
//----------------------------------------------------------------------------
bool mafOpApplyTrajectory::Accept(mafNode* vme)
//----------------------------------------------------------------------------
{  
  return !((mafVME *)vme)->IsAnimated() && !vme->IsA("mafVMERoot") 
    && !vme->IsA("mafVMEExternalData") && !vme->IsA("mafVMERefSysAbstract");
}
//----------------------------------------------------------------------------
bool mafOpApplyTrajectory::AcceptInputVME(mafNode* node)
//----------------------------------------------------------------------------
{
  mafVME *vme = mafVME::SafeDownCast(node);
  if ( ((mafVME *)vme)->IsAnimated() && !vme->IsA("mafVMERoot") 
    && !vme->IsA("mafVMEExternalData") && !vme->IsA("mafVMERefSys") )
  {
    return true;
  }
  return false;
}
//----------------------------------------------------------------------------
void mafOpApplyTrajectory::OpRun()   
//----------------------------------------------------------------------------
{
  // Create GUI
  if (!m_TestMode)
  {
    CreateGui();
    ShowGui();
  }
}
//----------------------------------------------------------------------------
void mafOpApplyTrajectory::OpStop(int result)
//----------------------------------------------------------------------------
{
  HideGui();
  mafEventMacro(mafEvent(this,result));
}
//----------------------------------------------------------------------------
void mafOpApplyTrajectory::CreateGui()
//----------------------------------------------------------------------------
{
  mafString pgd_wildc	= _R("txt (*.txt)|*.txt");

  m_Gui = new mafGUI(this);
  
  mafEvent buildHelpGui;
  buildHelpGui.SetSender(this);
  buildHelpGui.SetId(GET_BUILD_HELP_GUI);
  mafEventMacro(buildHelpGui);

  if (buildHelpGui.GetArg() == true)
  {
	  m_Gui->Button(ID_HELP, _R("Help"), _R(""));
  }

  m_Gui->Divider(0);
  m_Gui->Label(_R("Load trajectories"));
  m_Gui->FileOpen(ID_OPEN_FILE,_R("from file:"), &m_File, pgd_wildc,_R("Choose text file"));

  m_Gui->Divider(0);
  m_Gui->Divider(0);
  m_Gui->Button(ID_SELECT_VME,_R("Select"),_R("from VMEs:"),_R("Select VME")); 

  m_Gui->Divider(0);
  m_Gui->Divider(0);
  m_Gui->OkCancel();
}
//----------------------------------------------------------------------------
void mafOpApplyTrajectory::OpDo()   
//----------------------------------------------------------------------------
{
  // CASE 1: Read trajectories from a file
  if (m_VME==NULL)
  {
    mafString f = m_File;
    if(!f.IsEmpty() && mafFileExists(f))
	  {
      if (Read() != MAF_OK)
      {
        if(!this->m_TestMode)
          mafErrorMessage(_M(mafString(_L("Unsupported file format."))));
      }
	  }
    else
    {
      if(!this->m_TestMode)
        mafErrorMessage(_M(mafString(_L("File empty or file not found."))));
    }
  }
  // CASE 2: Apply trajectories from a time-varying VME 
  else
  {
    if (ApplyTrajectoriesFromVME() != MAF_OK)
    {
      if(!this->m_TestMode)
        mafErrorMessage(_M(mafString(_L("An error occurred while applying trajectories"))));
    }
  }
}
//----------------------------------------------------------------------------
void mafOpApplyTrajectory::OpUndo()
//----------------------------------------------------------------------------
{ 
  std::vector<mafTimeStamp> timestamps;
  mafVMEGenericAbstract *oldVme = mafVMEGenericAbstract::SafeDownCast(m_Input);

  oldVme->SetTimeStamp(m_OriginalMatrix->GetTimeStamp());

  mafMatrixVector *mv = oldVme->GetMatrixVector();
  if (mv)
  {
    mv->RemoveAllItems();
    mv->SetMatrix(m_OriginalMatrix);
  }

  oldVme->Modified();
  mafEventMacro(mafEvent(this, VME_MODIFIED, m_Input));
  mafEventMacro(mafEvent(this, CAMERA_UPDATE));
}
//----------------------------------------------------------------------------
void mafOpApplyTrajectory::OnEvent(mafEventBase *maf_event) 
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {
	
	case ID_HELP:
		{
			mafEvent helpEvent;
			helpEvent.SetSender(this);
			mafString operationLabel = GetLabel();
			helpEvent.SetString(&operationLabel);
			helpEvent.SetId(OPEN_HELP_PAGE);
			mafEventMacro(helpEvent);
		}
	break;
    
	case wxOK:
      {
        if (!m_VME && m_File.IsEmpty())
        {
          mafErrorMessage(_M(mafString(_L("No input has been selected."))));
          OpStop(OP_RUN_CANCEL);
        }
        else
        {
          OpStop(OP_RUN_OK);
        }
      }
      break;
    case wxCANCEL:
      OpStop(OP_RUN_CANCEL);
      break;
    case ID_OPEN_FILE:
      {
        if (!m_TestMode)
        {
          if (!m_File.IsEmpty())
          {
            m_Gui->Enable(ID_SELECT_VME,false);  
          }
        }
      }
      break;
    case ID_SELECT_VME:
      {
        mafString title = _R("Select a VME:");
        mafEvent e(this,VME_CHOOSE);
        e.SetString(&title);
        e.SetArg((long)(&mafOpApplyTrajectory::AcceptInputVME)) ; // accept only time-varying VME
        mafEventMacro(e);
        if (e.GetVme())
        {
          m_VME = (mafVME*)e.GetVme();
        }

        if (!m_TestMode)
        {
          if (m_VME)
          {
            m_Gui->Enable(ID_OPEN_FILE,false);  
          }
        }
      }
      break;
    default:
      mafEventMacro(*e);
    }
  }
}
//----------------------------------------------------------------------------
int mafOpApplyTrajectory::Read()   
//----------------------------------------------------------------------------
{
  if (!m_TestMode)
  {
    wxBusyInfo wait("Working, please wait ..");
  }

  mafNEW(m_OriginalMatrix);
  m_OriginalMatrix->DeepCopy(((mafVME *)m_Input)->GetOutput()->GetAbsMatrix());

  mafString path, name, ext;
  mafSplitPath(m_File,&path,&name,&ext);

  double time;
  double newPosition[3];
  double newOrientation[3];

  wxString s_file = m_File.toWx();
  wxFileInputStream inputFile( s_file );
  wxTextInputStream text( inputFile );
  
  wxString line;
  line = text.ReadLine(); //Ignore textual information
  do 
  {
    line = text.ReadLine();
    line.Replace(" ","\t");
    wxStringTokenizer tkz(line,wxT("\t"),wxTOKEN_RET_EMPTY_ALL);

    time = atof(tkz.GetNextToken().c_str());
    newPosition[0] = atof(tkz.GetNextToken().c_str());
    newPosition[1] = atof(tkz.GetNextToken().c_str());
    newPosition[2] = atof(tkz.GetNextToken().c_str());
    newOrientation[0] = atof(tkz.GetNextToken().c_str());
    newOrientation[1] = atof(tkz.GetNextToken().c_str());
    newOrientation[2] = atof(tkz.GetNextToken().c_str());
    
    wxString token = tkz.GetNextToken();
    if (!token.IsEmpty())
    {
      if(!this->m_TestMode)
        mafErrorMessage(_M(mafString(_L("Error reading trajectory file. Incorrect number of parameter."))));
      return MAF_ERROR;
    }

    mafSmartPointer<mafTransform> boxPose;
    boxPose->RotateY(newOrientation[1], POST_MULTIPLY);
    boxPose->RotateX(newOrientation[0], POST_MULTIPLY);
    boxPose->RotateZ(newOrientation[2], POST_MULTIPLY);
    boxPose->SetPosition(newPosition);

    ((mafVME *)m_Input)->SetAbsMatrix(boxPose->GetMatrix(), time);

  } while (!inputFile.Eof());

  mafEventMacro(mafEvent(this, VME_MODIFIED, m_Input));
  mafEventMacro(mafEvent(this, CAMERA_UPDATE));

  return MAF_OK;
}
//----------------------------------------------------------------------------
int mafOpApplyTrajectory::ApplyTrajectoriesFromVME()   
//----------------------------------------------------------------------------
{
  assert(m_VME);

  if (!m_TestMode)
  {
    wxBusyInfo wait("Please wait, working...");
  }

  mafNEW(m_OriginalMatrix);
  m_OriginalMatrix->DeepCopy(((mafVME *)m_Input)->GetOutput()->GetAbsMatrix());

  std::vector<mafTimeStamp> time_stamps;
  m_VME->GetTimeStamps(time_stamps);

  double time;

  for (int i=0; i<time_stamps.size(); i++)
  {
    time = time_stamps.at(i);

    mafMatrix boxPose;
    m_VME->GetOutput()->GetAbsMatrix(boxPose,time);
    ((mafVME *)m_Input)->SetAbsMatrix(boxPose, time);
  }

  mafEventMacro(mafEvent(this, VME_MODIFIED, m_Input));
  mafEventMacro(mafEvent(this, CAMERA_UPDATE));
  return MAF_OK;
}