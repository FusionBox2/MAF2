#include "ftk/Base/MTime.h"
#include "ftk/Base/SpinLock.h"
#include <mutex>

#ifdef MAF_USE_VTK
#include "vtkTimeStamp.h"
#endif

BEGIN_FTK_NAMESPACE

//------------------------------------------------------------------------------
void MTime::Modified()
//------------------------------------------------------------------------------
{
#ifdef MAF_USE_VTK  
  static vtkTimeStamp timeStamp;
  static SpinLock TimeStampMutex;

  std::lock_guard lock(TimeStampMutex);
  timeStamp.Modified();
  m_ModifiedTime = timeStamp.GetMTime();
#else
  static MTimeType mafMTimeTime = 0;
  static SpinLock TimeStampMutex;

  std::lock_guard lock(TimeStampMutex);
  m_ModifiedTime = ++mafMTimeTime;
#endif
}

//------------------------------------------------------------------------------
MTimeType MTime::GetMTime() const
//------------------------------------------------------------------------------
{
  return m_ModifiedTime;
}

END_FTK_NAMESPACE
