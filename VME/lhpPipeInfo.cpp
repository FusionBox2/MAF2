/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpPipeInfo.cpp,v $
  Language:  C++
  Date:      $Date: 2008-04-14 12:03:18 $
  Version:   $Revision: 1.6 $
  Authors:   Fedor Moiseev
==========================================================================
  Copyright (c) 2001/2007 
  ULB - Universite Libre de Bruxelles (www.ulb.ac.be)
=========================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "lhpPipeInfo.h"
#include "mafDecl.h"
#include "lhpViewInfo.h"
#include "lhpVMEKMInfo.h"

mafCxxTypeMacro(lhpPipeInfo);

//----------------------------------------------------------------------------
lhpPipeInfo::lhpPipeInfo()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
lhpPipeInfo::~lhpPipeInfo()
//----------------------------------------------------------------------------
{
  if (m_Node)
  {
    m_Node->RemoveObserver(this);
  }
  lhpViewInfo *igraph = lhpViewInfo::SafeDownCast(m_View);
  if(igraph && igraph->GetRenderWindow())
  {
    igraph->GetRenderWindow()->RemInfo(m_Node);
  }
}

//----------------------------------------------------------------------------
void lhpPipeInfo::Create(mafNode *node, mafView *view)
//----------------------------------------------------------------------------
{
  Superclass::Create(node, view);
  lhpViewInfo *igraph = lhpViewInfo::SafeDownCast(m_View);
  if(igraph)
    igraph->GetRenderWindow()->AddInfo(m_Node);
  m_Node->AddObserver(this);
}

void lhpPipeInfo::OnEvent(mafEventBase *maf_event)
{
  if(maf_event->GetSender() == m_Node && maf_event->GetId() == VME_MODIFIED)
    m_View->CameraUpdate();
  Superclass::OnEvent(maf_event);
}


