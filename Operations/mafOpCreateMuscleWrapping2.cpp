#include "mafOpCreateMuscleWrapping2.h"
#include "mafDecl.h"
#include "mafEvent.h"

#include "mafVMEMuscleWrapping.h"

//----------------------------------------------------------------------------
mafOpCreateMuscleWrapping2::mafOpCreateMuscleWrapping2(const mafString& label) : Superclass(label)
//----------------------------------------------------------------------------
{
	m_OpType = OPTYPE_OP;
	m_Canundo = true;
}
//----------------------------------------------------------------------------
mafOpCreateMuscleWrapping2::~mafOpCreateMuscleWrapping2()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
mafOp* mafOpCreateMuscleWrapping2::Copy()
//----------------------------------------------------------------------------
{
	return new mafOpCreateMuscleWrapping2(GetLabel());
}
//----------------------------------------------------------------------------
bool mafOpCreateMuscleWrapping2::Accept(mafNode *node)
//----------------------------------------------------------------------------
{
	return (node && node->IsMAFType(mafVME));
}
//----------------------------------------------------------------------------
void mafOpCreateMuscleWrapping2::OpRun()
//----------------------------------------------------------------------------
{
	auto quadricSurfaceFitting = mafVMEMuscleWrapping::NewSPtr();
	quadricSurfaceFitting->SetName(_R("Quadric_Surface_Wrapping"));
	SetOutput(quadricSurfaceFitting);
	{mafEvent evUnq(this, OP_RUN_OK); InvokeEvent(evUnq);}
}
