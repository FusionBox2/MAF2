/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpOpBonemat.h,v $
  Language:  C++
  Date:      $Date: 2008-06-30 08:36:22 $
  Version:   $Revision: 1.7 $
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

  void UpdateRhoAshGuiOnRhoAshIntervalsNumberChange();

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

  void EnableTwoIntervals(bool enable);
  void EnableRhoAshSingleInterval(bool enable);
  void DisableRhoAshThreeIntervals();

  void EnableRhoQCTSingleInterval(bool enable);
  void EnableRhoQCTThreeIntervals(bool enable);
  void EnableRhoQCTDensityInterval(bool enable);
  
  mafString m_ConfigurationFileName;
  mafString m_InputVolumeName;
  mafString m_FrequencyFileName;
 
  /** Ea, Eb, Ec */
  double m_a_DER,m_b_DER,m_c_DER;

  double m_a_RhoAshLessThanRhoAsh1,m_b_RhoAshLessThanRhoAsh1,m_c_RhoAshLessThanRhoAsh1;
  double m_a_RhoAshBetweenRhoAsh1andRhoAsh2,m_b_RhoAshBetweenRhoAsh1andRhoAsh2,m_c_RhoAshBetweenRhoAsh1andRhoAsh2;
  double m_a_RhoAshBiggerThanRhoAsh2,m_b_RhoAshBiggerThanRhoAsh2,m_c_RhoAshBiggerThanRhoAsh2;


  //ro calibration
  int m_RhoCalibrationCorrectionIsActive;
  int m_RhoCalibrationCorrectionType;

  //ro interval
  double m_RhoQCT1;
  double m_RhoQCT2;

  //single interval ro calibration
  double m_a_CalibrationCorrection;
  double m_b_CalibrationCorrection;

  //three intervals ro calibration
  double m_a_RhoQCTLessThanRhoQCT1;
  double m_b_RhoQCTLessThanRhoQCT1;

  double m_a_RhoQCTBetweenRhoQCT1AndRhoQCT2;
  double m_b_RhoQCTBetweenRhoQCT1AndRhoQCT2;

  double m_a_RhoQCTBiggerThanRhoQCT2;
  double m_b_RhoQCTBiggerThanRhoQCT2;


  /** density relationship */
  int m_DensityRelationshipListbox;
  

  /*ro = a + b * HU*/
  double m_RhoIntercept;
  double m_RhoSlope;

  //
  ///** HU0, d0 */
  double m_HU0_d0_el0,m_HU0_d0_el1;
 
  ///** HU1, d1 */
  double m_HU1_d1_el0,m_HU1_d1_el1;

  int m_StepsNumber;
  double m_Egap;
  double m_RhoAsh1,m_RhoAsh2;

  double m_a_OneInterval,m_b_OneInterval,m_c_OneInterval;
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
