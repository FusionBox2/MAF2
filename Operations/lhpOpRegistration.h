/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpOpRegistration.h,v $
  Language:  C++
  Date:      $Date: 2007-08-22 14:01:40 $
  Version:   $Revision: 1.1 $
  Authors:   Fedor Moiseev
==========================================================================
  Copyright (c) 2001/2007 
  ULB - Universite Libre de Bruxelles (www.ulb.ac.be)
=========================================================================*/

#ifndef __lhpOpRegistration_H__
#define __lhpOpRegistration_H__

//----------------------------------------------------------------------------
// Includes:
//----------------------------------------------------------------------------
#include "mafOp.h"
#include "mafVME.h"
#include "mafVectors.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVMERegData;
class mafVMELandmarkCloud;
class mafEvent;
class RegData;

//----------------------------------------------------------------------------
// lhpOpRegistration:
//----------------------------------------------------------------------------
class lhpOpRegistration: public mafOp
{
public:
  mafTypeMacro(lhpOpRegistration, mafOp)
  lhpOpRegistration(const mafString& label = _R("DSRegistration"));
 ~lhpOpRegistration() override;

  void OnEvent(mafEventBase *maf_event) override;
  mafOp* Copy() override;

  bool Accept(mafNode* vme) override;   
  void OpRun() override;
  void OpDo() override;
  void OpUndo() override;
  void CreateGui();
  void OpStop(int result) override;

protected: 

  //----------------------------------------------------------------------------
  // widget id's
  //----------------------------------------------------------------------------
  enum 
  {
    ID_DEFAULT = MINID,
    ID_REGTRG,
    ID_SCALE,
    ID_PRIMARY,
    ID_ADVANCED,
    ID_REGRESSION,
    ID_LAST,
    ID_FORCED_DWORD = 0x7fffffff
  };


  void SetNodeName(mafVME *pVME, mafString *pName);
  bool RegistrationProcedure();
  bool ScaleLMC(mafVMELandmarkCloud *lmc, double scale);
  bool StampsIdentical(std::map<int, mafVMELandmarkCloud*>& mp, std::vector<mafTimeStamp>& stmps);
  //bool GetOriPosL(std::map<int, mafVMELandmarkCloud*>& mp, mafTimeStamp ts, V3d<double>& vLP, V3d<double>& vLO);
  //bool GetOriPosR(std::map<int, mafVMELandmarkCloud*>& mp, mafTimeStamp ts, V3d<double>& vRP, V3d<double>& vRO);

  int               m_Scale;
  int               m_Primary;
  int               m_Advanced;
  int               m_Regression;
  mafVME            *m_Result;
  mafVME            *m_RegTarget;
  mafString         m_RegTrgName;
  mafTimeStamp      m_RefStampSrc;
  mafTimeStamp      m_RefStampTrg;
};
#endif
