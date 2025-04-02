#pragma once
#include "mafOp.h"

//----------------------------------------------------------------------------
// lhpOpCreateMetersScripted :
//----------------------------------------------------------------------------
/** */
class MAF_EXPORT lhpOpCreateMetersScripted: public mafOp
{
public:
  lhpOpCreateMetersScripted(const mafString& label = _R("Scripted meters"));
  ~lhpOpCreateMetersScripted() override; 

  mafTypeMacroN(lhpOpCreateMetersScripted);

  mafOp* Copy() override;

  bool Accept(mafNode *node) override;
  void OpRun() override;
};
