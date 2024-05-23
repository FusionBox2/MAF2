/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEAFRefSys.h,v $
  Language:  C++
  Date:      $Date: 2008-07-25 12:15:29 $
  Version:   $Revision: 1.7 $
  Authors:   Fedor Moiseev / Vladik Aranov
==========================================================================
  Copyright (c) 2001/2007 
  ULB - Universite Libre de Bruxelles (www.ulb.ac.be)
=========================================================================*/
 
#ifndef __mfAFRefSys_h
#define __mfAFRefSys_h

#include "mafDefines.h"

//----------------------------------------------------------------------------
// Includes:
//----------------------------------------------------------------------------
#include "mafVMERefSysAbstract.h"
#include "VecProc.h"

//----------------------------------------------------------------------------
// class fowarding:
//----------------------------------------------------------------------------
class mafVMELandmarkCloud;

//----------------------------------------------------------------------------
// class mafVMEAFRefSys
//----------------------------------------------------------------------------
class MAF_EXPORT mafVMEAFRefSys : public mafVMERefSysAbstract
{
public:
  mafTypeMacro(mafVMEAFRefSys,mafVMERefSysAbstract);

  /** print a dump of this object */
  virtual void Print(std::ostream& os, const int tabs=0);

  enum AFREFSYS_WIDGET_ID
  {
    ID_NAME_REF_SYS = Superclass::ID_LAST,
    ID_ACTIVE,
    ID_PRINT,
    ID_X_OFFSET,
    ID_Y_OFFSET,
    ID_Z_OFFSET,
    ID_X_ROTATE,
    ID_Y_ROTATE,
    ID_Z_ROTATE,
    ID_SELECT_BONEID,
    ID_FIRSTDYN,
    ID_LAST = ID_FIRSTDYN + 100
  };
  enum AFREFSYS_BONEID
  {
    ID_AFS_NOTDEFINED,
    ID_AFS_FIRST,
    ID_AFS_PELVIS = ID_AFS_FIRST,
    ID_AFS_RTHIGH,
    ID_AFS_LTHIGH,
    ID_AFS_RSHANK,
    ID_AFS_LSHANK,
    ID_AFS_RFOOT,
    ID_AFS_LFOOT,
    ID_AFS_LAST
  };

  /** Precess events coming from other objects */ 
  virtual void OnEvent(mafEventBase *maf_event);

  /** Copy the contents of another VME-RefSys into this one. */
  virtual int DeepCopy(mafNode *a);

  /** 
  Set Vector virtual machine*/
  void SetVM(VecManVM<double> *vm){m_vm = vm;}


  /** 
  Load vector virtual machine script from file*/
  void LoadScriptFromFile(const mafString& filename);

  /** 
  Set vector virtual machine script*/
  void SetScriptText(const std::vector<mafString>& script);

  /** 
  Return current activity state*/
  int  GetActive()const {return m_Active;}

  /** 
  Set current activity state*/
  void SetActive(int active);

  /** 
  Return current bone ID*/
  int  GetBoneID()const {return m_BoneID;}

  /** 
  Set current bone ID*/
  void SetBoneID(int ID);

  /** 
  Return vector for given name and timestamp from vector virtual machine*/
  bool GetVector(const char *name, mafTimeStamp ts, V3d<double>& output);

  /** 
  Return scalar for given name and timestamp from vector virtual machine*/
  bool GetScalar(const char *name, mafTimeStamp ts,     double&  output);
  /** 
  Calculate global matrix for given timestamp*/
  virtual void CalculateMatrix(mafMatrix& mat, mafTimeStamp ts = -1);

protected:
  virtual int SetParent(mafNode *parent);

  /** 
  Update vector virtual machine with given timestamp*/
  bool UpdateVM(mafTimeStamp ts);

  /** 
  Rebuild vector virtual machine from text*/
  bool ConvertTextToVM(bool buildMapping);

  mafVMEAFRefSys();
  virtual ~mafVMEAFRefSys();

  virtual int InternalStore(mafStorageElement *parent);
  virtual int InternalRestore(const mafStorageElement& node);

  void     SetTransf(double x, double y, double z, double xr, double yr, double zr);
  void     GetTransf(double &x, double &y, double &z, double &xr, double &yr, double &zr);

  /** Internally used to create a new instance of the GUI.*/
  virtual mafGUI *CreateGui();

  double                                              m_XOffset, m_YOffset, m_ZOffset;
  double                                              m_XRotate, m_YRotate, m_ZRotate;
  VecManVM<double>                                    *m_vm;
  std::vector<mafString>                              m_scriptText;
  int                                                 m_BoneID;

  int                                                 m_Active;
  std::map<mafString, mafString>                      m_lmMapping;
  std::map<int, mafString>                            m_buttonMapping;
  int                                                 m_textSize;
  bool                                                m_VMValid;
  mafTimeStamp                                        m_VMTime;
private:
  mafVMEAFRefSys(const mafVMEAFRefSys&); // Not implemented
  void operator=(const mafVMEAFRefSys&); // Not implemented
};

#endif
