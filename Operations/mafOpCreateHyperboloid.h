#pragma once

#include "mafOp.h"

//----------------------------------------------------------------------------
// mafOpCreateHyperboloid :
//----------------------------------------------------------------------------
/** */
class MAF_EXPORT mafOpCreateHyperboloid : public mafOp
{
public:
	mafOpCreateHyperboloid(const mafString& label = _R("Create Hyperboloid"));
	~mafOpCreateHyperboloid() override;

	mafTypeMacroN(mafOpCreateHyperboloid);

  mafOp* Copy() override;

  bool Accept(mafNode *node) override;
  void OpRun() override;
};
