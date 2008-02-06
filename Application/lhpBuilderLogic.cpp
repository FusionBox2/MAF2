/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpBuilderLogic.cpp,v $
  Language:  C++
  Date:      $Date: 2008-02-06 13:21:44 $
  Version:   $Revision: 1.3 $
  Authors:   Paolo Quadrani
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "lhpBuilderLogic.h"
#include "lhpBuilderDecl.h"
#include "mafDecl.h"
#include "mafTagArray.h"
#include "mafOp.h"

//----------------------------------------------------------------------------
lhpBuilderLogic::lhpBuilderLogic()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
lhpBuilderLogic::~lhpBuilderLogic()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void lhpBuilderLogic::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
	if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
	{
		switch(e->GetId())
		{
		case ABOUT_APPLICATION:
			{
				wxString message = m_AppTitle.GetCStr();
				message += _(" Application ");
				message += m_Revision;
				wxMessageBox(message, "About Application");
				mafLogMessage(wxString::Format("%s",m_Revision.GetCStr()));
			}
			break;
    case ID_MSF_DATA_CACHE:
      {
        //comunicate to operation msf directory
        e->SetString(&m_VMEManager->GetFileName());
      }
      break;
		default:
			mafLogicWithManagers::OnEvent(maf_event);
			break; 
		} // end switch case
	} // end if SafeDowncast
}
//----------------------------------------------------------------------------
void lhpBuilderLogic::VmeAdded(mafNode *vme)
//----------------------------------------------------------------------------
{
  mafLogicWithManagers::VmeAdded(vme);
  // check for the presence of the LHDL attribute

  mafTagArray *lhdlArray = mafTagArray::SafeDownCast(vme->GetAttribute("LHDL"));
  if (lhdlArray == NULL)
  {
    lhdlArray = mafTagArray::New();
    lhdlArray->SetName("LHDL");
    vme->SetAttribute("LHDL",lhdlArray);
  }
}
