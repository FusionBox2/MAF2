#include "mafOpCreateMeter.h"
#include "mafDecl.h"
#include "mafEvent.h"

#include "mafVMEMeter.h"

//----------------------------------------------------------------------------
mafOpCreateMeter::mafOpCreateMeter(const mafString& label) : Superclass(label)
//----------------------------------------------------------------------------
{
  m_OpType	= OPTYPE_OP;
  m_Canundo = true;
}
//----------------------------------------------------------------------------
mafOpCreateMeter::~mafOpCreateMeter( ) 
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
mafOp* mafOpCreateMeter::Copy()   
//----------------------------------------------------------------------------
{
	return new mafOpCreateMeter(GetLabel());
}
//----------------------------------------------------------------------------
bool mafOpCreateMeter::Accept(mafNode *node)
//----------------------------------------------------------------------------
{
  return (node && node->IsMAFType(mafVME));
}
//----------------------------------------------------------------------------
void mafOpCreateMeter::OpRun()   
//----------------------------------------------------------------------------
{
  auto meter = mafVMEMeter::NewSPtr();
  meter->SetName(_R("meter"));
  SetOutput(meter);
  {mafEvent evUnq(this,OP_RUN_OK); InvokeEvent(evUnq);}
}
