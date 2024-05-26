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

mafParser::mafParser(const mafString& filetype, const mafString& version)
	: m_FileType(filetype)
	, m_Version(version)
//------------------------------------------------------------------------------
{
  m_NeedsUpgrade    = false;
  m_ErrorCode       = 0;
}
//------------------------------------------------------------------------------
int mafParser::Store(mafStorable* doc)
//------------------------------------------------------------------------------
{
  SetErrorCode(0);
  return InternalStore(doc);
}
//------------------------------------------------------------------------------
int mafParser::Restore(mafStorable* doc)
//------------------------------------------------------------------------------
{
  SetErrorCode(0);
  return InternalRestore(doc);
}
