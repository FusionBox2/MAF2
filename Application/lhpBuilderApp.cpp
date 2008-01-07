/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpBuilderApp.cpp,v $
  Language:  C++
  Date:      $Date: 2008-01-07 10:31:11 $
  Version:   $Revision: 1.33 $
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

#include "lhpBuilderApp.h"

#include <wx/datetime.h>
#include <wx/config.h>

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

#include "mafOpDecomposeTimeVarVME.h"
#include "mafOpImporterMSF.h"
#include "mafOpImporterExternalFile.h"
#include "mafOpLabelExtractor.h"
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
#include "mmoLnSurf.h"
#include "mmoAFSys.h"
#include "mmoAverageLM.h"
#include "mmoHelAxis.h"
#include "mmoStickPalpation.h"
#include "medOpScaleDataset.h"
#include "medOpMove.h"
#include "mmoMAFTransform.h"
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
#include "lhpOpImporterAnsysInputFile.h"
#include "medOpFreezeVME.h"
#include "medOpExporterWrappedMeter.h"
#include "medOpIterativeRegistration.h"
#include "medOpCreateLabeledVolume.h"
#include "lhpOpUploadVME.h"
#include "medOpSurfaceMirror.h"
#include "medOpImporterEmgWS.h"

#include "mafViewVTK.h"
#include "mafViewCompound.h"
#include "mafViewRXCT.h"
#include "mafViewRX.h"
#include "mafViewOrthoSlice.h"
#include "mafViewArbitrarySlice.h"
#include "mafViewGlobalSliceCompound.h"
#include "mafViewSlice.h"
#include "mafViewImageCompound.h"
#include "medViewEmgGraph.h"
#include "mafViewIntGraph.h"
#include "medViewSlicer.h"
#include "lhpOpMultiscaleExplore.h"

//temporary for testing
#include "mafViewSingleSliceCompound.h"

#include <vtkTimerLog.h>

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
  m_Logic->Plug(new mmoDICOMImporter("DICOM"),"Images");
  m_Logic->Plug(new mmoSTLImporter("STL"),"Geometries");
  m_Logic->Plug(new mmoVTKImporter("VTK"),"Other");
  m_Logic->Plug(new mafOpImporterMSF("MSF"),"Other");
  m_Logic->Plug(new mmoMSF1xImporter("MAF 1.x"),"Other");
  m_Logic->Plug(new mmoRAWImporterVolume("RAW Volume"),"Images");
  m_Logic->Plug(new mmoRAWImporterImages("Raw Images"),"Images");
  m_Logic->Plug(new mmoImageImporter("Images"),"Images");
  m_Logic->Plug(new mmoLandmarkImporter("Landmark"),"Motion Analysis");
	m_Logic->Plug(new mmoLandmarkImporterWS("ASCII trajectories (VWs)"),"Motion Analysis");
  m_Logic->Plug(new mmoMotionDataImporter<mafVMEC3DData>("C3D", "C3D Motion Data (*.c3d)|*.c3d", "Dictionary (*.txt)|*.txt"),"Motion Analysis");
  m_Logic->Plug(new mmoMotionDataImporter<mafVMERawMotionData>("Raw Motion Data", "RAW Motion Data (*.MAN)|*.MAN", "Dictionary (*.txt)|*.txt"), "Motion Analysis");
  // m_Logic->Plug(new mmoLandmarkImporter("Landmark")); //Old Importer
  m_Logic->Plug(new mmoGRFImporterWS("ASCII Force Plates (VWs)"), "Motion Analysis");
  m_Logic->Plug(new mmoMeshImporter("Generic Mesh"), "Finite Element");
  m_Logic->Plug(new lhpOpImporterAnsysInputFile("Ansys Input File"), "Finite Element");	
  m_Logic->Plug(new mmoVRMLImporter("Geometry VRML"), "Geometries");
  m_Logic->Plug(new mmoINPImporter("Geometry INP/INP_AF"), "Geometries");
  m_Logic->Plug(new mmoMTRImporter("Geometry MTR"), "Geometries");
  m_Logic->Plug(new mafOpImporterExternalFile("External data"), "Other");
  m_Logic->Plug(new medOpImporterEmgWS("ASCII Analog (VWs)"), "Motion Analysis");

  //-------------------------------------------------------------

  //------------------------- Exporters -------------------------
  m_Logic->Plug(new mmoSTLExporter("STL"),"Geometries");
  m_Logic->Plug(new mmoINPExporter("INP"),"Geometries");
  m_Logic->Plug(new mmoMTRExporter("MTR"), "Geometries");
  m_Logic->Plug(new mmoVTKExporter("VTK"), "Other");
	m_Logic->Plug(new mmoRAWExporter("Raw"), "Images");
  m_Logic->Plug(new mmoBmpExporter("Bmp"), "Images");
  m_Logic->Plug(new mmoLandmarkExporter("Landmark"), "Motion Analysis");
  m_Logic->Plug(new medOpExporterWrappedMeter("Wrapped Meter"), "Other");
  
  //-------------------------------------------------------------

  //------------------------- Operations -------------------------
  m_Logic->Plug(new mmoCreateGroup("Group"),"Create/New");
  m_Logic->Plug(new mmoLnSurf("Lineset and surface"),"Create/Derive");
	m_Logic->Plug(new mmoCreateRefSys("Refsys"),"Create/New");
  m_Logic->Plug(new mmoCreateSlicer("Slicer"),"Create/Derive");
	m_Logic->Plug(new mmoCreateSurfaceParametric("Parametric Surface"),"Create/New");
	m_Logic->Plug(new mmoAddLandmark("Add Landmark \tCtrl+A"),"Create/New");
  m_Logic->Plug(new medOpFreezeVME("Freeze VME"),"Create/Derive");
  m_Logic->Plug(new mmoRegisterClusters("Register Landmark Cloud"),"Modify/Fuse");
  m_Logic->Plug(new mmoCreateMeter("Distance Meter"),"Create/Derive");
  m_Logic->Plug(new medOpCreateWrappedMeter("Wrapped Meter"),"Create/Derive");
 // m_Logic->Plug(new mmoEditMetadata("Metadata Editor"),"Modify");
	m_Logic->Plug(new mmoFilterSurface("Filter Surface"),"Modify");
	m_Logic->Plug(new mmoExtractIsosurface("Extract Isosurface"),"Create/Derive");
  m_Logic->Plug(new medOpSurfaceMirror("Surface Mirror"),"Modify");
	m_Logic->Plug(new mmoCrop("Crop Volume"),"Modify");
	m_Logic->Plug(new medOpVolumeResample("Volume Resample"),"Modify");
	m_Logic->Plug(new mmo2DMeasure("2D Measure"),"Measure");
	m_Logic->Plug(new mmoVOIDensity("VOI Density"),"Measure");
  m_Logic->Plug(new mmoReparentTo("Reparent to...  \tCtrl+R"),"Modify/Fuse");
  m_Logic->Plug(new medOpScaleDataset("Scale Dataset"),"Modify");
  m_Logic->Plug(new medOpMove(),"Modify");    
  m_Logic->Plug(new mmoVMEDataSetAttributesImporter("VME DataSet Attributes Adder"),"Modify");
  m_Logic->Plug(new mmoClassicICPRegistration("Register Surface"),"Modify/Fuse");
  m_Logic->Plug(new mmoAFSys("AFRefsys"),"Create/Derive");
  m_Logic->Plug(new mmoAverageLM("Average landmark"),"Create/Derive");
  m_Logic->Plug(new mmoStickPalpation("Wand palpated landmark"),"Create/Derive");
  m_Logic->Plug(new mmoHelAxis("Helical axis"),"Create/Derive");
  m_Logic->Plug(new mmoTimeReduce("Time reduce"),"Modify");
  m_Logic->Plug(new mmoBuildHierarchy("Make hierarchical"),"Modify/Fuse");
  m_Logic->Plug(new lhpOpBonemat("Bonemat"),"Modify");
  m_Logic->Plug(new medOpIterativeRegistration("Iterative Registration"),"Modify/Fuse");
  m_Logic->Plug(new mafOpOpenExternalFile("Open with external program"),"Manage"); 
  m_Logic->Plug(new medOpCreateLabeledVolume("Labeled Volume"),"Create/Derive");
  m_Logic->Plug(new lhpOpUploadVME("Upload VME"),"Manage");
  m_Logic->Plug(new mafOpDecomposeTimeVarVME("Decompose Time"),"Create/Derive");
  m_Logic->Plug(new mafOpLabelExtractor("Extract Label"),"Create/Derive");
  m_Logic->Plug(new lhpOpMultiscaleExplore("Multiscale Viewer"),"Manage");

  
  
  //-------------------------------------------------------------

  //------------------------- Views -------------------------
  //View Arbitrary Slice
  mafViewArbitrarySlice *ArbitraryView = new mafViewArbitrarySlice("Arbitrary");
  ArbitraryView->PackageView();
  m_Logic->Plug(ArbitraryView);


  // View DRR
  mafViewVTK *vdrr = new mafViewVTK("DRR");
  vdrr->PlugVisualPipe("mafVMEVolumeGray","medPipeVolumeDRR",MUTEX);
  m_Logic->Plug(vdrr);

  // View Analog graph
  medViewEmgGraph *graph = new medViewEmgGraph("Analog Graph");
  graph->PlugVisualPipe("mafVMEScalar", "medPipeGraph");
  m_Logic->Plug(graph);

  //View Global Slice
  mafViewGlobalSliceCompound *GlobalSlice = new mafViewGlobalSliceCompound("Global Slice");
  GlobalSlice->PackageView();
  m_Logic->Plug(GlobalSlice);

  mafViewVTK *viso = new mafViewVTK("Isosurface");
  viso->PlugVisualPipe("mafVMEVolumeGray", "mafPipeIsosurface",MUTEX);
  viso->PlugVisualPipe("medVMELabeledVolume", "mafPipeIsosurface",MUTEX);
  m_Logic->Plug(viso);

  mafViewOrthoSlice *viewOrthoSlice = new mafViewOrthoSlice("OrthoSlice");
  viewOrthoSlice->PackageView();
  m_Logic->Plug(viewOrthoSlice);

  mafViewRXCT *vrxctl = new mafViewRXCT("RXCT");
  vrxctl->PackageView();
  m_Logic->Plug(vrxctl);

	mafViewVTK *vsurface = new mafViewVTK("Surface");
	vsurface->PlugVisualPipe("mafVMESurface","mafPipeSurface");
  vsurface->PlugVisualPipe("mafVMELandmark", "medPipeTrajectories");
	m_Logic->Plug(vsurface);

  mafViewIntGraph *vgraph = new mafViewIntGraph("Biomechanical graph");
  m_Logic->Plug(vgraph);

  medViewSlicer *slicerView = new medViewSlicer("Slicer");
  slicerView->PackageView();
  m_Logic->Plug(slicerView);

  //temporary for testing
  //mafViewSingleSliceCompound *sliceView = new mafViewSingleSliceCompound("Test Slice");
  //sliceView->PackageView();
  //m_Logic->Plug(sliceView);

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
