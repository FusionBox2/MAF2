/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpOpImporterC3D.h,v $
  Language:  C++
  Date:      $Date: 2008-01-16 23:44:02 $
  Version:   $Revision: 1.2 $
  Authors:   Daniele  Giunchi
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __lhpOpImporterC3D_H__
#define __lhpOpImporterC3D_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVME;
class mafVMEMesh;
class mafEvent;
class mafVMELandmarkCloud;
class mafVMESurface;
class mafVMEVector;
class medVMEAnalog;

//----------------------------------------------------------------------------
// lhpOpImporterC3D :
//----------------------------------------------------------------------------
/** 
  This operation is a wrapper of C3D Reader SDK of Aurion. Need C3D_Reader.lib, C3D_Reader.dll,
  LicenseAurion.dll. The license can be obtained by Aurion following messagebox created by dll.
*/
class lhpOpImporterC3D : public mafOp
{
public:
	lhpOpImporterC3D(const wxString &label = "C3D Importer");
	~lhpOpImporterC3D(); 
	
  mafTypeMacro(lhpOpImporterC3D, mafOp);

  virtual void OnEvent(mafEventBase *maf_event);

  mafOp* Copy();

	/** Return true for the acceptable vme type. */
	bool Accept(mafNode *node);

  /** Builds operation's interface. */
	void OpRun();

  /** Execute the operation. */
  void OpDo();

  /** Makes the undo for the operation. */
  void OpUndo();

  /** Import the c3d*/
  bool Import();

  //methods useful for test

  /* Set/Get Full pathname of c3d File*/
  void SetC3DFileName(const char *filec3d){m_C3DInputFileNameFullPath = filec3d;};
  const char * GetC3DFileName(){return m_C3DInputFileNameFullPath;};

  /* Get Landmark Cloud */
  mafVMELandmarkCloud *GetLandmarkCloudVME(){return m_VmeCloud;};

  /* Get Analog VME */
  medVMEAnalog *GetAnalogVME(){return m_VmeAnalog;};
  
  /* Get Platform vmes from a std list*/
  mafVMESurface *GetPlatformVME(int index=0){return(index>=0 && index<m_PlatformList.size())?m_PlatformList[index]:NULL;};

  /* Get force vmes from a std list*/
  mafVMEVector *GetForceVME(int index=0){return(index>=0 && index<m_ForceList.size())?m_ForceList[index]:NULL;};

  /* Get moment vmes from a std list*/
  mafVMEVector *GetMomentVME(int index=0){return(index>=0 && index<m_MomentList.size())?m_MomentList[index]:NULL;};
  

protected:
  /** Create the dialog interface for the importer. */
  virtual void CreateGui();

	/** Initialize all structures needed for import data*/
  void Initialize();

	/** Open C3D File*/
	int OpenC3D();

	/** Read C3D File Header*/
	int ReadHeaderC3D();

	/** Read C3D Data*/
	int ReadDataC3D();

	/** Close C3D File, it should destroy objects*/
	int CloseC3D();

	/** Import the c3d Landmark coordinates*/
	void ImportTrajectories();
	
	/** Import the c3d analog data like EMG*/
	void ImportAnalog();

  /** Import the c3d platform data*/
  void ImportPlatform();

	/** Import the c3d events*/
	void ImportEvent();

  mafString m_C3DInputFileNameFullPath;
	mafString m_FileDir;
  mafString m_FileName;

	int m_ImportTrajectoriesFlag;
  int m_ImportAnalogFlag;
  int m_ImportPlatformFlag;
  int m_ImportEventFlag;

  //vmes
  mafVMELandmarkCloud  *m_VmeCloud;

  medVMEAnalog *m_VmeAnalog;

  std::vector<mafVMESurface *> m_PlatformList;
  
  std::vector<mafVMEVector *> m_ForceList;
  std::vector<mafVMEVector *> m_MomentList;

	//data filled by Aurion importer
	//int m_Errcode;
	float m_AnalogRate, m_VideoRate;
	long m_LengthMs;

  //derived member
  double m_TrajectorySampleFrequency;
  double m_AnalogSampleFrequency;
  double m_VectogramSampleFrequency;

	int m_NumTotTrajectories; 
	int m_NumTrajectories; 
	int m_NumAngles; 
	int m_NumMoments; 
	int m_NumPowers; 
	int m_NumFrames; 
	int m_NumChannels;
	int m_NumSamples;
	int m_NumEvents;
	int m_NumPlatforms;

	char *m_TrajectoryName;
	char *m_ChannelName;
	char *m_AngleName; 
	char *m_MomentName;
	char *m_PowerName;
	char *m_EventContext;

	char *m_TrajectoryUnit;
	char *m_ChannelUnit;
	char *m_AngleUnit;
	char *m_MomentUnit;
	char *m_PowerUnit;
	
	double m_X;
	double m_Y;
	double m_Z;

	double m_AnalogValue;
  double m_EventValue;
	
	double m_CopX;
	double m_CopY;
	
	double m_ForceX;
	double m_ForceY; 
	double m_ForceZ;
	
	double m_MomentX;
	double m_MomentY;
	double m_MomentZ;
	
	double m_CenterX;
	double m_CenterY;
	
};
#endif
