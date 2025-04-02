#include "medOpCreateWrappedMeter.h"
#include "mafDecl.h"
#include "mafEvent.h"

#include "medVMEWrappedMeter.h"

//----------------------------------------------------------------------------
medOpCreateWrappedMeter::medOpCreateWrappedMeter(const mafString& label) : Superclass(label)
//----------------------------------------------------------------------------
{
  m_OpType	= OPTYPE_OP;
  m_Canundo = true;
}
//----------------------------------------------------------------------------
medOpCreateWrappedMeter::~medOpCreateWrappedMeter( ) 
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
mafOp* medOpCreateWrappedMeter::Copy()   
//----------------------------------------------------------------------------
{
	return new medOpCreateWrappedMeter(GetLabel());
}
//----------------------------------------------------------------------------
bool medOpCreateWrappedMeter::Accept(mafNode *node)
//----------------------------------------------------------------------------
{
  return (node && node->IsMAFType(mafVME));
}
//----------------------------------------------------------------------------
void medOpCreateWrappedMeter::OpRun()   
//----------------------------------------------------------------------------
{
  auto meter = medVMEWrappedMeter::NewSPtr();
  meter->SetName(_R("Wrapped Meter"));
  SetOutput(meter);
  {mafEvent evUnq(this,OP_RUN_OK); InvokeEvent(evUnq);}
}
