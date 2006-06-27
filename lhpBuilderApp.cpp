/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpBuilderApp.cpp,v $
  Language:  C++
  Date:      $Date: 2006-06-27 14:24:00 $
  Version:   $Revision: 1.5 $
  Authors:   Paolo Quadrani
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


#include "lhpBuilderApp.h"
#include "mafDecl.h"
#include "mafVMEFactory.h"
#include "mafPics.h"
#include "mmgMDIFrame.h"

#include "mafNodeFactory.h"
#include "mafNodeGeneric.h"
#include "mafNodeRoot.h"
#include "mafVMERoot.h"
#include "mafVMESurface.h"
#include "mafPipeFactoryVME.h"

#include "mmoCreateGroup.h"
#include "mmoCreateMeter.h"
#include "mmoCreateSlicer.h"
#include "mmoDICOMImporter.h"
#include "mmoReparentTo.h"
#include "mmoSTLExporter.h"
#include "mmoSTLImporter.h"
#include "mmoVTKExporter.h"
#include "mmoVTKImporter.h"
#include "mmoMSF1xImporter.h"

#include "mafViewVTK.h"
#include "mafViewCompound.h"
#include "mafViewRXCTLHPBuilder.h"

//--------------------------------------------------------------------------------
// Create the Application
//--------------------------------------------------------------------------------
IMPLEMENT_APP(lhpBuilderApp)

//--------------------------------------------------------------------------------
bool lhpBuilderApp::OnInit()
//--------------------------------------------------------------------------------
{
  mafPics.Initialize();	
//#include "pic/SPLASH_SCREEN.xpm"
//	mafADDPIC(SPLASH_SCREEN);

  int result = mafVMEFactory::Initialize();
  assert(result==MAF_OK);
  
  // Initialize and Fill of PipeFactory -- could be a SideEffect of the node plug
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

  m_Logic->GetTopWin()->SetTitle("LHPBuilder");
  SetTopWindow(mafGetFrame());  

  //------------------------- Importers -------------------------
  /*m_Logic->Plug(new mmoDICOMImporter("DICOM"));
  m_Logic->Plug(new mmoSTLImporter("STL"));
  m_Logic->Plug(new mmoVTKImporter("VTK"));*/
  m_Logic->Plug(new mmoMSF1xImporter("MAF 1.x"));
  //-------------------------------------------------------------

  //------------------------- Exporters -------------------------
  //m_Logic->Plug(new mmoSTLExporter("STL"));
  //m_Logic->Plug(new mmoVTKExporter("VTK"));
  //-------------------------------------------------------------

  //------------------------- Operations -------------------------
  //m_Logic->Plug(new mmoCreateGroup("Create Group"));
  //m_Logic->Plug(new mmoCreateMeter("Create Meter"));
  //m_Logic->Plug(new mmoCreateSlicer("Create Slicer"));
  //m_Logic->Plug(new mmoReparentTo("Reparent to...  \tCtrl+R"));
  //-------------------------------------------------------------

  //------------------------- Views -------------------------
/*mafViewVTK *v = new mafViewVTK("Slice view", CAMERA_CT);
  v->PlugVisualPipe("mafVMEVolumeGray", "mafPipeVolumeSlice");
  m_Logic->Plug(v);
  m_Logic->Plug(new mafViewVTK("VTK view"));
*/
	mafViewVTK *viso = new mafViewVTK("Isosurface view", CAMERA_CT);
  viso->PlugVisualPipe("mafVMEVolume", "mafPipeIsosurface");
  m_Logic->Plug(viso);
/*
  mafViewCompound *vc = new mafViewCompound("view compound",3);
  mafViewVTK *v2 = new mafViewVTK("Slice view", CAMERA_CT);
  v2->PlugVisualPipe("mafVMEVolumeGray", "mafPipeVolumeSlice");
  vc->PlugChildView(v2);
  m_Logic->Plug(vc);
*/
  mafViewRXCTLHPBuilder *vrxct = new mafViewRXCTLHPBuilder("RXCT view");
  vrxct->PackageView();
  m_Logic->Plug(vrxct);
  //-------------------------------------------------------------

  wxBitmap splashBitmap;
   splashBitmap.LoadFile("Splash/SPLASH_SCREEN.bmp", wxBITMAP_TYPE_BMP);
   m_Logic->ShowSplashScreen(splashBitmap); 

  // show the application
	m_Logic->ShowSplashScreen(splashBitmap);
  m_Logic->Show();
  m_Logic->Init(0,NULL); // calls FileNew - which create the root
  return TRUE;
}
//--------------------------------------------------------------------------------
int lhpBuilderApp::OnExit()
//--------------------------------------------------------------------------------
{
  cppDEL(m_Logic);
  return 0;
}

