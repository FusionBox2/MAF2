#pragma once

#include "ftkConfigure.h"

#include <cstdint>

BEGIN_FTK_NAMESPACE

using MTimeType = uint64_t;
class FTK_BASE_EXPORT MTime
{
public:
  MTime() : m_ModifiedTime(0) {} 

	void Modified();

  MTimeType GetMTime() const;

  bool operator>(const MTime& ts) const {return (GetMTime() > ts.GetMTime());}
  bool operator<(const MTime& ts) const {return (GetMTime() < ts.GetMTime());}

  operator MTimeType() const {return GetMTime();}

private:
  MTimeType m_ModifiedTime;
};

using mafMTime = MTime;

END_FTK_NAMESPACE
