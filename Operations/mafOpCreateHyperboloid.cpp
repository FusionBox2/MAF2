#include "mafOpCreateHyperboloid.h"
#include "mafDecl.h"
#include "mafEvent.h"

#include "mafVMEHyperboloid.h"

//----------------------------------------------------------------------------
mafOpCreateHyperboloid::mafOpCreateHyperboloid(const mafString& label) : Superclass(label)
//----------------------------------------------------------------------------
{
  m_OpType	= OPTYPE_OP;
  m_Canundo = true;
}
//----------------------------------------------------------------------------
mafOpCreateHyperboloid::~mafOpCreateHyperboloid()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
mafOp* mafOpCreateHyperboloid::Copy()
//----------------------------------------------------------------------------
{
	return new mafOpCreateHyperboloid(GetLabel());
}
//----------------------------------------------------------------------------
bool mafOpCreateHyperboloid::Accept(mafNode *node)
//----------------------------------------------------------------------------
{
  return (node && node->IsMAFType(mafVME));
}
//----------------------------------------------------------------------------
void mafOpCreateHyperboloid::OpRun()
//----------------------------------------------------------------------------
{
	auto hyperboloid = mafVMEHyperboloid::NewSPtr();
	hyperboloid->SetName(_R("Hyperboloid"));
	SetOutput(hyperboloid);
  {mafEvent evUnq(this,OP_RUN_OK); InvokeEvent(evUnq);}
}
