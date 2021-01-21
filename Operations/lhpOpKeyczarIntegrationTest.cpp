/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpOpKeyczarIntegrationTest.cpp,v $
  Language:  C++
  Date:      $Date: 2008-12-05 14:58:57 $
  Version:   $Revision: 1.1.2.4 $
  Authors:   Stefano Perticoni
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

#include "lhpBuilderDecl.h"
#include "lhpOpKeyczarIntegrationTest.h"

#include "wx/busyinfo.h"

#include "mafDecl.h"
#include "mafGUI.h"

#include <iostream>
#include <fstream>

//----------------------------------------------------------------------------
mafCxxTypeMacro(lhpOpKeyczarIntegrationTest);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
lhpOpKeyczarIntegrationTest::lhpOpKeyczarIntegrationTest(const mafString& label) : Superclass(label)
//----------------------------------------------------------------------------
{
  m_OpType  = OPTYPE_OP;
  m_Canundo = false;
  
  m_PythonExe = _R("python.exe_UNDEFINED");
  m_PythonwExe = _R("pythonw.exe_UNDEFINED");  
}

//----------------------------------------------------------------------------
lhpOpKeyczarIntegrationTest::~lhpOpKeyczarIntegrationTest()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
bool lhpOpKeyczarIntegrationTest::Accept(mafNode *node)
//----------------------------------------------------------------------------
{
  return true;
}
//----------------------------------------------------------------------------
mafOp* lhpOpKeyczarIntegrationTest::Copy()   
//----------------------------------------------------------------------------
{
  lhpOpKeyczarIntegrationTest *cp = new lhpOpKeyczarIntegrationTest(GetLabel());
  return cp;
}
//----------------------------------------------------------------------------
void lhpOpKeyczarIntegrationTest::OpRun()   
//----------------------------------------------------------------------------
{
  // get python interpreters
  mafEvent eventGetPythonExe;
  eventGetPythonExe.SetSender(this);
  eventGetPythonExe.SetId(ID_REQUEST_PYTHON_EXE_INTERPRETER);
  mafEventMacro(eventGetPythonExe);

  if(eventGetPythonExe.GetString())
  {
    m_PythonExe.Erase(0);
    m_PythonExe = *eventGetPythonExe.GetString();
    m_PythonExe.Append(_R(" "));
  }

  mafEvent eventGetPythonwExe;
  eventGetPythonwExe.SetSender(this);
  eventGetPythonwExe.SetId(ID_REQUEST_PYTHONW_EXE_INTERPRETER);
  mafEventMacro(eventGetPythonwExe);

  if(eventGetPythonwExe.GetString())
  {
    m_PythonwExe.Erase(0);
    m_PythonwExe = *eventGetPythonwExe.GetString();
    m_PythonwExe.Append(_R(" "));
  }


  CreateGui();
  Execute();
}
//----------------------------------------------------------------------------
int lhpOpKeyczarIntegrationTest::Execute()
//----------------------------------------------------------------------------
{
  
  // This is for deploy: need to work on PYTHONPATH to solve issues with Python modules execution path...
  wxString keyCZarToolFullPath = "NONE";
  keyCZarToolFullPath = mafGetApplicationDirectory().toWx();
  keyCZarToolFullPath.Append("\\Security\\keyczar\\Keyczar-Python\\src\\keyczar");

  wxArrayString output;
  wxArrayString errors;

  mafString command2execute;

  wxString oldDir = wxGetCwd();

  mafLogMessage( _M(_R("Current working directory is: '") + mafWxToString(wxGetCwd()) + _R("' ")));

  wxSetWorkingDirectory(keyCZarToolFullPath);

  mafLogMessage( _M(_R("Now current working directory is: '") + mafWxToString(wxGetCwd()) + _R("' ")));

  command2execute = _R("cmd.exe /K \"");
  command2execute.Append(m_PythonExe);
  command2execute.Append(_R(" keyczart.py\""));
  mafLogMessage( _M(_R("Executing command: '") + command2execute + _R("'")));

  long pid = wxExecute(command2execute.toWx(), wxEXEC_SYNC);

  for (int i = 0; i < output.GetCount(); i++)
  {
    mafLogMessage(_M(mafWxToString(output[i])));
  }
  
  if ( !command2execute.GetCStr() )
    return MAF_ERROR;

  
  mafLogMessage(_M(_R("Command process '") + command2execute + _R("' terminated with exit code ") + mafToString(pid) + _R(".")));

  wxSetWorkingDirectory(oldDir);
  int result = OP_RUN_OK;
  mafEventMacro(mafEvent(this,result));
  
  return MAF_OK;
}

enum Mesh_Importer_ID
{
  ID_FIRST = MINID,
  ID_OK,
  ID_CANCEL,
};
//----------------------------------------------------------------------------
void lhpOpKeyczarIntegrationTest::CreateGui()
//----------------------------------------------------------------------------
{
  mafString wildcard = _R("inp files (*.inp)|*.inp|All Files (*.*)|*.*");

}
//----------------------------------------------------------------------------
void lhpOpKeyczarIntegrationTest::OnEvent(mafEventBase *maf_event) 
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {
      case wxOK:
      {
        this->OpStop(OP_RUN_OK);
      }
      break;
      case wxCANCEL:
      {
        this->OpStop(OP_RUN_CANCEL);
      }
      break;
      default:
        mafEventMacro(*e);
      break;
    }	
  }
}
