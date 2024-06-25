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
#include "lhpFusionBoxApp.h"

#include <wx/datetime.h>
#include <wx/config.h>

#include "mafDefines.h"
#include "mafDecl.h"

#include "mafDefines.h"

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
#include "mafPipeFactoryVME.h"
#include "mafVMEFactory.h"
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
#include "mafOpCreateMuscleWrapping2.h"
#include "mafOpCreateMeter2.h"
#include "mafOpCreateGravityLine.h"
#include "mafOpCreateCenterLine.h"
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
#include "mafOpExporterVRML.h"
#include "mafOpExporterGeomTex.h"
#include "mafOpImporterVTK.h"
#include "mafOpImporterMSF1x.h"
#include "mafOpImporterVRML.h"
#include "mafOpCreateVolume.h"
#include "mafOpCreatePlane.h"
#include "mafOpCreateOsteometricBoard.h"
#include "mafOpVOIDensityEditor.h"
#include "mafOpImporterBBF.h"
#include "mafOpCreateQuadricSurfaceFitting.h"
//#include "mafOpCreateMuscleWrapper.h"
//BES: 23.6.2008 - Large Volume - to be merged 
#include "mafOpImporterRAWVolume_BES.h"
#include "mafOpImporterRAWVolume.h"
#include "lhpOpKinectUtil.h"
#include "lhpOpKinectModel.h"
#include "lhpOpKinectAFs.h"
#include "lhpOpCreateMetersScripted.h"
#include "mafOpExporterRaw.h"
#include "medOpImporterRAWImages.h"
#include "mafOpExtractIsosurface.h"
#include "mafOpCrop.h"
#include "mafOpVOIDensity.h"
#include "medOpVolumeResample.h"
#include "mafOpAddLandmark.h"
#include "medOpRegisterClusters2.h"
#include "medOpRegisterClusters.h"

#include "lhpOpFuseLMScripted.h"
#include "lhpOpRegisterLMScripted.h"
#include "mafOpCreateSurfaceParametric.h"
#include "mafOpCreateEllipsoid.h"
#include "mafOpCreateHyperboloid.h"
#include "mafOpCreateHyperboloid2S.h"
#include "mafOpCreateCylinder.h"
#include "lhpOpBuildHierarchy.h"
#include "lhpOpTimeReduce.h"
#include "lhpOpTimeShift.h"
#include "mafOpExporterOBJ.h"
#include "lhpOpINPExporter.h"
#include "lhpOpMTRExporter.h"
#include "lhpOpINPImporter.h"
#include "lhpOpMTRImporter.h"
#include "lhpOpImporterOBJ.h"
#include "lhpOpImporterPLY.h"
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
#include "medOpFreezeVME.h"
#include "medOpExporterWrappedMeter.h"
#include "medOpIterativeRegistration.h"
#include "medOpCreateLabeledVolume.h"
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

#include "mafOpCrop3DSurface.h"
#include "medOpComputeWrapping.h"
#include "medVMEComputeWrapping.h"
#include "medPipeComputeWrapping.h"
#include "medGUIDicomSettings.h"

//temporary for testing
#include "mafViewSingleSliceCompound.h"

#include "lhpOpFindCentroid.h"
#include "lhpOpCreateRefSysLM.h"

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
#include "lhpOpImporterC3DFused.h" 
#include "lhpOpImporterPressionCenter.h" 
#include "lhpOpExporterC3DBTK.h" 
#include "lhpOpExporterCSVGraph.h" 
#include "lhpOpLMMirror.h"
#include "lhpOpLMProj.h"
#include "lhpOpSolidify.h"
#include "lhpOpJoinSurf.h"
#include "lhpOpMergeClouds.h"
#include "lhpOpICPRegFollow.h"
#include "lhpOpImporterRSScan.h"
#include "lhpOpCutSurface.h"
#include "vtkUnstructuredGrid.h"
#include "vtkSmartPointer.h"
#include "vtkTransformFilter.h"
#include "vtkStructuredPoints.h"

#include "lhpVMEKMInfo.h"
#include "lhpPipeInfo.h"
#include "lhpViewInfo.h"


// TODO: REFACTOR THIS 
// this component is used only to override the Accept,  
// and it`s the minimal amount of code in order to override the method
// it could go in a separate file with other redefined Accept`s

class lhpOpMove : public medOpMove
{
public:
  mafTypeMacro(lhpOpMove, medOpMove)

  lhpOpMove(const mafString& label = _R("Move\tCtrl+T")):Superclass(label){}
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
  lhpOpMoveSeq(const mafString& label = _R("Move Sequence")):Superclass(label){m_EnableScaling = 0;}
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
  lhpOpCreateLMCLines(const mafString& label = _R("CreateObject")):Superclass(label)
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
    m_Created->SetName(_R("Cloud lines"));
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
  bool fullVersion      = true;

  /*fullControl = mafDynamicLoader::OpenLibrary("full_ver");
  if(fullControl)
  {
    unsigned (*fnfull_ver)() = (unsigned(*)())mafDynamicLoader::GetSymbolAddress(fullControl, "fnfull_ver");
    if(fnfull_ver != NULL && fnfull_ver() > 0)
    {
      fullVersion = true;
    }
    m_Plugins.push_back(std::make_pair(fullControl, (void(*)())NULL));
  }*/

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
 
  result = mafVMEFactory::Initialize();
  assert(result == MAF_OK);

  result = mafPipeFactoryVME::Initialize();
  assert(result == MAF_OK);

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

  mafPlugNode<lhpVMEKMInfo>("VME storing large volume datasets with one scalar component");

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

  mafPlugPipe<lhpPipeInfo>("Visual pipe for lever arm");

  m_Logic = std::make_unique<medLogicWithManagers>();
  if(fullVersion)
  {
    SetAppName("lhpFusionBox");
    SetAppDisplayName("LHPFusionBox");
    m_Logic->GetTopWin()->SetTitle("LHPFusionBox");
  }
  else
  {
    SetAppName("ULBViewer");
    SetAppDisplayName("ULBViewer");
    m_Logic->GetTopWin()->SetTitle("ULBViewer");
  }
  m_Logic->Configure();
  SetTopWindow(mafGetFrame());  

  //------------------------- Editors -------------------------
  m_Logic->Plug(new mafOpDelete(_L("Delete   \tCtrl+Shift+D")), _R(""));
  m_Logic->Plug(new mafOpCut(_L("Cut   \tCtrl+Shift+X")), _R(""));
  m_Logic->Plug(new mafOpCopy(_L("Copy  \tCtrl+Shift+C")), _R(""));
  m_Logic->Plug(new mafOpPaste(_L("Paste \tCtrl+Shift+V")), _R(""));
  //------------------------- Importers -------------------------
  m_Logic->Plug(new mafOpImporterSTL(_R("STL")),_R("Geometries"));
  m_Logic->Plug(new lhpOpImporterOBJ(_R("OBJ")), _R("Geometries"));
  m_Logic->Plug(new lhpOpImporterPLY(_R("PLY")), _R("Geometries"));
  m_Logic->Plug(new mafOpImporterMSF(_R("MSF")),_R("Other"));
  m_Logic->Plug(new medOpImporterLandmark(_R("Landmark")),_R("Motion Analysis"));
  m_Logic->Plug(new medOpImporterMotionData<mafVMERawMotionData>(_R("Raw Motion Data"), _R("RAW Motion Data (*.MAN)|*.MAN"), _R("Dictionary (*.txt)|*.txt")), _R("Motion Analysis"));
  m_Logic->Plug(new medOpImporterMotionData<mafVMEPGDData>(_R("PGD Data"), _R("PGD Data (*.PGD)|*.PGD"), _R("Dictionary (*.txt)|*.txt")), _R("Motion Analysis"));
  m_Logic->Plug(new mafOpImporterVRML(_R("VRML")), _R("Geometries"));
  m_Logic->Plug(new lhpOpINPImporter(_R("INP/INP_AF")), _R("Geometries"));
  m_Logic->Plug(new lhpOpMTRImporter(_R("MTR")), _R("Geometries"));
  m_Logic->Plug(new lhpOpMTRULBImporter(_R("MTR (ULB)")), _R("Geometries"));
  m_Logic->Plug(new lhpOpImporterC3DBTK(_R("C3D BTK")),_R("Motion Analysis"));  
  m_Logic->Plug(new lhpOpImporterC3DFused(_R("C3D Automated")),_R("Motion Analysis"));  
  m_Logic->Plug(new lhpOpImporterPressionCenter(_R("plantar pression center")), _R("Gait Analysis"));
  if(fullVersion)
  {
    medGUIDicomSettings *dicomSettings=new medGUIDicomSettings(NULL,_R("DICOM"));
    medOpImporterDicomOffis *dimp = new medOpImporterDicomOffis(_R("DICOM"));
    dimp->SetSetting(dicomSettings);
    m_Logic->Plug(dimp,_R("DICOM Suite"));
    //m_Logic->Plug(new medOpImporterDicomOffis(_R("DICOM")),_R("Images"));
    m_Logic->Plug(new mafOpImporterVTK(_R("VTK")),_R("Other"));
    m_Logic->Plug(new mafOpImporterMSF1x(_R("MAF 1.x")),_R("Other"));
    m_Logic->Plug(new mafOpImporterBBF(_R("BFF (VolumeLarge)")),_R("Other"));
    m_Logic->Plug(new mafOpImporterRAWVolume_BES(_R("Raw Volume")),_R("Images"));
    m_Logic->Plug(new mafOpImporterRAWVolume(_R("Raw Volume Legacy")),_R("Images"));
    m_Logic->Plug(new medOpImporterRAWImages(_R("Raw Images")),_R("Images"));
    //m_Logic->Plug(new medOpImporterRAWImages(_R("Raw Images Legacy")),_R("Images"));
    m_Logic->Plug(new mafOpImporterImage(_R("Images")),_R("Images"));
    m_Logic->Plug(new medOpImporterLandmarkWS(_R("ASCII trajectories (VWs)")),_R("Motion Analysis"));
    if(C3DLib)
      m_Logic->Plug(new lhpOpImporterC3D(_R("C3D")),_R("Motion Analysis"));  
    // m_Logic->Plug(new lhpOpLandmarkImporter(_R("Landmark"))); //Old Importer
    m_Logic->Plug(new medOpImporterGRFWS(_R("ASCII Force Plates (VWs)")), _R("Motion Analysis"));
    m_Logic->Plug(new mafOpImporterMesh(_R("Generic Mesh")), _R("Finite Element"));
    m_Logic->Plug(new mafOpImporterExternalFile(_R("External data")), _R("Other"));
    m_Logic->Plug(new medOpImporterAnalogWS(_R("ASCII Analog (VWs)")), _R("Motion Analysis"));
    m_Logic->Plug(new lhpOpKinectUtil(false, _R("Kinect Full"),  false, true, true ),_R("Motion Analysis"));  
    m_Logic->Plug(new lhpOpKinectUtil(false, _R("Kinect Lower"), false, true, false),_R("Motion Analysis"));  
    m_Logic->Plug(new lhpOpKinectUtil(false, _R("Kinect Upper"), false, false, true),_R("Motion Analysis"));  
    m_Logic->Plug(new lhpOpKinectUtil(true, _R("Kinect App Full"),  false, true, true ),_R("Motion Analysis"));  
    m_Logic->Plug(new lhpOpKinectUtil(true, _R("Kinect App Lower"), false, true, false),_R("Motion Analysis"));  
    m_Logic->Plug(new lhpOpKinectUtil(true, _R("Kinect App Upper"), false, false, true),_R("Motion Analysis"));  
    m_Logic->Plug(new lhpOpImporterRSScan(_R("RSScan")), _R("Finite Element"));	
  }

  //-------------------------------------------------------------

  //------------------------- Exporters -------------------------
  m_Logic->Plug(new mafOpExporterSTL(_R("STL")),_R("Geometries"));
  m_Logic->Plug(new lhpOpINPExporter(_R("INP")),_R("Geometries"));
  m_Logic->Plug(new mafOpExporterOBJ(_R("OBJ")), _R("Geometries"));
  //m_Logic->Plug(new mafOpExporterVRML(_R("VRML")), _R("Geometries"));
  //m_Logic->Plug(new mafOpExporterVRML(_R("FBX")), _R("Geometries"));
  //m_Logic->Plug(new mafOpExporterGeomTex(_R("GEOMTEX")), _R("Geometries"));
  m_Logic->Plug(new mafOpExporterVTK(_R("VTK")), _R("Other"));
  m_Logic->Plug(new mafOpExporterBmp(_R("Bmp")), _R("Images"));
  m_Logic->Plug(new lhpOpMTRExporter(_R("MTR")), _R("Motion Analysis"));
  m_Logic->Plug(new medOpExporterLandmark(_R("Landmark")), _R("Motion Analysis"));
  m_Logic->Plug(new lhpOpExporterC3DBTK(_R("C3D BTK")),_R("Motion Analysis"));  
  m_Logic->Plug(new lhpOpExporterCSVGraph(_R("CSV Graph")), _R("Motion Analysis"));
  if (fullVersion)
  {
    m_Logic->Plug(new mafOpExporterRAW(_R("Raw")), _R("Images"));
    m_Logic->Plug(new medOpExporterWrappedMeter(_R("Wrapped Meter")), _R("Other"));
    m_Logic->Plug(new medOpExporterMeters(_R("Meters")), _R("Other"));
  }
  //-------------------------------------------------------------

  //------------------------- Operations -------------------------
  m_Logic->Plug(new mafOpValidateTree());
  m_Logic->Plug(new mafOpCreateGroup(_R("Group")),_R("Create/New"));
  m_Logic->Plug(new lhpOpCreateMetersScripted(_R("Scripted meters")), _R("Create/New"));
  m_Logic->Plug(new mafOpCreateSurfaceParametric(_R("Parametric Surface")),_R("Create/New"));
  m_Logic->Plug(new mafOpCreateEllipsoid(_R("Ellipsoid")), _R("Create/New/QuadricSurface"));
  m_Logic->Plug(new mafOpCreateHyperboloid(_R("Hyperboloid1S")), _R("Create/New/QuadricSurface"));
  m_Logic->Plug(new mafOpCreateHyperboloid2S(_R("Hyperboloid2S")), _R("Create/New/QuadricSurface"));
  m_Logic->Plug(new mafOpCreateCylinder(_R("Cylinder")), _R("Create/New/QuadricSurface"));
  m_Logic->Plug(new mafOpCreateQuadricSurfaceFitting(_R("Quadric Surface Fitting")), _R("Create/Derive"));
  m_Logic->Plug(new mafOpAddLandmark(_R("Add Landmark \tCtrl+A")),_R("Create/New"));
  m_Logic->Plug(new mafOpCreateMeter(_R("Meter")),_R("Create/Derive"));
  m_Logic->Plug(new mafOpCreateMeter2(_R("Meter2")), _R("Create/Derive"));
  m_Logic->Plug(new mafOpCreateCenterLine(_R("Centerline")), _R("Create/Derive"));
  m_Logic->Plug(new mafOpCreateGravityLine(_R("Virtual Osteometricboard")), _R("Create/New/Osteometric Tools"));
  m_Logic->Plug(new lhpOpFindCentroid(_R("Geometry centroid")),_R("Create/Derive"));

//  m_Logic->Plug(new mafOpCreateMuscleWrapperAQ(_R("Muscle Wrapper_AQ")), _R("Create/Derive"));
  m_Logic->Plug(new mafOpCreateMuscleWrapping2(_R("Muscle Wrapper_2")), _R("Create/Derive"));
  m_Logic->Plug(new mafOpReparentTo(_R("Reparent to...  \tCtrl+R")),_R("Modify/Fuse"));
  m_Logic->Plug(new mafOpReparentTo(_R("Local reparent to..."), false),_R("Modify/Fuse"));
  m_Logic->Plug(new lhpOpMove(),_R("Modify"));
  m_Logic->Plug(new lhpOpAverageLM(_R("Average landmark")),_R("Create/Derive"));
  m_Logic->Plug(new lhpOpCreateLMCLines(_R("Cloud lines")),_R("Create/Derive"));
  m_Logic->Plug(new lhpOpJoinSurf(_R("JoinSurface")),_R("Create/Derive"));
  m_Logic->Plug(new lhpOpMergeClouds(_R("Merge clouds")),_R("Create/Derive"));
  m_Logic->Plug(new mafOpCreatePlane(_R("Plane")), _R("Create/New/Osteometric Tools"));
  //m_Logic->Plug(new mafOpCreateOsteometricBoard(_R("OsteometricBoard")), _R("Create/New/Osteometric Tools"));
  if(fullVersion)
  {
    m_Logic->Plug(new mafOpCreateVolume(_R("Constant Volume")),_R("Create/New"));

#ifdef MAF_USE_ITK
    m_Logic->Plug(new lhpOpCreateSurfaceScalar(_R("Surface Scalar")),_R("Create/Derive"));
#endif
    m_Logic->Plug(new lhpOpLnSurf(_R("Lineset and surface")),_R("Create/Derive"));
    //m_Logic->Plug(new mafOpCreateRefSys(_R("Refsys")),_R("Create/New"));
    m_Logic->Plug(new mafOpCreateSlicer(_R("Slicer")),_R("Create/Derive"));
    m_Logic->Plug(new medOpFreezeVME(_R("Freeze VME")),_R("Create/Derive"));
    m_Logic->Plug(new medOpRegisterClusters(_R("Register Landmark Cloud")),_R("Modify/Fuse"));
    m_Logic->Plug(new medOpRegisterClusters2(_R("Motion Scale")), _R("Modify/Fuse"));
    m_Logic->Plug(new lhpOpFuseLMScripted(_R("Fuse LM based model")),_R("Modify/Fuse"));
    m_Logic->Plug(new lhpOpRegisterLMScripted(_R("Register Landmark Cloud Tree")),_R("Modify/Fuse"));
    //m_Logic->Plug(new medOpCreateWrappedMeter(_R("Wrapped Meter")),_R("Create/Derive"));
    m_Logic->Plug(new medOpComputeWrapping(_R("Wrapped Action Line")),_R("Create/Derive"));//15-1-2009
    m_Logic->Plug(new medOpCreateMuscleWrapper(_R("Muscle Wrapper")),_R("Create/Derive")); //BES: 14.11.2008
    // m_Logic->Plug(new lhpOpEditMetadata(_R("Metadata Editor")),_R("Modify"));
    m_Logic->Plug(new mafOpFilterSurface(_R("Filter Surface")),_R("Modify"));
    m_Logic->Plug(new mafOpVOIDensityEditor(_R("Volume Density")),_R("Modify"));
    m_Logic->Plug(new medOpMeshDeformation(_R("Deform Surface")), _R("Modify"));
    m_Logic->Plug(new mafOpApplyTrajectory(_R("Apply Trajectory")), _R("Modify"));
   
    m_Logic->Plug(new mafOpExtractIsosurface(_R("Extract Isosurface")),_R("Create/Derive"));
    m_Logic->Plug(new medOpSurfaceMirror(_R("Group/Surface Mirror")),_R("Modify"));
    m_Logic->Plug(new mafOpCrop(_R("Crop Volume")),_R("Modify"));
	m_Logic->Plug(new mafOpCrop3DSurface(_R("Crop 3D Surface")), _R("Modify"));
    m_Logic->Plug(new medOpVolumeResample(_R("Volume Resample")),_R("Modify"));
    m_Logic->Plug(new mafOp2DMeasure(_R("2D Measure")),_R("Measure"));
    m_Logic->Plug(new mafOpVOIDensity(_R("VOI Density")),_R("Measure"));
    m_Logic->Plug(new lhpOpScaleDataset(),_R("Modify"));
    m_Logic->Plug(new medOpCropDeformableROI(_L("Masking")),_L("Modify"));
    m_Logic->Plug(new mafOpImporterVMEDataSetAttributes(_R("VME DataSet Attributes Adder")),_R("Modify"));
    m_Logic->Plug(new medOpClassicICPRegistration(_R("Register Surface")),_R("Modify/Fuse"));
    m_Logic->Plug(new lhpOpAFSys(_R("AFRefsys")),_R("Create/Derive"));
    m_Logic->Plug(new lhpOpKinectAFs(_R("Kinect Refsys")),_R("Create/Derive"));
    m_Logic->Plug(new lhpOpStickPalpation(_R("Wand palpated landmark")),_R("Create/Derive"));

    m_Logic->Plug(new lhpOpHelAxis(_R("Helical axis")),_R("Create/Derive"));
    m_Logic->Plug(new lhpOpTimeReduce(_R("Time reduce")),_R("Modify"));
    m_Logic->Plug(new lhpOpTimeShift(_R("Time shift")),_R("Modify"));
    m_Logic->Plug(new lhpOpLMProj(true, _R("Landmark Cloud Projection")),_R("Create/Derive"));
    m_Logic->Plug(new lhpOpSolidify(_R("Solidify Landmark Cloud")),_R("Create/Derive"));
    m_Logic->Plug(new lhpOpSoftReg(_R("Soft tissue registration")),_R("Create/Derive"));
    m_Logic->Plug(new lhpOpCreateObject<mafVMEBSplineLine>(_R("BSplineLine"), _R("BSplineLine")),_R("Create/New"));
    m_Logic->Plug(new lhpOpCreateObject<mafVMEBSplineSurface>(_R("BSplineSurface"), _R("BSplineSurface")),_R("Create/New"));
    m_Logic->Plug(new lhpOpCreateObject<mafVMEBSplineVolume>(_R("BSplineVolume"), _R("BSplineVolume")),_R("Create/New"));
    m_Logic->Plug(new lhpOpCreateObject<lhpVMELeverArm>(_R("Lever Arm"), _R("Lever Arm")),_R("Create/Derive"));
    m_Logic->Plug(new lhpOpRegSurfWithCloud(_R("Register Surface with Landmark Cloud")),_R("Modify/Fuse"));
    m_Logic->Plug(new lhpOpRepresentInAF(_R("Represent in RefSys")),_R("Modify/Fuse"));
    m_Logic->Plug(new lhpOpLMMirror(_R("Landmark Cloud Mirror")),_R("Modify"));
    m_Logic->Plug(new lhpOpMoveSeq(),_R("Modify"));
    m_Logic->Plug(new lhpOpICPRegFollow(_R("Move Surface As Registered")),_R("Modify/Fuse"));
    m_Logic->Plug(new lhpOpRegression(_R("Regression")),_R("Create/Derive"));
    m_Logic->Plug(new lhpOpFingerStick(_R("Finger stick")),_R("Create/Derive"));
    m_Logic->Plug(new lhpOpCutSurface(_R("CutSurface")),_R("Create/Derive"));
    m_Logic->Plug(new medOpComputeWrapping(_R("Compute Wrapping")),_R("Create/Derive"));
    //m_Logic->Plug(new lhpOpMeanHelAxis(_R("Mean helical axis")),_R("Create/Derive"));
    m_Logic->Plug(new lhpOpRegistration(_R("DSRegistration")),_R("Modify"));
    m_Logic->Plug(new lhpOpKinectModel(_R("KinectModel")),_R("Modify"));  
    m_Logic->Plug(new lhpOpBuildHierarchy(_R("Make hierarchical")),_R("Modify/Fuse"));
    m_Logic->Plug(new lhpOpBonemat(_R("Bonemat")),_R("Modify"));
    m_Logic->Plug(new medOpIterativeRegistration(_R("Iterative Registration")),_R("Modify/Fuse"));
    m_Logic->Plug(new mafOpOpenExternalFile(_R("Open with external program")),_R("Manage")); 
    m_Logic->Plug(new medOpCreateLabeledVolume(_R("Labeled Volume")),_R("Create/Derive"));

    m_Logic->Plug(new mafOpDecomposeTimeVarVME(_R("Decompose Time")),_R("Create/Derive"));
    m_Logic->Plug(new mafOpLabelExtractor(_R("Extract Label")),_R("Create/Derive"));
    m_Logic->Plug(new lhpOpMultiscaleExplore(_R("Multiscale Viewer")),_R("Manage"));
    m_Logic->Plug(new medOpMML(_R("Register from template")),_R("Modify"));
    m_Logic->Plug(new lhpOpComputeTensor(_R("Compute Tensors")), _R("Modify"));

    m_Logic->Plug(new lhpOpTextureOrientation(_R("Texture Orientation")),_R("Create/Derive"));
    m_Logic->Plug(new lhpOpCreateRefSysLM(_R("Create RefSys Landmarks")), _R("Create/Derive"));

  }
  
  //-------------------------------------------------------------
  bool view_visibility = 0;//fullVersion;
  //------------------------- Views -------------------------
  //View Arbitrary Slice
  mafViewArbitrarySlice *ArbitraryView = new mafViewArbitrarySlice(_R("Arbitrary"));
  ArbitraryView->PackageView();
  m_Logic->Plug(ArbitraryView, view_visibility);


  // View DRR
  mafViewVTK *vdrr = new mafViewVTK(_R("DRR"));
  vdrr->PlugVisualPipe(_R("mafVMEVolumeGray"),_R("medPipeVolumeDRR"),MUTEX);
  vdrr->PlugVisualPipe(_R("mafVMEVolumeLarge"),_R("medPipeVolumeDRR"),MUTEX);
  m_Logic->Plug(vdrr, view_visibility);

  // View Analog graph
  mafViewVTK *graph = new mafViewVTK(_R("Analog Graph"), CAMERA_PERSPECTIVE, false);
  graph->PlugVisualPipe(_R("medVMEAnalog"), _R("medPipeGraph"),MUTEX);
  m_Logic->Plug(graph/*, view_visibility*/);

  //View Global Slice
  mafViewGlobalSliceCompound *GlobalSlice = new mafViewGlobalSliceCompound(_R("Global Slice"));
  GlobalSlice->PackageView();
  m_Logic->Plug(GlobalSlice, view_visibility);

  mafViewVTK *viso = new mafViewVTK(_R("Isosurface"));
  viso->PlugVisualPipe(_R("mafVMEVolumeGray"), _R("mafPipeIsosurface"),MUTEX);
  viso->PlugVisualPipe(_R("medVMELabeledVolume"), _R("mafPipeIsosurface"),MUTEX);
  viso->PlugVisualPipe(_R("mafVMEVolumeLarge"),_R("mafPipeIsosurface"),MUTEX);
  m_Logic->Plug(viso, view_visibility);

  mafViewVTK *visoGPU = new mafViewVTK(_R("Isosurface (GPU)"));
  visoGPU->PlugVisualPipe(_R("mafVMEVolumeGray"), _R("mafPipeIsosurfaceGPU"),MUTEX);   //BES: 13.11.2008 - GPU support, mafPipeIsosurfaceGPU to be merged with mafPipeIsosurface in future 
  visoGPU->PlugVisualPipe(_R("medVMELabeledVolume"), _R("mafPipeIsosurfaceGPU"),MUTEX);
  visoGPU->PlugVisualPipe(_R("mafVMEVolumeLarge"), _R("mafPipeIsosurfaceGPU"),MUTEX);
  m_Logic->Plug(visoGPU, view_visibility);

  mafViewOrthoSlice *viewOrthoSlice = new mafViewOrthoSlice(_R("OrthoSlice"));
  viewOrthoSlice->PackageView();
  m_Logic->Plug(viewOrthoSlice/*, view_visibility*/);

  mafViewRXCT *vrxctl = new mafViewRXCT(_R("RXCT"));
  vrxctl->PackageView();
  m_Logic->Plug(vrxctl, view_visibility);

	mafViewVTK *vsurface = new mafViewVTK(_R("Surface"));
	vsurface->PlugVisualPipe(_R("mafVMESurface"),_R("mafPipeSurface"));
  vsurface->PlugVisualPipe(_R("mafVMELandmark"), _R("medPipeTrajectories"));
	m_Logic->Plug(vsurface);

	mafViewVTK *vsurfacet = new mafViewVTK(_R("Textured Surface"));
	vsurfacet->PlugVisualPipe(_R("mafVMELandmark"), _R("medPipeTrajectories"));
	vsurfacet->PlugVisualPipe(_R("mafVMESurface"), _R("mafPipeSurfaceTextured"));
	m_Logic->Plug(vsurfacet);

  mafViewIntGraph *vgraph = new mafViewIntGraph(_R("Biomechanical graph"));
  vgraph->PlugVisualPipe(_R("mafVMEHelAxis"),_R("lhpPipeIntGraphHAxis"));
  vgraph->PlugVisualPipe(_R("medVMEComputeWrapping"),_R("lhpPipeIntGraphPolyline"));
  vgraph->PlugVisualPipe(_R("medVMEWrappedMeter"),_R("lhpPipeIntGraphPolyline"));
  vgraph->PlugVisualPipe(_R("mafVMEMuscleWrapping"), _R("lhpPipeIntGraphPolyline"));
  vgraph->PlugVisualPipe(_R("mafVMEMeter"),_R("lhpPipeIntGraphPolyline"));
  vgraph->PlugVisualPipe(_R("lhpVMELeverArm"),_R("lhpPipeIntGraphPolyline"));
  vgraph->PlugVisualPipe(_R("mafVMEVector"),_R("lhpPipeIntGraphPolyline"));
  vgraph->PlugVisualPipe(_R("medVMEAnalog"),_R("lhpPipeIntGraphAnalog"));
  lhpOpExporterCSVGraph::m_ViewIntGraph = vgraph;
  m_Logic->Plug(vgraph);

  medViewSlicer *slicerView = new medViewSlicer(_R("Slicer"));
  slicerView->PackageView();
  m_Logic->Plug(slicerView, view_visibility);

  lhpViewInfo *igraph = new lhpViewInfo(_R("Info view"));
  igraph->PlugVisualPipe(_R("lhpVMEKMInfo"),_R("lhpPipeInfo"));
  m_Logic->Plug(igraph);
  //temporary for testing
  //mafViewSingleSliceCompound *sliceView = new mafViewSingleSliceCompound(_R("Test Slice"));
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
int lhpFusionBoxApp::OnExit()
//--------------------------------------------------------------------------------
{
  m_Logic.reset();

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
