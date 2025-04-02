#pragma once

#include "mafOp.h"

//----------------------------------------------------------------------------
// mafOpCreateCenterLine :
//----------------------------------------------------------------------------
/** */
class MAF_EXPORT mafOpCreateCenterLine : public mafOp
{
public:
	mafOpCreateCenterLine(const mafString& label = _R("CreateCenterLine"));
	~mafOpCreateCenterLine() override;

	mafTypeMacroN(mafOpCreateCenterLine);

  mafOp* Copy() override;

  bool Accept(mafNode *node) override;
  void OpRun() override;
};
