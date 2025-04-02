#include "mafOpCreateSurfaceParametric.h"
#include "mafDecl.h"
#include "mafEvent.h"

#include "mafVMESurfaceParametric.h"

//----------------------------------------------------------------------------
mafOpCreateSurfaceParametric::mafOpCreateSurfaceParametric(const mafString& label) : Superclass(label)
//----------------------------------------------------------------------------
{
  m_OpType	= OPTYPE_OP;
  m_Canundo = true;
}
//----------------------------------------------------------------------------
mafOpCreateSurfaceParametric::~mafOpCreateSurfaceParametric( ) 
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
mafOp* mafOpCreateSurfaceParametric::Copy()   
//----------------------------------------------------------------------------
{
	return new mafOpCreateSurfaceParametric(GetLabel());
}
//----------------------------------------------------------------------------
bool mafOpCreateSurfaceParametric::Accept(mafNode *node)
//----------------------------------------------------------------------------
{
  return (node && node->IsMAFType(mafVME));
}
//----------------------------------------------------------------------------
void mafOpCreateSurfaceParametric::OpRun()   
//----------------------------------------------------------------------------
{
  auto surfaceParametric = mafVMESurfaceParametric::NewSPtr();
  surfaceParametric->SetName(_R("Surface Parametric"));
  SetOutput(surfaceParametric);
  {mafEvent evUnq(this,OP_RUN_OK); InvokeEvent(evUnq);}
}
