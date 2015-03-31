/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafBaseEventHandlerCallback.h,v $
  Language:  C++
  Date:      $Date: 2005-04-26 07:16:02 $
  Version:   $Revision: 1.3 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mafBaseEventHandlerCallback_h
#define __mafBaseEventHandlerCallback_h
//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "mafBaseEventHandler.h"

//------------------------------------------------------------------------------
// mafBaseEventHandlerCallback
//------------------------------------------------------------------------------
/** Concrete implementation of Observer calling a callback function.
  mafBaseEventHandlerCallback is a class implementing an "observer" that launches a callback
  function. It's useful for attaching as observers objects that do not inherit from mafBaseEventHandler.
  When an event is rised the callback function is called passing as argument some event content 
  plus event source Data and this class ClientData, that can be set to any value. Usually ClientData
  is used to store the (self) pointer of the class to whom the callback function belongs.
  @sa mafEventSource mafBaseEventHandler
  @todo 
  - create a test
*/
class MAF_EXPORT mafBaseEventHandlerCallback:public mafBaseEventHandler
{
public:
  mafBaseEventHandlerCallback();
  virtual ~mafBaseEventHandlerCallback();

  /** Set function callback to be called by this observer */
  void SetCallback(void (*f)(void *sender, mafID eid, void *calldata));
   
  /** process the events sent by subjects */
  virtual void OnEvent(mafEventBase *e);

protected:
  void (*m_Callback)(void *, mafID, void *);  
};

#endif /* __mafBaseEventHandlerCallback_h */
