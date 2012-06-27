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

#ifndef __mafAgentThreaded_h
#define __mafAgentThreaded_h

#include "mafAgentEventQueue.h"
#include "mafMultiThreader.h"

//------------------------------------------------------------------------------
// Forward declarations:
//------------------------------------------------------------------------------
class mafMutexLock;

/** An agent processing the queued events on a separate thread.
  This is a special case of agent, able to run a separate thread for processing
  queued events. The class also features an asynchronous mechanism for
  sending events to observers, allowing the internal thread to send
  an event to be processed by a separate thread. This happens by queuing 
  the output event wrapped in a AGENT_ASYNC_DISPATCH, which is later on processed in
  the main thread, by using the original mafAgentEventQueue mechanism for dispatching.
  Notice that, the asynchronous mechanism requires the  of this class 
  to be called on a different thread. Dispatching on another thread is therefore 
  responsibility of a different class, specifically designed to process it incoming events
  on a different tread (e.g. mafAgentEventHandler).
  The mafAgentThreaded DispatchEvents() will take care of extracting the message contained 
  inside a AGENT_ASYNC_DISPATCH event type and dispatch it.
  @sa mafAgent mafAgentEventQueue mafEventBase
*/
class MAF_EXPORT mafAgentThreaded : public mafAgentEventQueue
{
public:
  //------------------------------------------------------------------------------
  // Events
  //------------------------------------------------------------------------------
  MAF_ID_DEC(AGENT_ASYNC_DISPATCH);

  mafTypeMacro(mafAgentThreaded,mafAgentEventQueue);

  /**
    Return true if this is a Agent with its own polling thread. Dispatchers requiring
    a polling thread set this variable to != 0, which makes the initialize function to 
    spawn a thread with a loop continuously calling the InternalUpdate() function.*/
  int GetThreaded() {return m_Threaded;}

  /**
    This is used to enable thread spawning for this Agent*/
  void SetThreaded(int flag){m_Threaded=flag;} 

  /**
    This function forces the agent to update its state. Return 0 if Update is OK.*/
  int Update() {return IsInitialized()?InternalUpdate():-1;}

  /**
    This function enques events to be processed by agent. Events are processed by agent's
    thread by means of the ProcessDispatchedEvent() function.*/
  virtual void OnEvent(mafEventBase *event);
  
  /**
    Send an event to the specified object (i.e. Post to that listener) asynchronously. 
    This happens by queuing the output event wrapped in a AGENT_ASYNC_DISPATCH, 
    which is processed in the main thread, by using the original mafAgentEventQueue 
    mechanism for dispatching.
    @sa SendEvent() for general semantic */
  void AsyncSendEvent(mafObserver *target,mafEventBase *event, mafID channel=MCH_UP);
  void AsyncSendEvent(mafObserver *target, void *sender, mafID id, mafID channel=MCH_UP,void *data=NULL);

  /**
    This function implements asynchronous forward of an event. This happens by queuing 
    the output event wrapped in a AGENT_ASYNC_DISPATCH, which is processed in
    the main thread, by using the original mafAgentEventQueue mechanism for dispatching.
    @sa InvokeEvent() for general semantic */
  void AsyncInvokeEvent(mafEventBase *event, mafID channel=MCH_UP);
  void AsyncInvokeEvent(mafID id, mafID channel=MCH_UP,void *data=NULL);

protected:
  mafAgentThreaded();
  virtual ~mafAgentThreaded();

  /** Internal function used to request the dispatching*/
  virtual void RequestForDispatching();
  void StopThread();
  virtual int InternalInitialize();
  virtual void InternalShutdown();

  /**
    This function must be overridden by subclasses to perform custom polling of dispatcher when
    in threaded mode. This function should poll the dispatcher and return 0. In case it returns
    a value !=0 the polling loop is stopped. The default function stops immediately the loop.*/
  virtual int InternalUpdate();

  /**
   This function is used to startup the thread. Subclasses should override
   the InternalUpdateLoop() function which is called by this one.*/
  static void UpdateLoop(mmuThreadInfoStruct *data);
    
  /**
    Get the present value of the Thread Active flag. This function is
    thread safe and returns the value of the activeFlag member
    variable of the ThreadInfoStruct stored in ThreadData.*/
  int GetActiveFlag();

  /**
  // Internal functions used to send a wakeup signal among the different threads*/
  void SignalNewMessage();
  void WaitForNewMessage();

  mafMultiThreader*     m_Threader;
  mmuThreadInfoStruct*  m_ThreadData;

  int                   m_ThreadId;
  int                   m_Threaded;
  int                   m_ActiveFlag;

#ifdef _WIN32
 
  /** Event signaling the arrival of a new message. Windows implementation only. */
  HANDLE        m_MessageSignal;
#else
  
  /** This mutex is normally locked. It is used to block the execution of the receiving process when the send has not been called yet. */
  mafMutexLock* m_Gate;
#endif

private:
  mafAgentThreaded(const mafAgentThreaded&);  // Not implemented.
  void operator=(const mafAgentThreaded&);  // Not implemented.
};

#endif 
