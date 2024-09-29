#pragma once

#include "ftkConfigure.h"

#include "mafEventBase.h"

BEGIN_FTK_NAMESPACE

class FTK_BASE_EXPORT mafBaseEventHandler
{
public:
  mafBaseEventHandler() {}
  virtual ~mafBaseEventHandler() {}

  /** process the events sent by subjects */
  virtual void OnEvent(mafEventBase *e) = 0;
};

END_FTK_NAMESPACE
