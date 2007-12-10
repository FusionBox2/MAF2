/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: lhpOpUploadVME.cpp,v $
Language:  C++
Date:      $Date: 2007-12-10 14:45:44 $
Version:   $Revision: 1.21 $
Authors:   Daniele Giunchi, Stefano Perticoni
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
#include "mafVMEGenericAbstract.h"

#include "lhpFactoryTagHandler.h"
#include "vtkPolyData.h"

#include <string>
#include <istream>
#include <ostream>

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
  m_PythonwExe ="python.exe ";
  m_CacheDir = (mafGetApplicationDirectory() + "\\VMEUploaderDownloader\\UploadCache\\").c_str();
  m_OutgoingDir = (mafGetApplicationDirectory() + "\\VMEUploaderDownloader\\Outgoing\\").c_str();

  m_PythonUploadFullPath  = (mafGetApplicationDirectory() + "\\VMEUploaderDownloader\\").c_str();
  m_FileName = "";

  m_MsfDir = "";

  m_XMLDictionaryFileName = "lhpXMLDictionary.xml";
  m_AutoTagsListFromXMLDictionaryFileName = "autoTagsList.txt";
  m_ManualTagsListFromXMLDictionaryFileName = "manualTagsList.txt";
  m_UnhandledPlusManualTagsFileName = "unhandledPlusManualTagsList.txt";

  m_UnhandledAutoTagsListFromFactory.Clear();
  m_AutoTagsList.Clear();
  m_ManualTagsList.Clear();
  m_UnhandledAutoTagsListFromFactory.Clear();

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
	return (vme != NULL && (vme->IsMAFType(mafVMEGenericAbstract)));
}
//----------------------------------------------------------------------------
void lhpOpUploadVME::OpRun()
//----------------------------------------------------------------------------
{
  this->GeneratesManualTagsListFromXMLDictionary();
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
  wxString oldDir = wxGetCwd();
  mafLogMessage( _T("Current working directory is: '%s' "), wxGetCwd().c_str() );
  wxSetWorkingDirectory(m_PythonUploadFullPath.GetCStr());
//mafLogMessage( _T("Current working directory is: '%s' "), wxGetCwd().c_str() );
  wxBusyCursor wait;

  if(!CreateBaseCacheAndOutgoingDirectories())
  {
    wxMessageBox("Unable to create Cache Base Directory");
    return;
  }

  if(!CreateCache())
  {
    wxMessageBox("Unable to create a temporary cache, remember that msf must be saved locally");
    return;
  }

  
  // m_Pid = wxExecute(command2execute, output, errors, wxEXEC_NODISABLE);
 
  if ( ExistsRunningProcess() )
  {
    wxString command2execute;
    command2execute = m_PythonwExe;
    //command2execute.Append(m_PythonUploadFullPath.GetCStr());
    // script for client
    m_FileName = "Client.py ";
    command2execute.Append(m_FileName.GetCStr());
    command2execute.Append("127.0.0.1 "); //server address (localhost)
    command2execute.Append("50000 "); //port address (50000)
    command2execute.Append(wxString::Format("%d ",m_Input->GetId())); //vme id

    //workaround to understanding directory argument
    wxString directoryWorkAround = m_CurrentCache;
    directoryWorkAround.Replace(" ", "?");
    command2execute.Append(wxString::Format("%s ",directoryWorkAround)); //cache directory
    
    command2execute.Append(wxString::Format("%s ",m_Input->GetName())); //vme name
    //command2execute.Append(" > log.txt"); //logme
    
    
    //wxMessageBox(wxString::Format("Process %ld is running.", m_Pid));
    mafLogMessage( _T("Executing command: '%s'"), command2execute.c_str() );
    m_Pid = wxExecute(command2execute, wxEXEC_ASYNC);
    mafLogMessage(_T("ASYNC Command process '%s' terminated with exit code %d."),
      command2execute.c_str(), m_Pid);

  }
  else
  {
    wxString command2execute;
    command2execute = m_PythonwExe;
    //command2execute.Append(m_PythonUploadFullPath.GetCStr());
    //wxMessageBox(wxString::Format("No process with pid = %ld.", m_Pid));
    m_FileName = "ThreadedClient.py ";
    command2execute.Append(m_FileName.GetCStr());
    command2execute.Append("50000");
    //command2execute.Append(" > log.txt"); //logme
    mafLogMessage( _T("Executing command: '%s'"), command2execute.c_str() );

    m_Pid = wxExecute(command2execute, wxEXEC_ASYNC);
    
    mafLogMessage(_T("ASYNC Command process '%s' terminated with exit code %d."),
    command2execute.c_str(), m_Pid);

    mafSleep(5000);

    command2execute.clear();
    command2execute = m_PythonwExe;
    //command2execute.Append(m_PythonUploadFullPath.GetCStr());
    m_FileName = "Client.py ";
    command2execute.Append(m_FileName.GetCStr());
    command2execute.Append("127.0.0.1 "); //server address (localhost)
    command2execute.Append("50000 "); //port address (50000)
    command2execute.Append(wxString::Format("%d ",m_Input->GetId())); //vme id
    
    //workaround to understanding directory argument
    wxString directoryWorkAround = m_CurrentCache;
    directoryWorkAround.Replace(" ", "?");
    command2execute.Append(wxString::Format("%s ",directoryWorkAround)); //cache directory
    
    command2execute.Append(wxString::Format("%s ",m_Input->GetName())); //vme name

    mafLogMessage( _T("Executing command: '%s'"), command2execute.c_str() );
    m_Pid = wxExecute(command2execute, wxEXEC_ASYNC);
    mafLogMessage(_T("ASYNC Command process '%s' terminated with exit code %d."),
      command2execute.c_str(), m_Pid);
    
  }
  
  wxSetWorkingDirectory(oldDir);
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
  wxString exist = m_MsfDir.GetCStr();
  if ( !wxDirExists(exist) || !dir.IsOpened())
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
    sourceFile.Append("\\");
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
  bool result = false;
  
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
//----------------------------------------------------------------------------
int lhpOpUploadVME::GeneratesManualTagsListFromXMLDictionary()
//----------------------------------------------------------------------------
{
  wxString oldDir = wxGetCwd();
  mafLogMessage( _T("Current working directory is: '%s' "), wxGetCwd().c_str() );
  wxSetWorkingDirectory(m_PythonUploadFullPath.GetCStr());
  mafLogMessage( _T("Now current working directory is: '%s' "), wxGetCwd().c_str() );

  //def testRunAutoTags(self):
  //xmlDict = r'.\csv2XMLTestData\LHDL_Resources_Taxonomy_v7c.xml'
  //lhpXMLDictionaryParser.run(xmlDict,"auto_tags", "auto_tags.txt")


  //def estRunAutoTags(self):
  //xmlDict = r'.\csv2XMLTestData\LHDL_Resources_Taxonomy_v7c.xml'
  //lhpXMLDictionaryParser.run(xmlDict,"manual_tags", "manual_tags.txt")
  
  // get auto tags
  wxString command2execute;
  command2execute.Append(m_PythonExe.GetCStr());
  command2execute.Append(" lhpXMLDictionaryParser.py ");
  command2execute.Append(m_XMLDictionaryFileName.GetCStr());
  command2execute.Append(" auto_tags ");
  command2execute.Append(m_AutoTagsListFromXMLDictionaryFileName.GetCStr());
  
  mafLogMessage( _T("Executing command: '%s'"), command2execute.c_str() );

  int pid = wxExecute(command2execute, wxEXEC_SYNC);

  if ( !command2execute )
    return MAF_ERROR;

  mafLogMessage(_T("SYNC Command process '%s' terminated with exit code %d."),
    command2execute.c_str(), pid);

  // get manual tags
  command2execute.Clear();
  command2execute = m_PythonExe;
  
  command2execute.Append(" lhpXMLDictionaryParser.py ");
  command2execute.Append(m_XMLDictionaryFileName.GetCStr());
  command2execute.Append(" manual_tags ");
  command2execute.Append(m_ManualTagsListFromXMLDictionaryFileName.GetCStr());

  mafLogMessage( _T("Executing command: '%s'"), command2execute.c_str() );

  pid = wxExecute(command2execute, wxEXEC_SYNC);

  if ( !command2execute )
    return MAF_ERROR;

  mafLogMessage(_T("SYNC Command process '%s' terminated with exit code %d."),
    command2execute.c_str(), pid);

  // cleanup
  m_AutoTagsList.Clear();
  m_ManualTagsList.Clear();
  m_UnhandledAutoTagsListFromFactory.Clear();

  // open auto tags file and try to handle tags using tags factory 
  ifstream inManualTagsFile;

  inManualTagsFile.open(m_ManualTagsListFromXMLDictionaryFileName.GetCStr());
  if (!inManualTagsFile) {
    mafLogMessage("Unable to open file");
    return MAF_ERROR; // terminate with error
  }

  std::string mtag;

  while (inManualTagsFile >> mtag) 
  {
    m_ManualTagsList.Add(mtag.c_str());
  }
  inManualTagsFile.close();

  // open auto tags file and try to handle tags using tags factory 
  ifstream inAutoTagsFile;

  inAutoTagsFile.open(m_AutoTagsListFromXMLDictionaryFileName.GetCStr());
  if (!inAutoTagsFile) {
    mafLogMessage("Unable to open file");
    return MAF_ERROR; // terminate with error
  }

  std::string atag;

  while (inAutoTagsFile >> atag) 
  {
    m_AutoTagsList.Add(atag.c_str());
  }
  inAutoTagsFile.close();

  mafString tagName = "";
  
  for (int i = 0; i < m_AutoTagsList.size(); i++)
  {
    tagName = m_AutoTagsList[i].c_str();

    if (tagName != "")
    {
      lhpFactoryTagHandler *tagsFactory  = lhpFactoryTagHandler::GetInstance();
      assert(tagsFactory!=NULL);
      mafObject *obj = NULL;
      obj = tagsFactory->CreateInstance(tagName);
      lhpTagHandler *tagHandler = (lhpTagHandler*)obj;
      if (tagHandler)
      {
        tagHandler->HandleTag();
      }
      else
      {
        m_UnhandledAutoTagsListFromFactory.Add(tagName.GetCStr());
        mafLogMessage(_("Cannot handle \"%s\" tag!, this tag will become manual"),tagName.GetCStr());
      }
    }
  }
  
  // generates manual tag file 

  // open auto tags file and try to handle tags using tags factory 
  ofstream unhandledPlusManualTagsFile;

  unhandledPlusManualTagsFile.open(m_UnhandledPlusManualTagsFileName.GetCStr());

  if (!unhandledPlusManualTagsFile) {
    mafLogMessage("Unable to create file");
    return MAF_ERROR; // terminate with error
  }

  // write unhandled auto
  for (int i = 0; i < m_UnhandledAutoTagsListFromFactory.size(); i++)
  {
    tagName = m_UnhandledAutoTagsListFromFactory[i].c_str();
    unhandledPlusManualTagsFile << tagName.GetCStr() << std::endl ;
  }

  // write manuals
  for (int i = 0; i < m_ManualTagsList.size(); i++)
  {
    tagName = m_ManualTagsList[i].c_str();
    unhandledPlusManualTagsFile << tagName.GetCStr() << std::endl ;
  }

  unhandledPlusManualTagsFile.close();

  wxSetWorkingDirectory(oldDir);
  mafLogMessage( _T("Current working directory is: '%s' "), wxGetCwd().c_str() );

}
//----------------------------------------------------------------------------
bool lhpOpUploadVME::CreateBaseCacheAndOutgoingDirectories()
//----------------------------------------------------------------------------
{
  bool resultCache = false, resultOutgoing = false;

  wxString existCache = m_CacheDir.GetCStr();
  if ( wxDirExists(existCache) )
  {
     resultCache = true;
  }
  else
  {
    wxMkDir(existCache);
    if ( wxDirExists(existCache) ) resultCache = true;
  }

  wxString existOutgoing = m_OutgoingDir.GetCStr();
  if ( wxDirExists(existOutgoing) )
  {
    resultOutgoing = true;
  }
  else
  {
    wxMkDir(existOutgoing);
    if ( wxDirExists(existOutgoing) ) resultOutgoing = true;
  }

  return resultCache && resultOutgoing;
}