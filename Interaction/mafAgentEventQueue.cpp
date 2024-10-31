/*=========================================================================

Program:   Multimod Fundation Library
Module:    $RCSfile: mafAgentEventQueue.cpp,v $
Language:  C++
Date:      $Date: 2007-03-15 17:12:24 $
Version:   $Revision: 1.8 $

=========================================================================*/
#include "mafAgentEventQueue.h"
#include <mutex>
#include "mmuIdFactory.h"

#include <assert.h>

#include <deque>

//------------------------------------------------------------------------------
// PIMPL declarations
//------------------------------------------------------------------------------
class mafAgentEventQueue::EventQueueItem
{
public:
  EventQueueItem(mafEventBase *event=NULL):m_Event(event) {};

  mafEventBase *m_Event;
};


struct mafAgentEventQueue::InternalEventQueue 
{
  std::deque< mafAgentEventQueue::EventQueueItem > Q;
};

//------------------------------------------------------------------------------
// Events
//------------------------------------------------------------------------------
MAF_ID_IMP(mafAgentEventQueue::EVENT_DISPATCH);

//------------------------------------------------------------------------------
mafCxxTypeMacro(mafAgentEventQueue);
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
mafAgentEventQueue::mafAgentEventQueue()
//------------------------------------------------------------------------------
{
  m_Dispatched      = true;
  m_EventQueue      = std::make_unique<InternalEventQueue>();
  m_DispatchMode    = SelfProcessMode;
  m_DequeueMode     = MultipleEventMode;
  m_PushMode        = DispatchEventMode;
}

//------------------------------------------------------------------------------
mafAgentEventQueue::~mafAgentEventQueue()
//------------------------------------------------------------------------------
{
}

//------------------------------------------------------------------------------
void mafAgentEventQueue::SetDispatched(bool value)
//------------------------------------------------------------------------------
{
  std::lock_guard lock(m_Mutex);
  m_Dispatched=value;
}

//------------------------------------------------------------------------------
bool mafAgentEventQueue::DispatchEvents()
//------------------------------------------------------------------------------
{
  if (m_Dispatched)
    return false;

  if (m_DequeueMode==MultipleEventMode)
  {
    mafEventBase *event;
    unsigned long channel;
    
    mafEventBase *last_event=PeekLastEvent(); // store the last event of the queue.
  
    // flush the queue but avoid infinite loops, by looping for no more than queue size
    //for (int qsize=GetQueueSize();(qsize>0)&&this->PopEvent(event,channel);qsize--)    
    do 
    {
      this->PopEvent(event);
       
      if (event)
      {
        channel=event->GetChannel();
        if (event->GetId()==EVENT_DISPATCH)
        {
          mafAgentEventQueue *sender=(mafAgentEventQueue *)event->GetSender();
          sender->DispatchEvents();
        }
        else
        {
          if (m_DispatchMode==SelfProcessMode || channel==MCH_INPUT)
          {
            OnEvent(event);
          }
          else
          {
            InvokeEvent(event,channel);
          }
        }
        event->Delete(); // release memory
      }      
    }
    while (event&&PeekEvent()!=last_event);

    //this->SetDispatched();

    std::lock_guard lock(m_Mutex);
    if (m_EventQueue->Q.size()==0)
    {
        this->SetDispatched();
    }
    else
    {
      this->RequestForDispatching();
    }
  }
  else
  {
    mafEventBase *event;
    unsigned long channel;

    if (this->PopEvent(event))
    {
      channel=event->GetChannel();
      if (event->GetId()==EVENT_DISPATCH)
      {
        mafAgentEventQueue *sender=(mafAgentEventQueue *)event->GetSender();
        sender->DispatchEvents();
      }
      else
      {
        if (m_DispatchMode==SelfProcessMode)
        {
          this->OnEvent(event);
        }
        else
        {
          this->InvokeEvent(event,channel);
        }
        event->Delete(); // release memory
      }
    }

    if (this->IsQueueEmpty())
    {
      this->SetDispatched();
    }
    else
    {
      this->RequestForDispatching();
    }
  }

  return true;
}

//------------------------------------------------------------------------------
bool mafAgentEventQueue::PushEvent(mafID event, void *sender,void *data)
//------------------------------------------------------------------------------
{
  {mafEventBase evUnq(sender,event,data); return this->PushEvent(&evUnq);}
}

//------------------------------------------------------------------------------
bool mafAgentEventQueue::PushEvent(mafEventBase *event)
//------------------------------------------------------------------------------
{
  assert(event);
  if (event)
  {
    std::lock_guard lock(m_Mutex);

    mafEventBase *new_event=event->NewInstance();
    assert(new_event);
    *new_event=*event;
    
    mafAgentEventQueue::EventQueueItem item(new_event);
    
    m_EventQueue->Q.push_front(item);

    if (m_Dispatched&&m_PushMode==DispatchEventMode)
    {
      // advise listener class a new event is in the queue
      m_Dispatched=false;
      RequestForDispatching();
    }
    return true;
  }
  
  mafErrorMacro("Trying to push a NULL event");
  return false;
}

//------------------------------------------------------------------------------
void mafAgentEventQueue::RequestForDispatching()
//------------------------------------------------------------------------------
{
  InvokeEvent(EVENT_DISPATCH);
}

//------------------------------------------------------------------------------
int mafAgentEventQueue::PopEvent(mafEventBase *&event)
//------------------------------------------------------------------------------
{
  std::lock_guard lock(m_Mutex);

  int ret;
  if (m_EventQueue->Q.size()>0)
  {
    
    // Notice the event object destruction is left to DispatchEvents()
    // after dispatching to avoid object copying.
    mafAgentEventQueue::EventQueueItem item=m_EventQueue->Q.back();
    m_EventQueue->Q.pop_back();
    event=item.m_Event;
    assert(event);
    ret=true;
  }
  else
  {
    event=NULL;
    ret=false;
  }

  return ret;

}

//------------------------------------------------------------------------------
mafEventBase *mafAgentEventQueue::PopEvent()
//------------------------------------------------------------------------------
{
  mafEventBase *ev;

  if (this->PopEvent(ev))
  { 
    return ev;
  }

  return NULL;
}

//------------------------------------------------------------------------------
int mafAgentEventQueue::PopEvent(mafEventBase &event)
//------------------------------------------------------------------------------
{
  mafEventBase *ev;

  if (this->PopEvent(ev))
  {
    event.DeepCopy(ev);
    ev->Delete(); // destroy the queued event

    return true;
  }

  return false;
}

//------------------------------------------------------------------------------
mafEventBase *mafAgentEventQueue::PeekEvent()
//------------------------------------------------------------------------------
{
  std::lock_guard lock(m_Mutex);

  mafEventBase *event=NULL;
  if (m_EventQueue->Q.size()>0)
  {
    EventQueueItem item=m_EventQueue->Q.back();
    event=item.m_Event;  
  }
  
  return event;
}

//------------------------------------------------------------------------------
mafEventBase *mafAgentEventQueue::PeekLastEvent()
//------------------------------------------------------------------------------
{
  std::lock_guard lock(m_Mutex);

  mafEventBase *event=NULL;
  if (m_EventQueue->Q.size()>0)
  {
    EventQueueItem item=m_EventQueue->Q.front();
    event=item.m_Event;  
  }
  
  return event;
}
//------------------------------------------------------------------------------
int mafAgentEventQueue::GetQueueSize()
//------------------------------------------------------------------------------
{
  std::lock_guard lock(m_Mutex);
  return m_EventQueue->Q.size();
}

