#pragma once
#include "mafOp.h"

//----------------------------------------------------------------------------
// mafOpCreateProber :
//----------------------------------------------------------------------------
/** */
class MAF_EXPORT mafOpCreateProber: public mafOp
{
public:
  mafOpCreateProber(const mafString& label = _R("CreateProber"));
 ~mafOpCreateProber() override; 

  mafTypeMacroN(mafOpCreateProber);

  mafOp* Copy() override;

  bool Accept(mafNode *node) override;
  void OpRun() override;
};
