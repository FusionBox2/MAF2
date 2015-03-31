/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpOpKinectModel.h,v $
  Language:  C++
  Date:      $Date: 2007-08-22 14:01:40 $
  Version:   $Revision: 1.1 $
  Authors:   Fedor Moiseev
==========================================================================
  Copyright (c) 2001/2007 
  ULB - Universite Libre de Bruxelles (www.ulb.ac.be)
=========================================================================*/

#ifndef __lhpOpKinectModel_H__
#define __lhpOpKinectModel_H__

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
class lhpOpKinectModel: public mafOp
{
public:
  mafTypeMacro(lhpOpKinectModel, mafOp)
  lhpOpKinectModel(const mafString& label = "KinectUtil");
 ~lhpOpKinectModel(); 

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
  void Clear();

  std::vector<mafString>         m_C3DInputFileNameFullPaths;
  mafString                      m_ExtAppPath;
  mafString                      m_FileDir;
  double                         m_Scale1;
  double                         m_Scale2;
  double                         m_Scale3;
  std::vector<mafVME*>           m_Imported;
private:
};
#endif
