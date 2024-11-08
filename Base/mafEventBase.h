#pragma once

#include "ftkConfigure.h"

#include "ftk/Base/Object.h"

#include "mafDecl.h"

BEGIN_FTK_NAMESPACE

class FTK_BASE_EXPORT mafEventBase
{
  mafBaseTypeMacro(mafEventBase);

  mafEventBase() = default;
  
  mafEventBase(void *sender,mafID id);

  virtual ~mafEventBase();

  mafEventBase(const mafEventBase& c);

public:
  virtual void DeepCopy(const mafEventBase* maf_event);

  void* GetSender() const { return m_Sender; }

  void SetSender(void* sender) { m_Sender = sender; }

  mafID GetId() const { return m_Id; }

  void SetId(mafID id) { m_Id = m_Channel; }

  mafID GetChannel() const { return m_Channel; }
  
  void SetChannel(mafID channel) { m_Channel = channel; }

  void* GetData() const { return m_Data; }

  void SetData(void* calldata) { m_Data = calldata; }

protected:
  void            *m_Sender = nullptr;
  void            *m_Data = nullptr;
  mafID           m_Id = ID_NO_EVENT;
  mafID           m_Channel = MCH_UP;
};

END_FTK_NAMESPACE
