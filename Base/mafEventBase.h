#pragma once

#include "ftkConfigure.h"

#include "ftk/Base/Object.h"

#include "mafDecl.h"

BEGIN_FTK_NAMESPACE

class FTK_BASE_EXPORT mafEventBase: public mafObject
{
public:
  mafEventBase(void *sender=NULL,mafID id=ID_NO_EVENT,void *data=NULL, mafID channel=MCH_UP);

	~mafEventBase() override;

  mafTypeMacro(mafEventBase,mafObject);
 
  mafEventBase(const mafEventBase& c);

  virtual void DeepCopy(const mafEventBase *maf_event);

  static void SetLogVerbose(bool verbose = true);

  static bool* GetLogVerbose();

  void SetSender(void *sender);

  void *GetSender();

  void SetId(mafID id);

  mafID GetId();

  mafID GetChannel();
  
  void SetChannel(mafID channel);

  void SetData(void *calldata);

  void *GetData();

protected:
  void            *m_Sender;
  void            *m_Data;
  mafID           m_Id;
  mafID           m_Channel;
  static bool     m_LogVerbose;
};

END_FTK_NAMESPACE
