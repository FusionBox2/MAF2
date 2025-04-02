#include "mafOpCreateHyperboloid2S.h"
#include "mafDecl.h"
#include "mafEvent.h"

#include "mafVMEHyperboloid2S.h"

//----------------------------------------------------------------------------
mafOpCreateHyperboloid2S::mafOpCreateHyperboloid2S(const mafString& label) : Superclass(label)
//----------------------------------------------------------------------------
{
  m_OpType	= OPTYPE_OP;
  m_Canundo = true;
}
//----------------------------------------------------------------------------
mafOpCreateHyperboloid2S::~mafOpCreateHyperboloid2S()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
mafOp* mafOpCreateHyperboloid2S::Copy()
//----------------------------------------------------------------------------
{
	return new mafOpCreateHyperboloid2S(GetLabel());
}
//----------------------------------------------------------------------------
bool mafOpCreateHyperboloid2S::Accept(mafNode *node)
//----------------------------------------------------------------------------
{
  return (node && node->IsMAFType(mafVME));
}
//----------------------------------------------------------------------------
void mafOpCreateHyperboloid2S::OpRun()
//----------------------------------------------------------------------------
{
	auto hyperboloid = mafVMEHyperboloid2S::NewSPtr();
	hyperboloid->SetName(_R("Hyperboloid2S"));
	SetOutput(hyperboloid);
  {mafEvent evUnq(this,OP_RUN_OK); InvokeEvent(evUnq);}
}
