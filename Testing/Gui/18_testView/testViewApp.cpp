/*=========================================================================

 Program: MAF2
 Module: testViewApp
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


#include "testViewApp.h"
#include "mafVMEFactory.h"
#include "mafPics.h"
#include "mafGUIMDIFrame.h"

#include "mafNodeFactory.h"
#include "mafNodeGeneric.h"
#include "mafNodeRoot.h"
#include "mafVMERoot.h"
#include "mafVMESurface.h"
#include "mafPipeFactory.h"

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
  mafPics.Initialize();	

  int result = mafVMEFactory::Initialize();
  assert(result==MAF_OK);
  
  // Inizializzazione e Fill della PipeFactory -- potrebbe essere un SideEffect del Plug dei Nodi
  result = mafPipeFactory::Initialize();
  assert(result==MAF_OK);

  m_logic = new mafLogicWithManagers();
  //m_logic->PlugTimebar(false);
  //m_logic->PlugMenu(false);
  //m_logic->PlugToolbar(false);
  //m_logic->PlugLogbar(false);
  //m_logic->PlugSidebar(false);
  //m_logic->PlugOpManager(false);
  //m_logic->PlugViewManager(false);
  //m_logic->PlugVMEManager(false);  // the VmeManager at the moment cause 4 leaks of 200+32+24+56 bytes  //SIL. 20-4-2005: 
  m_logic->Configure();

  m_logic->GetTopWin()->SetTitle("testView");
  SetTopWindow(mafGetFrame());  

  m_logic->Plug(new mafOpCreateVmeSurface("Add Vme Surface \tCtrl+A"));
  //m_logic->Plug(new testView("testView"));
  testView *tv = new testView("testView");
  tv->PlugVisualPipe("mafVMESurface", "mafPipeSurface");
  m_logic->Plug(tv);

  m_logic->Show();
  m_logic->Init(0,NULL); // calls FileNew - which create the root
  return TRUE;
}
//--------------------------------------------------------------------------------
int testViewApp::OnExit()
//--------------------------------------------------------------------------------
{
  cppDEL(m_logic);
  return 0;
}










