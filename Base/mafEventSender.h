/*=========================================================================

 Program: MAF2
 Module: mafEventSender
 Authors: Marco Petrone
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __mafEventSender_h
#define __mafEventSender_h
//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "mafBaseEventHandler.h"
#include "mafEventBase.h"
//#include "mafObject.h"
#include <vector>

//------------------------------------------------------------------------------
// Forward declarations
//------------------------------------------------------------------------------
class mafObserversList;

//------------------------------------------------------------------------------
// mafEventSender
//------------------------------------------------------------------------------
/** class acting as an interface for objects using MAF hierarchical event commuication model
  This object simply defines a SetListener/GetListener functions for connecting with a listerner
  object to create a hierarcy of objects. This allows to send events up in the hierarchy
  implementing what was the original MAF event communication model: this is a unicast communication
  model very well suited for herarchically organized objects. For sending events up in the tree 
  the mafEventMacro() or the InvokeEvent() function can be use.
  @sa mafObserver mafObserverCallback
*/

class MAF_EXPORT mafEventSourceBase
{
public:
  mafEventSourceBase(void *owner=NULL);
  virtual ~mafEventSourceBase();

  void InvokeEvent(mafID id, void *data=NULL) {InvokeEvent(this,id,data);}
  /** invoke an event of this subject */
  void InvokeEvent(void *sender,mafID id, void *data=NULL){InvokeEvent(mafEventBase(sender,id,data));}
  /** invoke an event of this subject */
  void InvokeEvent(mafEventBase &e) {InvokeEvent(&e);}

  /** invoke an event of this subject */
  virtual void InvokeEvent(mafEventBase *e);
 
protected:
  /** Register an observer of this subject */
  void AddObserverBase(mafBaseEventHandler *obj, int priority=0);

  /** Register an observer of this subject */
  void AddObserverBase(mafBaseEventHandler &obj, int priority=0);


  /** Unregister an observer. Return false if object is not an observer */
  bool RemoveObserverBase(mafBaseEventHandler *obj);

  /** remove all observers at once */
  void RemoveAllObserversBase();

  /** return true if object is an observer of this subject */
  bool IsObserverBase(mafBaseEventHandler *obj)const;

  /** return true if this class has observers */
  bool HasObserversBase()const;

  /** return a vector with the list of observers of this event source */
  void GetObserversBase(std::vector<mafBaseEventHandler *> &olist)const;

  /** 
    set the channel Id assigned to this event source. If set to <0 
    no channel is assigned */
  void SetChannelBase(mafID ch);

  /** 
    return the channel assigned to this event source. If <0 no 
    channel has been assigned */
  mafID GetChannelBase();

  mafObserversList  *m_Observers;  ///< list of observers
  mafID             m_Channel;     ///< a channel assigned to this event source, if <0 no channel is assigned
private:
  
};

class MAF_EXPORT mafEventSource : public mafEventSourceBase
{
public:
  mafEventSource(void *owner=NULL):mafEventSourceBase(owner){}
  /** Register an observer of this subject */
  void AddObserver(mafBaseEventHandler *obj, int priority=0){AddObserverBase(obj, priority);}

  /** Register an observer of this subject */
  void AddObserver(mafBaseEventHandler &obj, int priority=0){AddObserverBase(obj, priority);}

  /** Unregister an observer. Return false if object is not an observer */
  bool RemoveObserver(mafBaseEventHandler *obj){return RemoveObserverBase(obj);}

  /** remove all observers at once */
  void RemoveAllObservers(){RemoveAllObserversBase();}

  /** return true if object is an observer of this subject */
  bool IsObserver(mafBaseEventHandler *obj)const{return IsObserverBase(obj);}

  /** return true if this class has observers */
  bool HasObservers()const{return HasObserversBase();}

  /** return a vector with the list of observers of this event source */
  void GetObservers(std::vector<mafBaseEventHandler *> &olist)const{GetObserversBase(olist);}

  /** 
  set the channel Id assigned to this event source. If set to <0 
  no channel is assigned */
  void SetChannel(mafID ch){SetChannelBase(ch);}

  /** 
  return the channel assigned to this event source. If <0 no 
  channel has been assigned */
  mafID GetChannel(){return GetChannelBase();}
};


class MAF_EXPORT mafEventSender : public mafEventSourceBase
{
public:
  mafEventSender() {}

  /** Set the listener object, i.e. the object receiving events sent by this object */
  void SetListener(mafBaseEventHandler *o) {RemoveAllObserversBase(); if (o != NULL ) AddObserverBase(o);}

  /** Return the listener object, i.e. the object receiving events sent by this object */
  mafBaseEventHandler *GetListener() const {if(!HasListener())return NULL; std::vector<mafBaseEventHandler *> a; GetObserversBase(a); return a[0];}

  /** return true if this class has observers */
  bool HasListener() const {return HasObserversBase();}
};

#endif /* __mafEventSender_h */
