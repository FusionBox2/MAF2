#include "mafReferenceCounted.h"

void mafReferenceCounted::Delete()
{
  UnRegister(nullptr);
}

mafReferenceCounted::mafReferenceCounted()
	: mafObject()
  , m_ReferenceCount(0) 
{
}

mafReferenceCounted::~mafReferenceCounted() 
{
  if ( m_ReferenceCount > 0)
  {
    mafErrorMacro(<< "Trying to delete object with non-zero reference count.");
  }
}

void mafReferenceCounted::Register(void *obj)
{
  if (m_ReferenceCount < 0)
  {
    mafErrorMacro("Trying to Register an object with negative reference counter equal.");
  }

	m_ReferenceCount++;

  if (m_ReferenceCount <= 0)
  {
    delete this;
  }
}

void mafReferenceCounted::UnRegister(void *obj)
{
	if(m_ReferenceCount <= 0)
  {
    mafErrorMacro("Trying to UnRegister an object with reference counter equal to zero.");
  }

  if (--m_ReferenceCount <= 0)
  {
    delete this;
  }
}
