/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafPipe.cpp,v $
  Language:  C++
  Date:      $Date: 2010-11-10 16:52:01 $
  Version:   $Revision: 1.16.2.2 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
//----------------------------------------------------------------------------


#include "mafSceneNode.h"
#include "mafSceneGraph.h"
#include "mafGUI.h"

#include "mafPipeVTK.h"
#include "mafViewVTK.h"
#include "mafTransformBase.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafPipeVTK);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafPipeVTK::mafPipeVTK()
//----------------------------------------------------------------------------
{
  m_Vme           = NULL;
	m_AssemblyFront	= NULL;
	m_AssemblyBack	= NULL;
	m_AlwaysVisibleAssembly = NULL;
	m_RenFront      = NULL;
	m_RenBack       = NULL;
	m_AlwaysVisibleRenderer = NULL;
}
//----------------------------------------------------------------------------
void mafPipeVTK::Create(mafNode *node, mafView *view)
//----------------------------------------------------------------------------
{
  Superclass::Create(node, view);
  mafViewVTK *viewVTK = mafViewVTK::SafeDownCast(m_View);
  if(viewVTK)
  {
    m_RenFront      = viewVTK->GetFrontRenderer();
    m_RenBack       = viewVTK->GetBackRenderer();
    m_AlwaysVisibleRenderer = viewVTK->GetAlwaysVisibleRenderer();
  }

  m_Vme = mafVME::SafeDownCast(m_Node);
  mafSceneGraph *sg = viewVTK->GetSceneGraph();
  mafSceneNode  *n  = NULL;
  if(sg)
    n = sg->Vme2Node(m_Vme);
  if(n)
  {
    m_AssemblyFront         = n->m_AssemblyFront;
    m_AssemblyBack          = n->m_AssemblyBack;
    m_AlwaysVisibleAssembly = n->m_AlwaysVisibleAssembly;
  }

  m_Selected = false;
}
//----------------------------------------------------------------------------
mafPipeVTK::~mafPipeVTK()
//----------------------------------------------------------------------------
{
}
