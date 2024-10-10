#pragma once

#include "ftkConfigure.h"

#include "ftk/Base/String.h"
#include <vector>

class FTK_BASE_EXPORT mafDirectory
{
public:  
  bool Load(const mafString& dir);

  size_t GetNumberOfFiles()const { return m_Files.size();}

  const mafString& GetFile(size_t index) const;

protected:
  std::vector<mafString> m_Files;
  mafString m_Path;
}; 
