/*=========================================================================

 Program: MAF2
 Module: mafEventSource
 Authors: Marco Petrone
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __mafEventSource_h
#define __mafEventSource_h
//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "mafObject.h"
#include <vector>

//------------------------------------------------------------------------------
// Forward declarations
//------------------------------------------------------------------------------
class mafObserver;
class mafObserversList;
class mafEventBase;

//------------------------------------------------------------------------------
// mafEventSource
//------------------------------------------------------------------------------
/** Implementation of the Subject/Observer design pattern.
  mafEventSource is a class implementing the "subject" in the Subject/Observer 
  design pattern. Objective of this object is to generate events to be sent to observers.
  An observer must be registered to a subject to create the communication channel 
  between the two.
  @sa mafObserver mafObserverCallback
*/
class MAF_EXPORT mafEventSource: public mafObject
{
public:
  mafEventSource(void *owner=NULL);
  virtual ~mafEventSource();

  mafTypeMacro(mafEventSource,mafObject);
 
  mafEventSource(const mafEventSource& c) {}

  /** Register an observer of this subject */
  void AddObserver(mafObserver *obj, int priority=0);

  /** Register an observer of this subject */
  void AddObserver(mafObserver &obj, int priority=0);


  /** Unregister an observer. Return false if object is not an observer */
  bool RemoveObserver(mafObserver *obj);

  /** remove all observers at once */
  void RemoveAllObservers();

  /** return true if object is an observer of this subject */
  bool IsObserver(mafObserver *obj);

  /** return true if this class has observers */
  bool HasObservers();

  /** return a vector with the list of observers of this event source */
  void GetObservers(std::vector<mafObserver *> &olist);

  /** invoke an event of this subject */
  void InvokeEvent(mafEventBase &e);

  /** invoke an event of this subject */
  void InvokeEvent(mafEventBase *e);

  /** invoke an event of this subject */
  void InvokeEvent(void *sender,mafID id=ID_NO_EVENT, void *data=NULL);

  /** 
    set the channel Id assigned to this event source. If set to <0 
    no channel is assigned */
  void SetChannel(mafID ch);

  /** 
    return the channel assigned to this event source. If <0 no 
    channel has been assigned */
  mafID GetChannel();

protected:
  mafObserversList  *m_Observers;  ///< list of observers
  mafID             m_Channel;     ///< a channel assigned to this event source, if <0 no channel is assigned
private:
  
};

#endif /* __mafEventSource_h */
