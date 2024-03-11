/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: lhpUser.cpp,v $
Language:  C++
Date:      $Date: 2009-04-10 13:51:20 $
Version:   $Revision: 1.11.2.8 $
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
#include "lhpUtils.h"

#include "lhpBuilderDecl.h"
#include "lhpUser.h"
#include <wx/stdpaths.h>
#include <wx/tokenzr.h>

#include "mafDecl.h"
#include "mafGUIDialogLogin.h"
#include "mafCrypt.h"

#include <fstream>

mafCxxTypeMacro(lhpUser);

//----------------------------------------------------------------------------
lhpUser::lhpUser(mafBaseEventHandler *listener)
//----------------------------------------------------------------------------
{
  SetListener(listener);
  m_PythonExe = _R("python.exe_UNDEFINED");
  m_PythonwExe = _R("pythonw.exe_UNDEFINED");
  m_VMEUploaderDownloaderDir  = lhpUtils::lhpGetApplicationDirectory() + _R("\\VMEUploaderDownloader\\");
  
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
    m_PythonwExe = *event.GetString();
    m_PythonwExe.Append(_R(" "));
  }
  
  wxString oldDir = wxGetCwd();
  mafLogMessage( _M(_R("Current working directory is: '") + mafWxToString(wxGetCwd()) + _R("' ")));
  wxSetWorkingDirectory(m_VMEUploaderDownloaderDir.toWx());
  mafLogMessage( _M(_R("Now current working directory is: '") + mafWxToString(wxGetCwd()) + _R("' ")));

  // get manual tags
  mafString command2execute;
  command2execute = m_PythonwExe;

  command2execute.Append(_R(" lhpAuthenticationControl.py "));
  command2execute.Append(m_Username);
  command2execute.Append(_R(" "));
  command2execute.Append(m_Password);
  
  if(m_ProxyFlag != 0 && !m_ProxyHost.IsEmpty())
  {
    command2execute.Append(_R(" "));
    command2execute.Append(m_ProxyHost);
    command2execute.Append(_R(" "));
    command2execute.Append(mafToString(m_ProxyPort));
    command2execute.Append(_R(" "));
  }
  
  //mafLogMessage( _T("Executing command: '%s'"), command2execute.c_str() );

  mafLogMessage(_M("Authenticating user (Timeout to 30 seconds). Please wait..."));
  long pid = wxExecute(command2execute.toWx(), wxEXEC_SYNC);

  wxArrayString output;
  wxArrayString errors;

  pid = wxExecute(command2execute.toWx(), output, errors);

  if (output.size() == 0)
  {
    return false;
  }


  mafLogMessage(_M("Command Output Messages:"));
  for (int i = 0; i < output.size(); i++)
  {
    mafLogMessage(_M(mafWxToString(output[i])));
  }

  mafLogMessage(_M("Command Errors Messages (if any...):"));
  for (int i = 0; i < errors.size(); i++)
  {
    mafLogMessage(_M(mafWxToString(errors[i])));
  }

  // gathering values from Python Output:

  //if dictVC.IsDictionaryUpToDate() == True:
  //print "UpToDate"
  //else:
  //print "NotUpToDate"

  wxString result = output[output.size() - 1];

  wxSetWorkingDirectory(oldDir);
  mafLogMessage( _M(_R("Current working directory is: '") + mafWxToString(wxGetCwd()) + _R("' ")));

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



