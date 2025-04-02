#include "mafOpCreateProber.h"
#include "mafDecl.h"
#include "mafEvent.h"
#include "mafVMEVolume.h"
#include "mafVMEProber.h"

//----------------------------------------------------------------------------
mafOpCreateProber::mafOpCreateProber(const mafString& label) : Superclass(label)
//----------------------------------------------------------------------------
{
  m_OpType	= OPTYPE_OP;
  m_Canundo = true;
}
//----------------------------------------------------------------------------
mafOpCreateProber::~mafOpCreateProber( ) 
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
mafOp* mafOpCreateProber::Copy()   
//----------------------------------------------------------------------------
{
	return new mafOpCreateProber(GetLabel());
}
//----------------------------------------------------------------------------
bool mafOpCreateProber::Accept(mafNode *node)
//----------------------------------------------------------------------------
{
  return (node != nullptr);
}
//----------------------------------------------------------------------------
void mafOpCreateProber::OpRun()
//----------------------------------------------------------------------------
{
  auto prober = mafVMEProber::NewSPtr();
  prober->SetName(_R("prober"));
  SetOutput(prober);
  {mafEvent evUnq(this,OP_RUN_OK); InvokeEvent(evUnq);}
}
