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
 ~lhpOpSoftReg(); 

  virtual void OnEvent(mafEventBase *maf_event);
  mafOp* Copy();

  bool Accept(mafNode* vme);   
  void OpRun();
  void OpDo();
  void OpUndo();

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
