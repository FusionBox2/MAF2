#include "mafOpReparentTo.h"
#include "mafDecl.h"
#include "mafEvent.h"
#include "mafVMERoot.h"
#include "mmuTimeSet.h"
#include "mafTransformFrame.h"
#include "mafVMELandmarkCloud.h"
#include "mafAbsMatrixPipe.h"

#include "vtkPolyData.h"
#include "vtkTransform.h"
#include "vtkTransformPolyDataFilter.h"

#include <vector>

//----------------------------------------------------------------------------
mafOpReparentTo::mafOpReparentTo(const mafString& label, bool keepGlobal) : Superclass(label)
//----------------------------------------------------------------------------
{
  m_OpType    = OPTYPE_OP;
  m_Canundo   = true;
  m_OldParent = NULL;
  m_TargetVme = NULL;
  m_KeepGlobal = keepGlobal;
}
//----------------------------------------------------------------------------
mafOpReparentTo::~mafOpReparentTo( ) 
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
bool mafOpReparentTo::Accept(mafNode *node)
//----------------------------------------------------------------------------
{
  return (node != NULL && node->IsMAFType(mafVME) && !node->IsMAFType(mafVMERoot) /*&& !node->IsMAFType(mafVMEExternalData)*/);
}
//----------------------------------------------------------------------------
mafOp* mafOpReparentTo::Copy()   
//----------------------------------------------------------------------------
{
  mafOpReparentTo *cp = new mafOpReparentTo(GetLabel());
	cp->m_OldParent = m_OldParent;
  cp->m_KeepGlobal = m_KeepGlobal;
  return cp;
}
//----------------------------------------------------------------------------
void mafOpReparentTo::OpRun()   
//----------------------------------------------------------------------------
{
  if (m_TargetVme == NULL)
  {
    mafEvent e(this,VME_CHOOSE);
    InvokeEvent(e);
    m_TargetVme = mafVME::SafeDownCast(e.GetVme());
  }
	
	int result = OP_RUN_CANCEL;
	if((m_TargetVme != NULL) && (GetInput()->CanReparentTo(m_TargetVme)))
		result = OP_RUN_OK;
  else
    mafErrorMessage(_M(mafString(_L("Cannot re-parent to specified node"))));

	{mafEvent evUnq(this,result); InvokeEvent(evUnq);}
}
//----------------------------------------------------------------------------
void mafOpReparentTo::SetTargetVme(mafVME *target)
//----------------------------------------------------------------------------
{
  m_TargetVme = target;

  if((m_TargetVme == NULL) || !GetInput()->CanReparentTo(m_TargetVme))
  {
    mafErrorMessage(_M(mafString(_L("Cannot re-parent to specified node"))));
    {mafEvent evUnq(this,OP_RUN_CANCEL); InvokeEvent(evUnq);}
  }
}
//----------------------------------------------------------------------------
void mafOpReparentTo::OpDo()
//----------------------------------------------------------------------------
{
  int num, t;
	mafTimeStamp cTime, startTime;
	
  m_OldParent = mafVME::SafeDownCast(GetInput()->GetParent());

	startTime = m_TargetVme->GetTimeStamp();

  if(m_KeepGlobal)
  {
    mmuTimeVector input_time;
    mmuTimeVector target_time;
    mafVME::StaticDownCast(GetInput())->GetAbsTimeStamps(input_time);
    m_TargetVme->GetAbsTimeStamps(target_time);
    mmuTimeVector time = mmuTimeSet::Merge(input_time,target_time);
    num = time.size();

    std::vector< std::shared_ptr<mafMatrix> > new_input_pose;
    new_input_pose.resize(num);

    for (t = 0; t < num; t++)
    {
      new_input_pose[t] = mafMatrix::NewSPtr();
    }

  //change reference system
  /*mafSmartPointer<mafTransformFrame> transform;
  for (t = 0; t < num; t++)
	{
		cTime = time[t];
		
		((mafVME *)GetInput())->SetTimeStamp(cTime);
		m_TargetVme->SetTimeStamp(cTime);
    m_OldParent->SetTimeStamp(cTime);
		
    transform->SetTimeStamp(cTime);
    mafMatrixPipe *mp = ((mafVME *)GetInput())->GetMatrixPipe();
    if (mp == NULL)
    {
      transform->SetInput(((mafVME *)GetInput())->GetOutput()->GetMatrix());
    }
    else
    {
      transform->SetInput(mp);
    }
    transform->SetInputFrame(m_OldParent->GetAbsMatrixPipe());
    transform->SetTargetFrame(m_TargetVme->GetAbsMatrixPipe());
		transform->Update();

    new_input_pose[t]->DeepCopy(transform->GetMatrixPointer());
	}
	
  ((mafVME *)GetInput())->SetTimeStamp(startTime);
  m_TargetVme->SetTimeStamp(startTime);
  m_OldParent->SetTimeStamp(startTime);*/


    for (t = 0; t < num; t++)
    {
      cTime = time[t];
      mafMatrix vmeMatr, parMatr, parMatrInv;

      mafVME::StaticDownCast(GetInput())->GetOutput()->GetAbsMatrix(vmeMatr, cTime);
      m_TargetVme->GetOutput()->GetAbsMatrix(parMatr, cTime);
      mafMatrix::Invert(parMatr,  parMatrInv);
      mafMatrix::Multiply4x4(parMatrInv, vmeMatr, *(new_input_pose[t]));
      new_input_pose[t]->SetTimeStamp(cTime);
    }
    for (t = 0; t < num; t++)
    {
      mafVME::StaticDownCast(GetInput())->SetMatrix(*new_input_pose[t]);
    }
  }
  else
  {
    mafMatrix vmeMatr, parMatr, parMatrInv, new_input_pose;
    mafVME::StaticDownCast(GetInput())->GetOutput()->GetAbsMatrix(vmeMatr, startTime);
    m_TargetVme->GetOutput()->GetAbsMatrix(parMatr, startTime);
    mafMatrix::Invert(parMatr,  parMatrInv);
    mafMatrix::Multiply4x4(parMatrInv, vmeMatr, new_input_pose);
    new_input_pose.SetTimeStamp(startTime);
    mafVME::StaticDownCast(GetInput())->SetMatrix(new_input_pose);
  }
  

  if (mafNode::ReparentTo(GetInput(), m_TargetVme) == MAF_OK)
  {
    {mafEvent evUnq(this,CAMERA_UPDATE); InvokeEvent(evUnq);}
  }
  else
  {
    mafLogMessage(_M(mafString(_L("Something went wrong while re-parenting (bad pointer or memory errors)"))));
  }
}
//----------------------------------------------------------------------------
void mafOpReparentTo::OpUndo()
//----------------------------------------------------------------------------
{
  mafVME *tmp = m_TargetVme;
  m_TargetVme = m_OldParent;
  OpDo();
  m_TargetVme = tmp;
}
