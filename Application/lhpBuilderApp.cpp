/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpBuilderApp.cpp,v $
  Language:  C++
  Date:      $Date: 2007-10-02 12:08:26 $
  Version:   $Revision: 1.13 $
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

#include <wx/datetime.h>
#include <wx/config.h>
#include <vtkTimerLog.h>

#include "lhpBuilderApp.h"
#include "mafDecl.h"
#include "mafVMEFactory.h"
#include "mafPics.h"
#include "mmgMDIFrame.h"
#include "mafInteractionFactory.h"

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
#include "mafVMEAFRefSys.h" 
#include "mafVMEHelAxis.h" 

#include "mafOpImporterMSF.h"
#include "mafOpImporterExternalFile.h"
#include "mafOpOpenExternalFile.h"
#include "mmoBmpExporter.h"
#include "mmoCreateGroup.h"
#include "mmoCreateMeter.h"
#include "medOpCreateWrappedMeter.h"
#include "mmoCreateSlicer.h"
#include "mmoCreateRefSys.h"
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
#include "mmoBuildHierarchy.h"
#include "mmoTimeReduce.h"
#include "mmoINPExporter.h"
#include "mmoMTRExporter.h"
#include "mmoINPImporter.h"
#include "mmoMTRImporter.h"
#include "mmoAFSys.h"
#include "mmoAverageLM.h"
#include "mmoHelAxis.h"
#include "mmoStickPalpation.h"
#include "medOpScaleDataset.h"
#include "medOpMove.h"
#include "mmoMAFTransform.h"
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
#include "mmoLandmarkImporter.h"
#include "mmoLandmarkImporterTXT.h"
#include "mmoLandmarkImporterWS.h"
#include "mmoLandmarkImporter.h"
#include "lhpOpBonemat.h"
#include "medOpFreezeVME.h"
#include "medOpExporterWrappedMeter.h"
#include "medOpIterativeRegistration.h"

#include "mafViewVTK.h"
#include "mafViewCompound.h"
#include "mafViewRXCT.h"
#include "mafViewRX.h"
#include "mafViewOrthoSlice.h"
#include "mafViewArbitrarySlice.h"
#include "mafViewGlobalSliceCompound.h"
#include "mafViewSlice.h"
#include "mafViewImageCompound.h"
#include "mafViewIntGraph.h"
#include "medViewSlicer.h"

//--------------------------------------------------------------------------------
// Create the Application
//--------------------------------------------------------------------------------
IMPLEMENT_APP(lhpBuilderApp)

//--------------------------------------------------------------------------------
bool lhpBuilderApp::OnInit()
//--------------------------------------------------------------------------------
{
  mafPics.Initialize();	

  int result;
  
  result = medVMEFactory::Initialize();
  assert(result == MAF_OK);

	result = medPipeFactoryVME::Initialize();
	assert(result==MAF_OK);

  result = mafInteractionFactory::Initialize();
  assert(result==MAF_OK);

  mafPlugNode<mafVMEC3DData>("VME representing c3d motion data");
  mafPlugNode<mafVMERawMotionData>("VME representing raw motion data");
  mafPlugNode<mafVMEAFRefSys>("VME representing anatomical frame");
  mafPlugNode<mafVMEHelAxis>("VME representing helical axis");

  m_Logic = new lhpBuilderLogic();
  m_Logic->GetTopWin()->SetTitle("LHPBuilder");
  m_Logic->Configure();
  SetTopWindow(mafGetFrame());  

	wxRegKey RegKey(wxString("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\lhpBuilder"));
	if(RegKey.Exists())
	{
		RegKey.Create();
		wxString revision;
		RegKey.QueryValue(wxString("DisplayVersion"), revision);
		m_Logic->SetRevision(revision);
	}
	else
	{
		wxString revision="0.1";
		m_Logic->SetRevision(revision);
	}

  //------------------------- Importers -------------------------
  m_Logic->Plug(new mmoDICOMImporter("DICOM"));
  m_Logic->Plug(new mmoSTLImporter("STL"));
  m_Logic->Plug(new mmoVTKImporter("VTK"));
  m_Logic->Plug(new mafOpImporterMSF("MSF"));
  m_Logic->Plug(new mmoMSF1xImporter("MAF 1.x"));
  m_Logic->Plug(new mmoRAWImporterVolume("RAW Volume"));
  m_Logic->Plug(new mmoImageImporter("Images"));
	m_Logic->Plug(new mmoRAWImporterImages("Raw Images"));
  m_Logic->Plug(new mmoLandmarkImporter("Landmark"));
	m_Logic->Plug(new mmoLandmarkImporterWS("ASCII trajectories (VWs)"));
  m_Logic->Plug(new mmoMotionDataImporter<mafVMEC3DData>("C3D", "C3D Motion Data (*.c3d)|*.c3d", "Dictionary (*.txt)|*.txt"));
  m_Logic->Plug(new mmoMotionDataImporter<mafVMERawMotionData>("Raw Motion Data", "RAW Motion Data (*.MAN)|*.MAN", "Dictionary (*.txt)|*.txt"));
  // m_Logic->Plug(new mmoLandmarkImporter("Landmark")); //Old Importer
  m_Logic->Plug(new mmoEMGImporterWS("ASCII Analog (VWs)"));
  m_Logic->Plug(new mmoGRFImporterWS("ASCII Force Plates (VWs)"));
  m_Logic->Plug(new mmoMeshImporter("MESH"));	
  m_Logic->Plug(new mmoVRMLImporter("Geometry VRML "));
  m_Logic->Plug(new mmoINPImporter("Geometry INP/INP_AF "));
  m_Logic->Plug(new mmoMTRImporter("Geometry MTR "));
  m_Logic->Plug(new mafOpImporterExternalFile("External data"));

  //-------------------------------------------------------------

  //------------------------- Exporters -------------------------
  m_Logic->Plug(new mmoSTLExporter("STL"));
  m_Logic->Plug(new mmoINPExporter("INP"));
  m_Logic->Plug(new mmoMTRExporter("MTR"));
  m_Logic->Plug(new mmoVTKExporter("VTK"));
	m_Logic->Plug(new mmoRAWExporter("Raw"));
  m_Logic->Plug(new mmoBmpExporter("Bmp"));
  m_Logic->Plug(new mmoLandmarkExporter("Landmark"));
  m_Logic->Plug(new medOpExporterWrappedMeter("Wrapped Meter"));
  
  //-------------------------------------------------------------

  //------------------------- Operations -------------------------
  m_Logic->Plug(new mmoCreateGroup("Group"),"Create");
	m_Logic->Plug(new mmoCreateRefSys("Refsys"),"Create");
  m_Logic->Plug(new mmoCreateSlicer("Slicer"),"Create");
	m_Logic->Plug(new mmoCreateSurfaceParametric("Parametric Surface"),"Create");
	m_Logic->Plug(new mmoAddLandmark("Add Landmark \tCtrl+A"),"Create");
  m_Logic->Plug(new medOpFreezeVME("Freeze VME"),"Create");
  m_Logic->Plug(new mmoRegisterClusters("Register Landmark Cloud"),"Fuse");
  m_Logic->Plug(new mmoCreateMeter("Distance Meter"),"Derive");
  m_Logic->Plug(new medOpCreateWrappedMeter("Wrapped Meter"),"Derive");
  m_Logic->Plug(new mmoEditMetadata("Metadata Editor"),"Modify");
	m_Logic->Plug(new mmoFilterSurface("Filter Surface"),"Modify");
	m_Logic->Plug(new mmoExtractIsosurface("Extract Isosurface"),"Modify");
	m_Logic->Plug(new mmoCrop("Crop Volume"),"Modify");
	m_Logic->Plug(new medOpVolumeResample("Volume Resample"),"Modify");
	m_Logic->Plug(new mmo2DMeasure("2D Measure"),"Measure");
	m_Logic->Plug(new mmoVOIDensity("VOI Density"),"Measure");
  m_Logic->Plug(new mmoReparentTo("Reparent to...  \tCtrl+R"),"Fuse");
  m_Logic->Plug(new medOpScaleDataset("Scale Dataset"),"Modify");
  m_Logic->Plug(new medOpMove("Move"),"Modify");    
  m_Logic->Plug(new mmoVMEDataSetAttributesImporter("VME DataSet Attributes Importer"),"Modify");
  m_Logic->Plug(new mmoClassicICPRegistration("Register Surface"),"Fuse");
  m_Logic->Plug(new mmoAFSys("AFRefsys"),"Create");
  m_Logic->Plug(new mmoAverageLM("Average landmark"),"Create");
  m_Logic->Plug(new mmoStickPalpation("Wand palpated landmark"),"Create");
  m_Logic->Plug(new mmoHelAxis("Helical axis"),"Create");
  m_Logic->Plug(new mmoTimeReduce("Time reduce"),"Modify");
  m_Logic->Plug(new mmoBuildHierarchy("Make hierarchical"),"Fuse");
  m_Logic->Plug(new lhpOpBonemat("Bonemat"),"Modify");
  m_Logic->Plug(new medOpIterativeRegistration("Iterative Registration"),"Modify");
  m_Logic->Plug(new mafOpOpenExternalFile("Open with external program"),"Show");  
  
  
  
  //-------------------------------------------------------------

  //------------------------- Views -------------------------
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

  mafViewVTK *graph = new mafViewVTK("Analog Graph", CAMERA_PERSPECTIVE, false);
  graph->PlugVisualPipe("medVMEEmg", "medPipeGraph");
  m_Logic->Plug(graph);

  //View Global Slice
  mafViewGlobalSliceCompound *GlobalSlice = new mafViewGlobalSliceCompound("Global Slice");
  GlobalSlice->PackageView();
  m_Logic->Plug(GlobalSlice);

  mafViewVTK *viso = new mafViewVTK("Isosurface");
  viso->PlugVisualPipe("mafVMEVolumeGray", "mafPipeIsosurface",MUTEX);
  m_Logic->Plug(viso);

  mafViewOrthoSlice *viewOrthoSlice = new mafViewOrthoSlice("OrthoSlice");
  viewOrthoSlice->PackageView();
  m_Logic->Plug(viewOrthoSlice);

  mafViewRXCT *vrxctl = new mafViewRXCT("RXCT");
  vrxctl->PackageView();
  m_Logic->Plug(vrxctl);

	mafViewVTK *vsurface = new mafViewVTK("Surface");
	vsurface->PlugVisualPipe("mafVMESurface","mafPipeSurface");
	m_Logic->Plug(vsurface);

  mafViewIntGraph *vgraph = new mafViewIntGraph("Biomechanical graph");
  m_Logic->Plug(vgraph);

  medViewSlicer *slicerView = new medViewSlicer("Slicer");
  slicerView->PackageView();
  m_Logic->Plug(slicerView);

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

  //this hack is fixing VTK internal memory leak
  vtkTimerLog::CleanupLog();
  return 0;
}
