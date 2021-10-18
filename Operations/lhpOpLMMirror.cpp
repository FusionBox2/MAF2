/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpOpLMMirror.cpp,v $
  Language:  C++
  Date:      $Date: 2008-07-25 11:14:48 $
  Version:   $Revision: 1.4 $
  Authors:   Paolo Quadrani - porting  Daniele Giunchi
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "lhpOpLMMirror.h"
#include <wx/busyinfo.h>

#include "mafDecl.h"
#include "mafEvent.h"
#include "mafGUI.h"


#include "mafVME.h"
#include "mafVMESurface.h"
#include "mafVMELandmarkCloud.h"

#include "vtkPolyData.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(lhpOpLMMirror);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
lhpOpLMMirror::lhpOpLMMirror(const mafString& label) : Superclass(label)
//----------------------------------------------------------------------------
{
	m_OpType			 		= OPTYPE_OP;
	m_Canundo			 		= true;
	m_InputPreserving = false; //Natural_preserving
	
  m_MirrorX      = 1;
  m_MirrorY      = 0;
  m_MirrorZ      = 0;
}
//----------------------------------------------------------------------------
lhpOpLMMirror::~lhpOpLMMirror( ) 
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
mafOp* lhpOpLMMirror::Copy()   
//----------------------------------------------------------------------------
{
  lhpOpLMMirror *cp = new lhpOpLMMirror(GetLabel());
  cp->m_Canundo		= m_Canundo;
  cp->m_OpType		= m_OpType;
  cp->SetListener(GetListener());
  cp->m_Next			= NULL;
  return cp;
}
//----------------------------------------------------------------------------
bool lhpOpLMMirror::Accept(mafNode* node)   
//----------------------------------------------------------------------------
{
  return  (node && (node->IsMAFType(mafVMELandmarkCloud)));
}
//----------------------------------------------------------------------------
enum SURFACE_MIRROR_ID
//----------------------------------------------------------------------------
{
	ID_MIRRORX = MINID,
	ID_MIRRORY,
	ID_MIRRORZ,
};
//----------------------------------------------------------------------------
void lhpOpLMMirror::OpRun()   
//----------------------------------------------------------------------------
{  
	if(!m_TestMode)
	{
		// interface:
		m_Gui = new mafGUI(this);
		m_Gui->SetListener(this);
		m_Gui->Label(_R("this doesn't work on animated vme"));
		m_Gui->Label(_R(""));
		
		m_Gui->Bool(ID_MIRRORX,_R("mirror x coords"), &m_MirrorX, 1);
		m_Gui->Bool(ID_MIRRORY,_R("mirror y coords"), &m_MirrorY, 1);
		m_Gui->Bool(ID_MIRRORZ,_R("mirror z coords"), &m_MirrorZ, 1);
		m_Gui->Label(_R(""));
		m_Gui->OkCancel();

		ShowGui();
	}

	mafEventMacro(mafEvent(this, CAMERA_UPDATE));
}
//----------------------------------------------------------------------------
void lhpOpLMMirror::OpDo()
//----------------------------------------------------------------------------
{
  mafVMELandmarkCloud *cloud = mafVMELandmarkCloud::SafeDownCast(m_Input);
  if(cloud != NULL)
  {
	  mafString synthetic_name = _R("Copied ");
	  mafAutoPointer<mafNode> node = m_Input->MakeCopy();
	  synthetic_name.Append(m_Input->GetName());
	  node->SetName(synthetic_name);
	  node->ReparentTo(m_Input->GetParent());

    std::vector<mafTimeStamp> stamps;
    cloud->GetLocalTimeStamps(stamps);
    for(unsigned i = 0; i < stamps.size(); i++)
    {
      for(unsigned j = 0; j < cloud->GetNumberOfLandmarks(); j++)
      {
        double xyz[3];
        cloud->GetLandmark(j, xyz, stamps[i]);
        if(m_MirrorX)
          xyz[0] = -xyz[0];
        if(m_MirrorY)
          xyz[1] = -xyz[1];
        if(m_MirrorZ)
          xyz[2] = -xyz[2];
        cloud->SetLandmark(j, xyz[0], xyz[1], xyz[2], stamps[i]);
      }
    }
  }
	mafEventMacro(mafEvent(this, CAMERA_UPDATE));
}
//----------------------------------------------------------------------------
void lhpOpLMMirror::OpUndo()
//----------------------------------------------------------------------------
{
  mafVMELandmarkCloud *cloud = mafVMELandmarkCloud::SafeDownCast(m_Input);
  if(cloud != NULL)
  {
    std::vector<mafTimeStamp> stamps;
    cloud->GetLocalTimeStamps(stamps);
    for(unsigned i = 0; i < stamps.size(); i++)
    {
      for(unsigned j = 0; j < cloud->GetNumberOfLandmarks(); j++)
      {
        double xyz[3];
        cloud->GetLandmark(j, xyz, stamps[i]);
        if(m_MirrorX)
          xyz[0] = -xyz[0];
        if(m_MirrorY)
          xyz[1] = -xyz[1];
        if(m_MirrorZ)
          xyz[2] = -xyz[2];
        cloud->SetLandmark(j, xyz[0], xyz[1], xyz[2], stamps[i]);
      }
    }
  }
	mafEventMacro(mafEvent(this, CAMERA_UPDATE));
}
//----------------------------------------------------------------------------
void lhpOpLMMirror::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
	if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
	{
	  switch(e->GetId())
	  {	
			case ID_MIRRORX:
			case ID_MIRRORY:
			case ID_MIRRORZ:
			break;
			case wxOK:
				OpStop(OP_RUN_OK);        
			break;
			case wxCANCEL:
				OpStop(OP_RUN_CANCEL);        
			break;
				default:
				mafEventMacro(*e);
			break; 
	  }
	}  
}
//----------------------------------------------------------------------------
void lhpOpLMMirror::OpStop(int result)
//----------------------------------------------------------------------------
{
  if(result == OP_RUN_CANCEL) OpUndo();

	if(!m_TestMode)
	{
	  HideGui();
	  delete m_Gui;
	}
	mafEventMacro(mafEvent(this,result));        
}
