/*=========================================================================

 Program: MAF2
 Module: mafAgentThreaded
 Authors: Marco Petrone
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "mafAgentThreaded.h"
#include <mutex>
#include "mmuIdFactory.h"

#include <assert.h>

//------------------------------------------------------------------------------
// Events
//------------------------------------------------------------------------------
MAF_ID_IMP(mafAgentThreaded::AGENT_ASYNC_DISPATCH);

//------------------------------------------------------------------------------
mafCxxTypeMacro(mafAgentThreaded);
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
mafAgentThreaded::mafAgentThreaded()
//------------------------------------------------------------------------------
{
  m_Threaded      = 1;
  m_ActiveFlag    = 0;

  SetDispatchModeToSelfProcess();

}

//------------------------------------------------------------------------------
mafAgentThreaded::~mafAgentThreaded()
//------------------------------------------------------------------------------
{
  Shutdown();
}

//------------------------------------------------------------------------------
void mafAgentThreaded::InternalShutdown()
//------------------------------------------------------------------------------
{
  if (m_Thread.joinable())
  {
    if (m_ActiveFlag)
    {
      m_ActiveFlag=0;
      cv.notify_all();
    }
    m_Thread.join();
  }
}

//------------------------------------------------------------------------------
int mafAgentThreaded::InternalInitialize()
//------------------------------------------------------------------------------
{
  // Spawns a thread if necessary
  if (m_Threaded)
  {
    if (m_ActiveFlag || m_Thread.joinable()) 
    {
      mafErrorMacro("Dispatcher handler polling thread already started!");
      return -1;
    }

    m_ActiveFlag=1;

    m_Thread = std::thread(&mafAgentThreaded::UpdateLoop,this);
  }

  return 0;
}

//------------------------------------------------------------------------------
void mafAgentThreaded::UpdateLoop()
//------------------------------------------------------------------------------
{
  // wait for initialization to be completed
  for (;m_ActiveFlag&&!IsInitialized();)
    mafSleep(100); 

  // This loop is termintated when active flag is set by TerminateThread() or 
  // if InternalUpdate() returns a value !=0
  for (;m_ActiveFlag&&!InternalUpdate();) ; // active loop


  for (;;)
  {
      std::unique_lock<std::mutex> lock(m_Gate);
      cv.wait(lock,
          [this] { return m_ActiveFlag || !IsQueueEmpty(); });
      if (!m_ActiveFlag)
          return;
      InternalUpdate();
  }


}

//------------------------------------------------------------------------------
int mafAgentThreaded::InternalUpdate()
//------------------------------------------------------------------------------
{
  this->DispatchEvents();
  
  return 0;
}

//------------------------------------------------------------------------------
void mafAgentThreaded::OnEvent(mafEventBase *event)
//------------------------------------------------------------------------------
{
  assert(event);

  if (event->GetId()==AGENT_ASYNC_DISPATCH)
  {
    if (event->GetSender()==this)
    {
      // this is used for the asynchronous InvokeEvent
      this->InvokeEvent((mafEventBase *)event->GetData());
    }
    else
    {
      assert(true); // should not pass from here
      
      // this is used for the asynchronous SendEvent
      // notice we are using the sender field of the AsyncInvokeEvent to store the recipient
      //((mafObserver *)event->GetSender())->OnEvent((mafEventBase *)event->GetData());
    }
  }
  else
  {
    Superclass::OnEvent(event);
  }
}

//------------------------------------------------------------------------------
void mafAgentThreaded::RequestForDispatching()
//------------------------------------------------------------------------------
{
  //if (m_Threaded&&this->Initialized&&this->GetActiveFlag())
  //{
    if (PeekLastEvent()->GetId()==AGENT_ASYNC_DISPATCH)
    {
      Superclass::RequestForDispatching();
    }
    else
    {
      // send my self an event to awoke my thread
        cv.notify_one();
    }
  //}
  //else
  //{
  //  this->DispatchEvents();
  //}

}

//------------------------------------------------------------------------------
void mafAgentThreaded::AsyncInvokeEvent(mafEventBase *event, mafID channel)
//------------------------------------------------------------------------------
{
  mafEventBase *copy_of_event = event->NewInstance();
  copy_of_event->DeepCopy(event);
  copy_of_event->SetChannel(channel);
  PushEvent(AGENT_ASYNC_DISPATCH,this,copy_of_event); // this make a copy of the event
  mafDEL(copy_of_event);
}

//------------------------------------------------------------------------------
void mafAgentThreaded::AsyncSendEvent(mafBaseEventHandler *target,mafEventBase *event, mafID channel)
//------------------------------------------------------------------------------
{
  if (event&&target&&target!=this)
  {
    // use the sender field to store the recipient.
    mafID old_ch=event->GetChannel();
    event->SetChannel(channel); // set the right channel
    PushEvent(AGENT_ASYNC_DISPATCH,target,event);
    event->SetChannel(old_ch);
  }
}

//----------------------------------------------------------------------------
void mafAgentThreaded::AsyncInvokeEvent(mafID id, mafID channel,void *data)
//----------------------------------------------------------------------------
{
  AsyncInvokeEvent(&mafEventBase(this,id,data,channel),channel);
}

//----------------------------------------------------------------------------
void mafAgentThreaded::AsyncSendEvent(mafBaseEventHandler *target, void *sender, mafID id, mafID channel,void *data)
//----------------------------------------------------------------------------
{
  AsyncSendEvent(target,&mafEventBase(sender,id,data,channel),channel);
}
