/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpOpBonemat.h,v $
  Language:  C++
  Date:      $Date: 2008-04-01 09:55:18 $
  Version:   $Revision: 1.2 $
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
  lhpOpBonemat(wxString label);
 ~lhpOpBonemat(); 
  mafOp* Copy();
	void OnEvent(mafEventBase *maf_event);

	/** Return true for the acceptable vme type. */
  bool Accept(mafNode *node);

	/** Builds operation's interface. */
  void OpRun();
  
  /** Set the configuration file name */
  void SetConfigurationFileName(const char* name);  

  /** Get the configuration file name */
  const char* GetConfigurationFileName();


  /** Set the InputCT file name */
  void SetInputCTFileName(const char* name);  

  /** Get the InputCT file name */
  const char* GetInputCTFileName();


  /** Set the Frequency file name */
  void SetFrequencyFileName(const char* name);  

  /** Get the Frequency file name */
  const char* GetFrequencyFileName();

  /** Create the operation graphical user interface*/
  void CreateGui();

  /** Execute the procedure that maps TAC values on the finite element mesh:
  DENSITY: FROM_DATASET == old version
  YOUNG MODULE: FROM_DENSITY == old version
  */
  int Execute1();

  /** Execute the procedure that maps TAC values on the finite element mesh:
  DENSITY: FROM_DATASET == old version
  YOUNG MODULE: FROM_DATASET  
  */
  int Execute2();

  /** Read configuration file and fill in member variables */
  int OpenConfigurationFile();
  
  /** Write the configuration file */
  int SaveConfigurationFileAs();
  int SaveConfigurationFile(const char *fileName);

  void OnOpenConfigurationFileButton();
  void OnSaveConfigurationFileButton();
  void OnSaveConfigurationFileAsButton();
  void OnOpenInputTacButton();
  void OnOutputFrequencyFileName();
  void OnExecute();
  
  void UpdateDensityIntegrationGui();

  mafNode *VolumeSelection();

  static bool VolumeAccept(mafNode* node) {return(node != NULL && node->IsMAFType(mafVMEVolumeGray));};


protected:

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
  mafString m_InputCTFileName;
  mafString m_FrequencyFileName;

    
  /** Ea, Eb, Ec */
  double m_Ea_Eb_Ec_V2_el0,m_Ea_Eb_Ec_V2_el1,m_Ea_Eb_Ec_V2_el2;

  double m_Ea0_Eb0_Ec0_V3_el0,m_Ea0_Eb0_Ec0_V3_el1,m_Ea0_Eb0_Ec0_V3_el2;
  double m_Ea1_Eb1_Ec1_V3_el0,m_Ea1_Eb1_Ec1_V3_el1,m_Ea1_Eb1_Ec1_V3_el2;
  double m_Ea2_Eb2_Ec2_V3_el0,m_Ea2_Eb2_Ec2_V3_el1,m_Ea2_Eb2_Ec2_V3_el2;


  //ro calibration
  int m_ROCorrectionActivation;
  int m_ROCorrectionType;

  //ro interval
  double m_ROCorrectionDensityInterval0;
  double m_ROCorrectionDensityInterval1;

  //single interval ro calibration
  double m_RoCorrectioSingleCoefficient0;
  double m_RoCorrectioSingleCoefficient1;

  //three intervals ro calibration
  double m_RoCorrectioFirstCoefficient0;
  double m_RoCorrectioFirstCoefficient1;

  double m_RoCorrectioSecondCoefficient0;
  double m_RoCorrectioSecondCoefficient1;

  double m_RoCorrectioThirdCoefficient0;
  double m_RoCorrectioThirdCoefficient1;


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
  mafVMEVolumeGray *m_VmeVolume;
  mafVME   *m_Vme; 
  mafVMEMesh *m_OriginalVMEMesh;
};
#endif
