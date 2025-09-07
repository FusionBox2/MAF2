#include "mafEventIO.h"
#include "mafNode.h"
#include "ftk/Core/Root.h"

mafEventIO::mafEventIO() = default;

mafEventIO::mafEventIO(void* sender, mafID id)
: mafEventBase(sender, id)
{
}

mafEventIO::~mafEventIO() = default;

void mafEventIO::DeepCopy(const mafEventIO *c)
{
  Superclass::DeepCopy(c);
}

void mafEventIO::SetStorage(mafStorage *storage)
{
  m_Storage = storage;
}

mafStorage *mafEventIO::GetStorage() const
{
  return m_Storage;
}

void mafEventIO::SetRoot(mafNode *root)
{
  if (auto rt = dynamic_cast<model::data::Root*>(root))
  {
	  m_Root = root;
  }
}

mafNode *mafEventIO::GetRoot() const
{
  return m_Root;  
}
