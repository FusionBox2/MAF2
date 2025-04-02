#include "mafOpCreateEllipsoid.h"
#include "mafDecl.h"
#include "mafEvent.h"

#include "mafVMEEllipsoid.h"


//----------------------------------------------------------------------------
mafOpCreateEllipsoid::mafOpCreateEllipsoid(const mafString& label) : Superclass(label)
//----------------------------------------------------------------------------
{
  m_OpType	= OPTYPE_OP;
  m_Canundo = true;
}
//----------------------------------------------------------------------------
mafOpCreateEllipsoid::~mafOpCreateEllipsoid()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
mafOp* mafOpCreateEllipsoid::Copy()
//----------------------------------------------------------------------------
{
	return new mafOpCreateEllipsoid(GetLabel());
}
//----------------------------------------------------------------------------
bool mafOpCreateEllipsoid::Accept(mafNode *node)
//----------------------------------------------------------------------------
{
  return (node && node->IsMAFType(mafVME));
}
//----------------------------------------------------------------------------
void mafOpCreateEllipsoid::OpRun()
//----------------------------------------------------------------------------
{
	auto ellipsoid = mafVMEEllipsoid::NewSPtr();
	ellipsoid->SetName(_R("Ellipsoid"));
	SetOutput(ellipsoid);
  {mafEvent evUnq(this,OP_RUN_OK); InvokeEvent(evUnq);}
}
