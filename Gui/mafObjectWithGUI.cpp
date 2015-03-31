/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafObjectWithGUI.cpp,v $
  Language:  C++
  Date:      $Date: 2008-07-25 06:56:04 $
  Version:   $Revision: 1.58 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mafDecl.h"

#include "mafObjectWithGUI.h"
#include "mafDecl.h"
#include "mafGUI.h"
#include <assert.h>



//-------------------------------------------------------------------------
mafObjectWithGUI::mafObjectWithGUI()
//-------------------------------------------------------------------------
{
  m_Gui = NULL;
}

//-------------------------------------------------------------------------
mafObjectWithGUI::~mafObjectWithGUI()
//-------------------------------------------------------------------------
{
  cppDEL(m_Gui);
}

//-------------------------------------------------------------------------
mafGUI *mafObjectWithGUI::GetGui()
//-------------------------------------------------------------------------
{
  if (m_Gui == NULL)
    CreateGui();
  assert(m_Gui);
  return m_Gui;
}
//-------------------------------------------------------------------------
void mafObjectWithGUI::DeleteGui()
//-------------------------------------------------------------------------
{
  cppDEL(m_Gui);
}
