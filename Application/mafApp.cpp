/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpFusionBoxApp.cpp,v $
  Language:  C++
  Date:      $Date: 2009-05-29 09:41:31 $
  Version:   $Revision: 1.71.2.24 $
  Authors:   Paolo Quadrani , Stefano Perticoni
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
#include "mafApp.h"
#include "mafLogicWithManagers.h"

#include <wx/datetime.h>
#include <wx/config.h>

#include "mafDecl.h"
#include "mafVMEFactory.h"
#include "mafPics.h"
#include "mafGUIMDIFrame.h"
#include "mafInteractionFactory.h"

#include "mafNodeFactory.h" 
#include "mafNodeGeneric.h"
#include "mafNodeRoot.h"
#include "mafVMERoot.h"
#include "mafVMESurface.h"
#include "mafVMELandmark.h" 
#include "mafPipeFactoryVME.h"
#include "mafPipeVolumeSlice.h"
#include "mafOpSelect.h"
#include "mafOpDecomposeTimeVarVME.h"
#include "mafOpImporterMSF.h"
#include "mafOpImporterExternalFile.h"
#include "mafOpLabelExtractor.h"
#include "mafOpOpenExternalFile.h"
#include "mafOpExporterBmp.h"
#include "mafOpCreateGroup.h"
#include "mafOpCreateMeter.h"
#include "mafOpCreateSlicer.h"
#include "mafOpCreateRefSys.h"
#include "mafOpFilterSurface.h"
#include "mafOpEditMetadata.h"
#include "mafOp2DMeasure.h"
#include "mafOpReparentTo.h"
#include "mafOpReparentTo.h"
#include "mafOpImporterImage.h"
#include "mafOpImporterSTL.h"
#include "mafOpExporterSTL.h"
#include "mafOpExporterVTK.h"
#include "mafOpImporterVTK.h"
#include "mafOpImporterMSF1x.h"
#include "mafOpImporterVRML.h"
#include "mafOpCreateVolume.h"
#include "mafOpVOIDensityEditor.h"
#include "mafOpImporterRAWVolume.h"
#include "mafOpExporterRaw.h"
#include "mafOpExtractIsosurface.h"
#include "mafOpCrop.h"
#include "mafOpVOIDensity.h"
#include "mafOpAddLandmark.h"
#include "mafOpCreateSurfaceParametric.h"
#include "mafOpImporterMesh.h"
#include "mafOpImporterVMEDataSetAttributes.h"
#include "mafViewVTK.h"
#include "mafViewCompound.h"
#include "mafOpValidateTree.h"
#include "mafOpApplyTrajectory.h"

#include <vtkTimerLog.h>

#include "vtkUnstructuredGrid.h"
#include "vtkMAFSmartPointer.h"
#include "vtkTransformFilter.h"
#include "vtkStructuredPoints.h"



//--------------------------------------------------------------------------------
// Create the Application
//--------------------------------------------------------------------------------

IMPLEMENT_APP(mafApp)

////BES: 14.5.2008 - OnIdle to unlock blocks
//BEGIN_EVENT_TABLE(mafApp, wxApp)
//  EVT_IDLE(mafApp::OnIdle)
//END_EVENT_TABLE()

//--------------------------------------------------------------------------------
bool mafApp::OnInit()
//--------------------------------------------------------------------------------
{
  ////BES: 14.5.2008 - initialize the manager (with default settings)
  ////It provides a sophisticated memory-disk swap algorithm that
  ////allows handling of many large size memory blocks that would
  ////take more memory than available in total.
  ////The mechanism is NOT GUARANTEED TO BE SAFE
  ////Swapping (especially, if HandleNewFailure option is enabled)
  ////may lead to artifacts in data or even 
  //vtkDataArrayMemMng::InitializeManagerUnSafeMode();  


  mafPictureFactory::GetPictureFactory()->Initialize();	
 
  #include "pic/mafApp/FRAME_ICON16x16.xpm"
  mafADDPIC(FRAME_ICON16x16);

  #include "pic/mafApp/FRAME_ICON32x32.xpm"
  mafADDPIC(FRAME_ICON32x32);

  #include "pic/mafApp/MDICHILD_ICON.xpm"
  mafADDPIC(MDICHILD_ICON);

  int result;
 
  result = mafVMEFactory::Initialize();
  assert(result == MAF_OK);

	result = mafPipeFactoryVME::Initialize();
	assert(result==MAF_OK);

  result = mafInteractionFactory::Initialize();
  assert(result==MAF_OK);

  mafLogicWithManagers *logic = new mafLogicWithManagers();
  m_Logic = logic;
  logic->GetTopWin()->SetTitle("mafApp");
  logic->Configure();
  SetTopWindow(mafGetFrame());  

  wxString regKeyName;
  regKeyName = "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\mafApp";
  wxRegKey RegKey(regKeyName);
	if(RegKey.Exists())
	{
		RegKey.Create();
		wxString revision;
		RegKey.QueryValue(wxString("DisplayVersion"), revision);
		logic->SetRevision(revision);
	}
	else
	{
		wxString revision="0.1";
		logic->SetRevision(revision);
	}
  //------------------------- Editors -------------------------
  logic->Plug(new mafOpDelete(_("Delete   \tCtrl+Shift+D")), "");
  logic->Plug(new mafOpCut(_("Cut   \tCtrl+Shift+X")), "");
  logic->Plug(new mafOpCopy(_("Copy  \tCtrl+Shift+C")), "");
  logic->Plug(new mafOpPaste(_("Paste \tCtrl+Shift+V")), "");
  //------------------------- Importers -------------------------
  logic->Plug(new mafOpImporterSTL("STL"),"Geometries");
  logic->Plug(new mafOpImporterMSF("MSF"),"Other");
  logic->Plug(new mafOpImporterVRML("VRML"), "Geometries");
  logic->Plug(new mafOpImporterVTK("VTK"),"Other");
  logic->Plug(new mafOpImporterMSF1x("MAF 1.x"),"Other");
  logic->Plug(new mafOpImporterRAWVolume("Raw Volume Legacy"),"Images");
  logic->Plug(new mafOpImporterImage("Images"),"Images");
  logic->Plug(new mafOpImporterMesh("Generic Mesh"), "Finite Element");
  logic->Plug(new mafOpImporterExternalFile("External data"), "Other");

  //-------------------------------------------------------------

  //------------------------- Exporters -------------------------
  logic->Plug(new mafOpExporterSTL("STL"),"Geometries");
  logic->Plug(new mafOpExporterVTK("VTK"), "Other");
  logic->Plug(new mafOpExporterBmp("Bmp"), "Images");
  logic->Plug(new mafOpExporterRAW("Raw"), "Images");
  //-------------------------------------------------------------

  //------------------------- Operations -------------------------
  logic->Plug(new mafOpValidateTree());
  logic->Plug(new mafOpCreateGroup("Group"),"Create/New");
	logic->Plug(new mafOpCreateSurfaceParametric("Parametric Surface"),"Create/New");
	logic->Plug(new mafOpAddLandmark("Add Landmark \tCtrl+A"),"Create/New");
  logic->Plug(new mafOpCreateMeter("Meter"),"Create/Derive");

  logic->Plug(new mafOpReparentTo("Reparent to...  \tCtrl+R"),"Modify/Fuse");
  logic->Plug(new mafOpReparentTo("Local reparent to...", false),"Modify/Fuse");
  logic->Plug(new mafOpCreateVolume("Constant Volume"),"Create/New");

  logic->Plug(new mafOpCreateRefSys("Refsys"),"Create/New");
  logic->Plug(new mafOpCreateSlicer("Slicer"),"Create/Derive");
  logic->Plug(new mafOpFilterSurface("Filter Surface"),"Modify");
  logic->Plug(new mafOpVOIDensityEditor("Volume Density"),"Modify");
  logic->Plug(new mafOpApplyTrajectory("Apply Trajectory"), "Modify");
  logic->Plug(new mafOpExtractIsosurface("Extract Isosurface"),"Create/Derive");
  logic->Plug(new mafOpCrop("Crop Volume"),"Modify");
  logic->Plug(new mafOp2DMeasure("2D Measure"),"Measure");
  logic->Plug(new mafOpVOIDensity("VOI Density"),"Measure");
  logic->Plug(new mafOpImporterVMEDataSetAttributes("VME DataSet Attributes Adder"),"Modify");
  logic->Plug(new mafOpOpenExternalFile("Open with external program"),"Manage"); 

  logic->Plug(new mafOpDecomposeTimeVarVME("Decompose Time"),"Create/Derive");
  logic->Plug(new mafOpLabelExtractor("Extract Label"),"Create/Derive");
  
  //-------------------------------------------------------------
  bool view_visibility = 0;//fullVersion;
  //------------------------- Views -------------------------

  // View DRR
  mafViewVTK *vdrr = new mafViewVTK("DRR");
  vdrr->PlugVisualPipe("mafVMEVolumeGray","medPipeVolumeDRR",MUTEX);
  vdrr->PlugVisualPipe("mafVMEVolumeLarge","medPipeVolumeDRR",MUTEX);
  logic->Plug(vdrr, view_visibility);

  // View Analog graph
  mafViewVTK *graph = new mafViewVTK("Analog Graph", CAMERA_PERSPECTIVE, false);
  graph->PlugVisualPipe("medVMEAnalog", "medPipeGraph",MUTEX);
  logic->Plug(graph/*, view_visibility*/);

  mafViewVTK *viso = new mafViewVTK("Isosurface");
  viso->PlugVisualPipe("mafVMEVolumeGray", "mafPipeIsosurface",MUTEX);
  viso->PlugVisualPipe("medVMELabeledVolume", "mafPipeIsosurface",MUTEX);
  viso->PlugVisualPipe("mafVMEVolumeLarge","mafPipeIsosurface",MUTEX);
  logic->Plug(viso, view_visibility);

  mafViewVTK *visoGPU = new mafViewVTK("Isosurface (GPU)");
  visoGPU->PlugVisualPipe("mafVMEVolumeGray", "mafPipeIsosurfaceGPU",MUTEX);   //BES: 13.11.2008 - GPU support, mafPipeIsosurfaceGPU to be merged with mafPipeIsosurface in future 
  visoGPU->PlugVisualPipe("medVMELabeledVolume", "mafPipeIsosurfaceGPU",MUTEX);
  visoGPU->PlugVisualPipe("mafVMEVolumeLarge", "mafPipeIsosurfaceGPU",MUTEX);
  logic->Plug(visoGPU, view_visibility);

	mafViewVTK *vsurface = new mafViewVTK("Surface");
	vsurface->PlugVisualPipe("mafVMESurface","mafPipeSurface");
  vsurface->PlugVisualPipe("mafVMELandmark", "medPipeTrajectories");
	logic->Plug(vsurface);

  //-------------------------------------------------------------
  wxBitmap splashBitmap;
  splashBitmap.LoadFile("../Splash/SPLASH_SCREEN.bmp", wxBITMAP_TYPE_BMP);
  logic->ShowSplashScreen(splashBitmap);

  // show the application
  logic->Show();

  logic->Init(0,NULL); // calls FileNew - which create the root

  return TRUE;
}
//--------------------------------------------------------------------------------
int mafApp::OnExit()
//--------------------------------------------------------------------------------
{
  cppDEL(m_Logic);

  //this hack is fixing VTK internal memory leak
  vtkTimerLog::CleanupLog();
  return 0;
}
