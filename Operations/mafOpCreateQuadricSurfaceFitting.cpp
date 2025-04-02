#include "mafOpCreateQuadricSurfaceFitting.h"
#include "mafDecl.h"
#include "mafEvent.h"

#include "mafVMEQuadricSurfaceFitting.h"

//----------------------------------------------------------------------------
mafOpCreateQuadricSurfaceFitting::mafOpCreateQuadricSurfaceFitting(const mafString& label) : Superclass(label)
//----------------------------------------------------------------------------
{
	m_OpType = OPTYPE_OP;
	m_Canundo = true;
}
//----------------------------------------------------------------------------
mafOpCreateQuadricSurfaceFitting::~mafOpCreateQuadricSurfaceFitting()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
mafOp* mafOpCreateQuadricSurfaceFitting::Copy()
//----------------------------------------------------------------------------
{
	return new mafOpCreateQuadricSurfaceFitting(GetLabel());
}
//----------------------------------------------------------------------------
bool mafOpCreateQuadricSurfaceFitting::Accept(mafNode *node)
//----------------------------------------------------------------------------
{
	return (node && node->IsMAFType(mafVME));
}
//----------------------------------------------------------------------------
void mafOpCreateQuadricSurfaceFitting::OpRun()
//----------------------------------------------------------------------------
{
	auto quadricSurfaceFitting = mafVMEQuadricSurfaceFitting::NewSPtr();
	quadricSurfaceFitting->SetName(_R("Quadric_Surface_Fitting"));
	SetOutput(quadricSurfaceFitting);
	{mafEvent evUnq(this, OP_RUN_OK); InvokeEvent(evUnq);}
}
