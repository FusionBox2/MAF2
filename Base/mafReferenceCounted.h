#pragma once

#include "ftkConfigure.h"

#include "ftk/Base/Object.h"

BEGIN_FTK_NAMESPACE

class FTK_BASE_EXPORT mafReferenceCounted : public mafObject
{
public:
  mafReferenceCounted();
  ~mafReferenceCounted() override; 

  mafAbstractTypeMacro(mafReferenceCounted,mafObject);

  void Register(void* obj);

  virtual void UnRegister(void* obj);

  void Delete() override;

  ssize_t GetReferenceCount();

  void SetReferenceCount(ssize_t);

protected:

	ssize_t m_ReferenceCount;  ///< Number of uses of this object by other objects
};

END_FTK_NAMESPACE
