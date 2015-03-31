/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafPipe.cpp,v $
  Language:  C++
  Date:      $Date: 2010-11-10 16:52:01 $
  Version:   $Revision: 1.16.2.2 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
//----------------------------------------------------------------------------


#include "mafGUI.h"

#include "mafPipeVTK.h"
#include "mafViewVTK.h"
#include "mafNode.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafPipeVTK);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafPipeVTK::mafPipeVTK()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
mafPipeVTK::~mafPipeVTK()
//----------------------------------------------------------------------------
{
}
