#pragma once
#include "mafOp.h"

//----------------------------------------------------------------------------
// mafOpCreateGroup :
//----------------------------------------------------------------------------
/** */
class MAF_EXPORT mafOpCreateGroup: public mafOp
{
public:
  mafOpCreateGroup(const mafString& label = _R("CreateGroup"));
  ~mafOpCreateGroup() override; 

  mafTypeMacroN(mafOpCreateGroup);

  mafOp* Copy() override;

  bool Accept(mafNode *node) override;
  void OpRun() override;
};
