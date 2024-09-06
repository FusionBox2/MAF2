/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpOpHelAxis.h,v $
  Language:  C++
  Date:      $Date: 2008-10-21 15:29:11 $
  Version:   $Revision: 1.2 $
  Authors:   Fedor Moiseev / Vladik Aranov
==========================================================================
  Copyright (c) 2001/2007 
  ULB - Universite Libre de Bruxelles (www.ulb.ac.be)
=========================================================================*/

#ifndef __lhpOpHelAxis_H__
#define __lhpOpHelAxis_H__

//----------------------------------------------------------------------------
// Includes:
//----------------------------------------------------------------------------
#include "mafOp.h"
#include "mafVME.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVMEHelAxis;

//----------------------------------------------------------------------------
// lhpOpHelAxis:
//----------------------------------------------------------------------------
class lhpOpHelAxis: public mafOp
{
public:
  lhpOpHelAxis(const mafString& label = _R("HelAxis"));
 ~lhpOpHelAxis() override; 

  mafTypeMacro(lhpOpHelAxis, mafOp);

  mafOp* Copy() override;

  bool Accept(mafNode* vme) override;
  void OpRun() override;
  void OpDo() override;

protected: 
  mafVMEHelAxis   *m_HelicalSys;
};
#endif
