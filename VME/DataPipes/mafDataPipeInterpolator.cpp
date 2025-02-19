#include "mafDataPipeInterpolator.h"

#include "mafVME.h"
#include "mafVMEItem.h"
#include "mafVMEGenericAbstract.h"

#include "mafDataVector.h"

mafDataPipeInterpolator::mafDataPipeInterpolator()
{
  m_CurrentItem     = nullptr;
  m_OldTimeStamp    = -1;
  m_OldItem         = nullptr;
  m_ReleaseDataFlag = false;
  m_DependOnVMETime = 0;
}

mafDataPipeInterpolator::~mafDataPipeInterpolator()
{
  SetCurrentItem(nullptr);
}

void mafDataPipeInterpolator::SetTimeStamp(mafTimeStamp time)
{
  if (m_CurrentTime==time)
    return;

  m_CurrentTime=time;
  // Do not call Modified(), to avoid the pipeline
  // to be automatically updated
}

// Get the MTime. Take in consideration also modifications to the Input Array
MTimeType mafDataPipeInterpolator::GetMTime()
{
  auto mtime = Superclass::GetMTime();
  
  mafVMEGenericAbstract *vme = (mafVMEGenericAbstract *)m_VME;

  if (vme && vme->GetDataVector())
  {
    mtime = (std::max)(mtime, vme->GetDataVector()->GetMTime());
  }

  return mtime;
}

bool mafDataPipeInterpolator::Accept(mafVME *vme)
{
  return vme && vme->IsA(mafVMEGenericAbstract::GetStaticTypeId());
}

void mafDataPipeInterpolator::PreExecute1()
{
  Superclass::PreExecute1();

  auto mtime=GetMTime();

  // If the current time has changed, check if
  // a new item should be considered according to
  // interpolation rules (InternalItemUpdate() )

  // the current time has changed or if output data has been regenerated...
  if (m_OldTimeStamp!=m_CurrentTime||mtime>m_UpdateTime.GetMTime())
  {
    m_OldTimeStamp=m_CurrentTime;
    
    // First of all find the right item to be used as input
    // and update the output bounds
    UpdateBounds();
  } 
}

void mafDataPipeInterpolator::PreExecute2()
{
  Superclass::PreExecute1();

  auto mtime=GetMTime();

  // If the current time has changed, check if
  // a new item should be considered according to
  // interpolation rules (InternalItemUpdate() )

  // the current time has changed or if output data has been regenerated...
  if (m_OldTimeStamp!=m_CurrentTime||mtime>m_UpdateTime.GetMTime())
  {
    m_OldTimeStamp=m_CurrentTime;
    
    // First of all find the right item to be used as input
    // and update the output bounds
    UpdateBounds();
  } 
}

void mafDataPipeInterpolator::UpdateBounds()
{
  auto old_item=m_CurrentItem;
  this->InternalItemUpdate();

  if (m_CurrentItem)
  { 
    if ( (!m_Bounds.IsValid())||(m_CurrentItem!=old_item)|| \
      (m_CurrentItem->GetMTime()>m_Bounds.GetMTime()))
    {
        m_Bounds.DeepCopy(m_CurrentItem->GetBounds());
    }
  }
}

void mafDataPipeInterpolator::InternalItemUpdate()
{  
  mafVMEGenericAbstract *vme=(mafVMEGenericAbstract *)m_VME;
  mafDataVector *array = vme ? vme->GetDataVector() : NULL;

  if (array)
  {
    auto item = array->GetItemBefore(m_CurrentTime);
    UpdateCurrentItem(item);
  }
  
}

void mafDataPipeInterpolator::SetCurrentItem(std::shared_ptr<mafVMEItem> data)
{
  if (data==m_CurrentItem)
    return;
  m_CurrentItem=data;
  Modified();
}

void mafDataPipeInterpolator::UpdateCurrentItem(std::shared_ptr<mafVMEItem> item)
{
  if (item)
  {	
    if (item!=m_CurrentItem)
    {
      if (m_ReleaseDataFlag&&m_CurrentItem)
        m_CurrentItem->ReleaseData();

      SetCurrentItem(item);
      //m_UpdateTime.Modified();
    }
    m_OldItem=m_CurrentItem;
  }
  else
  {
	  SetCurrentItem(NULL);
    //m_UpdateTime.Modified();
  }
}

void mafDataPipeInterpolator::OnPreUpdate1()
{
    if (GetMTime() > m_PreExecuteTime.GetMTime() || (m_CurrentItem && !m_CurrentItem->IsDataPresent()))
    {
      m_PreExecuteTime.Modified();
      PreExecute1();
      // forward event to VME
      if (m_VME) { m_VME->DoPreUpdate(); }
    }
}

void mafDataPipeInterpolator::OnPreUpdate2()
{
    if (GetMTime() > m_PreExecuteTime.GetMTime() || (m_CurrentItem && !m_CurrentItem->IsDataPresent()))
    {
      m_PreExecuteTime.Modified();
      PreExecute2();
      // forward event to VME
      if (m_VME) { m_VME->DoPreUpdate(); }
    }
}
