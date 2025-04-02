#pragma once
#include "mafOp.h"

//----------------------------------------------------------------------------
// mafOpCreateRefSys :
//----------------------------------------------------------------------------
/** */
class MAF_EXPORT mafOpCreateRefSys: public mafOp
{
public:
  mafOpCreateRefSys(const mafString& label = _R("CreateRefSys"));
 ~mafOpCreateRefSys() override; 

  mafTypeMacroN(mafOpCreateRefSys);

  mafOp* Copy() override;

  bool Accept(mafNode *node) override;
  void OpRun() override;
  void OpDo() override;
};
