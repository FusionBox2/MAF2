#include "mafOpCreateCylinder.h"
#include "mafDecl.h"
#include "mafEvent.h"

#include "mafVMECylinder.h"

//----------------------------------------------------------------------------
mafOpCreateCylinder::mafOpCreateCylinder(const mafString& label) : Superclass(label)
//----------------------------------------------------------------------------
{
  m_OpType	= OPTYPE_OP;
  m_Canundo = true;
}
//----------------------------------------------------------------------------
mafOpCreateCylinder::~mafOpCreateCylinder()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
mafOp* mafOpCreateCylinder::Copy()
//----------------------------------------------------------------------------
{
	return new mafOpCreateCylinder(GetLabel());
}
//----------------------------------------------------------------------------
bool mafOpCreateCylinder::Accept(mafNode *node)
//----------------------------------------------------------------------------
{
  return (node && node->IsMAFType(mafVME));
}
//----------------------------------------------------------------------------
void mafOpCreateCylinder::OpRun()
//----------------------------------------------------------------------------
{
	auto cylinder = mafVMECylinder::NewSPtr();
	cylinder->SetName(_R("Cylinder"));
	SetOutput(cylinder);
  {mafEvent evUnq(this,OP_RUN_OK); InvokeEvent(evUnq);}
}
