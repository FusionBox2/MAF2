/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: lhpOpUploadVME.cpp,v $
Language:  C++
Date:      $Date: 2007-12-03 11:32:24 $
Version:   $Revision: 1.7 $
Authors:   Daniele Giunchi
==========================================================================
Copyright (c) 2002/2007
SCS s.r.l. - BioComputing Competence Centre (www.scsolutions.it - www.b3c.it)

MafMedical Library use license agreement

The software named MafMedical Library and any accompanying documentation, 
manuals or data (hereafter collectively "SOFTWARE") is property of the SCS s.r.l.
This is an open-source copyright as follows:
Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:
* Redistributions of source code must retain the above copyright notice, 
this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice, 
this list of conditions and the following disclaimer in the documentation and/or 
other materials provided with the distribution.
* Modified source versions must be plainly marked as such, and must not be misrepresented 
as being the original software.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 'AS IS' 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
ARE DISCLAIMED. IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE FOR 
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND 
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

MafMedical is partially based on OpenMAF.
=========================================================================*/

#include "mafDefines.h"
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------
#include "lhpBuilderDecl.h"

#include <wx/process.h>
#include <wx/dir.h>
#include <wx/file.h>

#include "lhpOpUploadVME.h"

#include "mmgGui.h"

#include "mafNode.h"
#include "mafVMESurfaceParametric.h"
#include "mafVMESurface.h"

#include "vtkPolyData.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(lhpOpUploadVME);
//----------------------------------------------------------------------------
//static variables
long lhpOpUploadVME::m_Pid = -1;
mafString lhpOpUploadVME::m_CacheSubdir = "0";
//----------------------------------------------------------------------------
lhpOpUploadVME::lhpOpUploadVME(wxString label) :
mafOp(label)
//----------------------------------------------------------------------------
{
	m_OpType  = OPTYPE_OP;
	m_Canundo = false;

  //m_PythonExe ="C:\\Python25\\python.exe ";
  m_PythonExe ="python.exe ";
  m_PythonwExe ="pythonw.exe ";
  m_CacheDir = (mafGetApplicationDirectory() + "\\Data\\UploadCache\\").c_str();
  //m_CacheDir = "C:\\tmp\\";

  //m_PythonUploadFullPath = "C:\\cvsMAF\\builderAppNEWSTYLE\\VMEUploaderDownloader\\mt\\";
  m_PythonUploadFullPath  = (mafGetApplicationDirectory() + "\\VMEUploaderDownloader\\").c_str();
  m_FileName = "";
  //m_PythonUploadFullPathFileName = (mafGetApplicationDirectory() + "\\VMEUploaderDownloader\\mt\\gui.py").c_str();

  m_MsfDir = "";

}
//----------------------------------------------------------------------------
lhpOpUploadVME::~lhpOpUploadVME()
//----------------------------------------------------------------------------
{

}
//----------------------------------------------------------------------------
mafOp* lhpOpUploadVME::Copy()
//----------------------------------------------------------------------------
{
	/** return a copy of itself, needs to put it into the undo stack */
	return new lhpOpUploadVME(m_Label);
}
//----------------------------------------------------------------------------
bool lhpOpUploadVME::Accept(mafNode* vme)
//----------------------------------------------------------------------------
{
	return vme != NULL;
}
//----------------------------------------------------------------------------
void lhpOpUploadVME::OpRun()
//----------------------------------------------------------------------------
{
	OpStop(OP_RUN_OK);
}
//----------------------------------------------------------------------------
void lhpOpUploadVME::OnEvent(mafEventBase *maf_event) 
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {
    case ID_MSF_DATA_CACHE:
      {
        wxString temp;
        temp.Append((*e->GetString()).GetCStr());
        temp = temp.BeforeLast('/');
        m_MsfDir = temp;
      }
      break;
    default:
      mafEventMacro(*e);
      break;
    }	
  }
}
//----------------------------------------------------------------------------
void lhpOpUploadVME::OpDo()   
//----------------------------------------------------------------------------
{
//mafLogMessage( _T("Current working directory is: '%s' "), wxGetCwd().c_str() );
  wxBusyCursor wait;

  if(!CreateCache())
  {
    wxMessageBox("Unable to create a temporary cache, check free space on disk");
    return;
  }

  
  // m_Pid = wxExecute(command2execute, output, errors, wxEXEC_NODISABLE);
 
  if ( ExistsRunningProcess() )
  {
    wxString command2execute;
    command2execute = m_PythonwExe;
    command2execute.Append(m_PythonUploadFullPath.GetCStr());
    // script for client
    m_FileName = "Client.py ";
    command2execute.Append(m_FileName.GetCStr());
    command2execute.Append("127.0.0.1 "); //server address (localhost)
    command2execute.Append("50000 "); //port address (50000)
    command2execute.Append(wxString::Format("%d ",m_Input->GetId())); //vme id
    command2execute.Append(wxString::Format("%s ",m_CurrentCache)); //cache directory
    command2execute.Append(wxString::Format("%s ",m_Input->GetName())); //vme name
    //command2execute.Append("> log.txt"); //logme
    
    
    //wxMessageBox(wxString::Format("Process %ld is running.", m_Pid));
    wxExecute(command2execute, wxEXEC_ASYNC);
  }
  else
  {
    wxString command2execute;
    command2execute = m_PythonExe;
    command2execute.Append(m_PythonUploadFullPath.GetCStr());
    //wxMessageBox(wxString::Format("No process with pid = %ld.", m_Pid));
    m_FileName = "ThreadedClient.py ";
    command2execute.Append(m_FileName.GetCStr());
    command2execute.Append("50000");
    m_Pid = wxExecute(command2execute, wxEXEC_ASYNC);

    mafSleep(5000);

    command2execute.clear();
    command2execute = m_PythonwExe;
    command2execute.Append(m_PythonUploadFullPath.GetCStr());
    m_FileName = "Client.py ";
    command2execute.Append(m_FileName.GetCStr());
    command2execute.Append("127.0.0.1 "); //server address (localhost)
    command2execute.Append("50000 "); //port address (50000)
    command2execute.Append(wxString::Format("%d ",m_Input->GetId())); //vme id
    command2execute.Append(wxString::Format("%s ",m_CurrentCache)); //cache directory
    command2execute.Append(wxString::Format("%s ",m_Input->GetName())); //vme name

    wxExecute(command2execute, wxEXEC_ASYNC);
  }
  
  
}
//----------------------------------------------------------------------------
void lhpOpUploadVME::OpStop(int result)   
//----------------------------------------------------------------------------
{
	mafEventMacro(mafEvent(this,result));
}
//----------------------------------------------------------------------------
bool lhpOpUploadVME::CreateCache()
//----------------------------------------------------------------------------
{
  bool result = true;
  //create cache: logic comunicate the msf directory
  mafEventMacro(mafEvent(this, ID_MSF_DATA_CACHE));

  wxDir dir(m_MsfDir.GetCStr());

  if ( !dir.IsOpened() )
  {
    // deal with the error here - wxDir would already log an error message
    // explaining the exact reason of the failure
    return false;
  }

  wxString filename, filespec;
  filespec = "*.*";

  wxString filenameCopy;

  bool cont = dir.GetFirst(&filename, filespec);

  //control cache subdir
  mafString currentSubdir;
  currentSubdir = m_CacheDir + m_CacheSubdir.GetCStr();
  while(wxDirExists(currentSubdir))
  {
    int number = atoi(m_CacheSubdir.GetCStr());
    number += 1;
    m_CacheSubdir = "";
    m_CacheSubdir << number;
    currentSubdir = m_CacheDir + m_CacheSubdir.GetCStr();
  }
  currentSubdir = currentSubdir + "\\";
  wxMkDir(currentSubdir);
  while ( cont )
  {
    m_CurrentCache = currentSubdir;
    filenameCopy = currentSubdir;
    filenameCopy.Append(filename);
    
    wxString sourceFile;
    sourceFile = m_MsfDir;
    sourceFile.Append("//");
    sourceFile.Append(filename);

    if(wxFileExists(sourceFile))
      result = wxCopyFile(sourceFile, filenameCopy );
    cont = dir.GetNext(&filename);
    if(!result) break;
  }

  //wxMessageBox(m_MsfDir.GetCStr());
  return result;
  
}
//----------------------------------------------------------------------------
bool lhpOpUploadVME::ExistsRunningProcess()
//----------------------------------------------------------------------------
{
  bool result;
  
  wxFile lockFile;
  wxString lockpath = m_PythonUploadFullPath;
  lockpath += "activeLock.lhp";
  if (wxFileExists(lockpath))
  {
    result = lockFile.Open(lockpath);
  }
  
  if(result)
  {
    char processId[10];
    char *pointer;
    pointer = &processId[0];

    lockFile.Read(pointer, 10);
    long pidControl = atoi(processId);

    lockFile.Close();
    m_Pid = pidControl;

    result = wxProcess::Exists(m_Pid);

    if(!result)
    {
      wxRemoveFile(lockpath);
    }
  }
  return result;
}