#include "lhpOpCreateRefSysLM.h"
#include "mafDecl.h"
#include "mafEvent.h"

#include "mafVMERefSysAbstract.h"
#include "mafVMELandmarkCloud.h"

//----------------------------------------------------------------------------
lhpOpCreateRefSysLM::lhpOpCreateRefSysLM(const mafString& label) : Superclass(label)
//----------------------------------------------------------------------------
{
  m_OpType	= OPTYPE_OP;
  m_Canundo = true;
}
//----------------------------------------------------------------------------
lhpOpCreateRefSysLM::~lhpOpCreateRefSysLM()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
mafOp* lhpOpCreateRefSysLM::Copy()   
//----------------------------------------------------------------------------
{
	return new lhpOpCreateRefSysLM(GetLabel());
}
//----------------------------------------------------------------------------
bool lhpOpCreateRefSysLM::Accept(mafNode *node)
//----------------------------------------------------------------------------
{
  return (node && node->IsMAFType(mafVMERefSysAbstract));
}
//----------------------------------------------------------------------------
void lhpOpCreateRefSysLM::OpRun()
//----------------------------------------------------------------------------
{
  auto cloud = mafVMELandmarkCloud::NewSPtr();
  cloud->SetName(_R("points"));
  cloud->SetRadius(5.0);
  cloud->AppendLandmark(_R("O"));
  cloud->SetLandmark(_R("O"), 0.0, 0.0, 0.0);
  cloud->AppendLandmark(_R("X"));
  cloud->SetLandmark(_R("X"), 20.0, 0.0, 0.0);
  cloud->AppendLandmark(_R("Y"));
  cloud->SetLandmark(_R("Y"), 0.0, 20.0, 0.0);
  cloud->AppendLandmark(_R("Z"));
  cloud->SetLandmark(_R("Z"), 0.5, 0.0, 20.0);
  SetOutput(cloud);
  {mafEvent evUnq(this,OP_RUN_OK); InvokeEvent(evUnq);}
}
