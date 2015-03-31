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


#include "medDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------
#include "lhpFusionBoxApp.h"

#include <wx/datetime.h>
#include <wx/config.h>

#include "lhpBuilderDecl.h"
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
#include "mafOpSelect.h"
#include "mafOpDecomposeTimeVarVME.h"
#include "mafOpImporterMSF.h"
#include "mafOpImporterExternalFile.h"
#include "mafOpLabelExtractor.h"
#include "mafOpOpenExternalFile.h"
#include "mafOpExporterBmp.h"
#include "mafOpCreateGroup.h"
#include "mafOpCreateMeter.h"
#include "medOpCreateWrappedMeter.h"
#include "mafOpCreateSlicer.h"
#include "mafOpCreateRefSys.h"
#include "mafOpFilterSurface.h"
#include "mafOpEditMetadata.h"
#include "mafOp2DMeasure.h"
#include "mafOpReparentTo.h"
#include "medOpImporterDicomOffis.h"
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
#include "mafOpImporterBBF.h"
//BES: 23.6.2008 - Large Volume - to be merged 
#include "mafOpImporterRAWVolume_BES.h"
#include "mafOpImporterRAWVolume.h"
#include "lhpOpKinectUtil.h"
#include "lhpOpKinectModel.h"
#include "lhpOpKinectAFs.h"
#include "mafOpExporterRaw.h"
#include "medOpImporterRAWImages.h"
#include "mafOpExtractIsosurface.h"
#include "mafOpCrop.h"
#include "mafOpVOIDensity.h"
#include "medOpVolumeResample.h"
#include "mafOpAddLandmark.h"
#include "medOpRegisterClusters.h"
#include "lhpOpFuseLMScripted.h"
#include "lhpOpRegisterLMScripted.h"
#include "mafOpCreateSurfaceParametric.h"
#include "lhpOpBuildHierarchy.h"
#include "lhpOpTimeReduce.h"
#include "lhpOpTimeShift.h"
#include "lhpOpINPExporter.h"
#include "lhpOpMTRExporter.h"
#include "lhpOpINPImporter.h"
#include "lhpOpMTRImporter.h"
#include "lhpOpLnSurf.h"
#include "lhpOpAFSys.h"
#include "lhpOpAverageLM.h"
#include "lhpOpHelAxis.h"
#include "lhpOpStickPalpation.h"
#include "medOpScaleDataset.h"
#include "medOpMove.h"
//#include "mafOpMAFTransform.h"
#include "medOpImporterGRFWS.h"
#include "medPipeGraph.h"
#include "mafVMERawMotionData.h" 
#include "lhpOpImporterC3D.h" 
#include "medOpImporterMotionData.h"
#include "medOpExporterLandmark.h"
#include "medOpExporterMeters.h"
#include "medOpClassicICPRegistration.h"
#include "mafOpImporterMesh.h"
#include "mafOpImporterVMEDataSetAttributes.h"
#include "medOpImporterLandmark.h"
#include "medOpImporterLandmarkWS.h"
#include "lhpOpBonemat.h"
#include "lhpOpImporterAnsysInputFile.h"
#include "lhpOpExporterAnsysInputFile.h"
#include "lhpOpImporterAnsysCDBFile.h"
#include "medOpFreezeVME.h"
#include "medOpExporterWrappedMeter.h"
#include "medOpIterativeRegistration.h"
#include "medOpCreateLabeledVolume.h"
#include "lhpOpUploadVME.h"
#include "lhpOpUploadMultiVME.h"
#include "lhpOpEditTag.h"
#include "lhpOpKeyczarIntegrationTest.h"
#include "lhpOpDownloadVME.h"
#include "medOpSurfaceMirror.h"
#include "medOpImporterAnalogWS.h"
#include "medOpMML.h"
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
#include "lhpOpMultiscaleExplore.h"
#include "lhpOpTextureOrientation.h"
#include "lhpOpComputeTensor.h"
#include "medOpCropDeformableROI.h"
#include "mafOpValidateTree.h"
#include "mafOpApplyTrajectory.h"

#include "medOpComputeWrapping.h"
#include "medVMEComputeWrapping.h"
#include "medPipeComputeWrapping.h"
#include "medGUIDicomSettings.h"

//temporary for testing
#include "mafViewSingleSliceCompound.h"

// VMEUploaderDownloader Refactor Target
#include "lhpOpUploadVMERefactor.h"
#include "lhpOpUploadMultiVMERefactor.h"
#include "lhpOpEditTagRefactor.h"
#include "lhpOpDownloadVMERefactor.h"

#include "lhpOpFindCentroid.h"

#ifdef MAF_USE_ITK
#include "lhpOpCreateSurfaceScalar.h"
#include "lhpVMESurfaceScalarVarying.h"
#endif
#include "lhpVisualPipeSurfaceScalar.h"

//BES: 14.11.2008 - added muscle wrapping
#include "medOpCreateMuscleWrapper.h"
#include "medVMEMuscleWrapper.h"
#include "medOpMeshDeformation.h"

#include <vtkTimerLog.h>

#include "lhpPipeIntGraph.h"
#include "lhpPipeIntGraphHAxis.h"
#include "lhpPipeIntGraphPolyline.h"
#include "lhpPipeIntGraphAnalog.h"
#include "lhpPipeLeverArm.h"
#include "lhpVMELMCLines.h" 
#include "mafVMEArrow.h" 
#include "mafVMEC3DData.h" 
#include "mafVMEPGDData.h" 
#include "mafVMEBSplineLine.h"
#include "mafVMEBSplineSurface.h"
#include "mafVMEBSplineVolume.h"
#include "lhpVMELeverArm.h"
#include "mafVMESurfaceRegParam.h"
#include "lhpOpFingerStick.h"
#include "lhpOpMTRULBImporter.h"
#include "lhpOpSoftReg.h"
#include "lhpOpRegression.h"
#include "lhpOpCreateObject.h"
#include "lhpOpRegistration.h"
#include "lhpOpRegSurfWithCloud.h"
#include "lhpOpRepresentInAF.h"
#include "lhpOpImporterC3DBTK.h" 
#include "lhpOpExporterC3DBTK.h" 
#include "lhpOpLMMirror.h"
#include "lhpOpLMProj.h"
#include "lhpOpSolidify.h"
#include "lhpOpJoinSurf.h"
#include "lhpOpMergeClouds.h"
#include "lhpOpICPRegFollow.h"
#include "lhpOpImporterRSScan.h"
#include "lhpOpCutSurface.h"
#include "vtkUnstructuredGrid.h"
#include "vtkMAFSmartPointer.h"
#include "vtkTransformFilter.h"
#include "vtkStructuredPoints.h"


// TODO: REFACTOR THIS 
// this component is used only to override the Accept,  
// and it`s the minimal amount of code in order to override the method
// it could go in a separate file with other redefined Accept`s

class lhpOpMove : public medOpMove
{
public:
  mafTypeMacro(lhpOpMove, medOpMove)

  lhpOpMove(const mafString& label = "Move\tCtrl+T"):Superclass(label){}
  //----------------------------------------------------------------------------
  mafOp* Copy()   
    //----------------------------------------------------------------------------
  {
    return new lhpOpMove(GetLabel());
  }

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

class lhpOpMoveSeq : public lhpOpMove
{
public:
  mafTypeMacro(lhpOpMoveSeq, lhpOpMove)
  lhpOpMoveSeq(const mafString& label = "Move Sequence"):Superclass(label){m_EnableScaling = 0;}
  mafOp* Copy()   
    //----------------------------------------------------------------------------
  {
    return new lhpOpMoveSeq(GetLabel());
  }
  void OpDo();
  void OpUndo();
private:
  void TransfMatr(mafMatrix& convMatrix, mafTimeStamp tsSkip = -1);
  mafMatrix m_ConvMatrix;
};



class lhpOpCreateLMCLines: public mafOp
{
public:
  mafTypeMacro(lhpOpCreateLMCLines, mafOp);
  lhpOpCreateLMCLines(const mafString& label = "CreateObject"):Superclass(label)
  {
    m_OpType  = OPTYPE_OP;
    m_Canundo = true;
    m_Created = NULL;
  }
  ~lhpOpCreateLMCLines() 
  {
    mafDEL(m_Created);
  }

  mafOp* Copy(){return new lhpOpCreateLMCLines(GetLabel());}

  bool Accept(mafNode *node){return (node != NULL);}
  void OpRun()
  {
    mafNEW(m_Created);
    m_Created->SetName("Cloud lines");
    m_Output = m_Created;
    mafEventMacro(mafEvent(this,OP_RUN_OK));
  }
  void OpDo();
  void OpUndo();

protected: 
  lhpVMELMCLines *m_Created;
};

mafCxxTypeMacro(lhpOpMove)
mafCxxTypeMacro(lhpOpMoveSeq)
mafCxxTypeMacro(lhpOpCreateLMCLines);

void lhpOpCreateLMCLines::OpDo()
{
  m_Output->ReparentTo(m_Input);
  mafVMELandmarkCloud *lmc = mafVMELandmarkCloud::SafeDownCast(m_Input);
  if(m_Created && lmc)
  {
    m_Created->SetCloud(lmc);
  }
}
void lhpOpCreateLMCLines::OpUndo()
{
  if(m_Created)
    m_Created->SetCloud(NULL);
  m_Output->ReparentTo(NULL);
}
//----------------------------------------------------------------------------
void lhpOpMoveSeq::TransfMatr(mafMatrix& convMatrix, mafTimeStamp tsSkip)
//----------------------------------------------------------------------------
{
  mafMatrix newMatr;
  mafMatrix oldMatr;
  std::vector<mafTimeStamp> stamps;
  ((mafVME *)m_Input)->GetTimeStamps(stamps);
  for(int i = 0; i < stamps.size(); i++)
  {
    if(stamps[i] == tsSkip)
      continue;
    // apply roto-translation to abs pose
    ((mafVME *)m_Input)->GetOutput()->GetAbsMatrix(oldMatr, stamps[i]);
    mafMatrix::Multiply4x4(convMatrix, oldMatr, newMatr);
    ((mafVME *)m_Input)->SetAbsMatrix(newMatr, stamps[i]);
  }
  ((mafVME *)m_Input)->GetOutput()->Update();
  mafEventMacro(mafEvent(this, CAMERA_UPDATE));
}
void lhpOpMoveSeq::OpDo()
//----------------------------------------------------------------------------
{
  mafMatrix newMatr;
  mafMatrix oldMatr;
  mafMatrix convMatrix;
  newMatr = m_NewAbsMatrix;
  oldMatr = m_OldAbsMatrix;
  oldMatr.Invert();
  mafMatrix::Multiply4x4(newMatr, oldMatr, convMatrix);
  m_ConvMatrix = convMatrix;
  //((mafVME *)m_Input)->SetAbsMatrix(oldMatr);
  TransfMatr(convMatrix, ((mafVME *)m_Input)->GetTimeStamp());
}
void lhpOpMoveSeq::OpUndo()
//----------------------------------------------------------------------------
{
  mafMatrix convMatrix;
  convMatrix = m_ConvMatrix;
  convMatrix.Invert();
  TransfMatr(convMatrix);
}

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

IMPLEMENT_APP(lhpFusionBoxApp)

////BES: 14.5.2008 - OnIdle to unlock blocks
//BEGIN_EVENT_TABLE(lhpFusionBoxApp, wxApp)
//  EVT_IDLE(lhpFusionBoxApp::OnIdle)
//END_EVENT_TABLE()
#ifdef FULL_COMMON_FB

//--------------------------------------------------------------------------------
bool lhpFusionBoxApp::OnInit()
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

  LibHandle C3DLib      = NULL;
  LibHandle matlabLib   = NULL;
  LibHandle fullControl = NULL;
  bool fullVersion      = false;

  fullControl = mafDynamicLoader::OpenLibrary("full_ver");
  if(fullControl)
  {
    unsigned (*fnfull_ver)() = (unsigned(*)())mafDynamicLoader::GetSymbolAddress(fullControl, "fnfull_ver");
    if(fnfull_ver != NULL && fnfull_ver() > 0)
    {
      fullVersion = true;
    }
    m_Plugins.push_back(std::make_pair(fullControl, (void(*)())NULL));
  }

  matlabLib = mafDynamicLoader::OpenLibrary("MATLAB");
  if(matlabLib)
  {
    bool (*minit)() = (bool(*)())mafDynamicLoader::GetSymbolAddress(matlabLib, "Init");
    if(minit == NULL || !minit())
    {
      mafDynamicLoader::CloseLibrary(matlabLib);
      matlabLib = NULL;
    }
    else
    {
      void (*mterm)() = (void(*)())mafDynamicLoader::GetSymbolAddress(matlabLib, "Terminate");
      if(lhpOpRegression::Config(matlabLib))
      {
        m_Plugins.push_back(std::make_pair(matlabLib, mterm));
      }
      else
      {
        if(mterm)
          mterm();
        mafDynamicLoader::CloseLibrary(matlabLib);
        matlabLib = NULL;
      }
    }
  }

  if(fullVersion)
  {
    C3DLib = mafDynamicLoader::OpenLibrary("C3D_Reader");
    if(C3DLib)
    {
      if(lhpOpImporterC3D::Config(C3DLib))
      {
        m_Plugins.push_back(std::make_pair(C3DLib, (void(*)())NULL));
      }
      else
      {
        mafDynamicLoader::CloseLibrary(C3DLib);
        C3DLib = NULL;
      }
    }
  }

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
  mafPlugNode<mafVMEC3DData>("VME representing C3D data");
  mafPlugNode<mafVMEPGDData>("VME representing PGD data");
  mafPlugNode<mafVMEArrow>("VME representing helical axis");
  mafPlugNode<lhpVMELeverArm>("VME representing lever arm");
  mafPlugNode<lhpVMELMCLines>("VME representing lines between landmarks of cloud");
  mafPlugNode<mafVMEBSplineLine>("VME representing B-spline line");
  mafPlugNode<mafVMEBSplineSurface>("VME representing B-spline surface");
  mafPlugNode<mafVMEBSplineVolume>("VME representing B-spline volume");
  mafPlugNode<mafVMESurfaceRegParam>("VME representing regression parametric surface");
  mafPlugNode<mafVMEVolumeLarge>("VME storing large volume datasets with one scalar component");

mafPlugNode<medVMEComputeWrapping>("Generalized another VME Meter with wrapping geometry");
mafPlugPipe<medPipeComputeWrapping>("Pipe to Visualize Compute Wrapping Meter");

#ifdef MAF_USE_ITK
  mafPlugNode<lhpVMESurfaceScalarVarying>("VME representing surface with attached time varying mafVMEScalar");
#endif

  //BES: 14.11.2008 - some stupid VME to demonstrate muscle wrapping
  mafPlugNode<medVMEMuscleWrapper>("Procedural VME representing muscle deformed according to its action lines");

  mafPlugPipe<lhpVisualPipeSurfaceScalar>("Visual pipe to render a surface with its scalar values");
  mafPlugPipe<lhpPipeIntGraph>("Visual pipe for biomechanical graph");
  mafPlugPipe<lhpPipeIntGraphHAxis>("Visual pipe for helical axis in a biomechanical graph");
  mafPlugPipe<lhpPipeIntGraphPolyline>("Visual pipe for polyline in a biomechanical graph");
  mafPlugPipe<lhpPipeIntGraphAnalog>("Visual pipe for analog data in a biomechanical graph");
  mafPlugPipe<lhpPipeLeverArm>("Visual pipe for lever arm");

  m_Logic = new lhpBuilderLogic();
  if(fullVersion)
  {
    m_Logic->GetTopWin()->SetTitle("LHPFusionBox");
  }
  else
  {
    m_Logic->GetTopWin()->SetTitle("ULBViewer");
  }
  m_Logic->Configure();
  SetTopWindow(mafGetFrame());  

  wxString regKeyName;
  if(fullVersion)
    regKeyName = "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\lhpFusionBox";
  else
    regKeyName = "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\ULBViewer";
  wxRegKey RegKey(regKeyName);
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
  //------------------------- Editors -------------------------
  m_Logic->Plug(new mafOpDelete(_("Delete   \tCtrl+Shift+D")), "");
  m_Logic->Plug(new mafOpCut(_("Cut   \tCtrl+Shift+X")), "");
  m_Logic->Plug(new mafOpCopy(_("Copy  \tCtrl+Shift+C")), "");
  m_Logic->Plug(new mafOpPaste(_("Paste \tCtrl+Shift+V")), "");
  //------------------------- Importers -------------------------
  m_Logic->Plug(new mafOpImporterSTL("STL"),"Geometries");
  m_Logic->Plug(new mafOpImporterMSF("MSF"),"Other");
  m_Logic->Plug(new medOpImporterLandmark("Landmark"),"Motion Analysis");
  m_Logic->Plug(new medOpImporterMotionData<mafVMERawMotionData>("Raw Motion Data", "RAW Motion Data (*.MAN)|*.MAN", "Dictionary (*.txt)|*.txt"), "Motion Analysis");
  m_Logic->Plug(new medOpImporterMotionData<mafVMEPGDData>("PGD Data", "PGD Data (*.PGD)|*.PGD", "Dictionary (*.txt)|*.txt"), "Motion Analysis");
  m_Logic->Plug(new mafOpImporterVRML("VRML"), "Geometries");
  m_Logic->Plug(new lhpOpINPImporter("INP/INP_AF"), "Geometries");
  m_Logic->Plug(new lhpOpMTRImporter("MTR"), "Geometries");
  m_Logic->Plug(new lhpOpMTRULBImporter("MTR (ULB)"), "Geometries");
  m_Logic->Plug(new lhpOpImporterC3DBTK("C3D BTK"),"Motion Analysis");  
  if(fullVersion)
  {
    medGUIDicomSettings *dicomSettings=new medGUIDicomSettings(NULL,"DICOM");
    medOpImporterDicomOffis *dimp = new medOpImporterDicomOffis("DICOM");
    dimp->SetSetting(dicomSettings);
    m_Logic->Plug(dimp,"DICOM Suite");
    //m_Logic->Plug(new medOpImporterDicomOffis("DICOM"),"Images");
    m_Logic->Plug(new mafOpImporterVTK("VTK"),"Other");
    m_Logic->Plug(new mafOpImporterMSF1x("MAF 1.x"),"Other");
    m_Logic->Plug(new mafOpImporterBBF("BFF (VolumeLarge)"),"Other");
    m_Logic->Plug(new mafOpImporterRAWVolume_BES("Raw Volume"),"Images");
    m_Logic->Plug(new mafOpImporterRAWVolume("Raw Volume Legacy"),"Images");
    m_Logic->Plug(new medOpImporterRAWImages("Raw Images"),"Images");
    //m_Logic->Plug(new medOpImporterRAWImages("Raw Images Legacy"),"Images");
    m_Logic->Plug(new mafOpImporterImage("Images"),"Images");
    m_Logic->Plug(new medOpImporterLandmarkWS("ASCII trajectories (VWs)"),"Motion Analysis");
    if(C3DLib)
      m_Logic->Plug(new lhpOpImporterC3D("C3D"),"Motion Analysis");  
    // m_Logic->Plug(new lhpOpLandmarkImporter("Landmark")); //Old Importer
    m_Logic->Plug(new medOpImporterGRFWS("ASCII Force Plates (VWs)"), "Motion Analysis");
    m_Logic->Plug(new mafOpImporterMesh("Generic Mesh"), "Finite Element");
    m_Logic->Plug(new lhpOpImporterAnsysInputFile("Ansys Input File"), "Finite Element");	
    m_Logic->Plug(new lhpOpImporterAnsysCDBFile("Ansys CDB File"), "Finite Element");	
    m_Logic->Plug(new mafOpImporterExternalFile("External data"), "Other");
    m_Logic->Plug(new medOpImporterAnalogWS("ASCII Analog (VWs)"), "Motion Analysis");
    m_Logic->Plug(new lhpOpKinectUtil(false, "Kinect Full",  false, true, true ),"Motion Analysis");  
    m_Logic->Plug(new lhpOpKinectUtil(false, "Kinect Lower", false, true, false),"Motion Analysis");  
    m_Logic->Plug(new lhpOpKinectUtil(false, "Kinect Upper", false, false, true),"Motion Analysis");  
    m_Logic->Plug(new lhpOpKinectUtil(true, "Kinect App Full",  false, true, true ),"Motion Analysis");  
    m_Logic->Plug(new lhpOpKinectUtil(true, "Kinect App Lower", false, true, false),"Motion Analysis");  
    m_Logic->Plug(new lhpOpKinectUtil(true, "Kinect App Upper", false, false, true),"Motion Analysis");  
    m_Logic->Plug(new lhpOpImporterRSScan("RSScan"), "Finite Element");	
  }

  //-------------------------------------------------------------

  //------------------------- Exporters -------------------------
  m_Logic->Plug(new mafOpExporterSTL("STL"),"Geometries");
  m_Logic->Plug(new lhpOpINPExporter("INP"),"Geometries");
  m_Logic->Plug(new mafOpExporterVTK("VTK"), "Other");
  m_Logic->Plug(new mafOpExporterBmp("Bmp"), "Images");
  m_Logic->Plug(new lhpOpMTRExporter("MTR"), "Motion Analysis");
  m_Logic->Plug(new medOpExporterLandmark("Landmark"), "Motion Analysis");
  m_Logic->Plug(new lhpOpExporterC3DBTK("C3D BTK"),"Motion Analysis");  
  if(fullVersion)
  {
    m_Logic->Plug(new mafOpExporterRAW("Raw"), "Images");
    m_Logic->Plug(new medOpExporterWrappedMeter("Wrapped Meter"), "Other");
    m_Logic->Plug(new medOpExporterMeters("Meters"), "Other");
    m_Logic->Plug(new lhpOpExporterAnsysInputFile("Ansys Input File"),"Finite Element");
  }
  //-------------------------------------------------------------

  //------------------------- Operations -------------------------
  m_Logic->Plug(new mafOpValidateTree());
  m_Logic->Plug(new mafOpCreateGroup("Group"),"Create/New");
	m_Logic->Plug(new mafOpCreateSurfaceParametric("Parametric Surface"),"Create/New");
	m_Logic->Plug(new mafOpAddLandmark("Add Landmark \tCtrl+A"),"Create/New");
  m_Logic->Plug(new mafOpCreateMeter("Meter"),"Create/Derive");
  m_Logic->Plug(new lhpOpFindCentroid("Geometry centroid"),"Create/Derive");

  m_Logic->Plug(new mafOpReparentTo("Reparent to...  \tCtrl+R"),"Modify/Fuse");
  m_Logic->Plug(new mafOpReparentTo("Local reparent to...", false),"Modify/Fuse");
  m_Logic->Plug(new lhpOpMove(),"Modify");
  m_Logic->Plug(new lhpOpAverageLM("Average landmark"),"Create/Derive");
  m_Logic->Plug(new lhpOpCreateLMCLines("Cloud lines"),"Create/Derive");
  m_Logic->Plug(new lhpOpJoinSurf("JoinSurface"),"Create/Derive");
  m_Logic->Plug(new lhpOpMergeClouds("Merge clouds"),"Create/Derive");
  if(fullVersion)
  {
    m_Logic->Plug(new mafOpCreateVolume("Constant Volume"),"Create/New");

#ifdef MAF_USE_ITK
    m_Logic->Plug(new lhpOpCreateSurfaceScalar("Surface Scalar"),"Create/Derive");
#endif
    m_Logic->Plug(new lhpOpLnSurf("Lineset and surface"),"Create/Derive");
    m_Logic->Plug(new mafOpCreateRefSys("Refsys"),"Create/New");
    m_Logic->Plug(new mafOpCreateSlicer("Slicer"),"Create/Derive");
    m_Logic->Plug(new medOpFreezeVME("Freeze VME"),"Create/Derive");
    m_Logic->Plug(new medOpRegisterClusters("Register Landmark Cloud"),"Modify/Fuse");
    m_Logic->Plug(new lhpOpFuseLMScripted("Fuse LM based model"),"Modify/Fuse");
    m_Logic->Plug(new lhpOpRegisterLMScripted("Register Landmark Cloud Tree"),"Modify/Fuse");
    //m_Logic->Plug(new medOpCreateWrappedMeter("Wrapped Meter"),"Create/Derive");
    m_Logic->Plug(new medOpComputeWrapping("Wrapped Action Line"),"Create/Derive");//15-1-2009
    m_Logic->Plug(new medOpCreateMuscleWrapper("Muscle Wrapper"),"Create/Derive"); //BES: 14.11.2008
    // m_Logic->Plug(new lhpOpEditMetadata("Metadata Editor"),"Modify");
    m_Logic->Plug(new mafOpFilterSurface("Filter Surface"),"Modify");
    m_Logic->Plug(new mafOpVOIDensityEditor("Volume Density"),"Modify");
    m_Logic->Plug(new medOpMeshDeformation("Deform Surface"), "Modify");
    m_Logic->Plug(new mafOpApplyTrajectory("Apply Trajectory"), "Modify");
    m_Logic->Plug(new mafOpExtractIsosurface("Extract Isosurface"),"Create/Derive");
    m_Logic->Plug(new medOpSurfaceMirror("Surface Mirror"),"Modify");
    m_Logic->Plug(new mafOpCrop("Crop Volume"),"Modify");
    m_Logic->Plug(new medOpVolumeResample("Volume Resample"),"Modify");
    m_Logic->Plug(new mafOp2DMeasure("2D Measure"),"Measure");
    m_Logic->Plug(new mafOpVOIDensity("VOI Density"),"Measure");
    m_Logic->Plug(new lhpOpScaleDataset(),"Modify");
    m_Logic->Plug(new medOpCropDeformableROI(_("Masking")),_("Modify"));
    m_Logic->Plug(new mafOpImporterVMEDataSetAttributes("VME DataSet Attributes Adder"),"Modify");
    m_Logic->Plug(new medOpClassicICPRegistration("Register Surface"),"Modify/Fuse");
    m_Logic->Plug(new lhpOpAFSys("AFRefsys"),"Create/Derive");
    m_Logic->Plug(new lhpOpKinectAFs("Kinect Refsys"),"Create/Derive");
    m_Logic->Plug(new lhpOpStickPalpation("Wand palpated landmark"),"Create/Derive");

    m_Logic->Plug(new lhpOpHelAxis("Helical axis"),"Create/Derive");
    m_Logic->Plug(new lhpOpTimeReduce("Time reduce"),"Modify");
    m_Logic->Plug(new lhpOpTimeShift("Time shift"),"Modify");
    m_Logic->Plug(new lhpOpLMProj(true, "Landmark Cloud Projection"),"Create/Derive");
    m_Logic->Plug(new lhpOpSolidify("Solidify Landmark Cloud"),"Create/Derive");
    m_Logic->Plug(new lhpOpSoftReg("Soft tissue registration"),"Create/Derive");
    m_Logic->Plug(new lhpOpCreateObject<mafVMEBSplineLine>("BSplineLine", "BSplineLine"),"Create/New");
    m_Logic->Plug(new lhpOpCreateObject<mafVMEBSplineSurface>("BSplineSurface", "BSplineSurface"),"Create/New");
    m_Logic->Plug(new lhpOpCreateObject<mafVMEBSplineVolume>("BSplineVolume", "BSplineVolume"),"Create/New");
    m_Logic->Plug(new lhpOpCreateObject<lhpVMELeverArm>("Lever Arm", "Lever Arm"),"Create/Derive");
    m_Logic->Plug(new lhpOpRegSurfWithCloud("Register Surface with Landmark Cloud"),"Modify/Fuse");
    m_Logic->Plug(new lhpOpRepresentInAF("Represent in RefSys"),"Modify/Fuse");
    m_Logic->Plug(new lhpOpLMMirror("Landmark Cloud Mirror"),"Modify");
    m_Logic->Plug(new lhpOpMoveSeq(),"Modify");
    m_Logic->Plug(new lhpOpICPRegFollow("Move Surface As Registered"),"Modify/Fuse");
    m_Logic->Plug(new lhpOpRegression("Regression"),"Create/Derive");
    m_Logic->Plug(new lhpOpFingerStick("Finger stick"),"Create/Derive");
    m_Logic->Plug(new lhpOpCutSurface("CutSurface"),"Create/Derive");
    m_Logic->Plug(new medOpComputeWrapping("Compute Wrapping"),"Create/Derive");
    //m_Logic->Plug(new lhpOpMeanHelAxis("Mean helical axis"),"Create/Derive");
    m_Logic->Plug(new lhpOpRegistration("DSRegistration"),"Modify");
    m_Logic->Plug(new lhpOpKinectModel("KinectModel"),"Modify");  
    m_Logic->Plug(new lhpOpBuildHierarchy("Make hierarchical"),"Modify/Fuse");
    m_Logic->Plug(new lhpOpBonemat("Bonemat"),"Modify");
    m_Logic->Plug(new medOpIterativeRegistration("Iterative Registration"),"Modify/Fuse");
    m_Logic->Plug(new mafOpOpenExternalFile("Open with external program"),"Manage"); 
    m_Logic->Plug(new medOpCreateLabeledVolume("Labeled Volume"),"Create/Derive");
    //m_Logic->Plug(new lhpOpUploadVME("Upload VME"),"Manage");
    m_Logic->Plug(new lhpOpUploadMultiVME("Upload VME"),"Manage");
    m_Logic->Plug(new lhpOpEditTag("Edit Tag VME"),"Manage");
    m_Logic->Plug(new lhpOpDownloadVME("Download VME", lhpOpDownloadVME::FROM_BASKET),"Manage");
    m_Logic->Plug(new lhpOpDownloadVME("Download VME from sandbox",lhpOpDownloadVME::FROM_SANDBOX),"Manage");

    m_Logic->Plug(new mafOpDecomposeTimeVarVME("Decompose Time"),"Create/Derive");
    m_Logic->Plug(new mafOpLabelExtractor("Extract Label"),"Create/Derive");
    m_Logic->Plug(new lhpOpMultiscaleExplore("Multiscale Viewer"),"Manage");
    m_Logic->Plug(new medOpMML("Register from template"),"Modify");
    m_Logic->Plug(new lhpOpKeyczarIntegrationTest("Security Libraries Integration"),"Test");
    m_Logic->Plug(new lhpOpComputeTensor("Compute Tensors"), "Modify");

    m_Logic->Plug(new lhpOpTextureOrientation("Texture Orientation"),"Create/Derive");

    // Upload Download VME Refactor Target
    m_Logic->Plug(new lhpOpUploadMultiVMERefactor("Upload Multi VME Refactor"),"Devel");
    m_Logic->Plug(new lhpOpEditTagRefactor("Edit Tag VME Refactor"),"Devel");
    m_Logic->Plug(new lhpOpDownloadVMERefactor("Download Multi VME Refactor"),"Devel");
    // m_Logic->Plug(new lhpOpUploadVMERefactor("Upload VME Refactor"),"Devel");
  }
  
  //-------------------------------------------------------------
  bool view_visibility = 0;//fullVersion;
  //------------------------- Views -------------------------
  //View Arbitrary Slice
  mafViewArbitrarySlice *ArbitraryView = new mafViewArbitrarySlice("Arbitrary");
  ArbitraryView->PackageView();
  m_Logic->Plug(ArbitraryView, view_visibility);


  // View DRR
  mafViewVTK *vdrr = new mafViewVTK("DRR");
  vdrr->PlugVisualPipe("mafVMEVolumeGray","medPipeVolumeDRR",MUTEX);
  vdrr->PlugVisualPipe("mafVMEVolumeLarge","medPipeVolumeDRR",MUTEX);
  m_Logic->Plug(vdrr, view_visibility);

  // View Analog graph
  mafViewVTK *graph = new mafViewVTK("Analog Graph", CAMERA_PERSPECTIVE, false);
  graph->PlugVisualPipe("medVMEAnalog", "medPipeGraph",MUTEX);
  m_Logic->Plug(graph, view_visibility);

  //View Global Slice
  mafViewGlobalSliceCompound *GlobalSlice = new mafViewGlobalSliceCompound("Global Slice");
  GlobalSlice->PackageView();
  m_Logic->Plug(GlobalSlice, view_visibility);

  mafViewVTK *viso = new mafViewVTK("Isosurface");
  viso->PlugVisualPipe("mafVMEVolumeGray", "mafPipeIsosurface",MUTEX);
  viso->PlugVisualPipe("medVMELabeledVolume", "mafPipeIsosurface",MUTEX);
  viso->PlugVisualPipe("mafVMEVolumeLarge","mafPipeIsosurface",MUTEX);
  m_Logic->Plug(viso, view_visibility);

  mafViewVTK *visoGPU = new mafViewVTK("Isosurface (GPU)");
  visoGPU->PlugVisualPipe("mafVMEVolumeGray", "mafPipeIsosurfaceGPU",MUTEX);   //BES: 13.11.2008 - GPU support, mafPipeIsosurfaceGPU to be merged with mafPipeIsosurface in future 
  visoGPU->PlugVisualPipe("medVMELabeledVolume", "mafPipeIsosurfaceGPU",MUTEX);
  visoGPU->PlugVisualPipe("mafVMEVolumeLarge", "mafPipeIsosurfaceGPU",MUTEX);
  m_Logic->Plug(visoGPU, view_visibility);

  mafViewOrthoSlice *viewOrthoSlice = new mafViewOrthoSlice("OrthoSlice");
  viewOrthoSlice->PackageView();
  m_Logic->Plug(viewOrthoSlice/*, view_visibility*/);

  mafViewRXCT *vrxctl = new mafViewRXCT("RXCT");
  vrxctl->PackageView();
  m_Logic->Plug(vrxctl, view_visibility);

	mafViewVTK *vsurface = new mafViewVTK("Surface");
	vsurface->PlugVisualPipe("mafVMESurface","mafPipeSurface");
  vsurface->PlugVisualPipe("mafVMELandmark", "medPipeTrajectories");
	m_Logic->Plug(vsurface);

  mafViewIntGraph *vgraph = new mafViewIntGraph("Biomechanical graph");
  vgraph->PlugVisualPipe("mafVMEHelAxis","lhpPipeIntGraphHAxis");
  vgraph->PlugVisualPipe("medVMEComputeWrapping","lhpPipeIntGraphPolyline");
  vgraph->PlugVisualPipe("medVMEWrappedMeter","lhpPipeIntGraphPolyline");
  vgraph->PlugVisualPipe("mafVMEMeter","lhpPipeIntGraphPolyline");
  vgraph->PlugVisualPipe("lhpVMELeverArm","lhpPipeIntGraphPolyline");
  vgraph->PlugVisualPipe("mafVMEVector","lhpPipeIntGraphPolyline");
  vgraph->PlugVisualPipe("medVMEAnalog","lhpPipeIntGraphAnalog");
  m_Logic->Plug(vgraph);

  medViewSlicer *slicerView = new medViewSlicer("Slicer");
  slicerView->PackageView();
  m_Logic->Plug(slicerView, view_visibility);

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
  m_Logic->GetCredentials();

  m_Logic->Init(0,NULL); // calls FileNew - which create the root

  return TRUE;
}
//--------------------------------------------------------------------------------
int lhpFusionBoxApp::OnExit()
//--------------------------------------------------------------------------------
{
  cppDEL(m_Logic);

  //this hack is fixing VTK internal memory leak
  vtkTimerLog::CleanupLog();
  for(unsigned i = 0; i < m_Plugins.size(); i++)
  {
    if(m_Plugins[i].second)
      m_Plugins[i].second();
    mafDynamicLoader::CloseLibrary(m_Plugins[i].first);
  }
  return 0;
}

//void lhpFusionBoxApp::OnIdle( wxIdleEvent &event )
//{ 
//  vtkDataArrayMemMng::GetDataArrayMemMng()->UnlockAllMemory(0);
//  event.Skip();
//}
#else
//--------------------------------------------------------------------------------
bool lhpFusionBoxApp::OnInit()
  //--------------------------------------------------------------------------------
{
  mafPictureFactory::GetPictureFactory()->Initialize();	

#include "pic/lhpBuilder/FRAME_ICON16x16.xpm"
  mafADDPIC(FRAME_ICON16x16);
#include "pic/lhpBuilder/FRAME_ICON32x32.xpm"
  mafADDPIC(FRAME_ICON32x32);
#include "pic/lhpBuilder/MDICHILD_ICON.xpm"
  mafADDPIC(MDICHILD_ICON);

  LibHandle C3DLib      = NULL;
  bool fullVersion      = true;

  C3DLib = mafDynamicLoader::OpenLibrary("C3D_Reader");
  if(C3DLib)
  {
    if(lhpOpImporterC3D::Config(C3DLib))
    {
      m_Plugins.push_back(std::make_pair(C3DLib, (void(*)())NULL));
    }
    else
    {
      mafDynamicLoader::CloseLibrary(C3DLib);
      C3DLib = NULL;
    }
  }

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
  mafPlugNode<mafVMEC3DData>("VME representing C3D data");
  mafPlugNode<mafVMEPGDData>("VME representing PGD data");
  mafPlugNode<mafVMEArrow>("VME representing helical axis");
  mafPlugNode<lhpVMELeverArm>("VME representing lever arm");
  mafPlugNode<lhpVMELMCLines>("VME representing lines between landmarks of cloud");
  mafPlugNode<mafVMEBSplineLine>("VME representing B-spline line");
  mafPlugNode<mafVMEBSplineSurface>("VME representing B-spline surface");
  mafPlugNode<mafVMEBSplineVolume>("VME representing B-spline volume");
  mafPlugNode<mafVMESurfaceRegParam>("VME representing regression parametric surface");
  mafPlugNode<mafVMEVolumeLarge>("VME storing large volume datasets with one scalar component");

  mafPlugNode<medVMEComputeWrapping>("Generalized another VME Meter with wrapping geometry");
  mafPlugPipe<medPipeComputeWrapping>("Pipe to Visualize Compute Wrapping Meter");

#ifdef MAF_USE_ITK
  mafPlugNode<lhpVMESurfaceScalarVarying>("VME representing surface with attached time varying mafVMEScalar");
#endif

  //BES: 14.11.2008 - some stupid VME to demonstrate muscle wrapping
  mafPlugNode<medVMEMuscleWrapper>("Procedural VME representing muscle deformed according to its action lines");

  mafPlugPipe<lhpVisualPipeSurfaceScalar>("Visual pipe to render a surface with its scalar values");
  mafPlugPipe<lhpPipeIntGraph>("Visual pipe for biomechanical graph");
  mafPlugPipe<lhpPipeIntGraphHAxis>("Visual pipe for helical axis in a biomechanical graph");
  mafPlugPipe<lhpPipeIntGraphPolyline>("Visual pipe for polyline in a biomechanical graph");
  mafPlugPipe<lhpPipeIntGraphAnalog>("Visual pipe for analog data in a biomechanical graph");
  mafPlugPipe<lhpPipeLeverArm>("Visual pipe for lever arm");

  m_Logic = new lhpBuilderLogic();
  m_Logic->GetTopWin()->SetTitle("LHPFusionBox");
  m_Logic->Configure();
  SetTopWindow(mafGetFrame());  

  wxString regKeyName;
  regKeyName = "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\lhpFusionBox";
  wxRegKey RegKey(regKeyName);
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
  //------------------------- Editors -------------------------
  m_Logic->Plug(new mafOpDelete(_("Delete   \tCtrl+Shift+D")), "");
  m_Logic->Plug(new mafOpCut(_("Cut   \tCtrl+Shift+X")), "");
  m_Logic->Plug(new mafOpCopy(_("Copy  \tCtrl+Shift+C")), "");
  m_Logic->Plug(new mafOpPaste(_("Paste \tCtrl+Shift+V")), "");
  //------------------------- Importers -------------------------
  m_Logic->Plug(new mafOpImporterSTL("STL"),"Geometries");
  m_Logic->Plug(new mafOpImporterMSF("MSF"),"Other");
  m_Logic->Plug(new medOpImporterLandmark("Landmark"),"Motion Analysis");
  m_Logic->Plug(new lhpOpINPImporter("INP/INP_AF"), "Geometries");
  m_Logic->Plug(new lhpOpImporterC3DBTK("C3D BTK"),"Motion Analysis");  
  if(C3DLib)
    m_Logic->Plug(new lhpOpImporterC3D("C3D"),"Motion Analysis");  
  m_Logic->Plug(new lhpOpKinectUtil(false, "Kinect Full",  true, true, true ),"Motion Analysis");  
  m_Logic->Plug(new lhpOpKinectUtil(false, "Kinect Lower", true, true, false),"Motion Analysis");  
  m_Logic->Plug(new lhpOpKinectUtil(false, "Kinect Upper", true, false, true),"Motion Analysis");  
  m_Logic->Plug(new lhpOpKinectUtil(true, "Kinect App Full",  true, true, true ),"Motion Analysis");  
  m_Logic->Plug(new lhpOpKinectUtil(true, "Kinect App Lower", true, true, false),"Motion Analysis");  
  m_Logic->Plug(new lhpOpKinectUtil(true, "Kinect App Upper", true, false, true),"Motion Analysis");  

  //-------------------------------------------------------------

  //------------------------- Exporters -------------------------
  m_Logic->Plug(new mafOpExporterSTL("STL"),"Geometries");
  m_Logic->Plug(new lhpOpINPExporter("INP"),"Geometries");
  m_Logic->Plug(new medOpExporterLandmark("Landmark"), "Motion Analysis");
  m_Logic->Plug(new lhpOpExporterC3DBTK("C3D BTK"),"Motion Analysis");  

  //------------------------- Operations -------------------------
  m_Logic->Plug(new mafOpCreateGroup("Group"),"Create/New");

  m_Logic->Plug(new mafOpReparentTo("Reparent to...  \tCtrl+R"),"Modify/Fuse");
  m_Logic->Plug(new mafOpReparentTo("Local reparent to...", false),"Modify/Fuse");

  if(fullVersion)
  {
    m_Logic->Plug(new mafOpCreateRefSys("Refsys"),"Create/New");
    m_Logic->Plug(new medOpRegisterClusters("Register Landmark Cloud"),"Modify/Fuse");
    m_Logic->Plug(new lhpOpFuseLMScripted("Fuse LM based model"),"Modify/Fuse");
    m_Logic->Plug(new lhpOpRegisterLMScripted("Register Landmark Cloud Tree"),"Modify/Fuse");
    m_Logic->Plug(new lhpOpAFSys("AFRefsys"),"Create/Derive");
    m_Logic->Plug(new lhpOpKinectAFs("Kinect Refsys"),"Create/Derive");

    m_Logic->Plug(new lhpOpKinectModel("KinectModel"),"Modify");  

  }

  //-------------------------------------------------------------
  bool view_visibility = 0;//fullVersion;
  //------------------------- Views -------------------------
  //View Arbitrary Slice
  mafViewArbitrarySlice *ArbitraryView = new mafViewArbitrarySlice("Arbitrary");
  ArbitraryView->PackageView();
  m_Logic->Plug(ArbitraryView, view_visibility);


  // View DRR
  mafViewVTK *vdrr = new mafViewVTK("DRR");
  vdrr->PlugVisualPipe("mafVMEVolumeGray","medPipeVolumeDRR",MUTEX);
  vdrr->PlugVisualPipe("mafVMEVolumeLarge","medPipeVolumeDRR",MUTEX);
  m_Logic->Plug(vdrr, view_visibility);

  // View Analog graph
  mafViewVTK *graph = new mafViewVTK("Analog Graph", CAMERA_PERSPECTIVE, false);
  graph->PlugVisualPipe("medVMEAnalog", "medPipeGraph",MUTEX);
  m_Logic->Plug(graph, view_visibility);

  //View Global Slice
  mafViewGlobalSliceCompound *GlobalSlice = new mafViewGlobalSliceCompound("Global Slice");
  GlobalSlice->PackageView();
  m_Logic->Plug(GlobalSlice, view_visibility);

  mafViewVTK *viso = new mafViewVTK("Isosurface");
  viso->PlugVisualPipe("mafVMEVolumeGray", "mafPipeIsosurface",MUTEX);
  viso->PlugVisualPipe("medVMELabeledVolume", "mafPipeIsosurface",MUTEX);
  viso->PlugVisualPipe("mafVMEVolumeLarge","mafPipeIsosurface",MUTEX);
  m_Logic->Plug(viso, view_visibility);

  mafViewVTK *visoGPU = new mafViewVTK("Isosurface (GPU)");
  visoGPU->PlugVisualPipe("mafVMEVolumeGray", "mafPipeIsosurfaceGPU",MUTEX);   //BES: 13.11.2008 - GPU support, mafPipeIsosurfaceGPU to be merged with mafPipeIsosurface in future 
  visoGPU->PlugVisualPipe("medVMELabeledVolume", "mafPipeIsosurfaceGPU",MUTEX);
  visoGPU->PlugVisualPipe("mafVMEVolumeLarge", "mafPipeIsosurfaceGPU",MUTEX);
  m_Logic->Plug(visoGPU, view_visibility);

  mafViewOrthoSlice *viewOrthoSlice = new mafViewOrthoSlice("OrthoSlice");
  viewOrthoSlice->PackageView();
  m_Logic->Plug(viewOrthoSlice, view_visibility);

  mafViewRXCT *vrxctl = new mafViewRXCT("RXCT");
  vrxctl->PackageView();
  m_Logic->Plug(vrxctl, view_visibility);

  mafViewVTK *vsurface = new mafViewVTK("Surface");
  vsurface->PlugVisualPipe("mafVMESurface","mafPipeSurface");
  vsurface->PlugVisualPipe("mafVMELandmark", "medPipeTrajectories");
  m_Logic->Plug(vsurface);

  mafViewIntGraph *vgraph = new mafViewIntGraph("Biomechanical graph");
  vgraph->PlugVisualPipe("mafVMEHelAxis","lhpPipeIntGraphHAxis");
  vgraph->PlugVisualPipe("medVMEComputeWrapping","lhpPipeIntGraphPolyline");
  vgraph->PlugVisualPipe("medVMEWrappedMeter","lhpPipeIntGraphPolyline");
  vgraph->PlugVisualPipe("mafVMEMeter","lhpPipeIntGraphPolyline");
  vgraph->PlugVisualPipe("lhpVMELeverArm","lhpPipeIntGraphPolyline");
  vgraph->PlugVisualPipe("mafVMEVector","lhpPipeIntGraphPolyline");
  vgraph->PlugVisualPipe("medVMEAnalog","lhpPipeIntGraphAnalog");
  m_Logic->Plug(vgraph);

  medViewSlicer *slicerView = new medViewSlicer("Slicer");
  slicerView->PackageView();
  m_Logic->Plug(slicerView, view_visibility);

  //temporary for testing
  //mafViewSingleSliceCompound *sliceView = new mafViewSingleSliceCompound("Test Slice");
  //sliceView->PackageView();
  //m_Logic->Plug(sliceView);

  m_Logic->Show();
  m_Logic->GetCredentials();

  m_Logic->Init(0,NULL); // calls FileNew - which create the root

  return TRUE;
}
//--------------------------------------------------------------------------------
int lhpFusionBoxApp::OnExit()
  //--------------------------------------------------------------------------------
{
  cppDEL(m_Logic);

  //this hack is fixing VTK internal memory leak
  vtkTimerLog::CleanupLog();
  for(unsigned i = 0; i < m_Plugins.size(); i++)
  {
    if(m_Plugins[i].second)
      m_Plugins[i].second();
    mafDynamicLoader::CloseLibrary(m_Plugins[i].first);
  }
  return 0;
}

#endif
