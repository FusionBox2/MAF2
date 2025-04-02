#pragma once

#include "mafOp.h"

//----------------------------------------------------------------------------
// mafOpCreateGravityLine :
//----------------------------------------------------------------------------
/** */
class MAF_EXPORT mafOpCreateGravityLine : public mafOp
{
public:
	mafOpCreateGravityLine(const mafString& label = _R("CreateGravityLine"));
	~mafOpCreateGravityLine() override;

	mafTypeMacroN(mafOpCreateGravityLine);

  mafOp* Copy() override;

  bool Accept(mafNode *node) override;
  void OpRun() override;
};
