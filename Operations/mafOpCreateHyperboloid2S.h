#pragma once

#include "mafOp.h"

//----------------------------------------------------------------------------
// mafOpCreateHyperboloid2S :
//----------------------------------------------------------------------------
/** */
class MAF_EXPORT mafOpCreateHyperboloid2S : public mafOp
{
public:
	mafOpCreateHyperboloid2S(const mafString& label = _R("Create Hyperboloid"));
	~mafOpCreateHyperboloid2S() override;

	mafTypeMacroN(mafOpCreateHyperboloid2S);

  mafOp* Copy() override;

  bool Accept(mafNode *node) override;
  void OpRun() override;
};
