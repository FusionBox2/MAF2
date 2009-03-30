/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpTagHandlerRefactor.cpp,v $
  Language:  C++
  Date:      $Date: 2009-03-30 14:25:12 $
  Version:   $Revision: 1.1.2.1 $
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

#include "lhpTagHandlerRefactor.h"
#include "mafDecl.h"

mafCxxTypeMacro(lhpTagHandlerInputOutputParametersCargoRefactor);
//------------------------------------------------------------------------
lhpTagHandlerInputOutputParametersCargoRefactor::lhpTagHandlerInputOutputParametersCargoRefactor()
//------------------------------------------------------------------------------
{
  m_InputVme = NULL;
	m_InputUser = NULL;
	m_InputMSF = "";

  m_TagHandlerGeneratedString = "NOT YET HANDLED!";
}


mafCxxTypeMacro(lhpTagHandlerRefactor);
//------------------------------------------------------------------------
lhpTagHandlerRefactor::lhpTagHandlerRefactor()
//------------------------------------------------------------------------------
{
	m_PythonExe ="python.exe ";
	m_PythonwExe ="pythonw.exe ";
	m_PythonUploadFullPath  = (mafGetApplicationDirectory() + "\\..\\VMEUploaderDownloaderRefactor\\").c_str();
}

//------------------------------------------------------------------------
void lhpTagHandlerRefactor::ExtractTagName()
//------------------------------------------------------------------------
{
  // tag name from type
  m_TagName = this->GetTypeName();
  int endPos = m_TagName.FindFirst("_");
  m_TagName.Erase(0, endPos);
}