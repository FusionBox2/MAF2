#include "mafDecl.h"
#include "lhpOpHelAxis.h"
#include "mafVMEHelAxis.h"
#include "mafVMELandmarkCloud.h"


//----------------------------------------------------------------------------
lhpOpHelAxis::lhpOpHelAxis(const mafString& label) : Superclass(label)
//----------------------------------------------------------------------------
{
  m_OpType            = OPTYPE_OP;
  m_Canundo           = true;
}

//----------------------------------------------------------------------------
lhpOpHelAxis::~lhpOpHelAxis()
//----------------------------------------------------------------------------
{
  m_HelicalSys.reset();
}

//----------------------------------------------------------------------------
mafOp* lhpOpHelAxis::Copy()   
//----------------------------------------------------------------------------
{
  return new lhpOpHelAxis(GetLabel());
}

//----------------------------------------------------------------------------
bool lhpOpHelAxis::Accept(mafNode* vme)
//----------------------------------------------------------------------------
{
  if(!vme) return false;
  return true;
}


//----------------------------------------------------------------------------
void lhpOpHelAxis::OpRun()   
//----------------------------------------------------------------------------
{
  m_HelicalSys = mafVMEHelAxis::NewSPtr();
  m_HelicalSys->SetName(_R("Helical_axis"));
  auto lmc = mafVMELandmarkCloud::NewSPtr();
  lmc->SetName(_R("HA_Points"));
  lmc->AppendLandmark(0, 0,    0, _R("Pivot"));
  lmc->AppendLandmark(0, 0,  100, _R("PntFrt"));
  lmc->AppendLandmark(0, 0, -100, _R("PntBck"));
  mafNode::ReparentTo(lmc, m_HelicalSys.get());
  lmc.reset();
  {mafEvent evUnq(this,OP_RUN_OK); InvokeEvent(evUnq);} 
}


//----------------------------------------------------------------------------
void lhpOpHelAxis::OpDo()
//----------------------------------------------------------------------------
{
  assert(m_HelicalSys);
  mafNode::ReparentTo(m_HelicalSys, GetInput().get());
  m_HelicalSys->SetLengthFactor(100.0);
  m_HelicalSys->SetRadiusFactor(100.0);
  m_HelicalSys->Update();
  SetOutput(m_HelicalSys);
}

