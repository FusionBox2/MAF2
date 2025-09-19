#pragma once

#include "ftkConfigure.h"

#include "ftk/Base/MTime.h"

BEGIN_FTK_NAMESPACE

//------------------------------------------------------------------------------
// mafTimeStamped
//------------------------------------------------------------------------------
/** class acting as an interface for timestamped objects
  This object simply defines few methods for managing a modification time to be used in 
  for process objects.
  @sa mafMTime
*/
class FTK_BASE_EXPORT mafTimeStamped
{
public:
  /**
	Update this objects modification time. The modification time is
	just a monotonically increasing unsigned long integer. It is
	possible for this number to wrap around back to zero.
	This should only happen for processes that have been running
	for a very long time, while constantly changing objects
	within the program. When this does occur, the typical consequence
	should be that some process objects will update themselves when really
	they don't need to. */
  virtual void Modified();

  virtual MTimeType GetMTime();

protected:

	mafMTime m_MTime;
};

END_FTK_NAMESPACE
