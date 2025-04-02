#include "mafOpCreateCenterline.h"
#include "mafDecl.h"
#include "mafEvent.h"

#include "mafVMECenterLine.h"

//----------------------------------------------------------------------------
mafOpCreateCenterLine::mafOpCreateCenterLine(const mafString& label) : Superclass(label)
//----------------------------------------------------------------------------
{
  m_OpType	= OPTYPE_OP;
  m_Canundo = true;
}
//----------------------------------------------------------------------------
mafOpCreateCenterLine::~mafOpCreateCenterLine()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
mafOp* mafOpCreateCenterLine::Copy()
//----------------------------------------------------------------------------
{
	return new mafOpCreateCenterLine(GetLabel());
}
//----------------------------------------------------------------------------
bool mafOpCreateCenterLine::Accept(mafNode *node)
//----------------------------------------------------------------------------
{

  return (node && node->IsMAFType(mafVME));
}
//----------------------------------------------------------------------------
void mafOpCreateCenterLine::OpRun()
//----------------------------------------------------------------------------
{
  auto centerLine = mafVMECenterLine::NewSPtr();
  centerLine->SetName(_R("center_line"));
  SetOutput(centerLine);
  {mafEvent evUnq(this,OP_RUN_OK); InvokeEvent(evUnq);}
}
