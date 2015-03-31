/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafEventSource.cpp,v $
  Language:  C++
  Date:      $Date: 2007-06-07 07:33:35 $
  Version:   $Revision: 1.8 $
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



#include "mafEventBase.h"
#include "mafEventSender.h"
#include "mafBaseEventHandler.h"
#include <list>
#include <utility>
#include "assert.h"

//------------------------------------------------------------------------------
// PIMPL declarations
//------------------------------------------------------------------------------
typedef std::pair<int,mafBaseEventHandler *> mafObserversPairType;
typedef std::list< mafObserversPairType > mafObserversListType;
class mafObserversList
{
public:
  mafObserversListType m_List; 
};

//------------------------------------------------------------------------------
mafEventSourceBase::mafEventSourceBase(void *owner)
//------------------------------------------------------------------------------
{
  m_Observers   = new mafObserversList;
  m_Channel     = -1;
}

//------------------------------------------------------------------------------
mafEventSourceBase::~mafEventSourceBase()
//------------------------------------------------------------------------------
{
  delete m_Observers; 
  m_Observers = NULL;
}

//------------------------------------------------------------------------------
void mafEventSourceBase::AddObserverBase(mafBaseEventHandler &obj, int priority)
//------------------------------------------------------------------------------
{
  AddObserverBase(&obj,priority);
}

//------------------------------------------------------------------------------
void mafEventSourceBase::AddObserverBase(mafBaseEventHandler *obj, int priority)
//------------------------------------------------------------------------------
{
  // search for first element with priority <= priority
  mafObserversListType::iterator it;
  for (it=m_Observers->m_List.begin(); it!=m_Observers->m_List.end() && (*it).first>priority ;it++) ;

  m_Observers->m_List.insert(it,mafObserversPairType(priority,obj));
}
//------------------------------------------------------------------------------
bool mafEventSourceBase::RemoveObserverBase(mafBaseEventHandler *obj)
//------------------------------------------------------------------------------
{
  if (m_Observers->m_List.empty())
    return false;

  // an observer could be present more then one time!
  bool flag = false;
  mafObserversListType::iterator it;
  for (it=m_Observers->m_List.begin(); it!=m_Observers->m_List.end() ;it++)
  {
    if ((*it).second == obj)
    {
      m_Observers->m_List.erase(it);
      flag=true;
      break;      // Paolo 08-06-2005
    }
  }

  return flag;
}

//------------------------------------------------------------------------------
void mafEventSourceBase::RemoveAllObserversBase()
//------------------------------------------------------------------------------
{
  m_Observers->m_List.clear();
}

//------------------------------------------------------------------------------
bool mafEventSourceBase::IsObserverBase(mafBaseEventHandler *obj)const
//------------------------------------------------------------------------------
{
  if (m_Observers->m_List.empty())
    return false;

  mafObserversListType::iterator it;
  for (it = m_Observers->m_List.begin(); it != m_Observers->m_List.end(); it++)
    if ((*it).second == obj) return true;

  return false;
}

//------------------------------------------------------------------------------
bool mafEventSourceBase::HasObserversBase()const
//------------------------------------------------------------------------------
{
  return !m_Observers->m_List.empty();
}

//------------------------------------------------------------------------------
void mafEventSourceBase::GetObserversBase(std::vector<mafBaseEventHandler *> &olist)const
//------------------------------------------------------------------------------
{
  olist.clear();
  olist.resize(m_Observers->m_List.size());
  int i=0;
  for (mafObserversListType::iterator it=m_Observers->m_List.begin();it!=m_Observers->m_List.end();it++,i++)
  {
    olist[i]=it->second;
  }
}

//------------------------------------------------------------------------------
void mafEventSourceBase::InvokeEvent(mafEventBase *e)
//------------------------------------------------------------------------------
{
  if (m_Observers->m_List.empty())
    return;
  
  // store old channel
  mafID old_ch=(m_Channel<0)?-1:e->GetChannel();
    
  mafObserversListType::iterator it;
  if(m_Observers->m_List.size() == 1)
  {
    it=m_Observers->m_List.begin();
    if (m_Channel>=0&&m_Channel!=e->GetChannel())
      e->SetChannel(m_Channel);
    // rise an event to observers
    mafBaseEventHandler *observer=(*it).second;
    observer->OnEvent(e);
  }
  else for (it=m_Observers->m_List.begin();it!=m_Observers->m_List.end();it++)
  {
    // Set the event channel (if neccessary).
    // Must set it at each iteration since it could have
    // been changed by other event sources on the path
    if (m_Channel>=0&&m_Channel!=e->GetChannel())
      e->SetChannel(m_Channel);

    // rise an event to observers
    mafBaseEventHandler *observer=(*it).second;
    observer->OnEvent(e);
  }
  
  // restore old channel
  if (old_ch>0) e->SetChannel(old_ch);
}

//------------------------------------------------------------------------------
void mafEventSourceBase::SetChannelBase(mafID ch)
//------------------------------------------------------------------------------
{
  m_Channel = ch;
}
//------------------------------------------------------------------------------
mafID mafEventSourceBase::GetChannelBase()
//------------------------------------------------------------------------------
{
  return m_Channel;
}
