#include "mafOpCreateGroup.h"
#include "mafDecl.h"
#include "mafEvent.h"

#include "mafVMERoot.h"
#include "mafVMEGroup.h"

//----------------------------------------------------------------------------
mafOpCreateGroup::mafOpCreateGroup(const mafString& label) : Superclass(label)
//----------------------------------------------------------------------------
{
  m_OpType	= OPTYPE_OP;
  m_Canundo = true;
}
//----------------------------------------------------------------------------
mafOpCreateGroup::~mafOpCreateGroup()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
mafOp* mafOpCreateGroup::Copy()   
//----------------------------------------------------------------------------
{
	return new mafOpCreateGroup(GetLabel());
}
//----------------------------------------------------------------------------
bool mafOpCreateGroup::Accept(mafNode *node)
//----------------------------------------------------------------------------
{
  return (node && node->IsMAFType(mafVME));
}
//----------------------------------------------------------------------------
void mafOpCreateGroup::OpRun()
//----------------------------------------------------------------------------
{
  auto group = mafVMEGroup::NewSPtr();
  group->SetName(_R("group"));
  SetOutput(group);
  {mafEvent evUnq(this,OP_RUN_OK); InvokeEvent(evUnq);}
}
