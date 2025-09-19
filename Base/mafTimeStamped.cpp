#include "mafTimeStamped.h"

void mafTimeStamped::Modified()
{
  m_MTime.Modified();
}

MTimeType mafTimeStamped::GetMTime()
{
  return m_MTime.GetMTime();
}







