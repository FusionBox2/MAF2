#include "lhpOpCreateSurfaceScalar.h"
#include "mafDecl.h"

#include "lhpVMESurfaceScalarVarying.h"

//----------------------------------------------------------------------------
lhpOpCreateSurfaceScalar::lhpOpCreateSurfaceScalar(const mafString& label) : Superclass(label)
//----------------------------------------------------------------------------
{
  m_OpType	= OPTYPE_OP;
  m_Canundo = true;
}
//----------------------------------------------------------------------------
lhpOpCreateSurfaceScalar::~lhpOpCreateSurfaceScalar()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
mafOp* lhpOpCreateSurfaceScalar::Copy()   
//----------------------------------------------------------------------------
{
	return new lhpOpCreateSurfaceScalar(GetLabel());
}
//----------------------------------------------------------------------------
bool lhpOpCreateSurfaceScalar::Accept(mafNode *node)
//----------------------------------------------------------------------------
{
  return (node && node->IsMAFType(mafVME));
}
//----------------------------------------------------------------------------
void lhpOpCreateSurfaceScalar::OpRun()
//----------------------------------------------------------------------------
{
  auto surfaceScalar = lhpVMESurfaceScalarVarying::NewSPtr();
  surfaceScalar->SetName(_R("Surface Scalar"));
  SetOutput(surfaceScalar);
  {mafEvent evUnq(this,OP_RUN_OK); InvokeEvent(evUnq);}
}
