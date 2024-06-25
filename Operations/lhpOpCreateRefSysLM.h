/*=========================================================================

 Program: MAF2
 Module: lhpOpCreateRefSysLM
 Authors: Paolo Quadrani
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __lhpOpCreateRefSysLM_H__
#define __lhpOpCreateRefSysLM_H__

#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVMELandmarkCloud;
class mafNode;
//----------------------------------------------------------------------------
// lhpOpCreateRefSysLM :
//----------------------------------------------------------------------------
/** */
class MAF_EXPORT lhpOpCreateRefSysLM: public mafOp
{
public:
  lhpOpCreateRefSysLM(const mafString& label = _R("Create RefSys Landmarks"));
  ~lhpOpCreateRefSysLM(); 

  mafTypeMacro(lhpOpCreateRefSysLM, mafOp);

  mafOp* Copy();

  bool Accept(mafNode *node);
  void OpRun();
  void OpDo();

protected: 
  mafVMELandmarkCloud *m_Cloud;
};
#endif
