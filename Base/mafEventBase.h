#pragma once

#include "ftkConfigure.h"

#include "ftk/Base/Object.h"

#include "mafDecl.h"

BEGIN_FTK_NAMESPACE

class FTK_BASE_EXPORT mafEventBase: public mafObject
{
public:
  mafEventBase(void *sender = nullptr, mafID id = ID_NO_EVENT, void *data = nullptr, mafID channel=MCH_UP);

	~mafEventBase() override;

  mafTypeMacroN(mafEventBase);
 
  mafEventBase(const mafEventBase& c);

  virtual void DeepCopy(const mafEventBase *maf_event);

  static void SetLogVerbose(bool verbose = true);

  static bool* GetLogVerbose();

  void SetSender(void *sender);

  void *GetSender() const;

  void SetId(mafID id);

  mafID GetId() const;

  mafID GetChannel() const;
  
  void SetChannel(mafID channel);

  void SetData(void *calldata);

  void *GetData() const;

protected:
  void            *m_Sender = nullptr;
  void            *m_Data   = nullptr;
  mafID           m_Id      = ID_NO_EVENT;
  mafID           m_Channel = MCH_UP;
};

END_FTK_NAMESPACE
