#pragma once

#include "ftkConfigure.h"

#include "mafBaseEventHandler.h"
#include "mafEventBase.h"
#include <vector>

class mafObserversList;

BEGIN_FTK_NAMESPACE

class FTK_BASE_EXPORT mafEventSourceBase
{
public:
  mafEventSourceBase();
  virtual ~mafEventSourceBase();

  void InvokeEvent(void* sender, mafID id) {}//{mafEventBase evUnq(sender,id); InvokeEvent(evUnq);}}

  void InvokeEvent(mafEventBase &e) {InvokeEvent(&e);}

  virtual void InvokeEvent(mafEventBase *e);
 
protected:

  void AddObserverBase(mafBaseEventHandler *obj, int priority=0);

	void AddObserverBase(mafBaseEventHandler &obj, int priority=0);

	bool RemoveObserverBase(mafBaseEventHandler *obj);

  void RemoveAllObserversBase();

  bool IsObserverBase(mafBaseEventHandler *obj)const;

  bool HasObserversBase()const;

  std::vector<mafBaseEventHandler*> GetObserversBase() const;

  void SetChannelBase(mafID ch);

  mafID GetChannelBase();

  std::unique_ptr<mafObserversList> m_Observers;
  mafID             m_Channel = -1;
};

class FTK_BASE_EXPORT mafEventSource : public mafEventSourceBase
{
public:
  mafEventSource():mafEventSourceBase(){}

  void AddObserver(mafBaseEventHandler *obj, int priority=0){AddObserverBase(obj, priority);}

  void AddObserver(mafBaseEventHandler &obj, int priority=0){AddObserverBase(obj, priority);}

  bool RemoveObserver(mafBaseEventHandler *obj){return RemoveObserverBase(obj);}

  void RemoveAllObservers(){RemoveAllObserversBase();}

  bool IsObserver(mafBaseEventHandler *obj)const{return IsObserverBase(obj);}

  bool HasObservers()const{return HasObserversBase();}

  std::vector<mafBaseEventHandler*> GetObservers() const {return GetObserversBase();}

  void SetChannel(mafID ch){SetChannelBase(ch);}

  mafID GetChannel(){return GetChannelBase();}
};

class FTK_BASE_EXPORT mafEventSender : public mafEventSourceBase
{
public:
  mafEventSender() {}

  void SetListener(mafBaseEventHandler *o) {RemoveAllObserversBase(); if (o) AddObserverBase(o);}

  mafBaseEventHandler *GetListener() const {if(!HasListener()) return nullptr; return GetObserversBase()[0];}

  bool HasListener() const {return HasObserversBase();}
};

END_FTK_NAMESPACE