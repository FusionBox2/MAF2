/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: lhpUser.cpp,v $
Language:  C++
Date:      $Date: 2008-11-17 13:08:31 $
Version:   $Revision: 1.11.2.3 $
Authors:   Daniele Giunchi
==========================================================================
Copyright (c) 2002/2004
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
#include "lhpUser.h"
#include <wx/stdpaths.h>
#include <wx/tokenzr.h>

#include "mafDecl.h"
#include "mafGUIDialogLogin.h"
#include "mafCrypt.h"

#include <fstream>

//----------------------------------------------------------------------------
lhpUser::lhpUser(mafObserver *listener)
//----------------------------------------------------------------------------
{
  m_Listener = listener;
  m_PythonExe = "python.exe_UNDEFINED";
  m_PythonwExe = "pythonw.exe_UNDEFINED";
  m_PythonUploadFullPath  = (mafGetApplicationDirectory() + "\\VMEUploaderDownloader\\").c_str();
  
  // cmake must check if the Python stuff exists already
  // if not install it
  // otherwise proceed
  // cmake must copy all pythons dir to binary directory ( should alert the user )

  m_ProxyURL = "";
  m_ProxyPort = "";
  m_IsAuthenticated = false;
 
}
//----------------------------------------------------------------------------
lhpUser::~lhpUser()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
bool lhpUser::CheckUserCredentials()
//----------------------------------------------------------------------------
{
  if (!m_Initialized)
  {
    InitializeUserInformations();
  }
 
  int res = false;
  if(ShowLoginDialog() != wxID_CANCEL)
    res = !m_Username.IsEmpty() && !m_Password.IsEmpty() && ExecuteAuthenticationScript();

  if(!res)
  {
    int result = wxMessageBox("\
No server side authentication!\n\
Do you want to Retry?\n\
Pressing Cancel information about\n\
the user will not be stored on MSF and network enabled\n\
facilities will be disabled",wxMessageBoxCaptionStr, wxOK | wxCANCEL);
    //returns 4 for OK, 16 for CANCEL
    if (result == 16)
    {
      return false;
    }
    else
    {
      return true;
    }
  }
  m_IsAuthenticated = true;

  return false;
}

//----------------------------------------------------------------------------
bool lhpUser::IsAuthenticated()
//----------------------------------------------------------------------------
{
  return m_IsAuthenticated;
}

//----------------------------------------------------------------------------
bool lhpUser::ExecuteAuthenticationScript()
//----------------------------------------------------------------------------
{
  mafEvent event;
  event.SetSender(this);
  event.SetId(ID_REQUEST_PYTHONW_EXE_INTERPRETER);
  mafEventMacro(event);

  if(event.GetString())
  {
    m_PythonwExe.Erase(0);
    m_PythonwExe = event.GetString()->GetCStr();
    m_PythonwExe.Append(" ");
  }
  
  wxString oldDir = wxGetCwd();
  mafLogMessage( _T("Current working directory is: '%s' "), wxGetCwd().c_str() );
  wxSetWorkingDirectory(m_PythonUploadFullPath.GetCStr());
  mafLogMessage( _T("Now current working directory is: '%s' "), wxGetCwd().c_str() );

  // get manual tags
  wxString command2execute;
  command2execute.Clear();
  command2execute = m_PythonwExe.GetCStr();

  command2execute.Append(" lhpAuthenticationControl.py ");
  command2execute.Append(m_Username.GetCStr());
  command2execute.Append(" ");
  command2execute.Append(m_Password.GetCStr());
  
  if(!m_ProxyURL.Equals(""))
  {
    command2execute.Append(" ");
    command2execute.Append(m_ProxyURL.GetCStr());
    command2execute.Append(" ");
    command2execute.Append(m_ProxyPort.GetCStr());
  }
  
  //mafLogMessage( _T("Executing command: '%s'"), command2execute.c_str() );

  mafLogMessage("Authenticating user (Timeout to 15 seconds). Please wait...");
  long pid = wxExecute(command2execute, wxEXEC_SYNC);

  wxArrayString output;
  wxArrayString errors;

  pid = wxExecute(command2execute, output, errors);

  if (output.size() == 0)
  {
    return false;
  }


  mafLogMessage("Command Output Messages:");
  for (int i = 0; i < output.size(); i++)
  {
    mafLogMessage(output[i]);
  }

  mafLogMessage("Command Errors Messages \(if any...\):");
  for (int i = 0; i < errors.size(); i++)
  {
    mafLogMessage(errors[i]);
  }

  // gathering values from Python Output:

  //if dictVC.IsDictionaryUpToDate() == True:
  //print "UpToDate"
  //else:
  //print "NotUpToDate"

  wxString result = output[output.size() - 1];

  wxSetWorkingDirectory(oldDir);
  mafLogMessage( _T("Current working directory is: '%s' "), wxGetCwd().c_str() );

  if (result == "Authenticated")
  {
    return true;
  } 
  else 
  {
    int result = wxMessageBox("\
Problems during server side authentication!\n\
Please check the Log Area for more details on the error \n"\
,wxMessageBoxCaptionStr, wxOK);
    return false;
  }  
}



