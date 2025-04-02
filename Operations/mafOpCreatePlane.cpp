#include "mafOpCreatePlane.h"
#include "mafDecl.h"
#include "mafEvent.h"

#include "mafVMEPlane.h"

//----------------------------------------------------------------------------
mafOpCreatePlane::mafOpCreatePlane(const mafString& label) : Superclass(label)
//----------------------------------------------------------------------------
{
	m_OpType = OPTYPE_OP;
	m_Canundo = true;
}
//----------------------------------------------------------------------------
mafOpCreatePlane::~mafOpCreatePlane()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
mafOp* mafOpCreatePlane::Copy()
//----------------------------------------------------------------------------
{
	return new mafOpCreatePlane(GetLabel());
}
//----------------------------------------------------------------------------
bool mafOpCreatePlane::Accept(mafNode *node)
//----------------------------------------------------------------------------
{
  return (node && node->IsMAFType(mafVME));
}
//----------------------------------------------------------------------------
void mafOpCreatePlane::OpRun()
//----------------------------------------------------------------------------
{
	auto plane = mafVMEPlane::NewSPtr();
	plane->SetName(_R("Plane"));
	SetOutput(plane);
	{mafEvent evUnq(this, OP_RUN_OK); InvokeEvent(evUnq);}
}
