#include "mafOpCreateMeter2.h"
#include "mafDecl.h"
#include "mafEvent.h"
#include "mafVMEMeter.h"

//----------------------------------------------------------------------------
mafOpCreateMeter2::mafOpCreateMeter2(const mafString& label) : Superclass(label)
//----------------------------------------------------------------------------
{
  m_OpType	= OPTYPE_OP;
  m_Canundo = true;
}
//----------------------------------------------------------------------------
mafOpCreateMeter2::~mafOpCreateMeter2( ) 
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
mafOp* mafOpCreateMeter2::Copy()   
//----------------------------------------------------------------------------
{
	return new mafOpCreateMeter2(GetLabel());
}
//----------------------------------------------------------------------------
bool mafOpCreateMeter2::Accept(mafNode *node)
//----------------------------------------------------------------------------
{
  return (node && node->IsMAFType(mafVME));
}
//----------------------------------------------------------------------------
void mafOpCreateMeter2::OpRun()   
//----------------------------------------------------------------------------
{
  auto meter = mafVMEMeter::NewSPtr();
  meter->SetName(_R("meter"));
  SetOutput(meter);
  {mafEvent evUnq(this,OP_RUN_OK); InvokeEvent(evUnq);}
}
