/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpOpSoftReg.h,v $
  Language:  C++
  Date:      $Date: 2008-02-19 11:22:33 $
  Version:   $Revision: 1.3 $
  Authors:   Fedor Moiseev / Vladik Aranov
==========================================================================
  Copyright (c) 2001/2007 
  ULB - Universite Libre de Bruxelles (www.ulb.ac.be)
=========================================================================*/

#ifndef __lhpOpSoftReg_H__
#define __lhpOpSoftReg_H__

//----------------------------------------------------------------------------
// Includes:
//----------------------------------------------------------------------------
#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVME;

//----------------------------------------------------------------------------
// lhpOpRefSys :
//----------------------------------------------------------------------------
/** */
class lhpOpSoftReg: public mafOp
{

public:
  mafTypeMacro(lhpOpSoftReg, mafOp)
  lhpOpSoftReg(const mafString& label= _R(""));
 ~lhpOpSoftReg() override;

  void OnEvent(mafEventBase *maf_event) override;
  mafOp* Copy() override;

  bool Accept(mafNode* vme) override;   
  void OpRun() override;
  void OpDo() override;
  void OpUndo() override;

  static bool BonesSetAccept(mafNode* vme);

protected: 
  void OnChooseVme(mafNode *vme);
  enum 
  {
    ID_SELECTBONES = MINID,
    ID_LAST
  };

private:
  mafString    m_BonesName;
  mafVME       *m_Bones;
  mafTimeStamp m_Sample;
};
#endif
