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
  lhpOpKinectModel(const mafString& label = _R("KinectUtil"));
 ~lhpOpKinectModel() override;

  void OnEvent(mafEventBase *maf_event) override;
  mafOp* Copy() override;

  bool Accept(mafNode* vme) override;   
  void OpRun() override;
  void OpDo() override;
  void OpUndo() override;
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
