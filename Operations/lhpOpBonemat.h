/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpOpBonemat.h,v $
  Language:  C++
  Date:      $Date: 2008-05-27 14:18:29 $
  Version:   $Revision: 1.5 $
  Authors:   Daniele Giunchi , Stefano Perticoni
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __lhpOpBonemat_H__
#define __lhpOpBonemat_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafOp.h"
#include "mafNode.h"
#include "mafVMEVolumeGray.h"
#include <iostream>
//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVME;
class mafVMEMesh;

//----------------------------------------------------------------------------
// lhpOpBonemat :
//----------------------------------------------------------------------------
/** Operation to map CT properties on a finite element mesh*/
class lhpOpBonemat: public mafOp
{
public:

  /** Set the source volume for mapping*/
  void SetSourceVolume(mafVMEVolumeGray *volume) {m_InputVolume = volume;};
  mafVMEVolumeGray *GetVolume() {return m_InputVolume;};

  /** Fill ivars from configuration file */
  int LoadConfigurationFile(const char *configurationFileName);

  /** Set the Output Frequency file name */
  void SetFrequencyFileName(const char* name);  

  /** Get the Output Frequency file name */
  const char* GetFrequencyFileName();

  /** Set the execution modality, Default is HU integration */
  void SetYoungModuleCalculationModalityToHUIntegration() {m_YoungModuleCalculationModality = HU_INTEGRATION;};
  void SetYoungModuleCalculationModalityToYoungModuleIntegration() {m_YoungModuleCalculationModality = YOUNG_MODULE_INTEGRATION;};
  
  /** Execute the procedure that maps TAC values on the finite element mesh; 
  input data is changed in place*/
  int Execute();

  /** Create the operation graphical user interface*/
  void CreateGui();

  void PrintSelf(std::ostream &os);

  static bool VolumeAccept(mafNode* node) {return(node != NULL && node->IsMAFType(mafVMEVolumeGray));};

  /** Return true for the acceptable vme type. */
  bool Accept(mafNode *node);

  /** Builds operation's interface. */
  void OpRun();

  lhpOpBonemat(wxString label);
  ~lhpOpBonemat(); 
  mafOp* Copy();
  void OnEvent(mafEventBase *maf_event);

protected:

  /** Execute the procedure that maps TAC values on the finite element mesh:
  DENSITY: FROM_DATASET == old version
  YOUNG MODULE: FROM_DENSITY == old version
  */
  int HUIntegration();

  /** Execute the procedure that maps TAC values on the finite element mesh:
  DENSITY: FROM_DATASET == old version
  YOUNG MODULE: FROM_DATASET  
  */
  int YoungModuleIntegration();

  int SaveConfigurationFile(const char *configurationFileName);

  /** Set the configuration file name */
  void SetConfigurationFileName(const char* name);  

  /** Get the configuration file name */
  const char* GetConfigurationFileName();


  /** Read configuration file and fill in member variables */
  int OpenConfigurationFile();

  /** Write the configuration file */
  int SaveConfigurationFileAs();


  void OnOpenConfigurationFileButton();
  void OnSaveConfigurationFileButton();
  void OnSaveConfigurationFileAsButton();
  void OnOpenInputTacButton();
  void OnOutputFrequencyFileName();
  void OnExecute();

  void UpdateDensityIntegrationGui();

  mafNode *VolumeSelection();

  enum
  {
    POINTS_COORDINATES = 0,
    INTERCEPT_SLOPE = 1,
  };

  enum
  {
    SINGLE_INTERVAL = 0,
    THREE_INTERVALS = 1,
    INTERVALS_NUMBER,
  };

  enum 
  {
    HU_INTEGRATION = 0,
    YOUNG_MODULE_INTEGRATION = 1,
    INTEGRATION_MODALITIES_NUMBER,
  };

  /** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
	void OpStop(int result);

  void EnableV2(bool enable);
  void EnableV3SingleInterval(bool enable);
  void DisableV3();

  void EnableRoCorrectionSingleInterval(bool enable);
  void EnableRoCorrectionThreeInterval(bool enable);
  void EnableRoCorrectionDensityInterval(bool enable);
  
  mafString m_ConfigurationFileName;
  mafString m_InputVolumeName;
  mafString m_FrequencyFileName;
 
  /** Ea, Eb, Ec */
  double m_Ea_Eb_Ec_V2_el0,m_Ea_Eb_Ec_V2_el1,m_Ea_Eb_Ec_V2_el2;

  double m_Ea0_Eb0_Ec0_V3_el0,m_Ea0_Eb0_Ec0_V3_el1,m_Ea0_Eb0_Ec0_V3_el2;
  double m_Ea1_Eb1_Ec1_V3_el0,m_Ea1_Eb1_Ec1_V3_el1,m_Ea1_Eb1_Ec1_V3_el2;
  double m_Ea2_Eb2_Ec2_V3_el0,m_Ea2_Eb2_Ec2_V3_el1,m_Ea2_Eb2_Ec2_V3_el2;


  //ro calibration
  int m_ROCalibrationCorrectionIsActive;
  int m_ROCalibrationCorrectionType;

  //ro interval
  double m_RO1;
  double m_RO2;

  //single interval ro calibration
  double m_RoCorrection1IntervalCoefficient0;
  double m_RoCorrection1IntervalCoefficient1;

  //three intervals ro calibration
  double m_RoCorrection3IntervalsFirstCoefficient0;
  double m_RoCorrectio3IntervalsFirstCoefficient1;

  double m_RoCorrection3IntervalsSecondCoefficient0;
  double m_RoCorrection3IntervalsSecondCoefficient1;

  double m_RoCorrection3IntervalsThirdCoefficient0;
  double m_RoCorrection3IntervalsThirdCoefficient1;


  /** density relationship */
  int m_DensityRelationshipListbox;
  

  /*ro = a + b * HU*/
  double m_ROIntercept;
  double m_ROSlope;

  //
  ///** HU0, d0 */
  double m_HU0_d0_el0,m_HU0_d0_el1;
 
  ///** HU1, d1 */
  double m_HU1_d1_el0,m_HU1_d1_el1;

  int m_StepsNumber;
  double m_Egap;
  double m_RO1_RO2_el0,m_RO1_RO2_el1;

  double m_Ea_Eb_Ec_V3_OneDensityInterval_el0,m_Ea_Eb_Ec_V3_OneDensityInterval_el1,m_Ea_Eb_Ec_V3_OneDensityInterval_el2;
  int m_DensityIntervalsNumber;
  
  int m_YoungModuleCalculationModality;

  // my inclusions SUBSTITUTION
  mafVMEVolumeGray *m_InputVolume;
  mafVME   *m_Vme; 
  mafVMEMesh *m_OriginalVMEMesh;

  // friend test
  friend class lhpOpBonematTest;

};
#endif
