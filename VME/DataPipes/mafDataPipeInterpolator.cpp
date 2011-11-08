/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafDataPipeInterpolator.cpp,v $
  Language:  C++
  Date:      $Date: 2011-11-08 10:05:09 $
  Version:   $Revision: 1.1.2.1 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------


#include "mafDataPipeInterpolator.h"

#include "mafVME.h"
#include "mafVMEItem.h"
#include "mafVMEGenericAbstract.h"

#include "mafDataVector.h"

//------------------------------------------------------------------------------
mafCxxAbstractTypeMacro(mafDataPipeInterpolator)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
mafDataPipeInterpolator::mafDataPipeInterpolator()
//------------------------------------------------------------------------------
{
  m_CurrentItem     = NULL;
  m_OldTimeStamp    = -1;
  m_OldItem         = NULL;
  m_ReleaseDataFlag = false;
  m_DependOnVMETime = 0;
}

//------------------------------------------------------------------------------
mafDataPipeInterpolator::~mafDataPipeInterpolator()
//------------------------------------------------------------------------------
{
  SetCurrentItem(NULL);
}

//----------------------------------------------------------------------------
void mafDataPipeInterpolator::SetTimeStamp(mafTimeStamp time)
//----------------------------------------------------------------------------
{
  if (m_CurrentTime==time)
    return;

  m_CurrentTime=time;
  // Do not call Modified(), to avoid the pipeline
  // to be automatically updated
}

//----------------------------------------------------------------------------
// Get the MTime. Take in consideration also modifications to the Input Array
unsigned long mafDataPipeInterpolator::GetMTime()
//------------------------------------------------------------------------------
{
  unsigned long mtime = Superclass::GetMTime();
  
  mafVMEGenericAbstract *vme = (mafVMEGenericAbstract *)m_VME;

  if (vme && vme->GetDataVector())
  {
    unsigned long arrayMTime = vme->GetDataVector()->GetMTime();
    if (arrayMTime > mtime)
    {
      return arrayMTime;
    }
  }

  return mtime;
}

//------------------------------------------------------------------------------
bool mafDataPipeInterpolator::Accept(mafVME *vme)
//------------------------------------------------------------------------------
{
  return vme && vme->IsA(mafVMEGenericAbstract::GetStaticTypeId());
}

//------------------------------------------------------------------------------
void mafDataPipeInterpolator::PreExecute()
//------------------------------------------------------------------------------
{
  Superclass::PreExecute();

  unsigned long mtime=GetMTime();

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

//------------------------------------------------------------------------------
void mafDataPipeInterpolator::UpdateBounds()
//------------------------------------------------------------------------------
{
  mafVMEItem *old_item=m_CurrentItem;
  this->InternalItemUpdate();

  if (m_CurrentItem)
  { 
    if ( (!m_Bounds.IsValid())||(m_CurrentItem!=old_item)|| \
      (m_CurrentItem->GetMTime()>m_Bounds.GetMTime())|| \
      (m_CurrentItem && m_CurrentItem->GetUpdateTime()>m_Bounds.GetMTime()))
    {
        m_Bounds.DeepCopy(m_CurrentItem->GetBounds());
    }
  }
}

//------------------------------------------------------------------------------
void mafDataPipeInterpolator::InternalItemUpdate()
//------------------------------------------------------------------------------
{  
  mafVMEGenericAbstract *vme=(mafVMEGenericAbstract *)m_VME;
  mafDataVector *array = vme ? vme->GetDataVector() : NULL;

  if (array)
  {
    mafVMEItem *item = array->GetItemBefore(m_CurrentTime);
    UpdateCurrentItem(item);
  }
  
}

//-------------------------------------------------------------------------
void mafDataPipeInterpolator::SetCurrentItem(mafVMEItem *data)
//------------------------------------------------------------------------------
{
  if (data==m_CurrentItem)
    return;
  m_CurrentItem=data;
  Modified();
}

//-------------------------------------------------------------------------
void mafDataPipeInterpolator::UpdateCurrentItem(mafVMEItem *item)
//------------------------------------------------------------------------------
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
//------------------------------------------------------------------------------
void mafDataPipeInterpolator::OnEvent(mafEventBase *maf_event)
//------------------------------------------------------------------------------
{
  switch (maf_event->GetId())
  {
  case VME_OUTPUT_DATA_PREUPDATE:
    if (GetMTime() > m_PreExecuteTime.GetMTime() || (m_CurrentItem && !m_CurrentItem->IsDataPresent()))
    {
      m_PreExecuteTime.Modified();
      PreExecute();
      // forward event to VME
      if (m_VME) m_VME->OnEvent(maf_event);
    }
    break;
  case VME_OUTPUT_DATA_UPDATE:
    Execute();
    // forward event to VME
    if (m_VME) m_VME->OnEvent(maf_event);
    break;
  }; 
}
