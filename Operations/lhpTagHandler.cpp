/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpTagHandler.cpp,v $
  Language:  C++
  Date:      $Date: 2009-04-10 13:50:21 $
  Version:   $Revision: 1.3.2.2 $
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

#include "lhpUtils.h"
#include "lhpTagHandler.h"
#include "mafDecl.h"

mafCxxTypeMacro(lhpTagHandlerInputOutputParametersCargo);
//------------------------------------------------------------------------
lhpTagHandlerInputOutputParametersCargo::lhpTagHandlerInputOutputParametersCargo()
//------------------------------------------------------------------------------
{
  m_InputVme = NULL;
	m_InputMSF = _R("");

  m_TagHandlerGeneratedString = _R("NOT YET HANDLED!");
}


mafCxxTypeMacro(lhpTagHandler);
//------------------------------------------------------------------------
lhpTagHandler::lhpTagHandler()
//------------------------------------------------------------------------------
{
	m_PythonExe =_R("python.exe ");
	m_PythonwExe =_R("pythonw.exe ");
	m_VMEUploaderDownloaderDir  = lhpUtils::lhpGetApplicationDirectory() + _R("\\..\\VMEUploaderDownloader\\");
}

//------------------------------------------------------------------------
void lhpTagHandler::ExtractTagName()
//------------------------------------------------------------------------
{
  // tag name from type
  m_TagName = _R(this->GetTypeName());
  auto endPos = m_TagName.find(_R("_"));
  if(endPos != mafString::npos)
  {
	  m_TagName.erase(0, endPos + 1);
  }
  else
  {
	  m_TagName.clear();
  }
}