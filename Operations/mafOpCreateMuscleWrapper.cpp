#include "mafOpCreateMuscleWrapper.h"
#include "mafDecl.h"
#include "mafEvent.h"

#include "mafVMEMuscleWrapper.h"

//----------------------------------------------------------------------------
mafOpCreateMuscleWrapperAQ::mafOpCreateMuscleWrapperAQ(const mafString& label) : Superclass(label)
//----------------------------------------------------------------------------
{
	m_OpType = OPTYPE_OP;
	m_Canundo = true;
}
//----------------------------------------------------------------------------
mafOpCreateMuscleWrapperAQ::~mafOpCreateMuscleWrapperAQ()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
mafOp* mafOpCreateMuscleWrapperAQ::Copy()
//----------------------------------------------------------------------------
{
	return new mafOpCreateMuscleWrapperAQ(GetLabel());
}
//----------------------------------------------------------------------------
bool mafOpCreateMuscleWrapperAQ::Accept(mafNode *node)
//----------------------------------------------------------------------------
{
	
	return (node && node->IsMAFType(mafVME));
}
//----------------------------------------------------------------------------
void mafOpCreateMuscleWrapperAQ::OpRun()
//----------------------------------------------------------------------------
{
	auto muscleWrapper = mafVMEMuscleWrapperAQ::NewSPtr();
	muscleWrapper->SetName(_R("Muscle_Wrapper"));
	SetOutput(muscleWrapper);
	{mafEvent evUnq(this, OP_RUN_OK); InvokeEvent(evUnq);}
}
