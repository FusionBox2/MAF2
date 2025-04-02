#include "mafOpCreateSlicer.h"
#include "mafDecl.h"
#include "mafEvent.h"

#include "mafVMESlicer.h"

//----------------------------------------------------------------------------
mafOpCreateSlicer::mafOpCreateSlicer(const mafString& label) : Superclass(label)
//----------------------------------------------------------------------------
{
  m_OpType	= OPTYPE_OP;
  m_Canundo = true;
}
//----------------------------------------------------------------------------
mafOpCreateSlicer::~mafOpCreateSlicer()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
mafOp* mafOpCreateSlicer::Copy()   
//----------------------------------------------------------------------------
{
	return new mafOpCreateSlicer(GetLabel());
}
//----------------------------------------------------------------------------
bool mafOpCreateSlicer::Accept(mafNode *node)
//----------------------------------------------------------------------------
{
  return (node != NULL);
}
//----------------------------------------------------------------------------
void mafOpCreateSlicer::OpRun()
//----------------------------------------------------------------------------
{
  mafString title = _L("Choose VME to slice");
  mafEvent e;
  e.SetId(VME_CHOOSE);
  e.SetArg((intptr_t)&mafOpCreateSlicer::VolumeAccept);
  e.SetString(&title);
  InvokeEvent(e);

  int result = OP_RUN_CANCEL;

  mafNode *n = e.GetVme();
  if (n)
  {
    auto slicer = mafVMESlicer::NewSPtr();
		slicer->SetName(_R("slicer"));
		SetOutput(slicer);

    m_SlicedVME = n;
    slicer->SetSlicedVMELink(m_SlicedVME);
    result = OP_RUN_OK;
  }
  {mafEvent evUnq(this, result); InvokeEvent(evUnq);}
}
//----------------------------------------------------------------------------
void mafOpCreateSlicer::OpDo()
//----------------------------------------------------------------------------
{
  Superclass::OpDo();
  double center[3] = {0.0,0.0,0.0}, rot[3] = {0.0,0.0,0.0};
  mafOBB b;
  rot[0] = rot[1] = rot[2] = 0;
  mafVME::StaticDownCast(m_SlicedVME)->GetOutput()->GetVMELocalBounds(b);
  if (b.IsValid())
  {
    b.GetCenter(center);
  }
  mafVMESlicer::StaticDownCast(GetOutput())->SetPose(center,rot,0);
}
