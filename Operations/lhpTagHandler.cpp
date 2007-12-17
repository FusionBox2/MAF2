/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpTagHandler.cpp,v $
  Language:  C++
  Date:      $Date: 2007-12-17 09:15:47 $
  Version:   $Revision: 1.1 $
  Authors:   Stefano Perticoni - Daniele Giunchi
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------------

#include "lhpTagHandler.h"


mafCxxTypeMacro(lhpTagHandler);
//------------------------------------------------------------------------
lhpTagHandler::lhpTagHandler()
//------------------------------------------------------------------------------
{
}

//------------------------------------------------------------------------
void lhpTagHandler::ExtractTagName()
//------------------------------------------------------------------------
{
  // tag name from type
  m_TagName = this->GetTypeName();
  int endPos = m_TagName.FindFirst("_");
  m_TagName.Erase(0, endPos);
}