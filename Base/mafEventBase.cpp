#include "mafEventBase.h"

mafEventBase::mafEventBase(void *sender, mafID id):
m_Sender(sender),m_Id(id)
{
}

mafEventBase::~mafEventBase()
{
}

void mafEventBase::DeepCopy(const mafEventBase *maf_event)
{
  m_Sender    = maf_event->m_Sender;
  m_Data      = maf_event->m_Data;
  m_Id        = maf_event->m_Id;
}

mafEventBase::mafEventBase(const mafEventBase& c)
{
  DeepCopy(&c);
}
