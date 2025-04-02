#pragma once

#include "mafOp.h"

//----------------------------------------------------------------------------
// mafOpCreateSurfaceParametric :
//----------------------------------------------------------------------------
/** */
class MAF_EXPORT mafOpCreateMuscleWrapping2 : public mafOp
{
public:
	mafOpCreateMuscleWrapping2(const mafString& label = _R("Create Quadric Surface Fitting"));
	~mafOpCreateMuscleWrapping2() override;

	mafTypeMacroN(mafOpCreateMuscleWrapping2);

	mafOp* Copy() override;

	bool Accept(mafNode *node) override;
	void OpRun() override;
};
