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
  mafEventSourceBase(void *owner=NULL);
  virtual ~mafEventSourceBase();

  void InvokeEvent(mafID id, void *data=NULL) {InvokeEvent(this,id,data);}

  void InvokeEvent(void *sender,mafID id, void *data=NULL){{mafEventBase evUnq(sender,id,data); InvokeEvent(evUnq);}}

  void InvokeEvent(mafEventBase &e) {InvokeEvent(&e);}

  virtual void InvokeEvent(mafEventBase *e);
 
protected:

  void AddObserverBase(mafBaseEventHandler *obj, int priority=0);

	void AddObserverBase(mafBaseEventHandler &obj, int priority=0);

	bool RemoveObserverBase(mafBaseEventHandler *obj);

  void RemoveAllObserversBase();

  bool IsObserverBase(mafBaseEventHandler *obj)const;

  bool HasObserversBase()const;

  void GetObserversBase(std::vector<mafBaseEventHandler *> &olist)const;

  void SetChannelBase(mafID ch);

  mafID GetChannelBase();

  mafObserversList  *m_Observers;
  mafID             m_Channel;
};

class FTK_BASE_EXPORT mafEventSource : public mafEventSourceBase
{
public:
  mafEventSource(void *owner = nullptr):mafEventSourceBase(owner){}

  void AddObserver(mafBaseEventHandler *obj, int priority=0){AddObserverBase(obj, priority);}

  void AddObserver(mafBaseEventHandler &obj, int priority=0){AddObserverBase(obj, priority);}

  bool RemoveObserver(mafBaseEventHandler *obj){return RemoveObserverBase(obj);}

  void RemoveAllObservers(){RemoveAllObserversBase();}

  bool IsObserver(mafBaseEventHandler *obj)const{return IsObserverBase(obj);}

  bool HasObservers()const{return HasObserversBase();}

  void GetObservers(std::vector<mafBaseEventHandler *> &olist)const{GetObserversBase(olist);}

  void SetChannel(mafID ch){SetChannelBase(ch);}

  mafID GetChannel(){return GetChannelBase();}
};

class FTK_BASE_EXPORT mafEventSender : public mafEventSourceBase
{
public:
  mafEventSender() {}

  void SetListener(mafBaseEventHandler *o) {RemoveAllObserversBase(); if (o) AddObserverBase(o);}

  mafBaseEventHandler *GetListener() const {if(!HasListener()) return nullptr; std::vector<mafBaseEventHandler *> a; GetObserversBase(a); return a[0];}

  bool HasListener() const {return HasObserversBase();}
};

END_FTK_NAMESPACE