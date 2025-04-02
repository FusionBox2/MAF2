#pragma once
#include "mafOp.h"

//----------------------------------------------------------------------------
// mafOpCreateMuscleWrapperAQ :
//----------------------------------------------------------------------------
/** */
class MAF_EXPORT mafOpCreateMuscleWrapperAQ : public mafOp
{
public:
	mafOpCreateMuscleWrapperAQ(const mafString& label = _R("Muscle WrapperAQ"));
	~mafOpCreateMuscleWrapperAQ() override;

	mafTypeMacroN(mafOpCreateMuscleWrapperAQ);

	mafOp* Copy() override;

	bool Accept(mafNode *node) override;
	void OpRun() override;
};
