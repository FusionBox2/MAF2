/*=========================================================================

 Program: MAF2
 Module: mafOpCreateGroup
 Authors: Paolo Quadrani
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafOpCreateGroup_H__
#define __mafOpCreateGroup_H__

#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVMEGroup;
class mafNode;
//----------------------------------------------------------------------------
// mafOpCreateGroup :
//----------------------------------------------------------------------------
/** */
class MAF_EXPORT mafOpCreateGroup: public mafOp
{
public:
  mafOpCreateGroup(const mafString& label = _R("CreateGroup"));
  ~mafOpCreateGroup() override; 

  mafTypeMacro(mafOpCreateGroup, mafOp);

  mafOp* Copy() override;

  bool Accept(mafNode *node) override;
  void OpRun() override;
  void OpDo() override;

protected: 
  mafVMEGroup *m_Group;
};
#endif
