/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: testViewApp.cpp,v $
  Language:  C++
  Date:      $Date: 2011-05-25 09:57:03 $
  Version:   $Revision: 1.6.2.1 $
  Authors:   Silvano Imboden, Paolo Quadrani
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


#include "testViewApp.h"
#include "mafVMEFactory.h"
#include "mafPics.h"
#include "mafGUIMDIFrame.h"

#include "mafNodeGeneric.h"
#include "mafNodeRoot.h"
#include "mafVMERoot.h"
#include "mafVMESurface.h"
#include "mafPipeFactoryVME.h"
#include "mafPipeSurface.h"

#include "mafOpCreateVmeSurface.h"

#include "mafViewVTK.h"
#include "testView.h"

//--------------------------------------------------------------------------------
// Create the Application
//--------------------------------------------------------------------------------
IMPLEMENT_APP(testViewApp)

//--------------------------------------------------------------------------------
bool testViewApp::OnInit()
//--------------------------------------------------------------------------------
{
  mafPictureFactory::GetPictureFactory()->Initialize();	

  int result = mafVMEFactory::Initialize();
  assert(result==MAF_OK);
  
  // Inizializzazione e Fill della PipeFactory -- potrebbe essere un SideEffect del Plug dei Nodi
  result = mafPipeFactoryVME::Initialize();
  assert(result==MAF_OK);

  m_Logic = new mafLogicWithManagers();
  //m_Logic->PlugTimebar(false);
  //m_Logic->PlugMenu(false);
  //m_Logic->PlugToolbar(false);
  //m_Logic->PlugLogbar(false);
  //m_Logic->PlugSidebar(false);
  //m_Logic->PlugOpManager(false);
  //m_Logic->PlugViewManager(false);
  //m_Logic->PlugVMEManager(false);  // the VmeManager at the moment cause 4 leaks of 200+32+24+56 bytes  //SIL. 20-4-2005: 
  m_Logic->Configure();

  m_Logic->GetTopWin()->SetTitle("ViewVTK example");
  SetTopWindow(mafGetFrame());  

  m_Logic->Plug(new mafOpCreateVmeSurface("Add Vme Surface \tCtrl+A"));
  testView *tv = new testView("testView");
  tv->PlugVisualPipe("mafVMESurface", "mafPipeSurface");
  m_Logic->Plug(tv);

  m_Logic->Show();
  m_Logic->Init(0,NULL); // calls FileNew - which create the root
  return TRUE;
}
//--------------------------------------------------------------------------------
int testViewApp::OnExit()
//--------------------------------------------------------------------------------
{
  cppDEL(m_Logic);
  return 0;
}
