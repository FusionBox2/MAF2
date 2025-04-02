#pragma once
#include "mafOp.h"

//----------------------------------------------------------------------------
// mafOpCreateMeter :
//----------------------------------------------------------------------------
/** */
class MAF_EXPORT mafOpCreateMeter2: public mafOp
{
public:
  mafOpCreateMeter2(const mafString& label = _R("CreateMeter"));
  ~mafOpCreateMeter2() override; 

  mafTypeMacroN(mafOpCreateMeter2);

  mafOp* Copy() override;

  bool Accept(mafNode *node) override;
  void OpRun() override;
};
