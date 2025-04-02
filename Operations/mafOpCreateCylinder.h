#pragma once

#include "mafOp.h"

//----------------------------------------------------------------------------
// mafOpCreateCylinder :
//----------------------------------------------------------------------------
/** */
class MAF_EXPORT mafOpCreateCylinder : public mafOp
{
public:
	mafOpCreateCylinder(const mafString& label = _R("Create Cylinder"));
	~mafOpCreateCylinder() override;

	mafTypeMacroN(mafOpCreateCylinder);

  mafOp* Copy() override;

  bool Accept(mafNode *node) override;
  void OpRun() override;
};
