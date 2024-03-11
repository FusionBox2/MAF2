/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: BonematApp.cpp,v $
  Language:  C++
  Date:      $Date: 2009-05-15 10:01:19 $
  Version:   $Revision: 1.1.2.4 $
  Authors:   Roberto Mucci
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/


#include "medDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------
#include "BonematApp.h"

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
#include "medPipeFactoryVME.h"
#include "medVMEFactory.h"
#include "medPipeVolumeDRR.h"
#include "medPipeTrajectories.h" 
#include "mafVMEAFRefSys.h" 
#include "mafVMEHelAxis.h" 
//BES: 23.6.2008 - Large Volume - to be merged 
#include "mafVMEVolumeLarge.h"

#include "mafOpDecomposeTimeVarVME.h"
//#include "mafOpImporterMSF.h"
//#include "mafOpImporterExternalFile.h"
//#include "mafOpLabelExtractor.h"
//#include "mafOpOpenExternalFile.h"
//#include "mafOpExporterBmp.h"
//#include "mafOpCreateGroup.h"
//#include "mafOpCreateMeter.h"
//#include "medOpCreateWrappedMeter.h"
//#include "mafOpCreateSlicer.h"
//#include "mafOpCreateRefSys.h"
//#include "mafOpFilterSurface.h"
//#include "mafOpEditMetadata.h"
//#include "mafOp2DMeasure.h"
//#include "mafOpReparentTo.h"
#include "medOpImporterDicomOffis.h"
//#include "mafOpReparentTo.h"
//#include "mafOpImporterImage.h"
//#include "mafOpImporterSTL.h"
//#include "mafOpExporterSTL.h"
//#include "mafOpExporterVTK.h"
//#include "mafOpImporterVTK.h"
//#include "mafOpImporterMSF1x.h"
//#include "mafOpImporterVRML.h"
//BES: 23.6.2008 - Large Volume - to be merged 
#include "mafOpImporterRAWVolume_BES.h"
//#include "mafOpImporterRAWVolume.h"
//#include "mafOpExporterRaw.h"
#include "medOpImporterRAWImages.h"
//#include "mafOpExtractIsosurface.h"
//#include "mafOpCrop.h"
//#include "mafOpVOIDensity.h"
//#include "medOpVolumeResample.h"
//#include "mafOpAddLandmark.h"
//#include "medOpRegisterClusters.h"
//#include "mafOpCreateSurfaceParametric.h"
//#include "mmoBuildHierarchy.h"
//#include "mmoTimeReduce.h"
//#include "mmoINPExporter.h"
//#include "mmoMTRExporter.h"
//#include "mmoINPImporter.h"
//#include "mmoMTRImporter.h"
//#include "mmoLnSurf.h"
//#include "mmoAFSys.h"
//#include "mmoAverageLM.h"
//#include "mmoHelAxis.h"
//#include "mmoStickPalpation.h"
#include "medOpScaleDataset.h"
#include "medOpMove.h"
//#include "mafOpMAFTransform.h"
//#include "medOpImporterGRFWS.h"
//#include "medPipeGraph.h"
#include "mafVMERawMotionData.h" 
//#include "lhpOpImporterC3D.h" 
//#include "medOpImporterMotionData.h"
//#include "medOpExporterLandmark.h"
//#include "medOpExporterMeters.h"
//#include "medOpClassicICPRegistration.h"
#include "mafOpImporterMesh.h"
#include "mafOpImporterVMEDataSetAttributes.h"
//#include "medOpImporterLandmark.h"
//#include "medOpImporterLandmarkWS.h"
#include "lhpOpBonemat.h"
#include "lhpOpImporterAnsysInputFile.h"
#include "lhpOpExporterAnsysInputFile.h"
#include "lhpOpImporterAnsysCDBFile.h"
//#include "medOpFreezeVME.h"
//#include "medOpExporterWrappedMeter.h"
//#include "medOpIterativeRegistration.h"
//#include "medOpCreateLabeledVolume.h"
//#include "lhpOpUploadVME.h"
//#include "lhpOpUploadMultiVME.h"
//#include "lhpOpEditTag.h"
//#include "lhpOpKeyczarIntegrationTest.h"
//#include "lhpOpDownloadVME.h"
//#include "medOpSurfaceMirror.h"
//#include "medOpImporterAnalogWS.h"
//#include "medOpMML.h"
#include "mafViewVTK.h"
//#include "mafViewCompound.h"
//#include "mafViewRXCT.h"
//#include "mafViewRX.h"
#include "mafViewOrthoSlice.h"
#include "mafViewArbitrarySlice.h"
//#include "mafViewGlobalSliceCompound.h"
//#include "mafViewSlice.h"
//#include "mafViewImageCompound.h"
//#include "mafViewIntGraph.h"
//#include "medViewSlicer.h"
//#include "lhpOpMultiscaleExplore.h"

#include "mafOpValidateTree.h"

//#include "medOpImporterDicomXA.h"

//temporary for testing
#include "mafViewSingleSliceCompound.h"

#ifdef MAF_USE_ITK
#include "lhpOpCreateSurfaceScalar.h"
#include "lhpVMESurfaceScalarVarying.h"
#endif
#include "lhpVisualPipeSurfaceScalar.h"

#include <vtkTimerLog.h>


// TODO: REFACTOR THIS 
// this component is used only to override the Accept,  
// and it`s the minimal amount of code in order to override the method
// it could go in a separate file with other redefined Accept`s

class lhpOpMove : public medOpMove
{
public:

  /** Return true for the acceptable vme type. */
  bool Accept(mafNode* vme)
  {
    bool accepted = false;
    
    accepted =   !vme->IsA("lhpVMESurfaceScalarVarying") \
      && !vme->IsA("mafVMEMeter") \
      && !vme->IsA("medVMEWrappedMeter") \
      && !vme->IsA("medVMELabeledVolume") \
      && !vme->IsA("mafVMEHelicalAxis");  

    if (accepted == false)
    {
      return false;
    }
    else
    {
      return medOpMove::Accept(vme);
    }
  }
};

class lhpOpScaleDataset : public medOpScaleDataset
{
public:

  /** Return true for the acceptable vme type. */
  bool Accept(mafNode* vme)
  {
    bool accepted = false;

    accepted =   !vme->IsA("lhpVMESurfaceScalarVarying") \
      && !vme->IsA("mafVMEMeter") \
      && !vme->IsA("medVMEWrappedMeter") \
      && !vme->IsA("medVMELabeledVolume") \
      && !vme->IsA("mafVMEHelicalAxis");  

    if (accepted == false)
    {
      return false;
    }
    else
    {
      return medOpScaleDataset::Accept(vme);
    }
  }
};

// END TODO: REFACTOR THIS 


//BES: 23.6.2008 - TO BE UNCOMMENTED WHEN VTK IS PATCHED AS I SUGGESTED
////BES: 14.5.2008 - special memory manager for supporting of large data
//#include <vtkDataArrayMemMng.h>


//--------------------------------------------------------------------------------
// Create the Application
//--------------------------------------------------------------------------------

IMPLEMENT_APP(BonematApp)

////BES: 14.5.2008 - OnIdle to unlock blocks
//BEGIN_EVENT_TABLE(BonematApp, wxApp)
//  EVT_IDLE(BonematApp::OnIdle)
//END_EVENT_TABLE()


//--------------------------------------------------------------------------------
bool BonematApp::OnInit()
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
  #include "pic/lhpBuilder/FRAME_ICON16x16.xpm"
  mafADDPIC(FRAME_ICON16x16);
  
  #include "pic/lhpBuilder/FRAME_ICON32x32.xpm"
  mafADDPIC(FRAME_ICON32x32);
    
  #include "pic/lhpBuilder/MDICHILD_ICON.xpm"
  mafADDPIC(MDICHILD_ICON);

  int result;
 
  result = medVMEFactory::Initialize();
  assert(result == MAF_OK);

	result = medPipeFactoryVME::Initialize();
	assert(result==MAF_OK);

  result = mafInteractionFactory::Initialize();
  assert(result==MAF_OK);

  
  mafPlugNode<mafVMERawMotionData>("VME representing raw motion data");
  mafPlugNode<mafVMEAFRefSys>("VME representing anatomical frame");
  mafPlugNode<mafVMEHelAxis>("VME representing helical axis");
  mafPlugNode<mafVMEVolumeLarge>("VME storing large volume datasets with one scalar component");

#ifdef MAF_USE_ITK
  mafPlugNode<lhpVMESurfaceScalarVarying>("VME representing surface with attached time varying mafVMEScalar");
#endif

  mafPlugPipe<lhpVisualPipeSurfaceScalar>("Visual pipe to render a surface with its scalar values");

  m_Logic = new lhpBuilderLogic();
  m_Logic->GetTopWin()->SetTitle("Bonemat");
  m_Logic->Configure();
  SetTopWindow(mafGetFrame());  

	wxRegKey RegKey(wxString("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\psLoader"));
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
  m_Logic->Plug(new medOpImporterDicomOffis("DICOM"),"Images");
    // m_Logic->Plug(new medOpImporterDicomXA("DICOM XA"),"Images");
  //m_Logic->Plug(new mafOpImporterSTL("STL"),"Geometries");
  //m_Logic->Plug(new mafOpImporterVTK("VTK"),"Other");
  //m_Logic->Plug(new mafOpImporterMSF("MSF"),"Other");
  //m_Logic->Plug(new mafOpImporterMSF1x("MAF 1.x"),"Other");
    // m_Logic->Plug(new mafOpImporterBBF("BFF (VolumeLarge)"),"Other");
  m_Logic->Plug(new mafOpImporterRAWVolume_BES("Raw Volume"),"Images");
    // m_Logic->Plug(new mafOpImporterRAWVolume("Raw Volume Legacy"),"Images");
  m_Logic->Plug(new medOpImporterRAWImages("Raw Images"),"Images");
    // m_Logic->Plug(new medOpImporterRAWImages("Raw Images Legacy"),"Images");
  //m_Logic->Plug(new mafOpImporterImage("Images"),"Images");
  //m_Logic->Plug(new medOpImporterLandmark("Landmark"),"Motion Analysis");
	//m_Logic->Plug(new medOpImporterLandmarkWS("ASCII trajectories (VWs)"),"Motion Analysis");
  //m_Logic->Plug(new lhpOpImporterC3D("C3D"),"Motion Analysis");  
  //m_Logic->Plug(new medOpImporterMotionData<mafVMERawMotionData>("Raw Motion Data", "RAW Motion Data (*.MAN)|*.MAN", "Dictionary (*.txt)|*.txt"), "Motion Analysis");
  // m_Logic->Plug(new mmoLandmarkImporter("Landmark")); //Old Importer
  //m_Logic->Plug(new medOpImporterGRFWS("ASCII Force Plates (VWs)"), "Motion Analysis");
  m_Logic->Plug(new mafOpImporterMesh("Generic Mesh"), "Finite Element");
  m_Logic->Plug(new lhpOpImporterAnsysInputFile("Ansys Input File"), "Finite Element");	
  m_Logic->Plug(new lhpOpImporterAnsysCDBFile("Ansys CDB File"), "Finite Element");	
  //m_Logic->Plug(new mafOpImporterVRML("VRML"), "Geometries");
  //m_Logic->Plug(new mmoINPImporter("INP/INP_AF"), "Geometries");
  //m_Logic->Plug(new mmoMTRImporter("MTR"), "Geometries");
  //m_Logic->Plug(new mafOpImporterExternalFile("External data"), "Other");
  //m_Logic->Plug(new medOpImporterAnalogWS("ASCII Analog (VWs)"), "Motion Analysis");

  //-------------------------------------------------------------

  //------------------------- Exporters -------------------------
  //m_Logic->Plug(new mafOpExporterSTL("STL"),"Geometries");
  //m_Logic->Plug(new mmoINPExporter("INP"),"Geometries");
  //m_Logic->Plug(new mmoMTRExporter("MTR"), "Geometries");
  //m_Logic->Plug(new mafOpExporterVTK("VTK"), "Other");
	//m_Logic->Plug(new mafOpExporterRAW("Raw"), "Images");
  //m_Logic->Plug(new mafOpExporterBmp("Bmp"), "Images");
  //m_Logic->Plug(new medOpExporterLandmark("Landmark"), "Motion Analysis");
    // m_Logic->Plug(new medOpExporterWrappedMeter("Wrapped Meter"), "Other");
    // m_Logic->Plug(new medOpExporterMeters("Meters"), "Other");
  m_Logic->Plug(new lhpOpExporterAnsysInputFile("Ansys Input File"),"Finite Element");
  //-------------------------------------------------------------

  //------------------------- Operations -------------------------
//  m_Logic->Plug(new mafOpValidateTree());
  //m_Logic->Plug(new mafOpCreateGroup("Group"),"Create/New");
    // m_Logic->Plug(new lhpOpCreateSurfaceScalar("Surface Scalar"),"Create/Derive");
    // m_Logic->Plug(new mmoLnSurf("Lineset and surface"),"Create/Derive");
	//m_Logic->Plug(new mafOpCreateRefSys("Refsys"),"Create/New");
    // m_Logic->Plug(new mafOpCreateSlicer("Slicer"),"Create/Derive");
	//m_Logic->Plug(new mafOpCreateSurfaceParametric("Parametric Surface"),"Create/New");
	//m_Logic->Plug(new mafOpAddLandmark("Add Landmark \tCtrl+A"),"Create/New");
  //m_Logic->Plug(new medOpFreezeVME("Freeze VME"),"Create/Derive");
//  m_Logic->Plug(new medOpRegisterClusters("Register Landmark Cloud"),"Modify/Fuse");
    // m_Logic->Plug(new mafOpCreateMeter("Distance Meter"),"Create/Derive");
    // m_Logic->Plug(new medOpCreateWrappedMeter("Wrapped Meter"),"Create/Derive");
// // m_Logic->Plug(new mmoEditMetadata("Metadata Editor"),"Modify");
//	m_Logic->Plug(new mafOpFilterSurface("Filter Surface"),"Modify");
	  // m_Logic->Plug(new mafOpExtractIsosurface("Extract Isosurface"),"Create/Derive");
//  m_Logic->Plug(new medOpSurfaceMirror("Surface Mirror"),"Modify");
//	m_Logic->Plug(new mafOpCrop("Crop Volume"),"Modify");
//	m_Logic->Plug(new medOpVolumeResample("Volume Resample"),"Modify");
//	m_Logic->Plug(new mafOp2DMeasure("2D Measure"),"Measure");
//	m_Logic->Plug(new mafOpVOIDensity("VOI Density"),"Measure");
//  m_Logic->Plug(new mafOpReparentTo("Reparent to...  \tCtrl+R"),"Modify/Fuse");
//  m_Logic->Plug(new lhpOpScaleDataset(),"Modify");
    m_Logic->Plug(new lhpOpMove(),"Modify");    
    m_Logic->Plug(new mafOpImporterVMEDataSetAttributes("VME DataSet Attributes Adder"),"Modify");
//  m_Logic->Plug(new medOpClassicICPRegistration("Register Surface"),"Modify/Fuse");
    // m_Logic->Plug(new mmoAFSys("AFRefsys"),"Create/Derive");
    // m_Logic->Plug(new mmoAverageLM("Average landmark"),"Create/Derive");
    // m_Logic->Plug(new mmoStickPalpation("Wand palpated landmark"),"Create/Derive");
    // m_Logic->Plug(new mmoHelAxis("Helical axis"),"Create/Derive");
//  m_Logic->Plug(new mmoTimeReduce("Time reduce"),"Modify");
//  m_Logic->Plug(new mmoBuildHierarchy("Make hierarchical"),"Modify/Fuse");
    m_Logic->Plug(new lhpOpBonemat("Bonemat"),"Modify");
//  m_Logic->Plug(new medOpIterativeRegistration("Iterative Registration"),"Modify/Fuse");
  //m_Logic->Plug(new mafOpOpenExternalFile("Open with external program"),"Manage"); 
    // m_Logic->Plug(new medOpCreateLabeledVolume("Labeled Volume"),"Create/Derive");
//  m_Logic->Plug(new lhpOpUploadVME("Upload VME"),"Manage");
  //m_Logic->Plug(new lhpOpUploadMultiVME("Upload VME"),"Manage");
  //m_Logic->Plug(new lhpOpEditTag("Edit Tag VME"),"Manage");
  //m_Logic->Plug(new lhpOpDownloadVME("Download VME"),"Manage");
//  
  //m_Logic->Plug(new mafOpDecomposeTimeVarVME("Decompose Time"),"Create/Derive");
    // m_Logic->Plug(new mafOpLabelExtractor("Extract Label"),"Create/Derive");
//  m_Logic->Plug(new lhpOpMultiscaleExplore("Multiscale Viewer"),"Manage");
//  m_Logic->Plug(new medOpMML("Register from template"),"Modify");
//  m_Logic->Plug(new lhpOpKeyczarIntegrationTest("Security Libraries Integration"),"Test");
//
//  
//  
  //-------------------------------------------------------------

  //------------------------- Views -------------------------
  //View Arbitrary Slice
  mafViewArbitrarySlice *ArbitraryView = new mafViewArbitrarySlice("Arbitrary");
  ArbitraryView->PackageView();
  m_Logic->Plug(ArbitraryView);


  // View DRR
  //mafViewVTK *vdrr = new mafViewVTK("DRR");
  //vdrr->PlugVisualPipe("mafVMEVolumeGray","medPipeVolumeDRR",MUTEX);
  //vdrr->PlugVisualPipe("mafVMEVolumeLarge","medPipeVolumeDRR",MUTEX);
  //m_Logic->Plug(vdrr);

  // View Analog graph
  /*mafViewVTK *graph = new mafViewVTK("Analog Graph", CAMERA_PERSPECTIVE, false);
  graph->PlugVisualPipe("medVMEAnalog", "medPipeGraph",MUTEX);
  m_Logic->Plug(graph);*/

  //View Global Slice
  //mafViewGlobalSliceCompound *GlobalSlice = new mafViewGlobalSliceCompound("Global Slice");
  //GlobalSlice->PackageView();
  //m_Logic->Plug(GlobalSlice);

  /*mafViewVTK *viso = new mafViewVTK("Isosurface");
  viso->PlugVisualPipe("mafVMEVolumeGray", "mafPipeIsosurface",MUTEX);
  viso->PlugVisualPipe("medVMELabeledVolume", "mafPipeIsosurface",MUTEX);
  viso->PlugVisualPipe("mafVMEVolumeLarge","mafPipeIsosurface",MUTEX);
  m_Logic->Plug(viso);*/

  mafViewOrthoSlice *viewOrthoSlice = new mafViewOrthoSlice("OrthoSlice");
  viewOrthoSlice->PackageView();
  m_Logic->Plug(viewOrthoSlice);

  /*mafViewRXCT *vrxctl = new mafViewRXCT("RXCT");
  vrxctl->PackageView();
  m_Logic->Plug(vrxctl);*/

	mafViewVTK *vsurface = new mafViewVTK("Surface");
	vsurface->PlugVisualPipe("mafVMESurface","mafPipeSurface");
  vsurface->PlugVisualPipe("mafVMELandmark", "medPipeTrajectories");
	m_Logic->Plug(vsurface);

  //mafViewIntGraph *vgraph = new mafViewIntGraph("Biomechanical graph");
  //m_Logic->Plug(vgraph);

 /* medViewSlicer *slicerView = new medViewSlicer("Slicer");
  slicerView->PackageView();
  m_Logic->Plug(slicerView);*/

  //temporary for testing
  //mafViewSingleSliceCompound *sliceView = new mafViewSingleSliceCompound("Test Slice");
  //sliceView->PackageView();
  //m_Logic->Plug(sliceView);

  //-------------------------------------------------------------

  wxBitmap splashBitmap;
  splashBitmap.LoadFile("../Splash/BonematSplash.bmp", wxBITMAP_TYPE_BMP);
  m_Logic->ShowSplashScreen(splashBitmap); 

  // show the application
	m_Logic->ShowSplashScreen(splashBitmap);
  m_Logic->Show();
  m_Logic->GetCredentials();

  m_Logic->Init(0,NULL); // calls FileNew - which create the root

  return TRUE;
}
//--------------------------------------------------------------------------------
int BonematApp::OnExit()
//--------------------------------------------------------------------------------
{
  cppDEL(m_Logic);

  //this hack is fixing VTK internal memory leak
  vtkTimerLog::CleanupLog();
  return 0;
}

//void BonematApp::OnIdle( wxIdleEvent &event )
//{ 
//  vtkDataArrayMemMng::GetDataArrayMemMng()->UnlockAllMemory(0);
//  event.Skip();
//}
