#pragma once

#include "mafOp.h"

//----------------------------------------------------------------------------
// mafOpCreateMeter :
//----------------------------------------------------------------------------
/** */
class MAF_EXPORT mafOpCreateMeter: public mafOp
{
public:
  mafOpCreateMeter(const mafString& label = _R("CreateMeter"));
  ~mafOpCreateMeter() override; 

  mafTypeMacroN(mafOpCreateMeter);

  mafOp* Copy() override;

  bool Accept(mafNode *node) override;
  void OpRun() override;
};
