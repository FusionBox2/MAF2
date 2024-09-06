/*=========================================================================

 Program: MAF2
 Module: lhpOpCreateMetersScripted
 Authors: Paolo Quadrani
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __lhpOpCreateMetersScripted_H__
#define __lhpOpCreateMetersScripted_H__

#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVMEGroup;
class mafNode;
//----------------------------------------------------------------------------
// lhpOpCreateMetersScripted :
//----------------------------------------------------------------------------
/** */
class MAF_EXPORT lhpOpCreateMetersScripted: public mafOp
{
public:
  lhpOpCreateMetersScripted(const mafString& label = _R("Scripted meters"));
  ~lhpOpCreateMetersScripted() override; 

  mafTypeMacro(lhpOpCreateMetersScripted, mafOp);

  mafOp* Copy() override;

  bool Accept(mafNode *node) override;
  void OpRun() override;
  void OpDo() override;

protected: 
  mafVMEGroup *m_Group;
};
#endif
