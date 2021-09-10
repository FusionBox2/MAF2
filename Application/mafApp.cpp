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
#include "mafOpCreateMuscleWrapping2.h"
#include "mafOpCreateMeter2.h"
#include "mafOpCreateGravityLine.h"
#include "mafOpCreateCenterLine.h"
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
#include "mafOpExporterVRML.h"
#include "mafOpExporterGeomTex.h"
#include "mafOpImporterVTK.h"
#include "mafOpImporterMSF1x.h"
#include "mafOpImporterVRML.h"
#include "mafOpCreateVolume.h"
#include "mafOpCreatePlane.h"
#include "mafOpCreateOsteometricBoard.h"
#include "mafOpVOIDensityEditor.h"
#include "mafOpCreateQuadricSurfaceFitting.h"
//#include "mafOpCreateMuscleWrapper.h"
//BES: 23.6.2008 - Large Volume - to be merged 
#include "mafOpImporterRAWVolume.h"
#include "mafOpExporterRaw.h"
#include "mafOpExtractIsosurface.h"
#include "mafOpCrop.h"
#include "mafOpVOIDensity.h"
#include "mafOpAddLandmark.h"
#include "mafOpCreateSurfaceParametric.h"
#include "mafOpCreateEllipsoid.h"
#include "mafOpCreateHyperboloid.h"
#include "mafOpCreateHyperboloid2S.h"
#include "mafOpCreateCylinder.h"
#include "mafOpImporterMesh.h"
#include "mafOpImporterVMEDataSetAttributes.h"
#include "mafViewVTK.h"
#include "mafViewCompound.h"
#include "mafOpValidateTree.h"
#include "mafOpApplyTrajectory.h"
#include "mafOpCrop3DSurface.h"

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
		logic->SetRevision(mafWxToString(revision));
	}
	else
	{
		mafString revision=_R("0.1");
		logic->SetRevision(revision);
	}
  //------------------------- Editors -------------------------
  logic->Plug(new mafOpDelete(_L("Delete   \tCtrl+Shift+D")), _R(""));
  logic->Plug(new mafOpCut(_L("Cut   \tCtrl+Shift+X")), _R(""));
  logic->Plug(new mafOpCopy(_L("Copy  \tCtrl+Shift+C")), _R(""));
  logic->Plug(new mafOpPaste(_L("Paste \tCtrl+Shift+V")), _R(""));
  //------------------------- Importers -------------------------
  logic->Plug(new mafOpImporterSTL(_R("STL")),_R("Geometries"));
  logic->Plug(new mafOpImporterMSF(_R("MSF")),_R("Other"));
  logic->Plug(new mafOpImporterVRML(_R("VRML")), _R("Geometries"));
  logic->Plug(new mafOpImporterVTK(_R("VTK")),_R("Other"));
  logic->Plug(new mafOpImporterMSF1x(_R("MAF 1.x")),_R("Other"));
  logic->Plug(new mafOpImporterRAWVolume(_R("Raw Volume Legacy")),_R("Images"));
  logic->Plug(new mafOpImporterImage(_R("Images")),_R("Images"));
  logic->Plug(new mafOpImporterMesh(_R("Generic Mesh")), _R("Finite Element"));
  logic->Plug(new mafOpImporterExternalFile(_R("External data")), _R("Other"));

  //-------------------------------------------------------------

  //------------------------- Exporters -------------------------
  logic->Plug(new mafOpExporterSTL(_R("STL")),_R("Geometries"));
  logic->Plug(new mafOpExporterVTK(_R("VTK")), _R("Other"));
  logic->Plug(new mafOpExporterBmp(_R("Bmp")), _R("Images"));
  logic->Plug(new mafOpExporterRAW(_R("Raw")), _R("Images"));
  //-------------------------------------------------------------

  //------------------------- Operations -------------------------
  logic->Plug(new mafOpValidateTree());
  logic->Plug(new mafOpCreateGroup(_R("Group")),_R("Create/New"));
  logic->Plug(new mafOpCreateSurfaceParametric(_R("Parametric Surface")),_R("Create/New"));
  logic->Plug(new mafOpCreateEllipsoid(_R("Ellipsoid")), _R("Create/New/QuadricSurface"));
  logic->Plug(new mafOpCreateHyperboloid(_R("Hyperboloid1S")), _R("Create/New/QuadricSurface"));
  logic->Plug(new mafOpCreateHyperboloid2S(_R("Hyperboloid2S")), _R("Create/New/QuadricSurface"));
  logic->Plug(new mafOpCreateCylinder(_R("Cylinder")), _R("Create/New/QuadricSurface"));
  logic->Plug(new mafOpCreateQuadricSurfaceFitting(_R("Quadric Surface Fitting")), _R("Create/Derive"));
  logic->Plug(new mafOpAddLandmark(_R("Add Landmark \tCtrl+A")),_R("Create/New"));
  logic->Plug(new mafOpCreateMeter(_R("Meter")),_R("Create/Derive"));
  logic->Plug(new mafOpCreateMeter2(_R("Meter2")), _R("Create/Derive"));
  logic->Plug(new mafOpCreateCenterLine(_R("Centerline")), _R("Create/Derive"));
  logic->Plug(new mafOpCreateGravityLine(_R("GravityLine")), _R("Create/New/Osteometric Tools"));

  logic->Plug(new mafOpCreateMuscleWrapping2(_R("Muscle Wrapper_2")), _R("Create/Derive"));
  logic->Plug(new mafOpReparentTo(_R("Reparent to...  \tCtrl+R")),_R("Modify/Fuse"));
  logic->Plug(new mafOpReparentTo(_R("Local reparent to..."), false),_R("Modify/Fuse"));
  logic->Plug(new mafOpCreatePlane(_R("Plane")), _R("Create/New/Osteometric Tools"));
  logic->Plug(new mafOpCreateVolume(_R("Constant Volume")),_R("Create/New"));

  //logic->Plug(new mafOpCreateRefSys(_R("Refsys")),_R("Create/New"));
  logic->Plug(new mafOpCreateSlicer(_R("Slicer")),_R("Create/Derive"));
  logic->Plug(new mafOpFilterSurface(_R("Filter Surface")),_R("Modify"));
  logic->Plug(new mafOpVOIDensityEditor(_R("Volume Density")),_R("Modify"));
  logic->Plug(new mafOpApplyTrajectory(_R("Apply Trajectory")), _R("Modify"));
  logic->Plug(new mafOpExtractIsosurface(_R("Extract Isosurface")),_R("Create/Derive"));
  logic->Plug(new mafOpCrop(_R("Crop Volume")),_R("Modify"));
  logic->Plug(new mafOpCrop3DSurface(_R("Crop 3D Surface")), _R("Modify"));
  logic->Plug(new mafOp2DMeasure(_R("2D Measure")),_R("Measure"));
  logic->Plug(new mafOpVOIDensity(_R("VOI Density")),_R("Measure"));
  logic->Plug(new mafOpImporterVMEDataSetAttributes(_R("VME DataSet Attributes Adder")),_R("Modify"));

  logic->Plug(new mafOpOpenExternalFile(_R("Open with external program")),_R("Manage")); 

  logic->Plug(new mafOpDecomposeTimeVarVME(_R("Decompose Time")),_R("Create/Derive"));
  logic->Plug(new mafOpLabelExtractor(_R("Extract Label")),_R("Create/Derive"));
  
  //-------------------------------------------------------------
  bool view_visibility = 0;//fullVersion;
  //------------------------- Views -------------------------

  mafViewVTK *viso = new mafViewVTK(_R("Isosurface"));
  viso->PlugVisualPipe(_R("mafVMEVolumeGray"), _R("mafPipeIsosurface"),MUTEX);
  viso->PlugVisualPipe(_R("medVMELabeledVolume"), _R("mafPipeIsosurface"),MUTEX);
  viso->PlugVisualPipe(_R("mafVMEVolumeLarge"),_R("mafPipeIsosurface"),MUTEX);
  logic->Plug(viso, view_visibility);

  mafViewVTK *visoGPU = new mafViewVTK(_R("Isosurface (GPU)"));
  visoGPU->PlugVisualPipe(_R("mafVMEVolumeGray"), _R("mafPipeIsosurfaceGPU"),MUTEX);   //BES: 13.11.2008 - GPU support, mafPipeIsosurfaceGPU to be merged with mafPipeIsosurface in future 
  visoGPU->PlugVisualPipe(_R("medVMELabeledVolume"), _R("mafPipeIsosurfaceGPU"),MUTEX);
  logic->Plug(visoGPU, view_visibility);

  mafViewVTK *vsurface = new mafViewVTK(_R("Surface"));
  vsurface->PlugVisualPipe(_R("mafVMESurface"),_R("mafPipeSurface"));
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
