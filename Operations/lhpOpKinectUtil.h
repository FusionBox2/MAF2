/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpOpKinectUtil.h,v $
  Language:  C++
  Date:      $Date: 2007-08-22 14:01:40 $
  Version:   $Revision: 1.1 $
  Authors:   Fedor Moiseev
==========================================================================
  Copyright (c) 2001/2007 
  ULB - Universite Libre de Bruxelles (www.ulb.ac.be)
=========================================================================*/

#ifndef __lhpOpKinectUtil_H__
#define __lhpOpKinectUtil_H__

//----------------------------------------------------------------------------
// Includes:
//----------------------------------------------------------------------------
#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVMEAFRefSys;
class mafGui;
class mafEvent;
class mafVMELandmarkCloud;
class mafVME;
class vtkPoints;

//----------------------------------------------------------------------------
// mmoRefSys :
//----------------------------------------------------------------------------
/** */
class lhpOpKinectUtil: public mafOp
{
public:
  mafTypeMacro(lhpOpKinectUtil, mafOp)
  lhpOpKinectUtil(bool extapp = false, const mafString& label = "KinectUtil");
 ~lhpOpKinectUtil(); 

  virtual void OnEvent(mafEventBase *maf_event);
  mafOp* Copy();

  bool Accept(mafNode* vme);   
  void OpRun();
  void OpDo();
  void OpUndo();
  bool Import();

protected: 
  /** Create the dialog interface for the importer. */
  virtual void CreateGui();
  /** Import the c3d events*/
  mafVME* ImportSingleFile(const mafString &fullFileName);
  void Clear();

  void DictionaryUpdate();
  bool LoadDictionary();
  void DestroyDictionary();

  std::vector<mafString>         m_C3DInputFileNameFullPaths;
  mafString                      m_ExtAppPath;
  mafString                      m_ExtAppPathModel;
  mafString                      m_FileDir;
  mafString                      m_DictionaryFileName;
  std::vector<mafString>         m_dictionaryStruct;
  double                         m_Scale;
  double                         m_Freq;
  bool                           m_ExtApp;
  int                            m_AFs;
  int                            m_Model;
  double                         m_Scale1;
  double                         m_Scale2;
  double                         m_Scale3;
  int                            m_TypeOfRefs;
  std::vector<mafVME*>           m_Imported;
private:
};
#endif
