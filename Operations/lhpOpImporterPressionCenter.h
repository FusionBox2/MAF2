/*=========================================================================

  Authors:   Taha Jerbi
==========================================================================

=========================================================================*/

#ifndef __lhpOpImporterPressionCenter_H__
#define __lhpOpImporterPressionCenter_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafOp.h"
#include "btkAcquisitionFileReader.h"
#include "btkForcePlatformsExtractor.h"
#include "btkGroundReactionWrenchFilter.h"
#include "btkForcePlatformWrenchFilter.h"
#include <map>

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
class mafVMEGroup;

//----------------------------------------------------------------------------
// lhpOpImporterPressionCenter :
//----------------------------------------------------------------------------
class lhpOpImporterPressionCenter : public mafOp
{
public:
	lhpOpImporterPressionCenter(const mafString& label = _R("PressionCenter Importer"));
	~lhpOpImporterPressionCenter();
	
	mafTypeMacro(lhpOpImporterPressionCenter, mafOp);

//  virtual void OnEvent(mafEventBase *maf_event);

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

  /* Set/Get Full pathname of PC File*/
  void SetPCFileName(const char *filepc){Clear(); m_PCInputFileNameFullPaths.resize(1);m_PCInputFileNameFullPaths[0] = _R(filepc);}
  const char * GetPCFileName() {if(m_PCInputFileNameFullPaths.empty()) return NULL; return m_PCInputFileNameFullPaths[0].GetCStr();}

  /* Set/Get Full pathname of dictionary File*/
 /* void SetDictionaryFileName(const char *fileDict)
  {m_DictionaryFileName = fileDict;
 // DictionaryUpdate();
  }*
  /*const char * GetDictionaryFileName() {return m_DictionaryFileName;}*/

  /*void SetLMRenameFileName(const char *fileDict){
	  m_LMRenameFileName = fileDict;
	 // LMRenameUpdate();
  }*/
 /* const char * GetLMRenameFileName() {return m_LMRenameFileName;}*/

  /* Get Group representing result of import */
  mafVMEGroup *GetGroup(){if(m_intData.empty()) return NULL; return m_intData[0].m_VmeGroup;}

  /* Get Landmark Cloud */
  mafVMELandmarkCloud *GetLandmarkCloudVME(){if(m_intData.empty()) return NULL; if(m_intData[0].m_Clouds.empty()) return NULL; return m_intData[0].m_Clouds.begin()->second;}

  /* Get Analog VME */
 /* medVMEAnalog *GetAnalogVME(){return m_intData.empty() ? NULL : m_intData[0].m_VmeAnalog;}*/
  
  /* Get Platform vmes from a std list*/
  /*mafVMESurface *GetPlatformVME(int index=0){if(m_intData.empty()) return NULL; return(index>=0 && index<m_intData[0].m_PlatformList.size())?m_intData[0].m_PlatformList[index]:NULL;}*/

  /* Get force vmes from a std list*/
  /*mafVMEVector *GetForceVME(int index=0){if(m_intData.empty()) return NULL; return(index>=0 && index<m_intData[0].m_ForceList.size())?m_intData[0].m_ForceList[index]:NULL;}*/

  /* Get moment vmes from a std list*/
 /* mafVMEVector *GetMomentVME(int index=0){if(m_intData.empty()) return NULL; return(index>=0 && index<m_intData[0].m_MomentList.size())?m_intData[0].m_MomentList[index]:NULL;}*/

  /* Set/Get Trajectory import flag*/
  /*void SetImportTrajectories(int flag){m_ImportTrajectoriesFlag = flag;}*/
 /* int  GetImportTrajectories(){return m_ImportTrajectoriesFlag;}*/

  /* Set/Get Analog import flag*/
  /*void SetImportAnalog(int flag){m_ImportAnalogFlag = flag;}*/
 /* int  GetImportAnalog(){return m_ImportAnalogFlag;}*/

  /* Set/Get Platform import flag*/
  /*void SetImportPlatform(int flag){m_ImportPlatformFlag = flag;}*/
  /*int  GetImportPlatform(){return m_ImportPlatformFlag;}*/

  /* Set/Get Event import flag*/
  /*void SetImportEvent(int flag){m_ImportEventFlag = flag;}*/
  /*int  GetImportEvent(){return m_ImportEventFlag;}*/

  
  
protected:
	struct _InternalPCData;
  /** Create the dialog interface for the importer. */
//  virtual void CreateGui();

	/** Initialize all structures needed for import data*/
  void Initialize(const mafString &fullFileName , _InternalPCData &intData);

	/** Open PC File*/
	int OpenPC(const mafString &fullFileName);

	/** Import the c3d Landmark coordinates*/
//	void ImportTrajectories(_InternalPCData &intData);
	
	/** Import the c3d analog data like EMG*/
//	void ImportAnalog(_InternalPCData &intData);

  /** Import the c3d platform data*/
 // void ImportPlatform(_InternalPCData &intData);

	/** Import the c3d events*/
	//void ImportEvent(_InternalPCData &intData);

	/** Cleans allocated memory*/
	void Clear();

  /** Import the c3d events*/
	mafVMEGroup* ImportSingleFile(const mafString &fullFileName);// , _InternalPCData &intData);

  /** Do actions necessary on dictionary name changing*/
//  void DictionaryUpdate();

  /** Load dictionary from specified file*/
//  bool LoadDictionary();

  /** Destroy dictionary*/
//  void DestroyDictionary();

  /** Do actions necessary on dictionary name changing*/
  //void LMRenameUpdate();

  /** Load dictionary from specified file*/
//  bool LoadLMRename();

  /** Destroy dictionary*/
//  void DestroyLMRename();

//  btk::Acquisition::Pointer m_Acq;
//  btk::ForcePlatformCollection::Pointer m_Pfc;
//  btk::GroundReactionWrenchFilter::Pointer m_Grws;
//  btk::ForcePlatformsExtractor::Pointer m_Pfe;
  std::vector<mafString>         m_PCInputFileNameFullPaths;
  mafString                      m_FileDir;
  mafString                      m_DictionaryFileName;
  std::map<mafString, mafString> m_dictionaryStruct;
  mafString                      m_LMRenameFileName;
  std::map<mafString, mafString> m_LMRenameStruct;
	int m_ImportTrajectoriesFlag;
  int m_ImportAnalogFlag;
  int m_ImportPlatformFlag;
  int m_ImportEventFlag;

  double    m_DefaultRadius;

  //vmes
  struct _InternalPCData
  {
    _InternalPCData();

    mafString m_FileName;

    mafVMEGroup                               *m_VmeGroup;
    std::map<mafString, mafVMELandmarkCloud*> m_Clouds;

    medVMEAnalog *m_VmeAnalog;

    std::vector<mafVMESurface *> m_PlatformList;

    std::vector<mafVMEVector *> m_ForceList;
    std::vector<mafVMEVector *> m_MomentList;

	  //data filled by Aurion importer
	  //int m_Errcode;
	  float m_AnalogRate, m_VideoRate;
	  long m_LengthMs;

    //derived member
    double m_TrajectorySamplePeriod;
    double m_AnalogSamplePeriod;
    double m_VectogramSamplePeriod;

	  int m_NumTotTrajectories; 
	  int m_NumTrajectories; 
	  int m_NumAngles; 
	  int m_NumMoments; 
	  int m_NumPowers; 
	  int m_NumFrames; 
	  int m_StartFrame;
	  int m_EndFrame;
	  int m_NumChannels;
	  int m_NumSamples;
	  int m_NumEvents;
	  int m_NumPlatforms;

    mafString m_TrajectoryName;
	  mafString m_ChannelName;
	  mafString m_AngleName;
	  mafString m_MomentName;
	  mafString m_PowerName;
	  mafString m_EventContext;

	  mafString m_TrajectoryUnit;
	  mafString m_ChannelUnit;
	  mafString m_AngleUnit;
	  mafString m_MomentUnit;
	  mafString m_PowerUnit;
  	

	  int index;
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
	  double m_Time;
	  double m_pression;
	  double m_LR;

    double m_OriginX;
    double m_OriginY;
    double m_OriginZ;
  };

  std::vector<_InternalPCData> m_intData;
	
};
#endif
