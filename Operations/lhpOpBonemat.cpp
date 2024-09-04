/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpOpBonemat.cpp,v $
  Language:  C++
  Date:      $Date: 2009-01-20 16:21:36 $
  Version:   $Revision: 1.16.2.1 $
  Authors:   Daniele Giunchi, Stefano Perticoni
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

#include "lhpOpBonemat.h"
#include "lhpProceduralElements.h"

#include "wx/busyinfo.h"

#include "mafGUI.h"

#include "mafDecl.h"
#include "mafVMERoot.h"
#include "mafVMEMesh.h"
#include "mafString.h"
#include "mafAbsMatrixPipe.h"

#include <fstream>
#include <stdio.h>
#include <iostream>
#include <string>
#include <math.h>
#include <stdlib.h>

#include "vtkSmartPointer.h"
#include "vtkUnstructuredGrid.h"
#include "vtkRectilinearGrid.h"
#include "vtkImageData.h"
#include "vtkDoubleArray.h"
#include "vtkDoubleArray.h"
#include "vtkGenericCell.h"
#include "vtkCellData.h"
#include "vtkPointData.h"
#include "vtkCellArray.h"
#include "vtkIntArray.h"
#include "vtkFloatArray.h"
#include "vtkTransformFilter.h"
#include "vtkTransform.h"

#define _DEBUG_BONEMAT
// #define _DEBUG_BONEMAT_GUI
// #define _USE_MAXIMUM_DENSITY_FOR_GROUPING

const bool DEBUG_MODE = true;

const int decimalNumbersNumber = 16;

// Element Properties: element ID, density, Young Module
typedef struct {
  ID_TYPE elementID;  
  double rho;
  double E;
} ElementProp;

int compareE(const void *p1, const void *p2) 
{
  double result;

  result = ((ElementProp *) p2)->E -  ((ElementProp *) p1)->E; // decreasing order  
  if (result < 0)
    return -1;
  if (result > 0)
    return 1;
  return 0;  
}


int compareRO(const void *p1, const void *p2) 
{
  double result;

  result = ((ElementProp *) p2)->rho -  ((ElementProp *) p1)->rho; // decreasing order  
  if (result < 0)
    return -1;
  if (result > 0)
    return 1;
  return 0;  
}

mafCxxTypeMacro(lhpOpBonemat)

//----------------------------------------------------------------------------
lhpOpBonemat::lhpOpBonemat(const mafString& label) : Superclass(label)
//----------------------------------------------------------------------------
{
  m_OpType  = OPTYPE_OP;
  m_Canundo = false;
  m_InputPreserving = false;
  m_ConfigurationFileName = _R("");
  m_FrequencyFileName = _R("");

  m_HU0_d0_el0 = 0;
  m_HU0_d0_el1 = 0;
  m_HU1_d1_el0 = 1;
  m_HU1_d1_el1 = 1;
  
  m_RhoIntercept = 0;
  m_RhoSlope = 1;

  m_a_RhoAshLessThanRhoAsh1 = 0;
  m_b_RhoAshLessThanRhoAsh1 = 1;
  m_c_RhoAshLessThanRhoAsh1 = 1;

  m_a_RhoAshBetweenRhoAsh1andRhoAsh2 = 0;
  m_b_RhoAshBetweenRhoAsh1andRhoAsh2 = 1;
  m_c_RhoAshBetweenRhoAsh1andRhoAsh2 = 1;

  m_a_RhoAshBiggerThanRhoAsh2 = 0;
  m_b_RhoAshBiggerThanRhoAsh2 = 1;
  m_c_RhoAshBiggerThanRhoAsh2 = 1;

  m_a_DER = 0;
  m_b_DER = 1;
  m_c_DER = 1;

  m_Egap = 1;
  m_StepsNumber = 4;
  m_RhoAsh1 = m_RhoAsh2 = 0;

  m_DensityIntervalsNumber = THREE_INTERVALS;

  m_a_OneInterval = 0;
  m_b_OneInterval = 1;
  m_c_OneInterval = 1;

  m_YoungModuleCalculationModality = HU_INTEGRATION;
  m_DensityRelationshipListbox = INTERCEPT_SLOPE;

  m_InputVolume = NULL;
  m_OriginalVMEMesh = NULL;

  //Ro Calibration Flag
  m_RhoCalibrationCorrectionIsActive = 0;
  m_RhoCalibrationCorrectionType       = 0; //equals to single interval

  m_RhoQCT1 = 0;
  m_RhoQCT2 = 0;

  //single interval ro calibration
  m_a_CalibrationCorrection = 0;
  m_b_CalibrationCorrection = 1;

  //three intervals ro calibration
  m_a_RhoQCTLessThanRhoQCT1 = 0;
  m_b_RhoQCTLessThanRhoQCT1 = 1;

  m_a_RhoQCTBetweenRhoQCT1AndRhoQCT2 = 0;
  m_b_RhoQCTBetweenRhoQCT1AndRhoQCT2 = 1;

  m_a_RhoQCTBiggerThanRhoQCT2 = 0;
  m_b_RhoQCTBiggerThanRhoQCT2 = 1;
}
//----------------------------------------------------------------------------
lhpOpBonemat::~lhpOpBonemat()
//----------------------------------------------------------------------------
{
  
}
//----------------------------------------------------------------------------
bool lhpOpBonemat::Accept(mafNode *node)
//----------------------------------------------------------------------------
{ 
  return (node && mafVMEMesh::SafeDownCast(node));
}
//----------------------------------------------------------------------------
mafOp* lhpOpBonemat::Copy()   
//----------------------------------------------------------------------------
{
  lhpOpBonemat *cp = new lhpOpBonemat(GetLabel());
  return cp;
}
//----------------------------------------------------------------------------
// constants
//----------------------------------------------------------------------------
enum BONEMAT_ID
{
  ID_FIRST = MINID,
  ID_OPEN_CONFIGURATION_FILE,
  ID_SAVE_CONFIGURATION_FILE,
  ID_SAVE_CONFIGURATION_FILE_AS,
  ID_OPEN_INPUT_MESH,
  ID_OPEN_INPUT_TAC,
  ID_VOLUME_CHOOSE,
  ID_OUTPUT_MESH_NAME,
  ID_OUTPUT_FREQUENCY_FILE_NAME,
  ID_EXECUTE,

  ID_FIRST_EXPONENTIAL_COEFFICIENTS_VECTOR_V3_0,
  ID_FIRST_EXPONENTIAL_COEFFICIENTS_VECTOR_V3_1,
  ID_FIRST_EXPONENTIAL_COEFFICIENTS_VECTOR_V3_2,

  ID_SECOND_EXPONENTIAL_COEFFICIENTS_VECTOR_V3_0,
  ID_SECOND_EXPONENTIAL_COEFFICIENTS_VECTOR_V3_1,
  ID_SECOND_EXPONENTIAL_COEFFICIENTS_VECTOR_V3_2,

  ID_THIRD_EXPONENTIAL_COEFFICIENTS_VECTOR_V3_0,
  ID_THIRD_EXPONENTIAL_COEFFICIENTS_VECTOR_V3_1,
  ID_THIRD_EXPONENTIAL_COEFFICIENTS_VECTOR_V3_2,


  ID_HU_THRESHOLD,
  
  ID_HU0,
  ID_D0,
  ID_HU1,
  ID_D1,
  
  ID_CALIBRATION_FIRST_POINT,
  ID_CALIBRATION_SECOND_POINT,
  ID_STEPS_NUMBER,
  ID_GAP_VALUE,
  ID_DENSITY_INTERVAL_0,
  ID_DENSITY_INTERVAL_1,

  
  ID_EXPONENTIAL_COEFFICIENTS_VECTOR_V2_0,
  ID_EXPONENTIAL_COEFFICIENTS_VECTOR_V2_1,
  ID_EXPONENTIAL_COEFFICIENTS_VECTOR_V2_2,

  ID_EXPONENTIAL_COEFFICIENTS_VECTOR_V3_SINGLE_DENSITY_INTERVAL_0,
  ID_EXPONENTIAL_COEFFICIENTS_VECTOR_V3_SINGLE_DENSITY_INTERVAL_1,
  ID_EXPONENTIAL_COEFFICIENTS_VECTOR_V3_SINGLE_DENSITY_INTERVAL_2,

  ID_RO_INTERCEPT,
  ID_RO_SLOPE,
  ID_YOUNG_MODULE_CALCULATION_MODALITY,
  ID_RHOASH_DENSITY_INTERVALS_NUMBER, 
  ID_DENSITY_RELATIONSHIP_LISTBOX,

  //correction section
  ID_FLAG_RO_CORRECTION,
  ID_TYPE_RHOQCT_CORRECTION,

  ID_RO_CORRECTION_DENSITY_INTERVAL_0,
  ID_RO_CORRECTION_DENSITY_INTERVAL_1,

  //possible single interval
  ID_RO_CORRECTION_FIRST_EXPONENTIAL_COEFFICIENTS_SINGLE_0,
  ID_RO_CORRECTION_FIRST_EXPONENTIAL_COEFFICIENTS_SINGLE_1,

  //three intervals

  ID_RO_CORRECTION_FIRST_EXPONENTIAL_COEFFICIENTS_VECTOR_0,
  ID_RO_CORRECTION_FIRST_EXPONENTIAL_COEFFICIENTS_VECTOR_1,
  
  ID_RO_CORRECTION_SECOND_EXPONENTIAL_COEFFICIENTS_VECTOR_0,
  ID_RO_CORRECTION_SECOND_EXPONENTIAL_COEFFICIENTS_VECTOR_1,
  
  ID_RO_CORRECTION_THIRD_EXPONENTIAL_COEFFICIENTS_VECTOR_0,
  ID_RO_CORRECTION_THIRD_EXPONENTIAL_COEFFICIENTS_VECTOR_1,
  
  ID_PRINT_DEBUG_INFO,
};

//----------------------------------------------------------------------------
void lhpOpBonemat::CreateGui()
//----------------------------------------------------------------------------
{
  m_Gui = new mafGUI(this);
  m_Gui->Label(_R(""));

  m_Gui->Label(_R("Configuration File:"), true);
  m_Gui->Button(ID_OPEN_CONFIGURATION_FILE, _R("open configuration file"));
  m_Gui->Button(ID_SAVE_CONFIGURATION_FILE, _R("save configuration file"));
  m_Gui->Enable(ID_SAVE_CONFIGURATION_FILE, false); 
  m_Gui->Button(ID_SAVE_CONFIGURATION_FILE_AS, _R("save configuration file as"));
  m_Gui->Divider(2);

  m_Gui->Label(_R("Volume:"), &m_InputVolumeName, true);
  
  m_Gui->Button(ID_VOLUME_CHOOSE, _R("choose volume"));
  m_Gui->Divider(2);
  
  m_Gui->Label(_R("Output Frequency file: "),true);

  mafString wildc = _R("Frequency File (*.*)|*.*");
  m_FrequencyFileName = mafWxToString(wxGetCwd());
  m_FrequencyFileName +=  _R("\\") ;
  m_FrequencyFileName +=  m_Input->GetName();
  m_FrequencyFileName +=  _R("-Freq.txt");
  m_Gui->FileSave(ID_OUTPUT_FREQUENCY_FILE_NAME, _L("Freq file"), &m_FrequencyFileName, wildc);
  
  m_Gui->Divider(2);

#ifdef _DEBUG_BONEMAT_GUI

  const wxString densityRelationship[] = {"points coordinates","intercept slope"};
  m_Gui->Label("density relationship");
  m_Gui->Combo(ID_DENSITY_RELATIONSHIP_LISTBOX, "", &m_DensityRelationshipListbox, 2, densityRelationship);

  
  m_Gui->Double(ID_HU0,"HU0", &m_HU0_d0_el0);
  m_Gui->Double(ID_D0,"D0", &m_HU0_d0_el1);

  m_Gui->Double(ID_HU1,"HU1", &m_HU1_d1_el0);
  m_Gui->Double(ID_D1,"D1", &m_HU1_d1_el1);

  m_Gui->Enable(ID_HU0, false);
  m_Gui->Enable(ID_D0, false);

  m_Gui->Enable(ID_HU1, false);
  m_Gui->Enable(ID_D1, false);

  m_Gui->Divider(2);
#endif

  m_Gui->Label(_R("CT densitometric calibration"), true);
  m_Gui->Label(_R("RhoQCT = a + b * HU"), false);
  
  m_Gui->Double(ID_RO_INTERCEPT, _R("a"), &m_RhoIntercept);
  m_Gui->Double(ID_RO_SLOPE, _R("b"), &m_RhoSlope)   ;
  m_Gui->Divider(2);
  
  m_Gui->Enable(ID_RO_INTERCEPT, true);
  m_Gui->Enable(ID_RO_SLOPE, true);

  // flag ro correction
  m_Gui->Bool(ID_FLAG_RO_CORRECTION, _R("apply calibration correction"), &m_RhoCalibrationCorrectionIsActive,1);
  m_Gui->Divider(2);
  // ro correction (if yes) enable 3x2 gui->double
  //////////////////////////////////////////////////////////////////////////
  
  m_Gui->Divider(2);
  m_Gui->Label(_R("Correction of the calibration"),true);
  m_Gui->Label(_R("RhoAsh = a + b * RhoQCT"),false);
  const mafString densityChoicesRoCalibration[] = {_R("one interval"), _R("three intervals")};
  m_Gui->Combo(ID_TYPE_RHOQCT_CORRECTION, _R(""), &m_RhoCalibrationCorrectionType,2,densityChoicesRoCalibration);
  m_Gui->Divider();

  m_Gui->Double(ID_RO_CORRECTION_FIRST_EXPONENTIAL_COEFFICIENTS_SINGLE_0, _R("a"), &m_a_CalibrationCorrection);
  m_Gui->Double(ID_RO_CORRECTION_FIRST_EXPONENTIAL_COEFFICIENTS_SINGLE_1, _R("b"), &m_b_CalibrationCorrection);

  m_Gui->Divider();

  m_Gui->Double(ID_RO_CORRECTION_DENSITY_INTERVAL_0, _R("RhoQCT1"),&m_RhoQCT1);
  m_Gui->Double(ID_RO_CORRECTION_DENSITY_INTERVAL_1, _R("RhoQCT2"),&m_RhoQCT2);


  m_Gui->Divider();

  m_Gui->Label(_R("RhoQCT < RhoQCT1"));
  m_Gui->Double(ID_RO_CORRECTION_FIRST_EXPONENTIAL_COEFFICIENTS_VECTOR_0, _R("a"), &m_a_RhoQCTLessThanRhoQCT1);
  m_Gui->Double(ID_RO_CORRECTION_FIRST_EXPONENTIAL_COEFFICIENTS_VECTOR_1, _R("b"), &m_b_RhoQCTLessThanRhoQCT1);
  
  m_Gui->Label(_R("RhoQCT1 <= RhoQCT <= RhoQCT2"));
  m_Gui->Double(ID_RO_CORRECTION_SECOND_EXPONENTIAL_COEFFICIENTS_VECTOR_0, _R("a"), &m_a_RhoQCTBetweenRhoQCT1AndRhoQCT2);
  m_Gui->Double(ID_RO_CORRECTION_SECOND_EXPONENTIAL_COEFFICIENTS_VECTOR_1, _R("b"), &m_b_RhoQCTBetweenRhoQCT1AndRhoQCT2);
  
  m_Gui->Label(_R("RhoQCT > RhoQCT2"));
  m_Gui->Double(ID_RO_CORRECTION_THIRD_EXPONENTIAL_COEFFICIENTS_VECTOR_0, _R("a"), &m_a_RhoQCTBiggerThanRhoQCT2);
  m_Gui->Double(ID_RO_CORRECTION_THIRD_EXPONENTIAL_COEFFICIENTS_VECTOR_1, _R("b"), &m_b_RhoQCTBiggerThanRhoQCT2);
  
  m_Gui->Enable(ID_TYPE_RHOQCT_CORRECTION,false);
  EnableRhoQCTDensityInterval(false);
  EnableRhoQCTSingleInterval(false);
  EnableRhoQCTThreeIntervals(false);
  m_Gui->Divider(2);
  //////////////////////////////////////////////////////////////////////////

  m_Gui->Label(_R("density-elasticity relationship"), true);
  m_Gui->Label(_R("E = a + b * RhoAsh^c"), false);
  
  const mafString densityChoices[] = {_R("one interval"), _R("three intervals")};
  m_Gui->Combo(ID_RHOASH_DENSITY_INTERVALS_NUMBER, _R(""), &m_DensityIntervalsNumber,2,densityChoices);
  m_Gui->Divider(2);
  
  m_Gui->Double(ID_EXPONENTIAL_COEFFICIENTS_VECTOR_V3_SINGLE_DENSITY_INTERVAL_0, _R("a"), &m_a_OneInterval);
  m_Gui->Double(ID_EXPONENTIAL_COEFFICIENTS_VECTOR_V3_SINGLE_DENSITY_INTERVAL_1, _R("b"), &m_b_OneInterval);
  m_Gui->Double(ID_EXPONENTIAL_COEFFICIENTS_VECTOR_V3_SINGLE_DENSITY_INTERVAL_2, _R("c"), &m_c_OneInterval);

  m_Gui->Divider();

  m_Gui->Double(ID_DENSITY_INTERVAL_0, _R("RhoAsh1"),&m_RhoAsh1);
  m_Gui->Double(ID_DENSITY_INTERVAL_1, _R("RhoAsh2"),&m_RhoAsh2);

  m_Gui->Label(_R("RhoAsh < RhoAsh1"));
  m_Gui->Double(ID_FIRST_EXPONENTIAL_COEFFICIENTS_VECTOR_V3_0, _R("a"), &m_a_RhoAshLessThanRhoAsh1);
  m_Gui->Double(ID_FIRST_EXPONENTIAL_COEFFICIENTS_VECTOR_V3_1, _R("b"), &m_b_RhoAshLessThanRhoAsh1);
  m_Gui->Double(ID_FIRST_EXPONENTIAL_COEFFICIENTS_VECTOR_V3_2, _R("c"), &m_c_RhoAshLessThanRhoAsh1);
  
  
  m_Gui->Label(_R("RhoAsh1 <= RhoAsh <= RhoAsh2"));
  m_Gui->Double(ID_SECOND_EXPONENTIAL_COEFFICIENTS_VECTOR_V3_0, _R("a"), &m_a_RhoAshBetweenRhoAsh1andRhoAsh2);
  m_Gui->Double(ID_SECOND_EXPONENTIAL_COEFFICIENTS_VECTOR_V3_1, _R("b"), &m_b_RhoAshBetweenRhoAsh1andRhoAsh2);
  m_Gui->Double(ID_SECOND_EXPONENTIAL_COEFFICIENTS_VECTOR_V3_2, _R("c"), &m_c_RhoAshBetweenRhoAsh1andRhoAsh2);

  m_Gui->Label(_R("RhoAsh > RhoAsh2"));
  m_Gui->Double(ID_THIRD_EXPONENTIAL_COEFFICIENTS_VECTOR_V3_0, _R("a"), &m_a_RhoAshBiggerThanRhoAsh2);
  m_Gui->Double(ID_THIRD_EXPONENTIAL_COEFFICIENTS_VECTOR_V3_1, _R("b"), &m_b_RhoAshBiggerThanRhoAsh2);
  m_Gui->Double(ID_THIRD_EXPONENTIAL_COEFFICIENTS_VECTOR_V3_2, _R("c"), &m_c_RhoAshBiggerThanRhoAsh2);

  
  m_Gui->Divider(2);


  //   EnableTwoIntervals(true);

  
  m_Gui->Enable(ID_DENSITY_INTERVAL_0, true);
  m_Gui->Enable(ID_DENSITY_INTERVAL_1, true);

  m_Gui->Enable(ID_RHOASH_DENSITY_INTERVALS_NUMBER, true);
  
  DisableRhoAshThreeIntervals();

  /*m_Gui->Label("second coefficients vector");
  m_Gui->Double(ID_SECOND_EXPONENTIAL_COEFFICIENTS_VECTOR_V3, "", m_Ea1_Eb1_Ec1_V3);
  m_Gui->Label("hu threshold");
  m_Gui->Double(::ID_HU_THRESHOLD, "", &m_HUThreshold);*/


  m_Gui->Divider(); 
  const mafString choices[] = {_R("HU integration"), _R("E integration")};
  m_Gui->Label(_R("Young's modulus ( E ) calculation modality"), mafString(_R("")),true);
  m_Gui->Combo(ID_YOUNG_MODULE_CALCULATION_MODALITY, _R(""), &m_YoungModuleCalculationModality, 2, choices);
  m_Gui->Label(_R("integration steps"));
  m_Gui->Integer(ID_STEPS_NUMBER, _R(""), &m_StepsNumber);
  m_Gui->Label(_R("gap value"));
  m_Gui->Double(::ID_GAP_VALUE, _R(""), &m_Egap);
  m_Gui->Divider();

  //#ifndef _DEBUG_BONEMAT

  m_Gui->Button(ID_EXECUTE, _R("execute"));
//#endif
  
  m_Gui->Divider(2);
  m_Gui->Button(ID_PRINT_DEBUG_INFO, _R("Print Debug Info"));
  m_Gui->Divider(2);

  m_Gui->OkCancel();
  m_Gui->Label(_R(""));
  m_Gui->Label(_R(""));

  mafNEW(m_OriginalVMEMesh);
  m_OriginalVMEMesh->DeepCopy(mafVMEMesh::SafeDownCast(m_Input));
  m_OriginalVMEMesh->Update();

}

//----------------------------------------------------------------------------
void lhpOpBonemat::OpRun()   
//----------------------------------------------------------------------------
{
  CreateGui();
	ShowGui();
}

//----------------------------------------------------------------------------
int lhpOpBonemat::SaveConfigurationFileAs()
//----------------------------------------------------------------------------
{
  mafString initialFileName;
  initialFileName = mafGetApplicationDirectory();
  initialFileName.append(_R("\\newConfigurationFile.conf"));

  mafString wildc = _R("configuration file (*.conf)|*.conf");
  mafString newFileName = mafGetSaveFile(initialFileName, wildc);
 
  if (newFileName.empty()) return MAF_ERROR;

  return SaveConfigurationFile(newFileName.GetCStr());
}

//----------------------------------------------------------------------------
void lhpOpBonemat::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
	{
    switch(e->GetId())
    {
      case wxOK:
        {
          //WORKAROUND CODE 
          //referred to bug 933
          {mafEvent evUnq(this,VME_SHOW, m_Input, false); mafEventMacro(evUnq);}
          {mafEvent evUnq(this,VME_SHOW, m_Input, true); mafEventMacro(evUnq);}
          //END WORKAROUND CODE
          OpStop(OP_RUN_OK);
        }
      break;
      case wxCANCEL:
        mafVMEMesh::SafeDownCast(m_Input)->DeepCopy(m_OriginalVMEMesh);
        OpStop(OP_RUN_CANCEL);
      break;
  
      
      case ID_OPEN_CONFIGURATION_FILE:
      {
         OpenConfigurationFile();
      }  
      break;

      case ID_SAVE_CONFIGURATION_FILE:
      {
        SaveConfigurationFile(m_ConfigurationFileName.GetCStr());
      }
      break;

      case ID_SAVE_CONFIGURATION_FILE_AS	:
      {
        SaveConfigurationFileAs();
      }
      break;

      case ID_RHOASH_DENSITY_INTERVALS_NUMBER	:
      {
        UpdateRhoAshGuiOnRhoAshIntervalsNumberChange();        
      }
      break;
           
      case ID_FLAG_RO_CORRECTION:
      case ID_TYPE_RHOQCT_CORRECTION:
        {
          m_Gui->Enable(ID_TYPE_RHOQCT_CORRECTION,m_RhoCalibrationCorrectionIsActive?true:false);
          EnableRhoQCTDensityInterval(m_RhoCalibrationCorrectionIsActive && m_RhoCalibrationCorrectionType == THREE_INTERVALS ? true:false);
          EnableRhoQCTSingleInterval(m_RhoCalibrationCorrectionIsActive && m_RhoCalibrationCorrectionType == SINGLE_INTERVAL);
          EnableRhoQCTThreeIntervals(m_RhoCalibrationCorrectionIsActive && m_RhoCalibrationCorrectionType == THREE_INTERVALS);
        }
        break;

      case ID_YOUNG_MODULE_CALCULATION_MODALITY	:
      {
        if (m_YoungModuleCalculationModality == HU_INTEGRATION)
        {            
            // DisableRhoAshThreeIntervals();

        } 
        else if (m_YoungModuleCalculationModality == YOUNG_MODULE_INTEGRATION )
        {
            // UpdateRhoAshGuiOnRhoAshIntervalsNumberChange();
        }
      }
      break;
            
      case ID_DENSITY_RELATIONSHIP_LISTBOX	:
      {
        if (m_DensityRelationshipListbox == POINTS_COORDINATES)
        {
          m_Gui->Enable(ID_HU0, true);
          m_Gui->Enable(ID_D0, true);
          
          m_Gui->Enable(ID_HU1, true);
          m_Gui->Enable(ID_D1, true);


          m_Gui->Enable(ID_RO_INTERCEPT, false);
          m_Gui->Enable(ID_RO_SLOPE, false);
        } 

        // TODO: this seems the only one used, ask to the application experts...
        else if (m_DensityRelationshipListbox == INTERCEPT_SLOPE)
        {
          m_Gui->Enable(ID_HU0, false);
          m_Gui->Enable(ID_D0, false);

          m_Gui->Enable(ID_HU1, false);
          m_Gui->Enable(ID_D1, false);

          m_Gui->Enable(ID_RO_INTERCEPT, true);
          m_Gui->Enable(ID_RO_SLOPE, true);
        }
      }
      break;
            
      case ID_EXECUTE	:
      {
        Execute();

      }
      break;
      case ID_VOLUME_CHOOSE:
        {
          // select here volume
          m_InputVolume = mafVMEVolumeGray::SafeDownCast(VolumeSelection());
          if(m_InputVolume)
          {
            m_InputVolume->GetOutput()->Update();
            m_InputVolume->Update();
            m_InputVolumeName =  m_InputVolume->GetName();
            m_Gui->Update();
          }
          else
          {
            m_InputVolumeName = _R("");
            m_Gui->Update();
          }
        }
        break;
      case VME_ADD:
        {
          //trap the VME_ADD of the mmoCollapse and mmoExplode to update the
          //m_Input, then forward the message to mafDMLlogicMDI
          this->m_Input = e->GetVme();
          {mafEvent evUnq(this,VME_ADD,this->m_Input); mafEventMacro(evUnq);}
        }
        break;
      case ID_PRINT_DEBUG_INFO:
      {
        std::ostringstream stringStream;
        PrintSelf(stringStream);
        mafLogMessage(_M(stringStream.str().c_str()));
      }
      break;
      default:
        mafEventMacro(*e);
      break;
    }
	}
}
//----------------------------------------------------------------------------
void lhpOpBonemat::OpStop(int result)
//----------------------------------------------------------------------------
{
  mafDEL(m_OriginalVMEMesh);
	HideGui();
	{mafEvent evUnq(this,result); mafEventMacro(evUnq);}        
}

//----------------------------------------------------------------------------
const char* lhpOpBonemat::GetConfigurationFileName()
//----------------------------------------------------------------------------
{
  return m_ConfigurationFileName.GetCStr();
}

//----------------------------------------------------------------------------
void lhpOpBonemat::SetConfigurationFileName(const char* name)
//----------------------------------------------------------------------------
{
  m_ConfigurationFileName = _R(name);  
}

//----------------------------------------------------------------------------
const char* lhpOpBonemat::GetFrequencyFileName()
//----------------------------------------------------------------------------
{
  return m_FrequencyFileName.GetCStr();
}

//----------------------------------------------------------------------------
void lhpOpBonemat::SetFrequencyFileName(const char* name)
//----------------------------------------------------------------------------
{
  m_FrequencyFileName = _R(name);  
}


//----------------------------------------------------------------------------
int lhpOpBonemat::OpenConfigurationFile()
//----------------------------------------------------------------------------
{
  mafString wildcconf = _R("conf Data (*.conf)|*.conf");

  mafString initial = mafGetApplicationDirectory();

  mafString returnString = mafGetOpenFile(_R(""), wildcconf);

  if (returnString.empty())
  {
    return 1;
  }

  m_ConfigurationFileName = returnString;  
  
  int result = LoadConfigurationFile(m_ConfigurationFileName.GetCStr());


   m_Gui->Enable(ID_SAVE_CONFIGURATION_FILE,true);
   {mafEvent evUnq(this, ID_FLAG_RO_CORRECTION); OnEvent(&evUnq);}
   
   m_Gui->Update();

   return result;
}

//----------------------------------------------------------------------------
int lhpOpBonemat::HUIntegration()
//----------------------------------------------------------------------------
{
  std::ostringstream logStringStream;

  
  FILE  *freq_fp;
  
  ID_TYPE  numElements, numMats, elementNumber, i, numElementNodes;
  Element *element;  
  DataSet *dataset;
  double HU, E, ro, Ni = 0.3;
  ElementProp *rhoSource, **materialProperties;
  ID_TYPE freq;
  
  if ( (freq_fp = fopen(m_FrequencyFileName.GetCStr(), "w")) == NULL)
  {
    if (GetTestMode() == false)
    {
      wxMessageBox("Frequency file can't be opened");
    }
   
    assert(false);
    return MAF_ERROR;
  }
  
  mafVMEMesh *inMesh =  mafVMEMesh::SafeDownCast(m_Input);
  assert(inMesh);

  mafMatrix identityMatrix;

  mafMatrix inputMeshABSMatrix = inMesh->GetAbsMatrixPipe()->GetMatrix();

  // just to test that equals is working
  assert(inputMeshABSMatrix.Equals(&inputMeshABSMatrix));

  bool inputMeshABSMatrixEqualToIdentity = inputMeshABSMatrix.Equals(&identityMatrix);

  vtkUnstructuredGrid *inputUnstructuredGrid = inMesh->GetUnstructuredGridOutput()->GetUnstructuredGridData();
  assert(inputUnstructuredGrid);

  // this wil feed the algorithm...
  inMesh->GetUnstructuredGridOutput()->Update();
  
  // if VME matrix is not identity apply it to dataset
  vtkTransform *transform = NULL;
  vtkTransformFilter *transformFilter = NULL;
  vtkUnstructuredGrid *inputUGTransformed = NULL;

  if (inputMeshABSMatrixEqualToIdentity)
  {
    // do not transform geometry
    if (DEBUG_MODE)
    {
      std::ostringstream stringStream;
      stringStream << "Not applying abs pose to geometry... DeepCopy not needed"  << std::endl;
      mafLogMessage(_M(stringStream.str().c_str()));
    }

  } 
  else
  {
    // apply abs matrix to geometry
    transform = vtkTransform::New();
    transform->SetMatrix(inputMeshABSMatrix.GetVTKMatrix());

    // to delete
    transformFilter = vtkTransformFilter::New();

    inputUGTransformed = vtkUnstructuredGrid::New();

    transformFilter->SetInputConnection(inMesh->GetUnstructuredGridOutput()->GetVTKOutputPort());
    transformFilter->SetTransform(transform);
    transformFilter->Update();

    inputUGTransformed->DeepCopy(transformFilter->GetUnstructuredGridOutput());

    if (DEBUG_MODE)
    {
      std::ostringstream stringStream;
      stringStream << "Applying abs pose to geometry... Now working on DeepCopy"  << std::endl;
      mafLogMessage(_M(stringStream.str().c_str()));
    }

    inputUnstructuredGrid = inputUGTransformed;
  }


  // read all the pointsToBeExported in memory (vnl_matrix)

  vtkDataSet *volume = NULL;
  //scalars
  Scalars * scalars = NULL;
  if(m_InputVolume)
  {
    if(volume = vtkImageData::SafeDownCast(mafVMEVolumeGray::SafeDownCast(m_InputVolume)->GetOutput()->GetVTKData()))
    {
      vtkImageData * imagedata = vtkImageData::SafeDownCast(volume);
      m_InputVolume->GetOutput()->Update();
      dataset = SP::New();
      int dimensions[3]; 
      double spacing[3];
      imagedata->GetDimensions(dimensions);
      imagedata->GetSpacing(spacing);
      dataset->SetDimensions(dimensions[0], dimensions[1], dimensions[2]); 
      ((SP*)dataset)->SetSpacing(spacing[0], spacing[1], spacing[2]);
      double origin[3];
      imagedata->GetOrigin(origin);
      ((SP*)dataset)->SetOrigin(origin[0], origin[1], origin[2]);


      //if volume scalars type is short, unsigned short or unsigned char, create this scalar and put it into dataset

      if (imagedata->GetScalarType() == VTK_UNSIGNED_CHAR) 
      {
        scalars = TScalars<unsigned char>::New();
        scalars->SetNumberOfScalars(volume->GetNumberOfPoints());
      }
      else if (imagedata->GetScalarType() == VTK_UNSIGNED_SHORT)
      {
        scalars = TScalars<unsigned short>::New();
        scalars->SetNumberOfScalars(volume->GetNumberOfPoints());
      }
      else if (imagedata->GetScalarType() == VTK_SHORT)
      {
        scalars = TScalars<short>::New();
        scalars->SetNumberOfScalars(volume->GetNumberOfPoints());
      }

      dataset->SetScalars(scalars);
      for(int j= 0 ; j< imagedata->GetPointData()->GetScalars()->GetNumberOfTuples(); j++)
      {
        dataset->GetScalars()->SetScalar(j, imagedata->GetPointData()->GetScalars()->GetTuple1(j));
      }



    }
    else if(volume = vtkRectilinearGrid::SafeDownCast(mafVMEVolumeGray::SafeDownCast(m_InputVolume)->GetOutput()->GetVTKData()))
    {

      vtkRectilinearGrid *rectilinearGrid = vtkRectilinearGrid::SafeDownCast(volume);
      m_InputVolume->GetOutput()->Update();

      dataset = RG::New();
      int dimensions[3];

      rectilinearGrid->GetDimensions(dimensions);

      dataset->SetDimensions(dimensions[0], dimensions[1], dimensions[2]); 

      int lenghtX,lenghtY,lenghtZ;
      vtkDataArray *vtkArrayX;
      vtkDataArray *vtkArrayY;
      vtkDataArray *vtkArrayZ;

      if(vtkDoubleArray::SafeDownCast(rectilinearGrid->GetXCoordinates()))
      {
        vtkArrayX = vtkDoubleArray::SafeDownCast(rectilinearGrid->GetXCoordinates());
        vtkArrayY = vtkDoubleArray::SafeDownCast(rectilinearGrid->GetYCoordinates());
        vtkArrayZ = vtkDoubleArray::SafeDownCast(rectilinearGrid->GetZCoordinates());
      }
      else if(vtkFloatArray::SafeDownCast(rectilinearGrid->GetXCoordinates()))
      {
        vtkArrayX = vtkFloatArray::SafeDownCast(rectilinearGrid->GetXCoordinates());
        vtkArrayY = vtkFloatArray::SafeDownCast(rectilinearGrid->GetYCoordinates());
        vtkArrayZ = vtkFloatArray::SafeDownCast(rectilinearGrid->GetZCoordinates());
      }

      lenghtX = vtkArrayX->GetNumberOfTuples();
      lenghtY = vtkArrayY->GetNumberOfTuples();
      lenghtZ = vtkArrayZ->GetNumberOfTuples();

      float *arrayX = new float[lenghtX];
      float *arrayY = new float[lenghtY];
      float *arrayZ = new float[lenghtZ];

      for(int i=0; i< lenghtX; i++)
      {
        arrayX[i] = vtkArrayX->GetTuple1(i);
      }

      for(int i=0; i< lenghtY; i++)
      {
        arrayY[i] = vtkArrayY->GetTuple1(i);
      }

      for(int i=0; i< lenghtZ; i++)
      {
        arrayZ[i] = vtkArrayZ->GetTuple1(i);
      }

      ((RG*)dataset)->SetCoords(arrayX,arrayY,arrayZ);


      //if volume scalars type is short, unsigned short or unsigned char, create this scalar and put it into dataset

      if (rectilinearGrid->GetPointData()->GetScalars()->GetDataType() == VTK_UNSIGNED_CHAR) 
      {
        scalars = TScalars<unsigned char>::New();
        scalars->SetNumberOfScalars(volume->GetNumberOfPoints());
      }
      else if (rectilinearGrid->GetPointData()->GetScalars()->GetDataType() == VTK_UNSIGNED_SHORT)
      {
        scalars = TScalars<unsigned short>::New();
        scalars->SetNumberOfScalars(volume->GetNumberOfPoints());
      }
      else if (rectilinearGrid->GetPointData()->GetScalars()->GetDataType() == VTK_SHORT)
      {
        scalars = TScalars<short>::New();
        scalars->SetNumberOfScalars(volume->GetNumberOfPoints());
      }

      dataset->SetScalars(scalars);
      for(int j= 0 ; j< rectilinearGrid->GetPointData()->GetScalars()->GetNumberOfTuples(); j++)
      {
        dataset->GetScalars()->SetScalar(j, rectilinearGrid->GetPointData()->GetScalars()->GetTuple1(j));
      }
    }

  } 
  else
  {
    wxMessageBox("Must select a volume");
    return 1;
  }
  numElements = inputUnstructuredGrid->GetNumberOfCells();

  //  COMPUTE ELEMENTS DATA
  /*logStringStream << "-- computing elements data bonemat v2\n";
  mafLogMessage("%s",logStringStream.str().c_str());
  logStringStream.str("");*/

  // Array containing properties for each element from 0 to numElements - 1
  rhoSource = new ElementProp[numElements];

  vtkIntArray *arrayMaterial = NULL;
  vtkDoubleArray *arrayE = NULL;
	vtkDoubleArray *arrayPoisson = NULL;
  vtkDoubleArray *arrayRho = NULL;
  
	
  vtkNEW(arrayMaterial);
  arrayMaterial->SetName("material");

  vtkNEW(arrayE);
  arrayE->SetName("EX");

  vtkNEW(arrayPoisson);
  arrayPoisson->SetName("NUXY");

  vtkNEW(arrayRho);
  arrayRho->SetName("DENS");


 
  materialProperties = new ElementProp*[numElements];

  double ROSlope = -1;
  double ROIntercept = -1;

  if (m_DensityRelationshipListbox = POINTS_COORDINATES)
  {
    ROSlope = (m_HU1_d1_el1 - m_HU0_d0_el1) / double(m_HU1_d1_el0 - m_HU0_d0_el0);
    ROIntercept = m_HU0_d0_el1 - m_HU0_d0_el0 * m_RhoSlope;
  }
  else
  {
    ROSlope = m_RhoSlope;
    ROIntercept = m_RhoIntercept;
  }


  if (GetTestMode() == false)
  {
    wxBusyInfo wait_info("Computing elements data...");
    {mafEvent evUnq(this,PROGRESSBAR_SHOW); mafEventMacro(evUnq);}
  }
  
  //{mafEvent evUnq(this,PROGRESSBAR_SET_TEXT, ""); mafEventMacro(evUnq);}
  long progress = 0;

  for (elementNumber=0; elementNumber < numElements; elementNumber++) 
  {  
    vtkCell *cell;
    cell = inputUnstructuredGrid->GetCell(elementNumber);

    
    numElementNodes = cell->GetNumberOfPoints();

    //element control
    if (numElementNodes == 4) 
      element = Tetra::New();
    else if (numElementNodes == 10) 
      element = Tetra10::New();
    else if (numElementNodes == 8 || numElementNodes == 20) 
      element = Hexa::New();
    else
      element = Wedge::New();

    
    for(int num = 0; num < numElementNodes; num++)
    {
      Node *current = new Node;
      current->key = cell->GetPointId(num);
      double coordinates[3];
      cell->GetPoints()->GetPoint(num, coordinates);
      current->x[0] = coordinates[0];
      current->x[1] = coordinates[1];
      current->x[2] = coordinates[2];

      element->SetNode(num, current);
    }

     HU = element->ComputeScalar(dataset, m_StepsNumber);

     for(int h=0; h<numElementNodes; h++)
     {
       delete element->GetNode(h);
     }
     element->Delete();
     delete element;
     
    // rho = a + b * HU
    rhoSource[elementNumber].rho = ROIntercept + ROSlope * HU;
    arrayRho->InsertNextTuple1(ROIntercept + ROSlope * HU);
    if (arrayRho->GetValue(elementNumber) <= 0) 
    {
      arrayRho->SetValue(elementNumber, 1e-6);
      rhoSource[elementNumber].rho = 1e-6;
    }

    //RO CORRECTION///////////////////////////////////////////////////////////////
    // rhoSource[elementNumber].rho, arrayRho->GetValue(elementNumber)
    if(m_RhoCalibrationCorrectionIsActive)
    {
      if(m_RhoCalibrationCorrectionType == SINGLE_INTERVAL)
      {
        rhoSource[elementNumber].rho = m_a_CalibrationCorrection + m_b_CalibrationCorrection * rhoSource[elementNumber].rho;
        arrayRho->SetValue(elementNumber,rhoSource[elementNumber].rho);
      }
      else if (m_RhoCalibrationCorrectionType == THREE_INTERVALS)
      {
        if (rhoSource[elementNumber].rho < m_RhoQCT1)
        {
          rhoSource[elementNumber].rho = m_a_RhoQCTLessThanRhoQCT1 + m_b_RhoQCTLessThanRhoQCT1 * rhoSource[elementNumber].rho;
          arrayRho->SetValue(elementNumber,rhoSource[elementNumber].rho);
        } 
        else if (m_RhoQCT1 <= rhoSource[elementNumber].rho  && rhoSource[elementNumber].rho <= m_RhoQCT2)
        {
          rhoSource[elementNumber].rho = m_a_RhoQCTBetweenRhoQCT1AndRhoQCT2 + m_b_RhoQCTBetweenRhoQCT1AndRhoQCT2 * rhoSource[elementNumber].rho;
          arrayRho->SetValue(elementNumber,rhoSource[elementNumber].rho);
        }
        else if (rhoSource[elementNumber].rho > m_RhoQCT2)
        {
          rhoSource[elementNumber].rho = m_a_RhoQCTBiggerThanRhoQCT2 + m_b_RhoQCTBiggerThanRhoQCT2 * rhoSource[elementNumber].rho;
          arrayRho->SetValue(elementNumber,rhoSource[elementNumber].rho);
        }
      }

    }
    
    //////////////////////////////////////////////////////////////////////////////
    // E = a + b * rho ^ c

    // Element Properties: element ID, density, Young Module
    //typedef struct {
    //  ID_TYPE elementID;  
    //  double rho;
    //  double E;
    //} ElementProp;

    double rho = rhoSource[elementNumber].rho;
    double youngModule = -1;
    
    if (m_DensityIntervalsNumber == SINGLE_INTERVAL)
    {
       youngModule = m_a_OneInterval + m_b_OneInterval * pow(rho, m_c_OneInterval); 
    } 

    else if (m_DensityIntervalsNumber == THREE_INTERVALS)
    {
      if (rho < m_RhoAsh1)
      {
        youngModule = m_a_RhoAshLessThanRhoAsh1 + 
          m_b_RhoAshLessThanRhoAsh1 * pow(rho, m_c_RhoAshLessThanRhoAsh1);
      } 
      else if (m_RhoAsh1 <= rho  && rho <= m_RhoAsh2)
      {
        youngModule = m_a_RhoAshBetweenRhoAsh1andRhoAsh2 + 
          m_b_RhoAshBetweenRhoAsh1andRhoAsh2 * pow(rho, m_c_RhoAshBetweenRhoAsh1andRhoAsh2);

      }
      else if (rho > m_RhoAsh2)
      {
        youngModule = m_a_RhoAshBiggerThanRhoAsh2 + 
          m_b_RhoAshBiggerThanRhoAsh2 * pow(rho, m_c_RhoAshBiggerThanRhoAsh2);

      }
    }

    if (youngModule <= 0) {
      youngModule = 1e-6;
    } 

    arrayE->InsertNextTuple1(youngModule);

    rhoSource[elementNumber].E = youngModule;
    rhoSource[elementNumber].elementID = elementNumber;
    
		arrayPoisson->InsertNextTuple1(Ni);
    arrayMaterial->InsertNextTuple1(0);

    progress = (elementNumber + 1) * 100 / numElements;
    {mafEvent evUnq(this,PROGRESSBAR_SET_VALUE,(intptr_t)progress); mafEventMacro(evUnq);}

  }
  
  {mafEvent evUnq(this,PROGRESSBAR_HIDE); mafEventMacro(evUnq);}

  vtkNew<vtkPoints> pts;
  vtkNew<vtkCellArray> cells;

	vtkNew<vtkUnstructuredGrid> outputUG;
  pts->DeepCopy(inputUnstructuredGrid->GetPoints());
  cells->DeepCopy(inputUnstructuredGrid->GetCells());
  outputUG->SetPoints(pts);
  outputUG->SetCells(inputUnstructuredGrid->GetCellTypesArray(),inputUnstructuredGrid->GetCellLocationsArray(),cells);
	//outputUG->Update();

	vtkCellData *outCellData = outputUG->GetCellData();
  vtkPointData *outPointData = outputUG->GetPointData();
  vtkFieldData *outFieldData = outputUG->GetFieldData();

  outPointData->DeepCopy(inputUnstructuredGrid->GetPointData());
  outCellData->DeepCopy(inputUnstructuredGrid->GetCellData());
  outFieldData->DeepCopy(inputUnstructuredGrid->GetFieldData());

  outCellData->AddArray(arrayMaterial);
  outCellData->AddArray(arrayE);
	outCellData->AddArray(arrayPoisson);
  outCellData->AddArray(arrayRho);

	outputUG->Modified();
  //outputUG->Update();
	
  // COMPUTE MATERIALS & WRITE FREQUENCY FILE

  qsort(rhoSource, numElements, sizeof(ElementProp), compareE);

  /*logStringStream << "RO2: " << rhoSource[0].ro << "\n";
  logStringStream << "RO1: " << rhoSource[numElements - 1].ro << "\n";
  logStringStream << "Emax: " << rhoSource[0].E << "\n";
  logStringStream << "Emin: " << rhoSource[numElements - 1].E << "\n\n";*/

  /*logStringStream << "-- Writing frequency file\n";
  mafLogMessage("%s",logStringStream.str().c_str());
  logStringStream.str("");*/

  fprintf(freq_fp, "ro \t\t E \t\t NUMBER OF ELEMENTS\n\n");

  freq = 0;
  numMats = 1;
  ro = rhoSource[0].rho;
  E = rhoSource[0].E;  
  materialProperties[0] = &rhoSource[0];
  for (elementNumber = 0; elementNumber < numElements; elementNumber++) {
    if (E - rhoSource[elementNumber].E > m_Egap) {
      fprintf(freq_fp, "%f \t %f \t %d\n", ro, E, freq); 
      materialProperties[numMats] = &rhoSource[elementNumber];
      numMats++;
      E = rhoSource[elementNumber].E;
      ro = rhoSource[elementNumber].rho;
      freq = 1;
    }
    else
      freq++;
  }
  fprintf(freq_fp, "%f \t %f \t %d\n\n", ro, E, freq);
  fclose(freq_fp);


  //fielddata materials
  vtkFieldData *fdata = vtkFieldData::New();

  std::vector<mafString> stringVector;

  // correspondence material_id to insert in fielddata
  stringVector.push_back(_R("material_id"));

  vtkDoubleArray *iarr = vtkDoubleArray::New();
  iarr->SetName(stringVector[0].GetCStr());
  iarr->SetNumberOfValues(numMats);
  for (int j = 0; j < numMats; j++)
  {
    int materialAnsysId = j + 1;
    iarr->InsertValue(j, materialAnsysId/*materialProperties[j]->id*/);
  }

  // add the ith data array to the field data
  fdata->AddArray(iarr);
  //clean up
  iarr->Delete();

  
  stringVector.push_back(_R("EX"));
  stringVector.push_back(_R("NUXY"));
  stringVector.push_back(_R("DENS"));

  // create field data data array
  for (i = 1; i < stringVector.size(); i++)
  {
    // create the ith data array
    vtkDoubleArray *darr = vtkDoubleArray::New();
    darr->SetName(stringVector[i].GetCStr());
    darr->SetNumberOfValues(numMats);

    for (int j = 0; j < numMats; j++)
    {
      // fill ith data array with jth value 
      // cycle on materials
      if(stringVector[i] == _R("EX"))
        darr->InsertValue(j, materialProperties[j]->E);
      else if(stringVector[i] == _R("NUXY"))
        darr->InsertValue(j, 0.3);
      else if(stringVector[i] == _R("DENS"))
        darr->InsertValue(j, materialProperties[j]->rho);
       
    }
    // add the ith data array to the field data
    fdata->AddArray(darr);

    //clean up
    darr->Delete();
  }

  outputUG->SetFieldData(fdata);
  
  outputUG->Modified();
  //outputUG->Update();

  //grouping
  for(int currentCell = 0 ; currentCell<outputUG->GetNumberOfCells(); currentCell++)
  {
    double minDiff = VTK_DOUBLE_MAX;
    int index;
    for(int currentE=0; currentE<fdata->GetArray("EX")->GetNumberOfTuples(); currentE++)
    {
      double valueE = vtkDoubleArray::SafeDownCast(fdata->GetArray("EX"))->GetValue(currentE);
      double valueECell = arrayE->GetValue(currentCell);
      if((valueE-valueECell)< m_Egap && (valueE-valueECell)>= -(10e-5))
      {
        index = currentE;
      }      
    }

    double val = vtkDoubleArray::SafeDownCast(fdata->GetArray("EX"))->GetValue(index);
    arrayE->SetTuple1(currentCell,val);
    val = vtkDoubleArray::SafeDownCast(fdata->GetArray("DENS"))->GetValue(index);
    arrayRho->SetValue(currentCell,val);  
  
    int materialAnsysId = index + 1;
    arrayMaterial->SetValue(currentCell,materialAnsysId);
  }

  outputUG->Modified();
  //outputUG->Update();

  if (inputMeshABSMatrixEqualToIdentity)
  {
    // nothing to do
  } 
  else
  {
    // copy back old, not transformed, geometry in output data     
    vtkUnstructuredGrid *inputUG = \
    inMesh->GetUnstructuredGridOutput()->GetUnstructuredGridData();
    outputUG->GetPoints()->DeepCopy(inputUG->GetPoints());
  }

  // input modified in place
  mafVMEMesh::SafeDownCast(m_Input)->SetData(outputUG, 0);

  // clean up
  vtkDEL(inputUGTransformed);
  vtkDEL(transform);
  vtkDEL(transformFilter);

  fdata->Delete();

  vtkDEL(arrayMaterial);
  vtkDEL(arrayE);
  vtkDEL(arrayPoisson);
  vtkDEL(arrayRho);

  if(dataset)
  {
    dataset->Delete();
    delete dataset;
  }

  delete [] rhoSource;
  delete [] materialProperties;

  //logStringStream <<"Number of materials: " << numMats << std::endl << std::endl;

  return MAF_OK;
}


//----------------------------------------------------------------------------
int lhpOpBonemat::YoungModuleIntegration()
//----------------------------------------------------------------------------
{
  std::ostringstream logStringStream;
  logStringStream.precision(decimalNumbersNumber);
    
  FILE  *freq_fp;
  
  ID_TYPE numElements, numMats, id, i, numElementNodes;
  Element *element;  
  DataSet *dataset;
  double HU, E, ro, Ni = 0.3;
  ID_TYPE freq;
  char *header = "%2d%8d%8d%8d%8d%8d%8d%8d%8d\n";
  char *nodedata = "%16.8E%16.8E%16.8E\n";
  char *matdata = "%16.9E%16.9E%16.9E%16.9E%16.9E\n" ;
  char *elemdata1 = "%8d%8d%8d%8d%16.9E%16.9E%16.9E\n";

  if ((freq_fp = fopen(m_FrequencyFileName.GetCStr(), "w")) == NULL)
  {
    if (GetTestMode() == false)
    {
      wxMessageBox("Frequency file can't be opened");
    }
    
    return MAF_ERROR;
  }
  
  mafVMEMesh *inMesh =  mafVMEMesh::SafeDownCast(m_Input);
  assert(inMesh);

  mafMatrix identityMatrix;

  mafMatrix inputMeshABSMatrix = inMesh->GetAbsMatrixPipe()->GetMatrix();

  // just to test that equals is working
  assert(inputMeshABSMatrix.Equals(&inputMeshABSMatrix));

  bool inputMeshABSMatrixEqualToIdentity = inputMeshABSMatrix.Equals(&identityMatrix);

  vtkUnstructuredGrid *inputUnstructuredGrid = inMesh->GetUnstructuredGridOutput()->GetUnstructuredGridData();
  assert(inputUnstructuredGrid);

  // this wil feed the algorithm...
  inMesh->GetUnstructuredGridOutput()->Update();

  // if VME matrix is not identity apply it to dataset
  vtkTransform *transform = NULL;
  vtkTransformFilter *transformFilter = NULL;
  vtkUnstructuredGrid *inputUGTransformed = NULL;

  if (inputMeshABSMatrixEqualToIdentity)
  {
    // do not transform geometry
    if (DEBUG_MODE)
    {
      std::ostringstream stringStream;
      stringStream << "Not applying abs pose to geometry... DeepCopy not needed"  << std::endl;
      mafLogMessage(_M(stringStream.str().c_str()));
    }

  } 
  else
  {
    // apply abs matrix to geometry
    transform = vtkTransform::New();
    transform->SetMatrix(inputMeshABSMatrix.GetVTKMatrix());

    // to delete
    transformFilter = vtkTransformFilter::New();

    inputUGTransformed = vtkUnstructuredGrid::New();

    transformFilter->SetInputData(inputUnstructuredGrid);
    transformFilter->SetTransform(transform);
    transformFilter->Update();

    inputUGTransformed->DeepCopy(transformFilter->GetUnstructuredGridOutput());

    if (DEBUG_MODE)
    {
      std::ostringstream stringStream;
      stringStream << "Applying abs pose to geometry... Now working on DeepCopy"  << std::endl;
      mafLogMessage(_M(stringStream.str().c_str()));
    }

    inputUnstructuredGrid = inputUGTransformed;
  }


  vtkDataSet *volume = NULL;
  //scalars
  Scalars * scalars = NULL;
  if(m_InputVolume)
  {
    
    if(volume = vtkImageData::SafeDownCast(mafVMEVolumeGray::SafeDownCast(m_InputVolume)->GetOutput()->GetVTKData()))
    {
      vtkImageData * imagedata = vtkImageData::SafeDownCast(volume);
      m_InputVolume->GetOutput()->Update();
      dataset = SP::New();
      int dimensions[3]; 
      double spacing[3];
      imagedata->GetDimensions(dimensions);
      imagedata->GetSpacing(spacing);
      dataset->SetDimensions(dimensions[0], dimensions[1], dimensions[2]); 
      ((SP*)dataset)->SetSpacing(spacing[0], spacing[1], spacing[2]);
      double origin[3];
      imagedata->GetOrigin(origin);
      ((SP*)dataset)->SetOrigin(origin[0], origin[1], origin[2]);


      //if volume scalars type is short, unsigned short or unsigned char, create this scalar and put it into dataset

      if (imagedata->GetScalarType() == VTK_UNSIGNED_CHAR) 
      {
        scalars = TScalars<unsigned char>::New();
        scalars->SetNumberOfScalars(volume->GetNumberOfPoints());
      }
      else if (imagedata->GetScalarType() == VTK_UNSIGNED_SHORT)
      {
        scalars = TScalars<unsigned short>::New();
        scalars->SetNumberOfScalars(volume->GetNumberOfPoints());
      }
      else if (imagedata->GetScalarType() == VTK_SHORT)
      {
        scalars = TScalars<short>::New();
        scalars->SetNumberOfScalars(volume->GetNumberOfPoints());
      }

      dataset->SetScalars(scalars);
      for(int j= 0 ; j< imagedata->GetPointData()->GetScalars()->GetNumberOfTuples(); j++)
      {
        dataset->GetScalars()->SetScalar(j, imagedata->GetPointData()->GetScalars()->GetTuple1(j));
      }

    }
    else if(volume = vtkRectilinearGrid::SafeDownCast(mafVMEVolumeGray::SafeDownCast(m_InputVolume)->GetOutput()->GetVTKData()))
    {

      vtkRectilinearGrid *rectilinearGrid = vtkRectilinearGrid::SafeDownCast(volume);
      m_InputVolume->GetOutput()->Update();

      dataset = RG::New();
      int dimensions[3]; 

      rectilinearGrid->GetDimensions(dimensions);

      dataset->SetDimensions(dimensions[0], dimensions[1], dimensions[2]); 

      int lenghtX,lenghtY,lenghtZ;

      vtkDataArray *vtkArrayX;
      vtkDataArray *vtkArrayY;
      vtkDataArray *vtkArrayZ;

      if(vtkDoubleArray::SafeDownCast(rectilinearGrid->GetXCoordinates()))
      {
        vtkArrayX = vtkDoubleArray::SafeDownCast(rectilinearGrid->GetXCoordinates());
        vtkArrayY = vtkDoubleArray::SafeDownCast(rectilinearGrid->GetYCoordinates());
        vtkArrayZ = vtkDoubleArray::SafeDownCast(rectilinearGrid->GetZCoordinates());
      }
      else if(vtkFloatArray::SafeDownCast(rectilinearGrid->GetXCoordinates()))
      {
        vtkArrayX = vtkFloatArray::SafeDownCast(rectilinearGrid->GetXCoordinates());
        vtkArrayY = vtkFloatArray::SafeDownCast(rectilinearGrid->GetYCoordinates());
        vtkArrayZ = vtkFloatArray::SafeDownCast(rectilinearGrid->GetZCoordinates());
      }

      

      lenghtX = vtkArrayX->GetNumberOfTuples();
      lenghtY = vtkArrayY->GetNumberOfTuples();
      lenghtZ = vtkArrayZ->GetNumberOfTuples();

      float *arrayX = new float[lenghtX];
      float *arrayY = new float[lenghtY];
      float *arrayZ = new float[lenghtZ];

      for(int i=0; i< lenghtX; i++)
      {
        arrayX[i] = vtkArrayX->GetTuple1(i);
      }

      for(int i=0; i< lenghtY; i++)
      {
        arrayY[i] = vtkArrayY->GetTuple1(i);
      }

      for(int i=0; i< lenghtZ; i++)
      {
        arrayZ[i] = vtkArrayZ->GetTuple1(i);
      }

      ((RG*)dataset)->SetCoords(arrayX,arrayY,arrayZ);


      //if volume scalars type is short, unsigned short or unsigned char, create this scalar and put it into dataset

      if (rectilinearGrid->GetPointData()->GetScalars()->GetDataType() == VTK_UNSIGNED_CHAR) 
      {
        scalars = TScalars<unsigned char>::New();
        scalars->SetNumberOfScalars(volume->GetNumberOfPoints());
      }
      else if (rectilinearGrid->GetPointData()->GetScalars()->GetDataType() == VTK_UNSIGNED_SHORT)
      {
        scalars = TScalars<unsigned short>::New();
        scalars->SetNumberOfScalars(volume->GetNumberOfPoints());
      }
      else if (rectilinearGrid->GetPointData()->GetScalars()->GetDataType() == VTK_SHORT)
      {
        scalars = TScalars<short>::New();
        scalars->SetNumberOfScalars(volume->GetNumberOfPoints());
      }

      dataset->SetScalars(scalars);
      for(int j= 0 ; j< rectilinearGrid->GetPointData()->GetScalars()->GetNumberOfTuples(); j++)
      {
        dataset->GetScalars()->SetScalar(j, rectilinearGrid->GetPointData()->GetScalars()->GetTuple1(j));
      }

    }

  } 
  else
  {
    wxMessageBox("Must select a volume");
    return 1;
  }
  numElements = inputUnstructuredGrid->GetNumberOfCells();


  logStringStream << "-- Computing elements densities\n";
  mafLogMessage(_M(logStringStream.str().c_str()));
  logStringStream.str("");

  // considered this for rho density value
  ElementProp *roSource;
  ElementProp **materialProperties;

  // Array containing properties for each element from 0 to numElements - 1
  roSource = new ElementProp[numElements];

  vtkIntArray *arrayMaterial = NULL;
  vtkDoubleArray *arrayE = NULL;
  vtkDoubleArray *arrayPoisson = NULL;
  vtkDoubleArray *arrayRo = NULL;
	
  vtkNEW(arrayMaterial);
  arrayMaterial->SetName("material");
  
  vtkNEW(arrayE);
  arrayE->SetName("EX");

  vtkNEW(arrayPoisson);
  arrayPoisson->SetName("NUXY");

  vtkNEW(arrayRo);
  arrayRo->SetName("DENS");
  
  materialProperties = new ElementProp*[numElements];

  double ROSlope = -1;
  double ROIntercept = -1;

  if (m_DensityRelationshipListbox = POINTS_COORDINATES)
  {
    ROSlope = (m_HU1_d1_el1 - m_HU0_d0_el1) / double(m_HU1_d1_el0 - m_HU0_d0_el0);
    ROIntercept = m_HU0_d0_el1 - m_HU0_d0_el0 * m_RhoSlope;
  }
  else
  {
    ROSlope = m_RhoSlope;
    ROIntercept = m_RhoIntercept;
  }

  if (GetTestMode() == false)
  {
    wxBusyInfo wait_info("Computing elements density...");
    {mafEvent evUnq(this,PROGRESSBAR_SHOW); mafEventMacro(evUnq);}
  }
  
  long progress = 0;
  
  for (id=0; id < numElements; id++) 
  { 

    vtkCell *cell;
    cell = inputUnstructuredGrid->GetCell(id);

    numElementNodes = cell->GetNumberOfPoints();

    //element control
    if (numElementNodes == 4) 
      element = Tetra::New();
    else if (numElementNodes == 10) 
      element = Tetra10::New();
    else if (numElementNodes == 8 || numElementNodes == 20) 
      element = Hexa::New();
    else
      element = Wedge::New();

    for(int num = 0; num < numElementNodes; num++)
    {
      Node *current = new Node;
      current->key = cell->GetPointId(num);
      double coordinates[3];
      cell->GetPoints()->GetPoint(num, coordinates);
      current->x[0] = coordinates[0];
      current->x[1] = coordinates[1];
      current->x[2] = coordinates[2];

      element->SetNode(num, current);
    }

    HU = element->ComputeScalar(dataset, m_StepsNumber);

    for(int h=0; h<numElementNodes; h++)
    {
      delete element->GetNode(h);
    }
    element->Delete();
    delete element;

    // rho = a + b * HU
    roSource[id].rho = ROIntercept + ROSlope * HU;
    arrayRo->InsertNextTuple1(ROIntercept + ROSlope * HU);
    if (arrayRo->GetValue(id) <= 0) 
    {
      arrayRo->SetValue(id, 1e-6);
      roSource[id].rho = 1e-6;
    }

    
    // E = a + b * rho ^ c

    if (m_DensityIntervalsNumber == SINGLE_INTERVAL)
    {
      roSource[id].E = m_a_OneInterval + m_b_OneInterval * pow(roSource[id].rho, m_c_OneInterval);
      arrayE->InsertNextTuple1(roSource[id].E);
    } 
    else if (m_DensityIntervalsNumber == THREE_INTERVALS)
    {
      if (roSource[id].rho < m_RhoAsh1)
      {
        roSource[id].E = m_a_RhoAshLessThanRhoAsh1 + 
        m_b_RhoAshLessThanRhoAsh1 * pow(roSource[id].rho, m_c_RhoAshLessThanRhoAsh1);
        arrayE->InsertNextTuple1(roSource[id].E);
      } 
      else if (m_RhoAsh1 <= roSource[id].rho  && roSource[id].rho <= m_RhoAsh2)
      {
        roSource[id].E = m_a_RhoAshBetweenRhoAsh1andRhoAsh2 + 
        m_b_RhoAshBetweenRhoAsh1andRhoAsh2 * pow(roSource[id].rho, m_c_RhoAshBetweenRhoAsh1andRhoAsh2);
        arrayE->InsertNextTuple1(roSource[id].E);
      }
      else if (roSource[id].rho > m_RhoAsh2)
      {
        roSource[id].E = m_a_RhoAshBiggerThanRhoAsh2 + 
        m_b_RhoAshBiggerThanRhoAsh2 * pow(roSource[id].rho, m_c_RhoAshBiggerThanRhoAsh2);
        arrayE->InsertNextTuple1(roSource[id].E);
      }
    }

    if (arrayE->GetValue(id) <= 0) 
    {
      arrayE->SetValue(id, 1e-6);
      roSource[id].E = 1e-6;
    } 

    roSource[id].elementID = id;

		arrayPoisson->InsertNextTuple1(Ni);
    arrayMaterial->InsertNextTuple1(0);

    progress = (id + 1) * 100 / numElements;
    {mafEvent evUnq(this,PROGRESSBAR_SET_VALUE,(intptr_t)progress); mafEventMacro(evUnq);}
  }

  {mafEvent evUnq(this,PROGRESSBAR_HIDE); mafEventMacro(evUnq);}

  //  COMPUTE ELEMENTS YOUNG MODULE

  std::cout << "-- Computing elements young module\n";

  // considered this for E value
  ElementProp *eModuleSource; 
  eModuleSource = new ElementProp[numElements];

  int pointsNumber = dataset->GetDimensions()[0] * dataset->GetDimensions()[1] * dataset->GetDimensions()[2]; 
    
  for (int pointID = 0; pointID < pointsNumber; pointID++)
  {
    Scalars *datasetScalars  = dataset->GetScalars();
    HU =  datasetScalars->GetScalar(pointID);
    // rho = a + b * HU
    double rho = m_RhoIntercept + m_RhoSlope * HU;
    if (rho <= 0) {
      rho = 1e-6;
    }
    //RO CORRECTION///////////////////////////////////////////////////////////////
    // roSource[id].rho, arrayRo->GetValue(id)
    if(m_RhoCalibrationCorrectionIsActive)
    {
      if(m_RhoCalibrationCorrectionType == SINGLE_INTERVAL)
      {
        rho = m_a_CalibrationCorrection + m_b_CalibrationCorrection * rho;
      }
      else if (m_RhoCalibrationCorrectionType == THREE_INTERVALS)
      {
        if (rho < m_RhoQCT1)
        {
          rho = m_a_RhoQCTLessThanRhoQCT1 + m_b_RhoQCTLessThanRhoQCT1 * rho;
        } 
        else if (m_RhoQCT1 <= rho  && rho <= m_RhoQCT2)
        {
          rho = m_a_RhoQCTBetweenRhoQCT1AndRhoQCT2 + m_b_RhoQCTBetweenRhoQCT1AndRhoQCT2 * rho;
        }
        else if (rho > m_RhoQCT2)
        {
          rho = m_a_RhoQCTBiggerThanRhoQCT2 + m_b_RhoQCTBiggerThanRhoQCT2 * rho;
        }
      }
    }

    //////////////////////////////////////////////////////////////////////////////
    // E = a + b * rho ^ c

    double youngModule;
    
    if (m_DensityIntervalsNumber == SINGLE_INTERVAL)
    {
      youngModule = m_a_OneInterval +
        m_b_OneInterval * pow(rho, m_c_OneInterval);
    } 

    else if (m_DensityIntervalsNumber == THREE_INTERVALS)
    {
      if (rho < m_RhoAsh1)
      {
        youngModule = m_a_RhoAshLessThanRhoAsh1 + 
          m_b_RhoAshLessThanRhoAsh1 * pow(rho, m_c_RhoAshLessThanRhoAsh1);
      } 
      else if (m_RhoAsh1 <= rho  && rho <= m_RhoAsh2)
      {
        youngModule = m_a_RhoAshBetweenRhoAsh1andRhoAsh2 + 
          m_b_RhoAshBetweenRhoAsh1andRhoAsh2 * pow(rho, m_c_RhoAshBetweenRhoAsh1andRhoAsh2);

      }
      else if (rho > m_RhoAsh2)
      {
        youngModule = m_a_RhoAshBiggerThanRhoAsh2 + 
          m_b_RhoAshBiggerThanRhoAsh2 * pow(rho, m_c_RhoAshBiggerThanRhoAsh2);
       
      }
    }

    if (youngModule <= 0) {
      youngModule = 1e-6;
    } 

    datasetScalars->SetScalar(pointID, youngModule);
  }

  if (GetTestMode() == false)
  {
    wxBusyInfo wait_info_young_module("Computing elements Young's modulus...");
    {mafEvent evUnq(this,PROGRESSBAR_SHOW); mafEventMacro(evUnq);}
  }
  progress = 0;

  for (id=0; id < numElements; id++) 
  { 
    vtkCell *cell;
    cell = inputUnstructuredGrid->GetCell(id);

    numElementNodes = cell->GetNumberOfPoints();

    //element control
    if (numElementNodes == 4) 
      element = Tetra::New();
    else if (numElementNodes == 10) 
      element = Tetra10::New();
    else if (numElementNodes == 8 || numElementNodes == 20) 
      element = Hexa::New();
    else
      element = Wedge::New();

    for(int num = 0; num < numElementNodes; num++)
    {
      Node *current = new Node;
      current->key = cell->GetPointId(num);
      double coordinates[3];
      cell->GetPoints()->GetPoint(num, coordinates);
      current->x[0] = coordinates[0];
      current->x[1] = coordinates[1];
      current->x[2] = coordinates[2];

      element->SetNode(num, current);
    }


    double meanYoungModule = element->ComputeScalar(dataset, m_StepsNumber);

    for(int h=0; h<numElementNodes; h++)
    {
      delete element->GetNode(h);
    }
    element->Delete();
    delete element;

  
    if (meanYoungModule <= 0) 
    {
      meanYoungModule = 1e-6;
    } 
    
    eModuleSource[id].E = meanYoungModule;
    eModuleSource[id].elementID = id;
    eModuleSource[id].rho = 0;

    progress = (id + 1) * 100 / numElements;
    {mafEvent evUnq(this,PROGRESSBAR_SET_VALUE,(intptr_t)progress); mafEventMacro(evUnq);}
    float valueE = arrayE->GetValue(id);
    float valueRo = arrayRo->GetValue(id);
    arrayE->SetValue(id, eModuleSource[id].E);
  }

  {mafEvent evUnq(this,PROGRESSBAR_HIDE); mafEventMacro(evUnq);}

  vtkNew<vtkPoints> pts;
  vtkNew<vtkCellArray> cells;

  vtkNew<vtkUnstructuredGrid> outputUG;
  pts->DeepCopy(inputUnstructuredGrid->GetPoints());
  cells->DeepCopy(inputUnstructuredGrid->GetCells());
  outputUG->SetPoints(pts);
  outputUG->SetCells(inputUnstructuredGrid->GetCellTypesArray(),inputUnstructuredGrid->GetCellLocationsArray(),cells);
  
  //outputUG->Update();

  vtkCellData *outCellData = outputUG->GetCellData();
  vtkPointData *outPointData = outputUG->GetPointData();
  vtkFieldData *outFieldData = outputUG->GetFieldData();

  outPointData->DeepCopy(inputUnstructuredGrid->GetPointData());
  outCellData->DeepCopy(inputUnstructuredGrid->GetCellData());
  outFieldData->DeepCopy(inputUnstructuredGrid->GetFieldData());

  outCellData->AddArray(arrayMaterial);
  outCellData->AddArray(arrayE);
  outCellData->AddArray(arrayPoisson);
  outCellData->AddArray(arrayRo);

  outputUG->Modified();
  //outputUG->Update();


	mafVMEMesh::SafeDownCast(m_Input)->SetData(outputUG, 0);

  // COMPUTE MATERIALS & WRITE FREQUENCY FILE

  qsort(eModuleSource, numElements, sizeof(ElementProp), compareE);
  qsort(roSource, numElements, sizeof(ElementProp), compareRO);

  #ifdef _DEBUG_BONEMAT

  for (int elementID = 0;elementID < numElements;elementID++)
  {
    logStringStream << "elementID: " << eModuleSource[elementID].elementID << "E: " << eModuleSource[elementID].E << std::endl;
    logStringStream << "elementID: " << roSource[elementID].elementID << "rho: " << roSource[elementID].rho << std::endl << std::endl;
    
  }

  #endif

  logStringStream << "RO2: " << roSource[0].rho << "\n";
  logStringStream << "RO1: " << roSource[numElements - 1].rho << "\n";

  logStringStream << "Emax: " << eModuleSource[0].E << "\n";
  logStringStream << "Emin: " << eModuleSource[numElements - 1].E << "\n\n";

  // 
  typedef std::map<int, double, std::less<int> > mapType; 
  mapType elementIDtoROmap;
  for (id=0; id < numElements; id++) 
  {
    elementIDtoROmap.insert(mapType::value_type(roSource[id].elementID, roSource[id].rho) );
  }

  // assign density to eModuleSource from roSource
  for (id=0; id < numElements; id++) 
  {  
    eModuleSource[id].rho = elementIDtoROmap[eModuleSource[id].elementID];
  }

  logStringStream << "-- Writing frequency file\n";
  mafLogMessage(_M(logStringStream.str().c_str()));
  logStringStream.str("");

  fprintf(freq_fp, "rho \t\t E \t\t NUMBER OF ELEMENTS\n\n");

#ifdef _USE_MAXIMUM_DENSITY_FOR_GROUPING

  typedef std::vector<int> idVectorType;
  idVectorType idVector;

  freq = 0;
  numMats = 1;
  materialProperties[0] = &eModuleSource[0];
  E = eModuleSource[0].E;  
  
  idVector.push_back(0);

  // grouping materials according to E value
  for (id = 0; id < numElements; id++) 
  {
    if (E - eModuleSource[id].E > m_Egap) 
    {
      // search for maximum density in current group
      double maxDensity = -1;
      // for every element in the group
      for (int idVectorIndex = 0; idVectorIndex < idVector.size(); idVectorIndex++)
      {
        // get the current element density
        double currentDensity = eModuleSource[idVector[idVectorIndex]].rho;
        if (currentDensity > maxDensity)
        {
          maxDensity = currentDensity;
        }
      }
      
      eModuleSource[idVector[0]].rho = maxDensity;
      ro = maxDensity;

#ifdef _DEBUG_BONEMAT
      logStringStream << "material ID:" << numMats << '\t' << "material E:" << E << '\t' << "material ro" << ro << std::endl;
      mafLogMessage("%s",logStringStream.str().c_str());
      logStringStream.str("");
#endif

      // print statistics
      fprintf(freq_fp, "%f \t %f \t %d\n", ro, E, freq); 
     
      // clear ID vector
      idVector.clear();

      // this element is in a new group so push it in the cleared ID vector
      idVector.push_back(id);

      materialProperties[numMats] = &eModuleSource[id];
      E = eModuleSource[id].E;  
      numMats++;

      freq = 1;
    }
    else
    {
      // add another element in current group
      idVector.push_back(id);
      freq++;
    }

    mesh->GetElement(eModuleSource[id].elementID)->SetMatKey(numMats);
  }

  fprintf(freq_fp, "%f \t %f \t %d\n\n", ro, E, freq);

  #ifdef _DEBUG_BONEMAT
    logStringStream << "material ID:" << numMats << '\t' << "material E:" << E << '\t' << "material ro" << ro << std::endl;
    mafLogMessage("%s",logStringStream.str().c_str());
    logStringStream.str("");

  #endif
    
  fclose(freq_fp);

  logStringStream <<"Number of materials: " << numMats << std::endl << std::endl;

#endif


#ifndef _USE_MAXIMUM_DENSITY_FOR_GROUPING
// use mean group density as value for the material density

  typedef std::vector<int> idVectorType;
  idVectorType idVector;

  freq = 0;
  numMats = 1;
  materialProperties[0] = &eModuleSource[0];
  E = eModuleSource[0].E;  
  double densityAccumulator = 0;

  idVector.push_back(0);

  // grouping materials according to E value
  for (id = 0; id < numElements; id++) 
  {
    if (E - eModuleSource[id].E > m_Egap) // generate statistics for old group and create a new group
    {
      // for every element in the group
      for (int idVectorIndex = 0; idVectorIndex < idVector.size(); idVectorIndex++)
      {
        // get the current element density
        double currentDensity = eModuleSource[idVector[idVectorIndex]].rho;
        densityAccumulator += currentDensity;
      }
      
      eModuleSource[idVector[0]].rho = densityAccumulator / idVector.size();
      ro = eModuleSource[idVector[0]].rho;

#ifdef _DEBUG_BONEMAT
      logStringStream << "material ID:" << numMats << '\t' << "material E:" << E << '\t' << "material ro" << ro << std::endl;
      mafLogMessage(_M(logStringStream.str().c_str()));
      logStringStream.str("");
#endif

      // print statistics
      fprintf(freq_fp, "%f \t %f \t %d\n", ro, E, freq); 

      // clear ID vector
      idVector.clear();
      
      densityAccumulator = 0;

      // this element is in a new group so push it in the cleared ID vector
      idVector.push_back(id);

      materialProperties[numMats] = &eModuleSource[id];
      E = eModuleSource[id].E;
      numMats++;

      freq = 1;
    }
    else
    {
      // add another element in current group
      idVector.push_back(id);
      freq++;
    }
    
    // sets the current element material
    //mesh->GetElement(eModuleSource[elementID].elementID)->SetMatKey(numMats);
  }

  fprintf(freq_fp, "%f \t %f \t %d\n\n", ro, E, freq);

#ifdef _DEBUG_BONEMAT
  logStringStream << "material ID:" << numMats << '\t' << "material E:" << E << '\t' << "material ro" << ro << std::endl;
  mafLogMessage(_M(logStringStream.str().c_str()));
  logStringStream.str("");

#endif

  fclose(freq_fp);

  //fielddata materials
  vtkFieldData *fdata = vtkFieldData::New();

  // TO BE PORTED... THIS SHOULD BE NOT NEEDED BY vtk 4.4

  std::vector<mafString> stringVector;

  // correspondence material_id to insert in fielddata
  stringVector.push_back(_R("material_id"));

  vtkDoubleArray *iarr = vtkDoubleArray::New();
  iarr->SetName(stringVector[0].GetCStr());
  iarr->SetNumberOfValues(numMats);
  for (int j = 0; j < numMats; j++)
  {
    int materialAnsysId = j + 1;
    iarr->InsertValue(j, materialAnsysId/*materialProperties[j]->elementID*/);
  }

  // add the ith data array to the field data
  fdata->AddArray(iarr);
  //clean up
  iarr->Delete();

  
  stringVector.push_back(_R("EX"));
  stringVector.push_back(_R("NUXY"));
  stringVector.push_back(_R("DENS"));

  // create field data data array
  for (i = 1; i < stringVector.size(); i++)
  {
    // create the ith data array
    vtkDoubleArray *darr = vtkDoubleArray::New();
    darr->SetName(stringVector[i].GetCStr());
    darr->SetNumberOfValues(numMats);

    for (int j = 0; j < numMats; j++)
    {
      // fill ith data array with jth value 
      // ciclo sui materiali
      if(stringVector[i] == _R("DENS"))
        darr->InsertValue(j, materialProperties[j]->rho);
      else if(stringVector[i] == _R("EX"))
        darr->InsertValue(j, materialProperties[j]->E);
      else if(stringVector[i] == _R("NUXY"))
        darr->InsertValue(j, 0.3);
    }
    // add the ith data array to the field data
    fdata->AddArray(darr);

    //clean up
    darr->Delete();
  }

  outputUG->SetFieldData(fdata);
  outputUG->Modified();
  //outputUG->Update();

  //grouping
  for(int currentCell = 0 ; currentCell<outputUG->GetNumberOfCells(); currentCell++)
  {
    double minDiff = VTK_DOUBLE_MAX;
    int index;
    for(int currentE=0; currentE<fdata->GetArray("EX")->GetNumberOfTuples(); currentE++)
    {
      double valueE = vtkDoubleArray::SafeDownCast(fdata->GetArray("EX"))->GetValue(currentE);
      double valueECell = arrayE->GetValue(currentCell);
      if((valueE-valueECell)< m_Egap && (valueE-valueECell)>= -(10e-5))
      {
        index = currentE;
      }
    }
    double val = vtkDoubleArray::SafeDownCast(fdata->GetArray("EX"))->GetValue(index);
    arrayE->SetTuple1(currentCell,val);
    val = vtkDoubleArray::SafeDownCast(fdata->GetArray("DENS"))->GetValue(index);
    arrayRo->SetValue(currentCell,val);
    int ansysMaterialId = index + 1;
    arrayMaterial->SetValue(currentCell,ansysMaterialId);
  }


  outputUG->Modified();
  //outputUG->Update();

  if (inputMeshABSMatrixEqualToIdentity)
  {
    // nothing to do
  } 
  else
  {
    // copy back old, not transformed, geometry in output data     
    vtkUnstructuredGrid *inputUG = \
    inMesh->GetUnstructuredGridOutput()->GetUnstructuredGridData();
    outputUG->GetPoints()->DeepCopy(inputUG->GetPoints());
  }

  // input modified in place
  mafVMEMesh::SafeDownCast(m_Input)->SetData(outputUG, 0);

  // clean up
  vtkDEL(inputUGTransformed);
  vtkDEL(transform);
  vtkDEL(transformFilter);

  fdata->Delete();

  vtkDEL(arrayMaterial);
  vtkDEL(arrayRo);
  vtkDEL(arrayE);
  vtkDEL(arrayPoisson);

  

  logStringStream <<"Number of materials: " << numMats << std::endl << std::endl;

#endif

  if(dataset)
  {
    dataset->Delete();
    delete dataset;
  }

  delete [] roSource;
  delete [] materialProperties;
  delete [] eModuleSource;

  return 0;
}

//----------------------------------------------------------------------------
void lhpOpBonemat::UpdateRhoAshGuiOnRhoAshIntervalsNumberChange()
//----------------------------------------------------------------------------
{
  if (m_DensityIntervalsNumber == SINGLE_INTERVAL)
  {
    m_Gui->Enable(ID_DENSITY_INTERVAL_0, false);
    m_Gui->Enable(ID_DENSITY_INTERVAL_1, false);
    EnableRhoAshSingleInterval(true);

  }
  else if (m_DensityIntervalsNumber == THREE_INTERVALS)
  {
    m_Gui->Enable(ID_DENSITY_INTERVAL_0, true);
    m_Gui->Enable(ID_DENSITY_INTERVAL_1, true);
    EnableRhoAshSingleInterval(false);

  }
  
}


//----------------------------------------------------------------------------
void lhpOpBonemat::EnableRhoAshSingleInterval(bool enable)
//----------------------------------------------------------------------------
{
  m_Gui->Enable(ID_EXPONENTIAL_COEFFICIENTS_VECTOR_V3_SINGLE_DENSITY_INTERVAL_0, enable);
  m_Gui->Enable(ID_EXPONENTIAL_COEFFICIENTS_VECTOR_V3_SINGLE_DENSITY_INTERVAL_1, enable);
  m_Gui->Enable(ID_EXPONENTIAL_COEFFICIENTS_VECTOR_V3_SINGLE_DENSITY_INTERVAL_2, enable);
  
  m_Gui->Enable(ID_FIRST_EXPONENTIAL_COEFFICIENTS_VECTOR_V3_0, !enable);
  m_Gui->Enable(ID_FIRST_EXPONENTIAL_COEFFICIENTS_VECTOR_V3_1, !enable);
  m_Gui->Enable(ID_FIRST_EXPONENTIAL_COEFFICIENTS_VECTOR_V3_2, !enable);

  m_Gui->Enable(ID_SECOND_EXPONENTIAL_COEFFICIENTS_VECTOR_V3_0, !enable);
  m_Gui->Enable(ID_SECOND_EXPONENTIAL_COEFFICIENTS_VECTOR_V3_1, !enable);
  m_Gui->Enable(ID_SECOND_EXPONENTIAL_COEFFICIENTS_VECTOR_V3_2, !enable);
  
  m_Gui->Enable(ID_THIRD_EXPONENTIAL_COEFFICIENTS_VECTOR_V3_0, !enable);
  m_Gui->Enable(ID_THIRD_EXPONENTIAL_COEFFICIENTS_VECTOR_V3_1, !enable);
  m_Gui->Enable(ID_THIRD_EXPONENTIAL_COEFFICIENTS_VECTOR_V3_2, !enable);

}
//----------------------------------------------------------------------------
void lhpOpBonemat::DisableRhoAshThreeIntervals()
//----------------------------------------------------------------------------
{
  EnableRhoAshSingleInterval(false);
}


mafNode *lhpOpBonemat::VolumeSelection()
{
  mafString title = _L("Choose Volume");
  mafEvent *e; 
  e = new mafEvent();
  e->SetId(VME_CHOOSE);
  e->SetArg((intptr_t)&lhpOpBonemat::VolumeAccept);
  e->SetString(&title);

  mafEventMacro(*e);

  mafNode *n = e->GetVme();
  delete e;
  return n;
}

void lhpOpBonemat::EnableRhoQCTSingleInterval(bool enable)
{
  m_Gui->Enable(ID_RO_CORRECTION_FIRST_EXPONENTIAL_COEFFICIENTS_SINGLE_0, enable);
  m_Gui->Enable(ID_RO_CORRECTION_FIRST_EXPONENTIAL_COEFFICIENTS_SINGLE_1, enable);
}
void lhpOpBonemat::EnableRhoQCTThreeIntervals(bool enable)
{
  m_Gui->Enable(ID_RO_CORRECTION_FIRST_EXPONENTIAL_COEFFICIENTS_VECTOR_0, enable);
  m_Gui->Enable(ID_RO_CORRECTION_FIRST_EXPONENTIAL_COEFFICIENTS_VECTOR_1, enable);
  
  m_Gui->Enable(ID_RO_CORRECTION_SECOND_EXPONENTIAL_COEFFICIENTS_VECTOR_0, enable);
  m_Gui->Enable(ID_RO_CORRECTION_SECOND_EXPONENTIAL_COEFFICIENTS_VECTOR_1, enable);
  
  m_Gui->Enable(ID_RO_CORRECTION_THIRD_EXPONENTIAL_COEFFICIENTS_VECTOR_0, enable);
  m_Gui->Enable(ID_RO_CORRECTION_THIRD_EXPONENTIAL_COEFFICIENTS_VECTOR_1, enable);
}

void lhpOpBonemat::EnableRhoQCTDensityInterval(bool enable)
{
  m_Gui->Enable(ID_RO_CORRECTION_DENSITY_INTERVAL_0, enable);
  m_Gui->Enable(ID_RO_CORRECTION_DENSITY_INTERVAL_1, enable);
}


int lhpOpBonemat::LoadConfigurationFile( const char *configurationFileName )
{
  std::ifstream inputFile(configurationFileName, std::ios::in);

  if (!inputFile) {
    std::cerr << "Error opening " << configurationFileName << "\n";
    assert(false);
    return MAF_ERROR;
  }

  mafString LoadConfigurationFileCacheFileName = _R(configurationFileName);
  LoadConfigurationFileCacheFileName.append(_R(".Load.cache"));

  std::ofstream LoadConfigurationFileCache(LoadConfigurationFileCacheFileName.GetCStr(), std::ios::out);

  if (!LoadConfigurationFileCache) 
  {
    if (GetTestMode() == false)
    {

      wxMessageBox("Error creating configuration file cache");
    }
    assert(false);
    return MAF_ERROR;
  }

  
  std::string buf;
  while(getline(inputFile, buf)) 
  {
    // Find marker at start of line:finish
    size_t pos = buf.find('#');
    if(pos  == 0) 
    {
      // skip line    
    }
    else
    { 
      LoadConfigurationFileCache << buf << std::endl;
    }
  }

  LoadConfigurationFileCache.close();
  //<< std::endl;

  std::ifstream inputFileFromCache(LoadConfigurationFileCacheFileName.GetCStr(), std::ios::in);

  if (!inputFileFromCache) {
    std::cerr << "Error opening " << LoadConfigurationFileCacheFileName.GetCStr() << "\n";
    assert(false);
    return MAF_ERROR;
  }

  //<< "# #### CT Densitometric Calibration ####" << std::endl;

  //<< "# a (m_ROIntercept) : " << std::endl;
  inputFileFromCache >> m_RhoIntercept // << std::endl;
  //   << "# b (m_ROSlope): " // << std::endl;
  >> m_RhoSlope// << std::endl;

  // // << std::endl;
  // << "# apply calibration correction (m_ROCalibrationCorrectionIsActive): " // << std::endl;
  >> m_RhoCalibrationCorrectionIsActive// << std::endl;

  /*// << std::endl;
  << "# #### Correction of the calibration ####" // << std::endl;

  << "# Intervals Type (m_ROCalibrationCorrectionType) {SINGLE_INTERVAL = 0, THREE_INTERVALS = 1}: "  // << std::endl;*/
  >> m_RhoCalibrationCorrectionType// << std::endl;


  // << "# R01 (m_RhoQCT1) : " // << std::endl;
  >> m_RhoQCT1// << std::endl;
  // << "# R02 (m_RhoQCT2) : " // << std::endl;
  >> m_RhoQCT2// << std::endl;

  // << "# a (m_a_CalibrationCorrection): " // << std::endl;
  >> m_a_CalibrationCorrection// << std::endl;
  // << "# b (m_b_CalibrationCorrection): " // << std::endl;
  >> m_b_CalibrationCorrection// << std::endl;

  /*// << std::endl;
  << "# R0 < R01" // << std::endl;*/
  // << "# a (m_a_RhoQCTLessThanRhoQCT1): " // << std::endl;
  >> m_a_RhoQCTLessThanRhoQCT1 // << std::endl;
  // << "# b (m_b_RhoQCTLessThanRhoQCT1): " // << std::endl;
  >> m_b_RhoQCTLessThanRhoQCT1 // << std::endl;

  /*// << std::endl;
  << "# R01 <= R0 <= R02" // << std::endl;  
  << "# a (m_a_RhoQCTBetweenRhoQCT1AndRhoQCT2): " // << std::endl;*/
  >> m_a_RhoQCTBetweenRhoQCT1AndRhoQCT2 // << std::endl;
  // << "# b (m_b_RhoQCTBetweenRhoQCT1AndRhoQCT2): " // << std::endl;
  >> m_b_RhoQCTBetweenRhoQCT1AndRhoQCT2 // << std::endl;

  // << std::endl;

  /*<< "# R0 > R02" // << std::endl;
  << "# a (m_a_RhoQCTBiggerThanRhoQCT2): " // << std::endl;*/
  >> m_a_RhoQCTBiggerThanRhoQCT2// << std::endl;
  // << "# b (m_b_RhoQCTBiggerThanRhoQCT2): " // << std::endl;
  >> m_b_RhoQCTBiggerThanRhoQCT2// << std::endl;

  // << std::endl;
  // << "# ####  density-elasticity relationship #### " // << std::endl;

  /*// << std::endl;
  << "# density-intervals for E integration" // << std::endl;
  << "# Density intervals type (m_DensityIntervalsNumber) {SINGLE_INTERVAL = 0,THREE_INTERVALS = 1}: " // << std::endl;*/
  >> m_DensityIntervalsNumber // << std::endl;

  // << std::endl;
  // << "# R01 (m_RhoAsh1): " // << std::endl;
  >> m_RhoAsh1// << std::endl;
  // << "# R02 (m_RhoAsh2): " // << std::endl;
  >> m_RhoAsh2// << std::endl;
  // << "# a (m_a_OneInterval): " // << std::endl;
  >> m_a_OneInterval// << std::endl;
  // << "# b (m_b_OneInterval): " // << std::endl;
  >> m_b_OneInterval// << std::endl;
  // << "# c (m_c_OneInterval): " // << std::endl;
  >> m_c_OneInterval// << std::endl;

  /*// << std::endl;
  << "# R0 < R01" // << std::endl;
  << "# a (m_a_RhoAshLessThanRhoAsh1): " // << std::endl;*/
  >> m_a_RhoAshLessThanRhoAsh1// << std::endl;
  // << "# b (m_b_RhoAshLessThanRhoAsh1): " // << std::endl;
  >> m_b_RhoAshLessThanRhoAsh1// << std::endl;
  // << "# c (m_c_RhoAshLessThanRhoAsh1): " // << std::endl;
  >> m_c_RhoAshLessThanRhoAsh1// << std::endl;
  /*// << std::endl;
  << "# R01 <= R0 <= R02" // << std::endl;  
  << "# a (m_a_RhoAshBetweenRhoAsh1andRhoAsh2): " // << std::endl;*/
  >> m_a_RhoAshBetweenRhoAsh1andRhoAsh2// << std::endl;
  // << "# b (m_b_RhoAshBetweenRhoAsh1andRhoAsh2): " // << std::endl;
  >> m_b_RhoAshBetweenRhoAsh1andRhoAsh2// << std::endl;
  // << "# c (m_c_RhoAshBetweenRhoAsh1andRhoAsh2): " // << std::endl;
  >> m_c_RhoAshBetweenRhoAsh1andRhoAsh2// << std::endl;
  /*// << std::endl;
  << "# R0 > R02" // << std::endl;
  << "# a (m_a_RhoAshBiggerThanRhoAsh2): " // << std::endl;*/
  >> m_a_RhoAshBiggerThanRhoAsh2// << std::endl;
  // << "# b (m_b_RhoAshBiggerThanRhoAsh2): " // << std::endl;
  >> m_b_RhoAshBiggerThanRhoAsh2// << std::endl;
  // << "# c (m_c_RhoAshBiggerThanRhoAsh2): " // << std::endl;
  >> m_c_RhoAshBiggerThanRhoAsh2// << std::endl;

  // << std::endl;
  // << std::endl;
  /*<< "# Young`s modulus (E) calculation modality" // << std::endl;
  << "# Modality (m_YoungModuleCalculationModality) {HU_INTEGRATION = 0,YOUNG_MODULE_INTEGRATION = 1}: " // << std::endl;*/
  >> m_YoungModuleCalculationModality // << std::endl;

  // << "# Integration Steps (m_StepsNumber): " // << std::endl;
  >> m_StepsNumber// << std::endl;

  // << std::endl;

  // << "# Gap Value (m_Egap): " // << std::endl;
  >> m_Egap; // << std::endl;

  // << std::endl;


  if (DEBUG_MODE)
  {
    std::ostringstream stringStream;
    PrintSelf(stringStream);
    mafLogMessage(_M(stringStream.str().c_str()));
  }

  return MAF_OK;
}

void lhpOpBonemat::PrintSelf(std::ostream &os)
{

  os << std::endl;

  os << "#### CT Densitometric Calibration ####" << std::endl;

  os << "a (m_ROIntercept) : " << m_RhoIntercept << std::endl;
  os << "b (m_ROSlope): " << m_RhoSlope<< std::endl;

  os << std::endl;
  os << "apply calibration correction (m_ROCalibrationCorrectionIsActive): " << m_RhoCalibrationCorrectionIsActive<< std::endl;
  
  os << std::endl;
  os << "#### Correction of the calibration ####" << std::endl;
  
  os << "Intervals Type (m_ROCalibrationCorrectionType) {SINGLE_INTERVAL = 0, THREE_INTERVALS = 1}: " << m_RhoCalibrationCorrectionType<< std::endl;

  //rho interval
  os << "R01 (m_RhoQCT1) : " << m_RhoQCT1<< std::endl;
  os << "R02 (m_RhoQCT2) : " << m_RhoQCT2<< std::endl;
  
  //single interval rho calibration
  os << "a (m_a_CalibrationCorrection): " << m_a_CalibrationCorrection<< std::endl;
  os << "b (m_b_CalibrationCorrection): " << m_b_CalibrationCorrection<< std::endl;

  //three intervals rho calibration
  os << std::endl;
  os << "R0 < R01" << std::endl;
  os << "a (m_a_RhoQCTLessThanRhoQCT1): " << m_a_RhoQCTLessThanRhoQCT1<< std::endl;
  os << "b (m_b_RhoQCTLessThanRhoQCT1): " << m_b_RhoQCTLessThanRhoQCT1<< std::endl;

  os << std::endl;
  os << "R01 <= R0 <= R02" << std::endl;  
  os << "a (m_a_RhoQCTBetweenRhoQCT1AndRhoQCT2): " << m_a_RhoQCTBetweenRhoQCT1AndRhoQCT2<< std::endl;
  os << "b (m_b_RhoQCTBetweenRhoQCT1AndRhoQCT2): " << m_b_RhoQCTBetweenRhoQCT1AndRhoQCT2<< std::endl;

  os << std::endl;

  os << "R0 > R02" << std::endl;
  os << "a (m_a_RhoQCTBiggerThanRhoQCT2): " << m_a_RhoQCTBiggerThanRhoQCT2<< std::endl;
  os << "b (m_b_RhoQCTBiggerThanRhoQCT2): " << m_b_RhoQCTBiggerThanRhoQCT2<< std::endl;

  
  os << std::endl;
  os << "Young`s modulus (E) calculation modality" << std::endl;
  os << "Modality (m_YoungModuleCalculationModality) {HU_INTEGRATION = 0,YOUNG_MODULE_INTEGRATION = 1}: " << m_YoungModuleCalculationModality << std::endl;

  os << std::endl;
  os << "####  density-elasticity relationship #### " << std::endl;

  os << "a (m_a_DensityElasticityRelationship): " << m_a_DER<< std::endl;
  os << "b (m_b_DensityElasticityRelationship): " << m_b_DER<< std::endl;
  os << "c (m_c_DensityElasticityRelationship): " << m_c_DER<< std::endl;

  os << std::endl;
  os << "density-intervals for E integration" << std::endl;
  os << "Density intervals type (m_DensityIntervalsNumber) {SINGLE_INTERVAL = 0,THREE_INTERVALS = 1}: " << m_DensityIntervalsNumber << std::endl;

  os << std::endl;
  os << "R01 (m_RhoAsh1): " << m_RhoAsh1<< std::endl;
  os << "R02 (m_RhoAsh2): " << m_RhoAsh2<< std::endl;
  os << "a (m_a_OneInterval): " << m_a_OneInterval<< std::endl;
  os << "b (m_b_OneInterval): " << m_b_OneInterval<< std::endl;
  os << "c (m_c_OneInterval): " << m_c_OneInterval<< std::endl;

  os << std::endl;
  os << "R0 < R01" << std::endl;
  os << "a (m_a_RhoAshLessThanRhoAsh1): " << m_a_RhoAshLessThanRhoAsh1<< std::endl;
  os << "b (m_b_RhoAshLessThanRhoAsh1): " << m_b_RhoAshLessThanRhoAsh1<< std::endl;
  os << "c (m_c_RhoAshLessThanRhoAsh1): " << m_c_RhoAshLessThanRhoAsh1<< std::endl;
  os << std::endl;
  os << "R01 <= R0 <= R02" << std::endl;  
  os << "a (m_a_RhoAshBetweenRhoAsh1andRhoAsh2): " << m_a_RhoAshBetweenRhoAsh1andRhoAsh2<< std::endl;
  os << "b (m_b_RhoAshBetweenRhoAsh1andRhoAsh2): " << m_b_RhoAshBetweenRhoAsh1andRhoAsh2<< std::endl;
  os << "c (m_c_RhoAshBetweenRhoAsh1andRhoAsh2): " << m_c_RhoAshBetweenRhoAsh1andRhoAsh2<< std::endl;
  os << std::endl;
  os << "R0 > R02" << std::endl;
  os << "a (m_a_RhoAshBiggerThanRhoAsh2): " << m_a_RhoAshBiggerThanRhoAsh2<< std::endl;
  os << "b (m_b_RhoAshBiggerThanRhoAsh2): " << m_b_RhoAshBiggerThanRhoAsh2<< std::endl;
  os << "c (m_c_RhoAshBiggerThanRhoAsh2): " << m_c_RhoAshBiggerThanRhoAsh2<< std::endl;

  os << std::endl;

  os << "Integration Steps (m_StepsNumber): " << m_StepsNumber<< std::endl;
  
  os << std::endl;

  os << "Gap Value (m_Egap): " << m_Egap<< std::endl;

  os << std::endl;
}

int lhpOpBonemat::SaveConfigurationFile( const char *configurationFileName )
{

  std::ofstream outputFile(configurationFileName, std::ios::out);

  if (!outputFile) 
  {
    if (GetTestMode() == false)
    {
      wxMessageBox("Error creating configuration file");
    }
    return MAF_ERROR;
  }

  outputFile.precision(decimalNumbersNumber);
  outputFile 

  << std::endl

  << "# #### CT Densitometric Calibration ####" << std::endl

  << "# a (m_ROIntercept) : " << std::endl
  << m_RhoIntercept << std::endl
  << "# b (m_ROSlope): " << std::endl
  << m_RhoSlope<< std::endl

  << std::endl
  << "# apply calibration correction (m_ROCalibrationCorrectionIsActive): " << std::endl
  << m_RhoCalibrationCorrectionIsActive<< std::endl

  << std::endl
  << "# #### Correction of the calibration ####" << std::endl

  << "# Intervals Type (m_ROCalibrationCorrectionType) {SINGLE_INTERVAL = 0, THREE_INTERVALS = 1}: "  << std::endl
  << m_RhoCalibrationCorrectionType<< std::endl

  
  << "# R01 (m_RhoQCT1) : " << std::endl
  << m_RhoQCT1<< std::endl
  << "# R02 (m_RhoQCT2) : " << std::endl
  << m_RhoQCT2<< std::endl

  << "# a (m_a_CalibrationCorrection): " << std::endl
  << m_a_CalibrationCorrection<< std::endl
  << "# b (m_b_CalibrationCorrection): " << std::endl
  << m_b_CalibrationCorrection<< std::endl

  << std::endl
  << "# R0 < R01" << std::endl
  << "# a (m_a_RhoQCTLessThanRhoQCT1): " << std::endl
  << m_a_RhoQCTLessThanRhoQCT1 << std::endl
  << "# b (m_b_RhoQCTLessThanRhoQCT1): " << std::endl
  << m_b_RhoQCTLessThanRhoQCT1 << std::endl

  << std::endl
  << "# R01 <= R0 <= R02" << std::endl  
  << "# a (m_a_RhoQCTBetweenRhoQCT1AndRhoQCT2): " << std::endl
  << m_a_RhoQCTBetweenRhoQCT1AndRhoQCT2 << std::endl
  << "# b (m_b_RhoQCTBetweenRhoQCT1AndRhoQCT2): " << std::endl
  << m_b_RhoQCTBetweenRhoQCT1AndRhoQCT2 << std::endl

  << std::endl

  << "# R0 > R02" << std::endl
  << "# a (m_a_RhoQCTBiggerThanRhoQCT2): " << std::endl
  << m_a_RhoQCTBiggerThanRhoQCT2<< std::endl
  << "# b (m_b_RhoQCTBiggerThanRhoQCT2): " << std::endl
  << m_b_RhoQCTBiggerThanRhoQCT2<< std::endl


  
  << std::endl
  << "# ####  density-elasticity relationship #### " << std::endl


  << std::endl
  << "# density-intervals for E integration" << std::endl
  << "# Density intervals type (m_DensityIntervalsNumber) {SINGLE_INTERVAL = 0,THREE_INTERVALS = 1}: " << std::endl
  << m_DensityIntervalsNumber << std::endl

  << std::endl
  << "# R01 (m_RhoAsh1): " << std::endl
  << m_RhoAsh1<< std::endl
  << "# R02 (m_RhoAsh2): " << std::endl
  << m_RhoAsh2<< std::endl
  << "# a (m_a_OneInterval): " << std::endl
  << m_a_OneInterval<< std::endl
  << "# b (m_b_OneInterval): " << std::endl
  << m_b_OneInterval<< std::endl
  << "# c (m_c_OneInterval): " << std::endl
  << m_c_OneInterval<< std::endl

  << std::endl
  << "# R0 < R01" << std::endl
  << "# a (m_a_RhoAshLessThanRhoAsh1): " << std::endl
  << m_a_RhoAshLessThanRhoAsh1<< std::endl
  << "# b (m_b_RhoAshLessThanRhoAsh1): " << std::endl
  << m_b_RhoAshLessThanRhoAsh1<< std::endl
  << "# c (m_c_RhoAshLessThanRhoAsh1): " << std::endl
  << m_c_RhoAshLessThanRhoAsh1<< std::endl
  << std::endl
  << "# R01 <= R0 <= R02" << std::endl  
  << "# a (m_a_RhoAshBetweenRhoAsh1andRhoAsh2): " << std::endl
  << m_a_RhoAshBetweenRhoAsh1andRhoAsh2<< std::endl
  << "# b (m_b_RhoAshBetweenRhoAsh1andRhoAsh2): " << std::endl
  << m_b_RhoAshBetweenRhoAsh1andRhoAsh2<< std::endl
  << "# c (m_c_RhoAshBetweenRhoAsh1andRhoAsh2): " << std::endl
  << m_c_RhoAshBetweenRhoAsh1andRhoAsh2<< std::endl
  << std::endl
  << "# R0 > R02" << std::endl
  << "# a (m_a_RhoAshBiggerThanRhoAsh2): " << std::endl
  << m_a_RhoAshBiggerThanRhoAsh2<< std::endl
  << "# b (m_b_RhoAshBiggerThanRhoAsh2): " << std::endl
  << m_b_RhoAshBiggerThanRhoAsh2<< std::endl
  << "# c (m_c_RhoAshBiggerThanRhoAsh2): " << std::endl
  << m_c_RhoAshBiggerThanRhoAsh2<< std::endl


  << "# Young`s modulus (E) calculation modality" << std::endl
  << "# Modality (m_YoungModuleCalculationModality) {HU_INTEGRATION = 0,YOUNG_MODULE_INTEGRATION = 1}: " << std::endl
  << m_YoungModuleCalculationModality << std::endl

  << "# Integration Steps (m_StepsNumber): " << std::endl
  << m_StepsNumber<< std::endl

  << std::endl

  << "# Gap Value (m_Egap): " << std::endl
  << m_Egap<< std::endl

  << std::endl;

  outputFile.close();

  return MAF_OK;
}

int lhpOpBonemat::Execute()
{
  int res = MAF_ERROR;

  if (m_YoungModuleCalculationModality == HU_INTEGRATION)
  {
    return HUIntegration();
  } 
  else if (m_YoungModuleCalculationModality == YOUNG_MODULE_INTEGRATION )
  { 
    return YoungModuleIntegration();
  }
  else
  {
    return MAF_ERROR;
  }
}

