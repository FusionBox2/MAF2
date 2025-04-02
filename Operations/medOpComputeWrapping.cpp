#include "medOpComputeWrapping.h"
#include "mafDecl.h"
#include "mafEvent.h"

#include "medVMEComputeWrapping.h"

//----------------------------------------------------------------------------
medOpComputeWrapping::medOpComputeWrapping(const mafString& label) : Superclass(label)
//----------------------------------------------------------------------------
{
  m_OpType	= OPTYPE_OP;
  m_Canundo = true;
}
//----------------------------------------------------------------------------
medOpComputeWrapping::~medOpComputeWrapping( ) 
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
mafOp* medOpComputeWrapping::Copy()   
//----------------------------------------------------------------------------
{
	return new medOpComputeWrapping(GetLabel());
}
//----------------------------------------------------------------------------
bool medOpComputeWrapping::Accept(mafNode *node)
//----------------------------------------------------------------------------
{
  return (node && node->IsMAFType(mafVME));
}
//----------------------------------------------------------------------------
void medOpComputeWrapping::OpRun()   
//----------------------------------------------------------------------------
{
  auto meter = medVMEComputeWrapping::NewSPtr();
  meter->SetName(_R("Wrapped Action Line"));
  SetOutput(meter);
  {mafEvent evUnq(this,OP_RUN_OK); InvokeEvent(evUnq);}
}
