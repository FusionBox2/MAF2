/*=========================================================================

 Program: MAF2
 Module: mafVTKLog
 Authors: Marco Petrone
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "mafDefines.h" 
#include "mafDecl.h"
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mafVTKLog.h"
#include "vtkObjectFactory.h"

//----------------------------------------------------------------------------
mafVTKLog *mafVTKLog::New() 
//----------------------------------------------------------------------------
{
  vtkObject* ret = vtkObjectFactory::CreateInstance("mafVTKLog");
  if(ret)
    return (mafVTKLog*)ret;
  return new mafVTKLog;
}
//----------------------------------------------------------------------------
mafVTKLog::mafVTKLog() 
//----------------------------------------------------------------------------
{
  this->m_Enabled=1;
}
//----------------------------------------------------------------------------
mafVTKLog::~mafVTKLog() 
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafVTKLog::DisplayText(const char* text)
//----------------------------------------------------------------------------
{
  if(!text)
  {
    return;
  }

  if (this->m_Enabled)
  {
    mafString message=_R("[VTK]");
    message += _R(text);

    // Strip CR
    for (;message.EndsWith(_R("\n"));)
    {
      message.Erase(message.Length() - 1);
    }

    mafLogMessage(_M(message));
  }
}
