#include "mafEventSender.h"

#include "mafEventBase.h"
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

mafEventSourceBase::mafEventSourceBase()
{
  m_Observers = std::make_unique<mafObserversList>();
}

mafEventSourceBase::~mafEventSourceBase() = default;

void mafEventSourceBase::AddObserverBase(mafBaseEventHandler &obj, int priority)
{
  AddObserverBase(&obj,priority);
}

void mafEventSourceBase::AddObserverBase(mafBaseEventHandler *obj, int priority)
{
  // search for first element with priority <= priority
  mafObserversListType::iterator it;
  for (it=m_Observers->m_List.begin(); it!=m_Observers->m_List.end() && (*it).first>priority ;it++) ;

  m_Observers->m_List.insert(it,mafObserversPairType(priority,obj));
}

bool mafEventSourceBase::RemoveObserverBase(mafBaseEventHandler *obj)
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

void mafEventSourceBase::RemoveAllObserversBase()
{
  m_Observers->m_List.clear();
}

bool mafEventSourceBase::IsObserverBase(mafBaseEventHandler *obj)const
{
  if (m_Observers->m_List.empty())
    return false;

  mafObserversListType::iterator it;
  for (it = m_Observers->m_List.begin(); it != m_Observers->m_List.end(); it++)
    if ((*it).second == obj) return true;

  return false;
}

bool mafEventSourceBase::HasObserversBase()const
{
  return !m_Observers->m_List.empty();
}

std::vector<mafBaseEventHandler*> mafEventSourceBase::GetObserversBase()const
{
  std::vector<mafBaseEventHandler*> res;
  res.reserve(m_Observers->m_List.size());
  for (auto& entry : m_Observers->m_List)
  {
    res.push_back(entry.second);
  }
  return res;
}

void mafEventSourceBase::InvokeEvent(mafEventBase *e)
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
  else for (it=m_Observers->m_List.begin();it!=m_Observers->m_List.end();)
  {
    // Set the event channel (if neccessary).
    // Must set it at each iteration since it could have
    // been changed by other event sources on the path
    if (m_Channel>=0&&m_Channel!=e->GetChannel())
      e->SetChannel(m_Channel);

    // rise an event to observers
    mafBaseEventHandler *observer=(*it).second;
    it++;
    observer->OnEvent(e);
  }
  
  // restore old channel
  if (old_ch>0) e->SetChannel(old_ch);
}

void mafEventSourceBase::SetChannelBase(mafID ch)
{
  m_Channel = ch;
}

mafID mafEventSourceBase::GetChannelBase()
{
  return m_Channel;
}
