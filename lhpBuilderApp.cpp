/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpBuilderApp.cpp,v $
  Language:  C++
  Date:      $Date: 2007-07-05 09:22:08 $
  Version:   $Revision: 1.75 $
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

#include <wx/datetime.h>
#include <wx/config.h>

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
#include "mafVMELandmark.h" 
#include "mafPipeFactoryVME.h"
#include "mafPipeVolumeSlice.h"
#include "medPipeFactoryVME.h"
#include "medVMEFactory.h"
#include "medVMEEmg.h"
#include "medPipeVolumeDRR.h"
#include "medPipeTrajectories.h" 

#include "mmoBmpExporter.h"
#include "mmoCreateGroup.h"
#include "mmoCreateMeter.h"
#include "mmoCreateRefSys.h"

#include "mmoExplodeCollapse.h"
#include "mmoFilterSurface.h"
#include "mmoEditMetadata.h"
#include "mmo2DMeasure.h"
#include "mmoReparentTo.h"
#include "mmoDICOMImporter.h"
#include "mmoReparentTo.h"
#include "mmoImageImporter.h"
#include "mmoSTLExporter.h"
#include "mmoSTLImporter.h"
#include "mmoVTKExporter.h"
#include "mmoVTKImporter.h"
#include "mmoMSF1xImporter.h"
#include "mmoVRMLImporter.h"
#include "mmoRAWImporterVolume.h"
#include "mmoRAWExporter.h"
#include "mmoRAWImporterImages.h"
#include "mmoExtractIsosurface.h"
#include "mmoCrop.h"
#include "mmoVOIDensity.h"
#include "medOpVolumeResample.h"
#include "mmoAddLandmark.h"
#include "mmoRegisterClusters.h"
#include "mmoCreateSurfaceParametric.h"


#include "mmoMAFTransformScale.h"
#include "mmoMAFTransform.h"
#ifdef MAF_USE_ITK  
  #include "mmoEMGImporterWS.h"
  #include "mmoGRFImporterWS.h"
  #include "medPipeGraph.h"
  #include "mafVMERawMotionData.h" 
  #include "mafVMEC3DData.h" 
  #include "mmoMotionDataImporter.h"
  #include "mmoLandmarkExporter.h"
  #include "mmoClassicICPRegistration.h"
  #include "mmoMeshImporter.h"
  #include "mmoVMEDataSetAttributesImporter.h"
#endif
#include "mmoLandmarkImporter.h"
#include "mmoLandmarkImporterTXT.h"
#include "mmoLandmarkImporterWS.h"
#include "mmoLandmarkImporter.h"

#include "mafViewVTK.h"

#include "mafViewCompound.h"
#include "mafViewRXCT.h"
#include "mafViewRX.h"
#include "mafViewOrthoSlice.h"
#include "mafViewHTML.h"
#include "mafViewArbitrarySlice.h"
#include "mafViewGlobalSliceCompound.h"
#include "mafViewSlice.h"
#include "mafViewImage.h"
#include "mafViewRXCompound.h"
#include "mafViewImageCompound.h"

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

  result = medVMEFactory::Initialize();
  assert(result == MAF_OK);

	result = medPipeFactoryVME::Initialize();
	assert(result==MAF_OK);
  m_Logic = new lhpBuilderLogic();
  //m_Logic->PlugTimebar(false);
  //m_Logic->PlugMenu(false);
  //m_Logic->PlugToolbar(false);
  m_Logic->PlugLogbar(true);
  
  m_Logic->PlugSidebar(true,mafSideBar::DOUBLE_NOTEBOOK);
  //m_Logic->PlugOpManager(false);
  //m_Logic->PlugViewManager(false);
  //m_Logic->PlugVMEManager(false);  // the VmeManager at the moment cause 4 leaks of 200+32+24+56 bytes  //SIL. 20-4-2005: 
  
  m_Logic->Configure();

	wxRegKey RegKey(wxString("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\lhpBuilder"));
	if(RegKey.Exists())
	{
		RegKey.Create();
		wxString revision;
		RegKey.QueryValue(wxString("DisplayVersion"), revision);
		//revision=revision.AfterLast('_');
		m_Logic->SetRevision(revision);
	}
	else
	{
		wxString revision="0.1";
		m_Logic->SetRevision(revision);
	}
	m_Logic->GetTopWin()->SetTitle("LHPBuilder");
  SetTopWindow(mafGetFrame());  

  //------------------------- Importers -------------------------
  m_Logic->Plug(new mmoDICOMImporter("DICOM"));
  m_Logic->Plug(new mmoSTLImporter("STL"));
  m_Logic->Plug(new mmoVTKImporter("VTK"));
  m_Logic->Plug(new mmoMSF1xImporter("MAF 1.x"));
  m_Logic->Plug(new mmoRAWImporterVolume("RAW Volume"));
  m_Logic->Plug(new mmoImageImporter("Images"));
	m_Logic->Plug(new mmoRAWImporterImages("Raw Images"));
 // m_Logic->Plug(new mmoLandmarkImporterTXT("Landmark TXT")); //Importer for BodyBuilder Files
  m_Logic->Plug(new mmoLandmarkImporter("Landmark"));
	m_Logic->Plug(new mmoLandmarkImporterWS("Landmark WS"));
  m_Logic->Plug(new mmoMotionDataImporter<mafVMEC3DData>("C3D", "C3D Motion Data (*.c3d)|*.c3d", "Dictionary (*.txt)|*.txt"));
  
#ifdef MAF_USE_ITK
    m_Logic->Plug(new mmoMotionDataImporter<mafVMERawMotionData>("Raw Motion Data", "RAW Motion Data (*.MAN)|*.MAN", "Dictionary (*.txt)|*.txt"));
  // m_Logic->Plug(new mmoLandmarkImporter("Landmark")); //Old Importer
    m_Logic->Plug(new mmoEMGImporterWS("EMG"));
    m_Logic->Plug(new mmoGRFImporterWS("GRF"));
    m_Logic->Plug(new mmoMeshImporter("MESH"));
#endif
	
    m_Logic->Plug(new mmoVRMLImporter("Geometry VRML "));
  //-------------------------------------------------------------

  //------------------------- Exporters -------------------------
  m_Logic->Plug(new mmoSTLExporter("STL"));
  m_Logic->Plug(new mmoVTKExporter("VTK"));
	m_Logic->Plug(new mmoRAWExporter("Raw"));
  m_Logic->Plug(new mmoBmpExporter("Bmp"));

  #ifdef MAF_USE_ITK
    m_Logic->Plug(new mmoLandmarkExporter("Landmark"));
  #endif

  //-------------------------------------------------------------

  //------------------------- Operations -------------------------
  m_Logic->Plug(new mmoCreateGroup("Group"),"Create");
	m_Logic->Plug(new mmoCreateRefSys("Refsys"),"Create");
	m_Logic->Plug(new mmoCreateSurfaceParametric("Parametric Surface"),"Create");
	m_Logic->Plug(new mmoAddLandmark("Add Landmark \tCtrl+A"),"Create");
  m_Logic->Plug(new mmoRegisterClusters("Register Landmark Cloud"),"Fuse");
  m_Logic->Plug(new mmoCreateMeter("Distance Meter"),"Derive");
	m_Logic->Plug(new mmoExplodeCollapse("Explode/Collapse Landamark Cloud"),"Modify");
  m_Logic->Plug(new mmoEditMetadata("Metadata Editor"),"Modify");
	m_Logic->Plug(new mmoFilterSurface("Filter Surface"),"Modify");
	m_Logic->Plug(new mmoExtractIsosurface("Extract Isosurface"),"Modify");
	m_Logic->Plug(new mmoCrop("Crop Volume"),"Modify");
	m_Logic->Plug(new medOpVolumeResample("Volume Resample"),"Modify");
	m_Logic->Plug(new mmoMAFTransformScale("Scale Transform"),"Modify");
  m_Logic->Plug(new mmoMAFTransform("Transform"),"Modify");
	m_Logic->Plug(new mmo2DMeasure("2D Measure"),"Measure");
	m_Logic->Plug(new mmoVOIDensity("VOI Density"),"Measure");
  m_Logic->Plug(new mmoReparentTo("Reparent to...  \tCtrl+R"),"Fuse");
	#ifdef MAF_USE_ITK
    m_Logic->Plug(new mmoVMEDataSetAttributesImporter("VME DataSet Attributes Importer"),"Modify");
		m_Logic->Plug(new mmoClassicICPRegistration("Register Surface"),"Fuse");
	#endif
  


  //-------------------------------------------------------------

  //------------------------- Views -------------------------
	/*mafViewVTK *v = new mafViewVTK("Slice view", CAMERA_CT);
  v->PlugVisualPipe("mafVMEVolumeGray", "mafPipeVolumeSlice");
  m_Logic->Plug(v);*/
  //m_Logic->Plug(new mafViewVTK("VTK view"));

	

  mafViewVTK *traj = new mafViewVTK("AL Trajectories");
  traj->PlugVisualPipe("mafVMELandmark", "medPipeTrajectories");
  m_Logic->Plug(traj);

  //View Arbitrary Slice
  mafViewArbitrarySlice *ArbitraryView = new mafViewArbitrarySlice("Arbitrary");
  ArbitraryView->PackageView();
  m_Logic->Plug(ArbitraryView);


  // View DRR
  mafViewVTK *vdrr = new mafViewVTK("DRR");
  vdrr->PlugVisualPipe("mafVMEVolumeGray","medPipeVolumeDRR",MUTEX);
  m_Logic->Plug(vdrr);

#ifdef MAF_USE_ITK
  mafViewVTK *graph = new mafViewVTK("EMG Graph");
  graph->PlugVisualPipe("medVMEEmg", "medPipeGraph");
  m_Logic->Plug(graph);
#endif

  //View Global Slice
  mafViewGlobalSliceCompound *GlobalSlice = new mafViewGlobalSliceCompound("Global Slice");
  GlobalSlice->PackageView();
  m_Logic->Plug(GlobalSlice);

  // View HTML
  mafViewHTML *vhtml = new mafViewHTML("HTML");
  m_Logic->Plug(vhtml);

  mafViewImageCompound *vImage = new mafViewImageCompound("Image");
  vImage->PackageView();
  m_Logic->Plug(vImage);


  mafViewVTK *viso = new mafViewVTK("Isosurface");
  viso->PlugVisualPipe("mafVMEVolumeGray", "mafPipeIsosurface",MUTEX);
  m_Logic->Plug(viso);

  mafViewOrthoSlice *viewOrthoSlice = new mafViewOrthoSlice("OrthoSlice");
  viewOrthoSlice->PackageView();
  m_Logic->Plug(viewOrthoSlice);

  mafViewRXCompound *RX = new mafViewRXCompound("RX");
  RX->PackageView();
  m_Logic->Plug(RX);

  mafViewRXCT *vrxctl = new mafViewRXCT("RXCT");
  vrxctl->PackageView();
  m_Logic->Plug(vrxctl);

	mafViewVTK *vsurface = new mafViewVTK("Surface");
	vsurface->PlugVisualPipe("mafVMESurface","mafPipeSurface");
	m_Logic->Plug(vsurface);

/*
  mafViewCompound *vc = new mafViewCompound("view compound",3);
  mafViewVTK *v2 = new mafViewVTK("Slice view", CAMERA_CT);
  v2->PlugVisualPipe("mafVMEVolumeGray", "mafPipeVolumeSlice");
  vc->PlugChildView(v2);
  m_Logic->Plug(vc);
*/


	
  // View 2D
  /*mafViewSlice *vImage = new mafViewSlice("View Image",CAMERA_OS_Z,false,false,false);
  vImage->PlugVisualPipe("mafVMEVolumeGray","mafPipeBox",NON_VISIBLE);
  vImage->PlugVisualPipe("mafVMESurface","mafPipeSurface",NON_VISIBLE);
  vImage->PlugVisualPipe("mafVMEImage", "mafPipeImage3D", MUTEX);
  m_Logic->Plug(vImage);*/

  /*mafViewImage *vImage = new mafViewImage("View Image",CAMERA_FRONT,false,false,false);
  vImage->PlugVisualPipe("mafVMEVolumeGray","mafPipeBox",NON_VISIBLE);
  vImage->PlugVisualPipe("mafVMESurface","mafPipeSurface",NON_VISIBLE);
  m_Logic->Plug(vImage);*/



	/*mafViewVTK *vslice = new mafViewVTK("Slice view", CAMERA_CT);
  vslice->PlugVisualPipe("mafVMEVolumeGray", "mafPipeVolumeSlice");
  m_Logic->Plug(vslice);*/

  //-------------------------------------------------------------

  wxBitmap splashBitmap;
   splashBitmap.LoadFile("../Splash/SPLASH_SCREEN.bmp", wxBITMAP_TYPE_BMP);
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
