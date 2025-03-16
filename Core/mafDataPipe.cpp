#include "mafDataPipe.h"

#include "mafVME.h"
#include "mafAbsMatrixPipe.h"
#include "mafOBB.h"
#include "mafEventBase.h"
#include "mafIndent.h"

//------------------------------------------------------------------------------
mafDataPipe::mafDataPipe()
//------------------------------------------------------------------------------
{
  m_CurrentTime = 0;
  m_Bounds.Reset();
  m_VME = nullptr;
  m_DependOnAbsPose=0;
  m_DependOnPose=0;
  m_DependOnVMETime = 1; //Paolo
}

mafDataPipe::~mafDataPipe() = default;

//------------------------------------------------------------------------------
void mafDataPipe::SetTimeStamp(mafTimeStamp t)
//------------------------------------------------------------------------------
{
  if (t!=m_CurrentTime)
  {
    m_CurrentTime = t;
    Modified();
  }
}

//------------------------------------------------------------------------------
int mafDataPipe::SetVME(mafVME *vme) 
//------------------------------------------------------------------------------
{
  if (Accept(vme)||vme==NULL)
  {
    m_VME=vme;
    Modified();
    return MAF_OK;
  }
  
  mafErrorMacro("Wrong m_VME type: " << vme->GetTypeName());
  return MAF_ERROR;
}

//------------------------------------------------------------------------------
std::shared_ptr<mafDataPipe> mafDataPipe::MakeACopy()
//------------------------------------------------------------------------------
{
  std::shared_ptr<mafDataPipe> newpipe(NewInstance());
  newpipe->DeepCopy(this);
  return newpipe;
}

//------------------------------------------------------------------------------
int mafDataPipe::DeepCopy(mafDataPipe *pipe)
//------------------------------------------------------------------------------
{
  if (pipe->IsA(this->GetTypeName()))
  {
    m_VME=pipe->GetVME();
    return MAF_OK;
  }

  return MAF_ERROR;
}


//----------------------------------------------------------------------------
// Get the MTime. Take in consideration also modifications to the Input Array
MTimeType mafDataPipe::GetMTime()
//------------------------------------------------------------------------------
{
  auto mtime = this->mafTimeStamped::GetMTime();

  //if (m_Bounds.GetMTime() > mtime)
  //{
  //  mtime = m_Bounds.GetMTime();
  //}

  if (m_VME)
  {
    if (m_DependOnVMETime) 
    {
      mtime = (std::max)(mtime, m_VME->GetMTime());
    }

    if (m_DependOnAbsPose)
    {
      if (m_VME->GetAbsMatrixPipe())
      {
        mtime = (std::max)(mtime, m_VME->GetAbsMatrixPipe()->GetMTime());
      }
    }
    else if (m_DependOnPose) // if we depend from AbsPose we do not need to check for pose
    {
      if (m_VME->GetMatrixPipe())
      {
        mtime = (std::max)(mtime, m_VME->GetMatrixPipe()->GetMTime());
      }
    }
  }

  return mtime;
}

//------------------------------------------------------------------------------
void mafDataPipe::Execute()
//------------------------------------------------------------------------------
{
}

//------------------------------------------------------------------------------
void mafDataPipe::OnPreUpdate()
//------------------------------------------------------------------------------
{
    if (GetMTime()>m_PreExecuteTime.GetMTime())
    {
      m_PreExecuteTime.Modified();
      Execute();
      // forward event to VME
      if (m_VME)
      {
        mafEventBase evUnq(this, VME_OUTPUT_DATA_PREUPDATE);
      	m_VME->OnEvent(&evUnq);
      }
    }
}

//------------------------------------------------------------------------------
void mafDataPipe::OnUpdate()
//------------------------------------------------------------------------------
{
    // forward event to VME
    if (m_VME)
    {
      mafEventBase evUnq(this, VME_OUTPUT_DATA_UPDATE);
    	m_VME->OnEvent(&evUnq);
    }
}

//------------------------------------------------------------------------------
void mafDataPipe::Print(std::ostream& os, const int tabs) const
//------------------------------------------------------------------------------
{
  mafIndent indent(tabs);
  os << indent << "DataPipe Type Name: " << GetTypeName() << std::endl;
  mafString dp_name;
  dp_name = m_VME ? m_VME->GetName() : _R("(NULL)");
  os << indent << "VME:" << dp_name.GetCStr() << std::endl;
}
