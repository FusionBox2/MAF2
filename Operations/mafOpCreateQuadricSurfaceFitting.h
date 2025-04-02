#pragma once

#include "mafOp.h"

//----------------------------------------------------------------------------
// mafOpCreateQuadricSurfaceFitting :
//----------------------------------------------------------------------------
/** */
class MAF_EXPORT mafOpCreateQuadricSurfaceFitting : public mafOp
{
public:
	mafOpCreateQuadricSurfaceFitting(const mafString& label = _R("Create Quadric Surface Fitting"));
	~mafOpCreateQuadricSurfaceFitting() override;

	mafTypeMacroN(mafOpCreateQuadricSurfaceFitting);

	mafOp* Copy() override;

	bool Accept(mafNode *node) override;
	void OpRun() override;
};
