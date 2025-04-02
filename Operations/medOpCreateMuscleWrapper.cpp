#include "medOpCreateMuscleWrapper.h"
#include "mafDecl.h"
#include "mafEvent.h"

#include "../VME/medVMEMuscleWrapper.h"

//----------------------------------------------------------------------------
medOpCreateMuscleWrapper::medOpCreateMuscleWrapper(const mafString& label) : Superclass(label)
//----------------------------------------------------------------------------
{
  m_OpType	= OPTYPE_OP;
  m_Canundo = true;
}
//----------------------------------------------------------------------------
medOpCreateMuscleWrapper::~medOpCreateMuscleWrapper( ) 
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
mafOp* medOpCreateMuscleWrapper::Copy()   
//----------------------------------------------------------------------------
{
	return new medOpCreateMuscleWrapper(GetLabel());
}
//----------------------------------------------------------------------------
bool medOpCreateMuscleWrapper::Accept(mafNode *node)
//----------------------------------------------------------------------------
{
  return (node && node->IsMAFType(mafVME));
}
//----------------------------------------------------------------------------
void medOpCreateMuscleWrapper::OpRun()   
//----------------------------------------------------------------------------
{
  auto meter = medVMEMuscleWrapper::NewSPtr();
  meter->SetName(_R("Muscle Wrapped Meter"));
  SetOutput(meter);
  {mafEvent evUnq(this,OP_RUN_OK); InvokeEvent(evUnq);}
}
