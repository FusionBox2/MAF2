#include "mafOpCreateRefSys.h"
#include "mafDecl.h"

#include "mafVME.h"
#include "mafVMERoot.h"
#include "mafVMERefSys.h"

//----------------------------------------------------------------------------
mafOpCreateRefSys::mafOpCreateRefSys(const mafString& label) : Superclass(label)
//----------------------------------------------------------------------------
{
  m_OpType	= OPTYPE_OP;
  m_Canundo = true;
}

//----------------------------------------------------------------------------
mafOpCreateRefSys::~mafOpCreateRefSys()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
mafOp* mafOpCreateRefSys::Copy()   
//----------------------------------------------------------------------------
{
	return new mafOpCreateRefSys(GetLabel());
}

//----------------------------------------------------------------------------
bool mafOpCreateRefSys::Accept(mafNode *node)
//----------------------------------------------------------------------------
{
  return (node && node->IsMAFType(mafVME));
}

//----------------------------------------------------------------------------
void mafOpCreateRefSys::OpRun()   
//----------------------------------------------------------------------------
{
  auto refSys = mafVMERefSys::NewSPtr();
  refSys->SetName(_R("ref_sys"));
  SetOutput(refSys);
  {mafEvent evUnq(this,OP_RUN_OK); InvokeEvent(evUnq);}
}

//----------------------------------------------------------------------------
void mafOpCreateRefSys::OpDo()
//----------------------------------------------------------------------------
{
  Superclass::OpDo();
  if (!GetInput()->IsMAFType(mafVMERoot))
    mafVMERefSys::StaticDownCast(GetOutput())->SetAbsMatrix(*mafVME::StaticDownCast(GetInput())->GetOutput()->GetAbsMatrix());
}
