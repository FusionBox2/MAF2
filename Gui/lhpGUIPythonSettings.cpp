/*=========================================================================
Program:   LHPBuilder
Module:    $RCSfile: lhpGUIPythonSettings.cpp,v $
Language:  C++
Date:      $Date: 2008-11-18 14:49:16 $
Version:   $Revision: 1.1.2.2 $
Authors:   Stefano Perticoni
==========================================================================
Copyright (c) 2008
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "lhpGUIPythonSettings.h"
#include "mafCrypt.h"

#include "mafDecl.h"
#include "mafGUI.h"

lhpGUIPythonSettings::lhpGUIPythonSettings(mafObserver *Listener, const mafString &label):
mafGUISettings(Listener, label)
{
  // Default interpreter will be set at application startup
  
  // default to system python
  m_PythonExe = "python.exe";
  m_PythonwExe = "pythonw.exe";

  mafString pythonDir  = (mafGetApplicationDirectory() + "\\Python\\Python25").c_str();
  mafString pythonExe = pythonDir + "\\python.exe";
  mafString pythonwExe = pythonDir + "\\pythonw.exe";

  // use the embedded python if available
  if (wxFileExists(pythonExe.GetCStr()))
  {
    m_PythonExe = pythonExe.GetCStr();
  }
  
  if (wxFileExists(pythonExe.GetCStr()))
  {
    m_PythonwExe = pythonwExe.GetCStr();
  }

  InitializeSettings();
}

lhpGUIPythonSettings::~lhpGUIPythonSettings()
{
}

void lhpGUIPythonSettings::CreateGui()
{
  m_Gui = new mafGUI(this);
  m_Gui->Label(_("Python Settings"));
  m_Gui->Label(_(""));
  m_Gui->Label(_("python.exe interpreter full path"));
  m_Gui->String(ID_PYTHON_EXE,_(""),&m_PythonExe,1);
  m_Gui->Label(_(""));
  m_Gui->Label(_("pythonw.exe interpreter full path"));
  m_Gui->String(ID_PYTHONW_EXE,_(""),&m_PythonwExe,1);
  m_Gui->Label(_(""));
  m_Gui->Divider(2);
  EnableItems(true);
}

void lhpGUIPythonSettings::EnableItems( bool enable )
{
  m_Gui->Enable(ID_PYTHON_EXE, enable);
  m_Gui->Enable(ID_PYTHONW_EXE, enable);
}

void lhpGUIPythonSettings::OnEvent(mafEventBase *maf_event)
{
  switch(maf_event->GetId())
  {
    case ID_PYTHON_EXE:
      m_Config->Write("m_PythonExe",m_PythonExe.GetCStr());
    break;
    case ID_PYTHONW_EXE:
      m_Config->Write("m_PythonwExe",m_PythonwExe.GetCStr());
    break;
    default:
      mafEventMacro(*maf_event);
    break; 
  }
  
  m_Config->Flush();
}

void lhpGUIPythonSettings::InitializeSettings()
{
  wxString stringItem;

  // For the moment the Python interpreter is set at startup...

  //if(m_Config->Read("m_PythonExe", &stringItem))
  //{
  //  m_PythonExe = stringItem;
  //}
  //else
  //{
  m_Config->Write("m_PythonExe",m_PythonExe);
    // }
  // 
  /*if(m_Config->Read("m_PythonwWxe", &stringItem))
  {
    m_PythonwExe = stringItem.c_str();
  }
  else
  {*/
  m_Config->Write("m_PythonwWxe",m_PythonwExe.GetCStr());
  //   }

  m_Config->Flush();
}

