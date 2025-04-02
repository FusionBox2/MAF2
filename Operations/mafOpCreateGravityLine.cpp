#include "mafOpCreateGravityLine.h"
#include "mafDecl.h"
#include "mafEvent.h"

#include "mafVMEGravityLine.h"

//----------------------------------------------------------------------------
mafOpCreateGravityLine::mafOpCreateGravityLine(const mafString& label) : Superclass(label)
//----------------------------------------------------------------------------
{
  m_OpType	= OPTYPE_OP;
  m_Canundo = true;
}
//----------------------------------------------------------------------------
mafOpCreateGravityLine::~mafOpCreateGravityLine()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
mafOp* mafOpCreateGravityLine::Copy()
//----------------------------------------------------------------------------
{
	return new mafOpCreateGravityLine(GetLabel());
}
//----------------------------------------------------------------------------
bool mafOpCreateGravityLine::Accept(mafNode *node)
//----------------------------------------------------------------------------
{
	//return (node && node->IsMAFType(mafVMEPlane));
	return node;
}
//----------------------------------------------------------------------------
void mafOpCreateGravityLine::OpRun()
//----------------------------------------------------------------------------
{
  auto gravityLine = mafVMEGravityLine::NewSPtr();
  gravityLine->SetName(_R("gravity_line"));
  SetOutput(gravityLine);
  {mafEvent evUnq(this,OP_RUN_OK); InvokeEvent(evUnq);}
}
