/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafStorage.cpp,v $
  Language:  C++
  Date:      $Date: 2007-12-11 11:25:08 $
  Version:   $Revision: 1.16 $
  Authors:   Marco Petrone m.petrone@cineca.it
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafParser.h"
#include "mafStorable.h"

//------------------------------------------------------------------------------
mafCxxAbstractTypeMacro(mafParser);
//------------------------------------------------------------------------------
mafParser::mafParser()
//------------------------------------------------------------------------------
{
  m_Document        = NULL;
  m_NeedsUpgrade    = false;
  m_ErrorCode       = 0;
  m_Version         = "1.1";
}
//------------------------------------------------------------------------------
int mafParser::Store()
//------------------------------------------------------------------------------
{
  SetErrorCode(0);
  return InternalStore();
}
//------------------------------------------------------------------------------
int mafParser::Restore()
//------------------------------------------------------------------------------
{
  SetErrorCode(0);
  return InternalRestore();
}
