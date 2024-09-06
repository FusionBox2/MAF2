/*=========================================================================

 Program: MAF2
 Module: mafOpCreateMeter
 Authors: Paolo Quadrani
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafOpCreateMeter2_H__
#define __mafOpCreateMeter2_H__

#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVMEMuscleWrapperAQ;
class mafVMEMeter;
class mafGUI;
class mafEvent;
//----------------------------------------------------------------------------
// mafOpCreateMeter :
//----------------------------------------------------------------------------
/** */
class MAF_EXPORT mafOpCreateMeter2: public mafOp
{
public:
  mafOpCreateMeter2(const mafString& label = _R("CreateMeter"));
  ~mafOpCreateMeter2() override; 

  mafTypeMacro(mafOpCreateMeter2, mafOp);

  mafOp* Copy() override;

  bool Accept(mafNode *node) override;
  void OpRun() override;
  void OpDo() override;

protected: 
  mafVMEMeter *m_Meter;
};
#endif
