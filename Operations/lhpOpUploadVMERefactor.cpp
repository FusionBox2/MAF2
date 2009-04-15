/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: lhpOpUploadVMERefactor.cpp,v $
Language:  C++
Date:      $Date: 2009-04-15 16:31:40 $
Version:   $Revision: 1.1.2.10 $
Authors:   Daniele Giunchi, Stefano Perticoni, Roberto Mucci
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

#include "lhpUtils.h"
#include <wx/process.h>
#include <wx/dir.h>
#include <wx/file.h>
#include <wx/busyinfo.h>

#include "lhpOpUploadVMERefactor.h"
#include "lhpTagHandler.h"

#include "mafGUI.h"
#include "lhpUser.h"
#include "mafNode.h"
#include "mafVMEGenericAbstract.h"
#include "mafTagArray.h"
#include "mafVMEStorage.h"
#include "mafVMERoot.h"
#include "mafVMEFactory.h"
#include "mafVMELandmarkCloud.h"
#include "mafVMELandmark.h"
#include "mafVMESurface.h"
#include "mafVMEExternalData.h"
#include "mafSmartPointer.h"

#include "lhpFactoryTagHandler.h"
#include "vtkPolyData.h"

#include <string>
#include <istream>
#include <ostream>

//----------------------------------------------------------------------------
mafCxxTypeMacro(lhpOpUploadVMERefactor);
//----------------------------------------------------------------------------
//static variables
long lhpOpUploadVMERefactor::m_Pid = -1;
mafString lhpOpUploadVMERefactor::m_CacheChildFolderLocalName = "0";

enum  m_SubdictionaryId_VALUES
{
  NO_SUBDICTIONARY = 0,
  MOTION_ANALYSIS_SUBDICTIONARY = 1,
  DICOM_SUBDICTIONARY = 2,
};

enum lhpOpUploadVME_ID
{
  ID_SUBDICTIONARY = MINID, 
};

//----------------------------------------------------------------------------
lhpOpUploadVMERefactor::lhpOpUploadVMERefactor(wxString label) :
mafOp(label)
//----------------------------------------------------------------------------
{
	m_OpType  = OPTYPE_OP;
	m_Canundo = false;
  m_HasLink = false;
  m_HasChild = false;
  m_LinkNode.clear();
  m_LinkName.clear();
  m_SubId = -1;
  m_User = NULL;

  m_PythonExe = "python.exe_UNDEFINED";
  m_PythonwExe = "pythonw.exe_UNDEFINED";  

  m_CacheMasterFolderABSName = (lhpUtils::lhpGetApplicationDirectory() + "\\VMEUploaderDownloaderRefactor\\UploadCache\\").c_str();
  m_OutgoingFolderABSName = (lhpUtils::lhpGetApplicationDirectory() + "\\VMEUploaderDownloaderRefactor\\Outgoing\\").c_str();

  m_VMEUploaderDownloaderABSFolderName  = (lhpUtils::lhpGetApplicationDirectory() + "\\VMEUploaderDownloaderRefactor\\").c_str();

  m_MsfABSFolder = "";
  m_UnhandledPlusManualTagsLocalFileName = "manualTagFile.csv";

  //m_RepositoryServiceURL = "http://devel.fec.cineca.it:12680/town/biomed_town/LHDL/users/repository/lhprepository2/";
  m_RepositoryServiceURL ="https://www.biomedtown.org/biomed_town/LHDL/users/repository/lhprepository2/";


  m_MasterXMLDictionaryFilePrefix = "lhpXMLDictionary_";
  m_MasterXMLDictionaryFileName = "UNDEFINED";
  m_SubXMLDictionaryFilePrefix = "UNDEFINED" ;
  m_SubXMLDictionaryFileName = "UNDEFINED";
  m_AssembledXMLDictionaryFileName = "assembledXMLDictionary.xml";
  m_SubDictionaryBuildingCommand = "UNDEFINED";

  m_AutoTagsListFromXMLDictionaryLocalFileName = "autoTagsList.txt";
  m_ManualTagsListFromXMLDictionaryLocalFileName = "manualTagsList.txt";
  m_HandledAutoTagsLocalFileName = "handledAutoTagsList.csv";
 

  m_HandledAutoTagsListFromFactory.Clear();
  m_UnhandledAutoTagsListFromFactory.Clear();
  m_AutoTagsList.Clear();
  m_ManualTagsList.Clear();
  
  m_SubdictionaryId = NO_SUBDICTIONARY; // default to none
  m_ConnectionConfigurationFileName = "vmeUploaderConnectionConfiguration.conf" ;

  m_ProxyURL = "";
  m_ProxyPort = "0";


  m_WithChild = false;
  m_IsLast = true;
  m_XMLUploadedResourcesRollBackLocalFileName = "m_InputXMLDataResourcesRollBackFile_UNDEFINED.txt";
  m_IsBinaryDataPresent = false;
  m_RemoteXMLResourceURI = "m_RemoteXMLResourceURI_UNDEFINED.txt";

}

//----------------------------------------------------------------------------
lhpOpUploadVMERefactor::~lhpOpUploadVMERefactor()
//----------------------------------------------------------------------------
{
  cppDEL (m_User);
}
//----------------------------------------------------------------------------
mafOp* lhpOpUploadVMERefactor::Copy()
//----------------------------------------------------------------------------
{
	/** return a copy of itself, needs to put it into the undo stack */
	return new lhpOpUploadVMERefactor(m_Label);
}
//----------------------------------------------------------------------------
bool lhpOpUploadVMERefactor::Accept(mafNode* vme)
//----------------------------------------------------------------------------
{
	return (vme != NULL && (!vme->IsMAFType(mafVMERoot)));
}
//----------------------------------------------------------------------------
void lhpOpUploadVMERefactor::OpRun()
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
    m_PythonExe = eventGetPythonExe.GetString()->GetCStr();
    m_PythonExe.Append(" ");
  }

  mafEvent eventGetPythonwExe;
  eventGetPythonwExe.SetSender(this);
  eventGetPythonwExe.SetId(ID_REQUEST_PYTHONW_EXE_INTERPRETER);
  mafEventMacro(eventGetPythonwExe);

  if(eventGetPythonwExe.GetString())
  {
    m_PythonwExe.Erase(0);
    m_PythonwExe = eventGetPythonwExe.GetString()->GetCStr();
    m_PythonwExe.Append(" ");
  }

  //Get Proxy values
  if(m_User->GetProxyFlag())
  {
    m_ProxyURL = m_User->GetProxyHost();
    m_ProxyPort = m_User->GetProxyPort();
    // load the connection configuration file:
    this->SaveConnectionConfigurationFile();
  }
  else
  {
    wxString oldDir = wxGetCwd();
    if (m_DebugMode)
      mafLogMessage( _T("Current working directory is: '%s' "), wxGetCwd().c_str() );
    wxSetWorkingDirectory(m_VMEUploaderDownloaderABSFolderName.GetCStr());
    if (m_DebugMode)
      mafLogMessage( _T("Now current working directory is: '%s' "), wxGetCwd().c_str() );

    //if file exists , delete it
    if(wxFileExists(m_ConnectionConfigurationFileName.GetCStr()))
    {
      wxRemoveFile(m_ConnectionConfigurationFileName.GetCStr());
    }

    wxSetWorkingDirectory(oldDir);
  }

  int result = OP_RUN_CANCEL;

  bool upToDate = false;

  upToDate = this->IsClientSoftwareVersionUpToDate();
  if(upToDate)
  {
    mafEventMacro(mafEvent(this, MENU_FILE_SAVE));
    this->OpStop(OP_RUN_OK);
  }
}
//------------------------------------------------------------
void lhpOpUploadVMERefactor::SaveConnectionConfigurationFile()
//------------------------------------------------------------
{
  wxString oldDir = wxGetCwd();
  if (m_DebugMode)
    mafLogMessage( _T("Current working directory is: '%s' "), wxGetCwd().c_str() );
  wxSetWorkingDirectory(m_VMEUploaderDownloaderABSFolderName.GetCStr());
  if (m_DebugMode)
    mafLogMessage( _T("Now current working directory is: '%s' "), wxGetCwd().c_str() );

  //if file exists , delete it
  if(wxFileExists(m_ConnectionConfigurationFileName.GetCStr()))
  {
    wxRemoveFile(m_ConnectionConfigurationFileName.GetCStr());
  }

  // open auto tags file and try to handle tags using tags factory 
  ofstream configurationFile;

  configurationFile.open(m_ConnectionConfigurationFileName.GetCStr());
  if (!configurationFile) {
    wxString message = m_ConnectionConfigurationFileName.GetCStr();
    message.Append(" not found! Unable to write configuration connection file");
    mafLogMessage(message.c_str());
  }
  else
  {
    configurationFile << m_ProxyURL;   
    configurationFile << "\n";
    configurationFile << m_ProxyPort;

    wxString message = m_ConnectionConfigurationFileName.GetCStr();
    message.Append("Found connection configuration file: using connection parameters");
    message.Append("m_ProxyURL: ");
    message.Append(m_ProxyURL.GetCStr());
    message.Append("m_ProxyPort: ");
    message.Append(m_ProxyPort.GetCStr());

    if (m_DebugMode)
      mafLogMessage(message.c_str());

    configurationFile.close();
  }

  wxSetWorkingDirectory(oldDir);
  if (m_DebugMode)
    mafLogMessage( _T("Current working directory is: '%s' "), wxGetCwd().c_str() );
}

int lhpOpUploadVMERefactor::Upload()
{
  mafEvent eventGetPythonExe;
  eventGetPythonExe.SetSender(this);
  eventGetPythonExe.SetId(ID_REQUEST_PYTHON_EXE_INTERPRETER);
  mafEventMacro(eventGetPythonExe);

  if(eventGetPythonExe.GetString())
  {
    m_PythonExe.Erase(0);
    m_PythonExe = eventGetPythonExe.GetString()->GetCStr();
    m_PythonExe.Append(" ");
  }

  mafEvent eventGetPythonwExe;
  eventGetPythonwExe.SetSender(this);
  eventGetPythonwExe.SetId(ID_REQUEST_PYTHONW_EXE_INTERPRETER);
  mafEventMacro(eventGetPythonwExe);

  if(eventGetPythonwExe.GetString())
  {
    m_PythonwExe.Erase(0);
    m_PythonwExe = eventGetPythonwExe.GetString()->GetCStr();
    m_PythonwExe.Append(" ");
  }

  mafSleep(3000); //to avoid DB conflicts..

  //Get User values
  mafEvent event;
  event.SetSender(this);
  event.SetId(ID_REQUEST_USER);
  mafEventMacro(event);
  if(event.GetMafObject() != NULL) //if proxy string contains something != ""
  {
    m_User = (lhpUser*)event.GetMafObject();
  }

  //check if vme has a name
  if(strcmp(m_Input->GetName(), "") == 0)
  {
    wxMessageBox("Can not upload VME without name.", wxMessageBoxCaptionStr, wxSTAY_ON_TOP | wxOK);
    return MAF_ERROR;
  }

  mafString hasLink = "false";
  mafString isLastResource = "true";
  mafString uploadWithChildren = "false";
  m_HasLink = false;

  if (m_WithChild)
  {
    uploadWithChildren = "true";
  }
  if (!m_IsLast)
  {
    isLastResource = "false";
  }  

  if (m_Input->GetNumberOfLinks() != 0)
  {
    m_HasLink = true;
    hasLink = "true";
    SaveLinkInfo();

    //remove links that will be linked again after
    m_Input->RemoveAllLinks();
    mafEventMacro(mafEvent(this, MENU_FILE_SAVE));
  }

  wxString oldDir = wxGetCwd();
  if (m_DebugMode)
    mafLogMessage( _T("Current working directory is: '%s' "), wxGetCwd().c_str() );
  wxSetWorkingDirectory(m_VMEUploaderDownloaderABSFolderName.GetCStr());

  //if already exist file with binary URI, remove it
  wxString fileName = m_Input->GetName();
  fileName << wxString::Format("%d", m_Input->GetId());
  wxString lockPath = m_VMEUploaderDownloaderABSFolderName;
  lockPath += fileName;
  if (wxFileExists(lockPath))
    wxRemoveFile(lockPath); //fileName

  //logic comunicate the msf directory
  event.SetSender(this);
  event.SetId(ID_MSF_DATA_CACHE);
  mafEventMacro(event);

  wxString temp;
  temp.Append((*event.GetString()).GetCStr());
  m_MsfABSFileName = temp;
  temp = temp.BeforeLast('/');
  m_MsfABSFolder = temp;  

  if (m_MsfABSFolder == "")
  {
    wxMessageBox("Can't edit VME tags: msf must be saved locally. Uploading stopped", wxMessageBoxCaptionStr, wxSTAY_ON_TOP | wxOK);
    return MAF_ERROR;
  }

  if(!CreateMasterCacheAndOutgoingFolders())
  {
    wxMessageBox("Unable to create Cache Base Directory. Uploading stopped", wxMessageBoxCaptionStr, wxSTAY_ON_TOP | wxOK);
    return MAF_ERROR;
  }

  if(!CreateCache())
  {
    wxMessageBox("Unable to create a temporary cache, remember that msf must be saved locally. Uploading stopped", wxMessageBoxCaptionStr, wxSTAY_ON_TOP | wxOK);
    return MAF_ERROR;
  }

  int ret = this->GeneratesTagsListsFromXMLDictionary();
  if (ret == MAF_ERROR)
  {
    wxMessageBox("Problems generating tags list! Uploading stopped", wxMessageBoxCaptionStr, wxSTAY_ON_TOP | wxOK);
    return ret;
  } 

  //If doesn't exist yet, append a TagArray:
  if (m_Input->IsA("mafVMERoot") && m_Input->GetTagArray() == NULL)
  {
    mafTagItem rootTag;
    rootTag.SetName("ROOT_TAG");
    m_Input->GetTagArray()->SetTag(rootTag);
  }

  //------Edit Tag----------------------------//
  wxString command2execute;

  if (m_DebugMode)
    command2execute = m_PythonExe.GetCStr();
  else
    command2execute = m_PythonwExe.GetCStr();
  // script for client

  mafString pythonScriptName = "lhpEditVMETag.py ";

  //workaround to understanding directory argument
  wxString msfCacheDirectoryWorkaround = m_CurrentCacheChildABSFolderName;
  msfCacheDirectoryWorkaround.Replace(" ", "??");
  
  //workaround to understanding directory argument
  wxString msfDirectoryWorkaround = m_MsfABSFolder;
  msfDirectoryWorkaround.Replace(" ", "??");

  command2execute.Append(pythonScriptName.GetCStr());
  command2execute.Append(wxString::Format("%s ",msfCacheDirectoryWorkaround)); //cache directory
  command2execute.Append(wxString::Format("%s ",msfDirectoryWorkaround)); //MSF directory
  command2execute.Append(wxString::Format("%d ",m_Input->GetId())); //vme id
  command2execute.Append(wxString::Format("%s ", m_UnhandledPlusManualTagsLocalFileName.c_str())); //manualTagFile
  command2execute.Append(wxString::Format("%s", m_HandledAutoTagsLocalFileName.GetCStr())); //autoTagFile

  if (m_DebugMode)
    mafLogMessage( _T("Executing command: '%s'"), command2execute.c_str() );
  long pid = -1;
  if (pid = wxExecute(command2execute, wxEXEC_SYNC) != 0)
  {
    wxMessageBox("Can't edit MSF. Uploading stopped!", wxMessageBoxCaptionStr, wxSTAY_ON_TOP | wxOK);
    mafLogMessage(_T("ASYNC Command process '%s' terminated with exit code %d."),
    command2execute.c_str(), pid);
    return MAF_ERROR;
  }

  //------Edit Tag----------------------------//
 
  //import msf generated by python to get tag edited  
  ImportMSF();

  mafEventMacro(mafEvent(this, MENU_FILE_SAVE));

  if(!CopyInputVMEInCache())
  {
    wxMessageBox("Unable to create a temporary cache, remember that msf must be saved locally. Uploading stopped", wxMessageBoxCaptionStr, wxSTAY_ON_TOP | wxOK);
    return MAF_ERROR;
  }

  command2execute.Clear();
  command2execute = m_PythonExe.GetCStr();
  command2execute.Append("lhpGetXMLURI.py ");
  command2execute.Append(m_User->GetName());
  command2execute.Append(" ");
  command2execute.Append(m_User->GetPwd());
  command2execute.Append(" ");
  command2execute.Append(m_RepositoryServiceURL.GetCStr());
  
  if (m_DebugMode)
    mafLogMessage( _T("Executing command: '%s'"), command2execute.c_str() );

  wxArrayString output;
  wxArrayString errors;
  pid = -1;
  if (pid = wxExecute(command2execute, output, errors, wxEXEC_SYNC) != 0)
  {
    wxMessageBox("Error in lhpGetXMLURI.py. Cannot retrieve XML resource URI from repository! Uploading stopped", wxMessageBoxCaptionStr, wxSTAY_ON_TOP | wxOK);
    if (m_DebugMode)
      mafLogMessage(_T("SYNC Command process '%s' terminated with exit code %d."),
      command2execute.c_str(), pid);
    return MAF_ERROR;
  }

  if (m_DebugMode)
  {
     mafLogMessage("Command Output Messages:");
    for (int i = 0; i < output.size(); i++)
    {
      mafLogMessage(output[i]);
    }

    mafLogMessage("Command Errors Messages:");
    for (int i = 0; i < errors.size(); i++)
    {
      mafLogMessage(errors[i]);
    }
  }

  m_RemoteXMLResourceURI = output[output.size() - 1];

  if (m_RemoteXMLResourceURI == "OverQuota")
  {
    wxMessageBox("Over Quota!. Uploading stopped.", wxMessageBoxCaptionStr, wxSTAY_ON_TOP | wxOK);
    return MAF_ERROR;
  }

  wxSetWorkingDirectory(oldDir);
  if (m_DebugMode)
    mafLogMessage( _T("Current working directory is: '%s' "), wxGetCwd().c_str());
 
  if ( ExistsRunningProcess() )
  {
    //PROCESS EXIST, ONLY CALL CLIENT
    wxString command2execute;
    if (m_DebugMode)
      command2execute = m_PythonExe.GetCStr();
    else
      command2execute = m_PythonwExe.GetCStr();
  
    pythonScriptName = "Client.py ";
    command2execute.Append(pythonScriptName.GetCStr());
    command2execute.Append("127.0.0.1 "); //server address (localhost)
    command2execute.Append("50000 "); //port address (50000)
    command2execute.Append(wxString::Format("UPLOAD ")); //UPLOAD command

    //Get the right id
    if (m_Input->IsA("mafVMELandmark"))
    {
      command2execute.Append(wxString::Format("%d ", m_CacheVme->GetFirstChild()->GetId())); //vme id
    }
    else
    {
      command2execute.Append(wxString::Format("%d ",m_CacheVme->GetId())); //vme id
    }
   
    //workaround to understanding directory argument
    wxString directoryWorkAround = m_CurrentCacheChildABSFolderName;
    directoryWorkAround.Replace(" ", "??");
    command2execute.Append(wxString::Format("%s ",directoryWorkAround)); //cache directory
    command2execute.Append(wxString::Format("%s ",m_User->GetName())); //user
    command2execute.Append(wxString::Format("%s ",m_User->GetPwd())); //pwd
    command2execute.Append(wxString::Format("%s ",m_RepositoryServiceURL.GetCStr())); //dev repository
    command2execute.Append(wxString::Format("%d ",m_Input->GetId())); //id in original tree
    command2execute.Append(wxString::Format("%s ", hasLink.GetCStr())); //has link?
    command2execute.Append(wxString::Format("%s ", uploadWithChildren.GetCStr())); //upload with children?
    command2execute.Append(wxString::Format("%s ", m_XMLUploadedResourcesRollBackLocalFileName.GetCStr())); //file to be used for rollback operation, in case of error in msf upload
    command2execute.Append(wxString::Format("%s ", m_RemoteXMLResourceURI.GetCStr())); //XML resource URI
    command2execute.Append(wxString::Format("%s ", isLastResource.GetCStr())); //true if is last VME to be uploaded
    wxString name = m_Input->GetName();
    name.Replace(" ", "??");
    command2execute.Append(wxString::Format("%s ", name.c_str())); //vme name

    m_Pid = wxExecute(command2execute, wxEXEC_ASYNC);

    if (m_DebugMode)
      mafLogMessage(_T("ASYNC Command process '%s' terminated with exit code %d."),
      command2execute.c_str(), m_Pid);
  }
  else
  {
    //PROCESS NOT EXIST, CREATE SERVER AND CALL CLIENT
   
    wxString command2execute;
    if (m_DebugMode)
      command2execute = m_PythonExe.GetCStr();
    else
      command2execute = m_PythonwExe.GetCStr();

    pythonScriptName = "ThreadedClient.py ";
    command2execute.Append(pythonScriptName.GetCStr());
    command2execute.Append("50000");
    if (m_DebugMode)
      mafLogMessage( _T("Executing command: '%s'"), command2execute.c_str() );

    m_Pid = wxExecute(command2execute, wxEXEC_ASYNC);

    if (m_DebugMode)
      mafLogMessage(_T("ASYNC Command process '%s' terminated with exit code %d."),
      command2execute.c_str(), m_Pid);

    mafSleep(5000);
    command2execute.clear();
    if (m_DebugMode)
      command2execute = m_PythonExe.GetCStr();
    else
      command2execute = m_PythonwExe.GetCStr();

    pythonScriptName = "Client.py ";
    command2execute.Append(pythonScriptName.GetCStr());
    command2execute.Append("127.0.0.1 "); //server address (localhost)
    command2execute.Append("50000 "); //port address (50000)
    command2execute.Append(wxString::Format("UPLOAD ")); //UPLOAD command

    //Get the right id
    if (m_Input->IsA("mafVMELandmark"))
    {
      command2execute.Append(wxString::Format("%d ", m_CacheVme->GetFirstChild()->GetId())); //vme id
    }
    else
    {
      command2execute.Append(wxString::Format("%d ",m_CacheVme->GetId())); //vme id
    }

    //workaround to understanding directory argument
    wxString directoryWorkAround = m_CurrentCacheChildABSFolderName;
    directoryWorkAround.Replace(" ", "??");
    command2execute.Append(wxString::Format("%s ",directoryWorkAround)); //cache directory
    command2execute.Append(wxString::Format("%s ",m_User->GetName())); //user
    command2execute.Append(wxString::Format("%s ",m_User->GetPwd())); //pwd
    command2execute.Append(wxString::Format("%s ",m_RepositoryServiceURL.GetCStr())); 
    command2execute.Append(wxString::Format("%d ",m_Input->GetId())); //id in original tree
    command2execute.Append(wxString::Format("%s ", hasLink.GetCStr())); //has link?
    command2execute.Append(wxString::Format("%s ", uploadWithChildren.GetCStr())); //upload with children?
    command2execute.Append(wxString::Format("%s ", m_XMLUploadedResourcesRollBackLocalFileName.GetCStr())); //file to be used for rollback operation, in case of error in msf upload
    command2execute.Append(wxString::Format("%s ", m_RemoteXMLResourceURI.GetCStr())); //XML resource URI
    command2execute.Append(wxString::Format("%s ", isLastResource.GetCStr())); //true if is last VME to be uploaded
    wxString name = m_Input->GetName();
    name.Replace(" ", "??");
    command2execute.Append(wxString::Format("%s ", name.c_str())); //vme name

    m_Pid = wxExecute(command2execute, wxEXEC_ASYNC);
    
    if (m_DebugMode)
      mafLogMessage(_T("ASYNC Command process '%s' terminated with exit code %d."),
        command2execute.c_str(), m_Pid);
  }


  wxSetWorkingDirectory(oldDir);


  return MAF_OK;
}
//----------------------------------------------------------------------------
void lhpOpUploadVMERefactor::OpDo()   
//----------------------------------------------------------------------------
{
  // nothing to do for the moment...
}

//-------------------------------------------------------------------
int lhpOpUploadVMERefactor::ImportMSF()
//-------------------------------------------------------------------
{
  //msf name created by phyton tag editor is standard: OutputMSF.lhp
  mafString msfPythonFileName;
  mafString msfFullPath;
  msfPythonFileName.Append(m_MsfABSFolder.GetCStr());
  msfPythonFileName.Append("/");
  msfPythonFileName.Append("OutputMSF");
  int fileNumber = 0;
  msfFullPath = msfPythonFileName;

  while(wxFileExists(msfFullPath.Append(".msf").GetCStr()))
  {
    msfFullPath = msfPythonFileName;
    msfFullPath << fileNumber;
    fileNumber++;   
  }

  msfPythonFileName.Append(".lhp");
  int result = rename(msfPythonFileName, msfFullPath);
  if ( result != 0 )
    return MAF_ERROR;

  mafVMEStorage *storage;
  storage = mafVMEStorage::New();
  storage->SetURL(msfFullPath.GetCStr());

  mafVMERoot *root;
  root = storage->GetRoot();
  root->Initialize();
  root->SetName("RootB");

  int res = storage->Restore();
  if (res != MAF_OK)
  {
    // if some problems occurred during import give feedback to the user
    if (!m_TestMode)
      mafErrorMessage(_("Errors during file parsing! Look the log area for error messages."));
    return MAF_ERROR;
  }
  if (m_Input->IsA("mafVMERoot"))
  {
    //copy tags from MSF genereted by python editor, to orginal MSF.
    m_Input->GetTagArray()->DeepCopy(root->GetTagArray());
  }
  else
  {
    mafNode *temporaryNode = root->GetFirstChild();
    if (temporaryNode != NULL)
    {
      //copy tags from MSF genereted by python editor, to orginal MSF.
      m_Input->GetTagArray()->DeepCopy(temporaryNode->GetTagArray());
    }
    mafDEL(temporaryNode);
  }

  //attach links previously removed
  if (m_HasLink)
  {
    for (int i = 0; i < m_LinkNode.size(); i++)
    {
      m_Input->SetLink(m_LinkName[i].GetCStr(), m_LinkNode[i], m_SubId);
    }
  }

  //remove msf created by phyton tag editor
  remove(msfFullPath);
  mafDEL(storage);
  return MAF_OK;
}

//----------------------------------------------------------------------------
void lhpOpUploadVMERefactor::SaveLinkInfo()   
//----------------------------------------------------------------------------
{
  m_LinkNode.clear();
  m_LinkName.clear();
  for (mafNode::mafLinksMap::iterator i = m_Input->GetLinks()->begin(); i != m_Input->GetLinks()->end(); i++)
  {
    if (i->second.m_Node != NULL)
    {
      mafNode *link = i->second.m_Node;
      if (link->IsA("mafVMELandmarkCloud") && i->second.m_NodeSubId != -1)
      {
        m_SubId = i->second.m_NodeSubId;
      }
      m_LinkNode.push_back(link);
      m_LinkName.push_back(i->first);
    }
  }
}
//----------------------------------------------------------------------------
void lhpOpUploadVMERefactor::SetDictionary(int subDictionary)   
//----------------------------------------------------------------------------
{
  m_SubdictionaryId = subDictionary;
}
//----------------------------------------------------------------------------
void lhpOpUploadVMERefactor::OpStop(int result)   
//----------------------------------------------------------------------------
{
  if(m_Gui) HideGui();
	mafEventMacro(mafEvent(this,result));
}
//----------------------------------------------------------------------------
bool lhpOpUploadVMERefactor::CreateCache()
//----------------------------------------------------------------------------
{
  bool result = false;
  //control cache subdir
  wxString currentSubdir;
  currentSubdir = m_CacheMasterFolderABSName + m_CacheChildFolderLocalName.GetCStr();
  while(wxDirExists(currentSubdir))
  {
    int number = atoi(m_CacheChildFolderLocalName.GetCStr());
    number += 1;
    m_CacheChildFolderLocalName = "";
    m_CacheChildFolderLocalName << number;
    currentSubdir = m_CacheMasterFolderABSName + m_CacheChildFolderLocalName.GetCStr();
  }
  currentSubdir = currentSubdir + "\\";
  if(wxMkDir(currentSubdir) == 0)
    result = true;

  m_CurrentCacheChildABSFolderName = currentSubdir;
  return result;
}
//----------------------------------------------------------------------------
bool lhpOpUploadVMERefactor::CopyInputVMEInCache()
//----------------------------------------------------------------------------
{
  bool copied = false;
  //If VME is ExternalData, copy the external file
  if (m_Input->IsA("mafVMEExternalData"))
  {
    wxString externalFileName = ((mafVMEExternalData *)m_Input)->GetFileName();
    externalFileName.append(".");
    externalFileName.append(((mafVMEExternalData *)m_Input)->GetExtension());
    wxString externalFilePath = ((mafVMEExternalData *)m_Input)->GetAbsoluteFileName();
    wxString externalCopiedName =  m_CurrentCacheChildABSFolderName + "\\" + externalFileName;
    if(wxFileExists(externalFilePath))
    {
      copied = wxCopyFile(externalFilePath, externalCopiedName);
    }
  }

  wxString oldDir = wxGetCwd();
  if (m_DebugMode)
    mafLogMessage( _T("Current working directory is: '%s' "), wxGetCwd().c_str() );
  wxSetWorkingDirectory(m_CurrentCacheChildABSFolderName.c_str());

  wxString msfname = m_CacheChildFolderLocalName;
  msfname.Append(".msf");

  // restore due attributes
  mafString typeVme;
  typeVme = m_Input->GetTypeName();

  if (typeVme.Equals("mafVMELandmark"))
  {
    mafSmartPointer<mafVMEFactory> factory;
    m_CacheVme = factory->CreateVMEInstance("mafVMELandmarkCloud");
    if (!m_CacheVme)
      return false;

    m_CacheVme->DeepCopy(m_Input->GetParent());
    ((mafVMELandmarkCloud *)m_CacheVme)->SetNumberOfLandmarks(0);
    mafSmartPointer<mafVMELandmark> landmark;
    landmark->DeepCopy(m_Input);
    mafVMEGenericAbstract *vmeGeneric = mafVMEGenericAbstract::SafeDownCast(m_Input);
    landmark->SetMatrix(*vmeGeneric->GetOutput()->GetAbsMatrix());
    m_CacheVme->AddChild(landmark);
  }
  else
  {
    mafSmartPointer<mafVMEFactory> factory;
    m_CacheVme = factory->CreateVMEInstance(typeVme);
    if (!m_CacheVme)
      return false;

    m_CacheVme->DeepCopy(m_Input);
    mafVMEGenericAbstract *vmeGeneric = mafVMEGenericAbstract::SafeDownCast(m_Input);
    if (vmeGeneric != NULL)
    {
      m_CacheVme->SetMatrix(*vmeGeneric->GetOutput()->GetAbsMatrix());
    }
  }

  mafSmartPointer<mafVMESurface> fakeLinkNode;
  //link a fake node, in order to have link with Id = -1. So when the VME will be downloaded
  //it will be imported even if the linked VME are still to be downloaded
  if (m_HasLink)
  {
    for (int i = 0; i < m_LinkNode.size(); i++)
    {
      m_CacheVme->SetLink(m_LinkName[i].GetCStr(), fakeLinkNode);
    }
  }

  mafVMEStorage *storage;
  storage = mafVMEStorage::New();
  //Substitute character
  storage->SetURL(msfname.c_str());

  mafVMERoot *root;
  root = storage->GetRoot();
  root->Initialize();
  root->SetName("Root");

  if (m_CacheVme->IsA("mafVMERoot"))
    root->DeepCopy(m_CacheVme);
  else
    root->AddChild(m_CacheVme);
  
  storage->Store();
  wxSetWorkingDirectory(oldDir);

  copied = true;
  return copied;
}
//----------------------------------------------------------------------------
bool lhpOpUploadVMERefactor::ExistsRunningProcess()
//----------------------------------------------------------------------------
{
  bool result = false;
  
  wxFile lockFile;
  wxString lockpath = m_VMEUploaderDownloaderABSFolderName;
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
int lhpOpUploadVMERefactor::GeneratesTagsListsFromXMLDictionary()
//----------------------------------------------------------------------------
{
  wxString oldDir = wxGetCwd();
  if (m_DebugMode)
    mafLogMessage( _T("Current working directory is: '%s' "), wxGetCwd().c_str() );
  wxSetWorkingDirectory(m_VMEUploaderDownloaderABSFolderName.GetCStr());
  if (m_DebugMode)
    mafLogMessage( _T("Now current working directory is: '%s' "), wxGetCwd().c_str() );

  m_MasterXMLDictionaryFileName = this->GetXMLDictionaryFileName(m_MasterXMLDictionaryFilePrefix);
  if (m_MasterXMLDictionaryFileName == "NOT FOUND")
  {
    return MAF_ERROR;
  }
  
  mafString dictionaryToProcessLocalFileName;

  // handle sub dictionaries creation...
  if (m_SubdictionaryId == DICOM_SUBDICTIONARY)
  {
    // build dicom
    m_SubXMLDictionaryFilePrefix = "lhpXMLDicomSourceSubdictionary_";
    m_SubDictionaryBuildingCommand = "dicom";
    if (this->AssembleDictionaries() == MAF_ERROR)
    {
      return MAF_ERROR;
    }
    dictionaryToProcessLocalFileName = m_AssembledXMLDictionaryFileName;
  } 
  else if (m_SubdictionaryId == MOTION_ANALYSIS_SUBDICTIONARY)
  {
    // build motion analysis
    m_SubXMLDictionaryFilePrefix = "lhpXMLMotionAnalysisSourceSubdictionary_";
    m_SubDictionaryBuildingCommand = "motion_analysis";
    // build sub dictionary code
    if (this->AssembleDictionaries() == MAF_ERROR)
    {
      return MAF_ERROR;
    }

    dictionaryToProcessLocalFileName = m_AssembledXMLDictionaryFileName;
  }
  else if (m_SubdictionaryId == NO_SUBDICTIONARY)
  {
    dictionaryToProcessLocalFileName = m_MasterXMLDictionaryFileName;
    // nothing to do...continue...
  }  
  else
  {
    mafLogMessage("this case is not handled...");
    return MAF_ERROR;
  }

  // get auto tags
  wxString command2execute;
  command2execute = m_PythonExe.GetCStr();
  command2execute.Append(" lhpXMLDictionaryParser.py ");
  command2execute.Append(dictionaryToProcessLocalFileName.GetCStr());
  command2execute.Append(" auto_tags ");
  command2execute.Append(m_AutoTagsListFromXMLDictionaryLocalFileName.GetCStr());
  
  if (m_DebugMode)
    mafLogMessage( _T("Executing command: '%s'"), command2execute.c_str() );

  long pid = -1;
  if (pid = wxExecute(command2execute, wxEXEC_SYNC) != 0)
  {
    wxMessageBox("Error in lhpXMLDictionaryParser.py. Uploading stopped", wxMessageBoxCaptionStr, wxSTAY_ON_TOP | wxOK);
    if (m_DebugMode)
     mafLogMessage(_T("SYNC Command process '%s' terminated with exit code %d."),
      command2execute.c_str(), pid);
    return MAF_ERROR;
  }
  if ( !command2execute )
    return MAF_ERROR;

  // get manual tags
  command2execute.Clear();
  command2execute = m_PythonExe.GetCStr();
  command2execute.Append(" lhpXMLDictionaryParser.py ");
  command2execute.Append(dictionaryToProcessLocalFileName.GetCStr());
  command2execute.Append(" manual_tags ");
  command2execute.Append(m_ManualTagsListFromXMLDictionaryLocalFileName.GetCStr());

  if (m_DebugMode)
    mafLogMessage( _T("Executing command: '%s'"), command2execute.c_str() );

  pid = -1;
  if (pid = wxExecute(command2execute, wxEXEC_SYNC) != 0)
  {
    wxMessageBox("Error in lhpXMLDictionaryParser.py. Uploading stopped", wxMessageBoxCaptionStr, wxSTAY_ON_TOP | wxOK);
    if (m_DebugMode)
      mafLogMessage(_T("SYNC Command process '%s' terminated with exit code %d."),
      command2execute.c_str(), pid);
    return MAF_ERROR;
  }
  if ( !command2execute )
    return MAF_ERROR;


  // cleanup
  m_AutoTagsList.Clear();
  m_ManualTagsList.Clear();
  m_UnhandledAutoTagsListFromFactory.Clear();

  // open auto tags file and try to handle tags using tags factory 
  ifstream inManualTagsFile;

  inManualTagsFile.open(m_ManualTagsListFromXMLDictionaryLocalFileName.GetCStr());
  if (!inManualTagsFile) {
    wxString message = m_ManualTagsListFromXMLDictionaryLocalFileName.GetCStr();
    message.Append(" not found! Unable to open XML dictionary file");
    mafLogMessage(message.c_str());
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

  inAutoTagsFile.open(m_AutoTagsListFromXMLDictionaryLocalFileName.GetCStr());
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
  
  lhpTagHandlerInputOutputParametersCargo *parametersCargo = lhpTagHandlerInputOutputParametersCargo::New();
  parametersCargo->SetInputVme(mafVME::SafeDownCast(m_Input));
  parametersCargo->SetInputUser(m_User);
	parametersCargo->SetInputMSF(m_MsfABSFileName);

  for (int i = 0; i < m_AutoTagsList.size(); i++)
  {
    tagName = m_AutoTagsList[i].c_str();

    if (tagName != "")
    {
      
      lhpFactoryTagHandler *tagsFactory  = lhpFactoryTagHandler::GetInstance();
      assert(tagsFactory!=NULL);
      lhpTagHandler *tagHandler = NULL;
      tagHandler = tagsFactory->CreateTagHandlerInstance("lhpTagHandler_" + tagName);
      
      if (tagHandler)
      {
        tagHandler->SetPythonExe(m_PythonExe.GetCStr());
        tagHandler->SetPythonwExe(m_PythonwExe.GetCStr());
        tagHandler->HandleAutoTag(parametersCargo);
        wxString tagValue = "\"";
        tagValue.Append(tagName.GetCStr());
        tagValue.Append("\",\"");
        tagValue.Append(parametersCargo->GetTagHandlerGeneratedString());
        tagValue.Append('\"');
        m_HandledAutoTagsListFromFactory.Add(tagValue.c_str());
      }
      else
      {
        m_UnhandledAutoTagsListFromFactory.Add(tagName.GetCStr());
        if (m_DebugMode)
          mafLogMessage(_("Cannot handle \"%s\" tag!, this tag will become manual"),tagName.GetCStr());
      }
    }
  }
  
  // clean up
  parametersCargo->Delete();

  // generates handled auto file
  // open auto tags file and try to handle tags using tags factory 
  ofstream handledAutoTagsFile;

  handledAutoTagsFile.open(m_HandledAutoTagsLocalFileName.GetCStr());

  for (int i = 0; i < m_HandledAutoTagsListFromFactory.size(); i++)
  {
    tagName = m_HandledAutoTagsListFromFactory[i].c_str();
    handledAutoTagsFile << tagName.GetCStr() << std::endl ;
  }
  
  handledAutoTagsFile.close();

  // generates manual tag file 
  // open auto tags file and try to handle tags using tags factory 
  ofstream unhandledPlusManualTagsFile;

  unhandledPlusManualTagsFile.open(m_CurrentCacheChildABSFolderName + m_UnhandledPlusManualTagsLocalFileName.c_str());

  if (!unhandledPlusManualTagsFile) {
    mafLogMessage("Unable to create file");
    return MAF_ERROR; // terminate with error
  }

  std::vector<std::string> tagList;
  m_Input->GetTagArray()->GetTagList(tagList);

  bool tagFound;
  mafString tagValue = "";
  // write unhandled auto
  for (int i = 0; i < m_UnhandledAutoTagsListFromFactory.size(); i++)
  {
    tagFound = false;
    tagName = m_UnhandledAutoTagsListFromFactory[i].c_str();

    for (int n = 0; n < tagList.size(); n++)
    {
      if (tagName.Equals(tagList[n].c_str()))
      {
        tagValue =  m_Input->GetTagArray()->GetTag(tagList[n].c_str())->GetValue();
        unhandledPlusManualTagsFile << "\"" << tagName.GetCStr() << "\",\"" << tagValue.GetCStr() << "\"" << std::endl ;
        tagFound = true;
        break;
      }
    }
    if (!tagFound)
      unhandledPlusManualTagsFile << "\"" << tagName.GetCStr() << "\",\"enter a value\"" << std::endl ;
  }

  // write manuals
  for (int i = 0; i < m_ManualTagsList.size(); i++)
  {
    tagFound = false;
    tagName = m_ManualTagsList[i].c_str();

    for (int n = 0; n < tagList.size(); n++)
    {
      if (tagName.Equals(tagList[n].c_str()))
      {
        tagValue =  m_Input->GetTagArray()->GetTag(tagList[n].c_str())->GetValue();
        unhandledPlusManualTagsFile << "\"" << tagName.GetCStr() << "\",\"" << tagValue.GetCStr() << "\"" << std::endl ;
        tagFound = true;
        break; 
      }
    }
    if (!tagFound)
      unhandledPlusManualTagsFile << "\"" << tagName.GetCStr() << "\",\"enter a value\"" << std::endl ;
  }
  unhandledPlusManualTagsFile.close();
  wxSetWorkingDirectory(oldDir);
  if (m_DebugMode)
    mafLogMessage( _T("Current working directory is: '%s' "), wxGetCwd().c_str() );
  return MAF_OK;
}


//----------------------------------------------------------------------------
bool lhpOpUploadVMERefactor::CreateMasterCacheAndOutgoingFolders()
//----------------------------------------------------------------------------
{
  bool cacheMasterFolderCreated = false, outgoingFolderCreated = false;

  wxString cacheMasterFolderABSName = m_CacheMasterFolderABSName.GetCStr();
  if ( wxDirExists(cacheMasterFolderABSName) )
  {
     cacheMasterFolderCreated = true;
  }

  else
  {
    wxMkDir(cacheMasterFolderABSName);
    if ( wxDirExists(cacheMasterFolderABSName) ) cacheMasterFolderCreated = true;
  }

  wxString outgoingFolderABSName = m_OutgoingFolderABSName.GetCStr();
  if ( wxDirExists(outgoingFolderABSName) )
  {
    outgoingFolderCreated = true;
  }
  else
  {
    wxMkDir(outgoingFolderABSName);
    if ( wxDirExists(outgoingFolderABSName) ) outgoingFolderCreated = true;
  }

  return cacheMasterFolderCreated && outgoingFolderCreated;
}

//----------------------------------------------------------------------------
bool lhpOpUploadVMERefactor::IsClientSoftwareVersionUpToDate()
//----------------------------------------------------------------------------
{
  wxBusyInfo("Checking if  your software is up-to-date in order to upload, please wait...");
  wxString oldDir = wxGetCwd();
  if (m_DebugMode)
    mafLogMessage( _T("Current working directory is: '%s' "), wxGetCwd().c_str() );
  wxSetWorkingDirectory(m_VMEUploaderDownloaderABSFolderName.GetCStr());
  if (m_DebugMode)
    mafLogMessage( _T("Now current working directory is: '%s' "), wxGetCwd().c_str() );

  // get manual tags
  wxString command2execute;
  command2execute.Clear();
  if (m_DebugMode)
    command2execute = m_PythonExe.GetCStr();
  else
    command2execute = m_PythonwExe.GetCStr();

  // get application name
  mafEvent eventGetApplicationName;
  eventGetApplicationName.SetSender(this);
  eventGetApplicationName.SetId(ID_REQUEST_APPLICATION_NAME);
  mafEventMacro(eventGetApplicationName);
  mafString appName = eventGetApplicationName.GetString()->GetCStr();  

  command2execute.Append(" lhpDictionaryVersionChecker.py ");
  command2execute.Append(appName.GetCStr());
  command2execute.Append(" ");
  if( !m_ProxyURL.Equals("") )
  {
    command2execute.Append(m_ProxyURL.GetCStr());
    command2execute.Append(" ");
    command2execute.Append(m_ProxyPort.GetCStr());
  }

  if (m_DebugMode)
    mafLogMessage( _T("Executing command: '%s'"), command2execute.c_str() );

  wxArrayString output;
  wxArrayString errors;
  long pid = -1;
  if (pid = wxExecute(command2execute, output, errors, wxEXEC_SYNC) != 0)
  {
    wxMessageBox("Error in lhpDictionaryVersionChecker.py. Uploading stopped", wxMessageBoxCaptionStr, wxSTAY_ON_TOP | wxOK);
    return false;
  }

  if (m_DebugMode)
  {
    mafLogMessage("Command Output Messages:");
    for (int i = 0; i < output.size(); i++)
    {
      mafLogMessage(output[i]);
    }

    mafLogMessage("Command Errors Messages:");
    for (int i = 0; i < errors.size(); i++)
    {
      mafLogMessage(errors[i]);
    }
  }

  wxString result = output[output.size() - 1];

  wxSetWorkingDirectory(oldDir);
  if (m_DebugMode)
    mafLogMessage( _T("Current working directory is: '%s' "), wxGetCwd().c_str() );

  if (result == "UpToDate")
  {
    return true;
  } 
  else
  {
    return false;
  }  
}

//----------------------------------------------------------------------------
mafString lhpOpUploadVMERefactor::GetXMLDictionaryFileName( mafString dictionaryFileNamePrefix )
//----------------------------------------------------------------------------
{
  mafString dictionaryFileName = "NOT FOUND";
  wxString oldDir = wxGetCwd();

  if (m_DebugMode)
    mafLogMessage( _T("Current working directory is: '%s' "), wxGetCwd().c_str() );
  wxSetWorkingDirectory(m_VMEUploaderDownloaderABSFolderName.GetCStr());
  if (m_DebugMode)
    mafLogMessage( _T("Now current working directory is: '%s' "), wxGetCwd().c_str() );

  wxArrayString files;
  wxString filePattern = dictionaryFileNamePrefix ;
  filePattern.Append("*.xml");

  wxDir::GetAllFiles(wxGetWorkingDirectory(), &files, filePattern, wxDIR_FILES);
  
  if (files.size() == 0)
  {
    std::ostringstream stringStream;
    mafLogMessage(stringStream.str().c_str());
    mafLogMessage("lhpXMLDictionary_*.xml not found! exiting");

    return dictionaryFileName;
  }
  if (files.size() != 1)
  {
    std::ostringstream stringStream;
    stringStream << "Found " << files.size() << " dictionaries!"  << std::endl;    
    mafLogMessage(stringStream.str().c_str());
    mafLogMessage("Too many lhpXMLDictionary_*.xml! exiting");

    return dictionaryFileName;
  }
  else
  {
    assert(files.size() == 1);
    dictionaryFileName = files[0];
    int pos = dictionaryFileName.FindLast("\\");
    dictionaryFileName.Erase(0, pos);
    if (m_DebugMode)
    {
      mafLogMessage("Found dictionary!");
      mafLogMessage(dictionaryFileName.GetCStr());
    }
  }
  
  wxSetWorkingDirectory(oldDir);
  if (m_DebugMode)
    mafLogMessage( _T("Current working directory is: '%s' "), wxGetCwd().c_str() );
  
  return dictionaryFileName;
}

//----------------------------------------------------------------------------
int lhpOpUploadVMERefactor::AssembleDictionaries()
//----------------------------------------------------------------------------
{
  wxString oldDir = wxGetCwd();
  if (m_DebugMode)
    mafLogMessage( _T("Current working directory is: '%s' "), wxGetCwd().c_str() );
  wxSetWorkingDirectory(m_VMEUploaderDownloaderABSFolderName.GetCStr());
  if (m_DebugMode)
  {
    mafLogMessage( _T("Now current working directory is: '%s' "), wxGetCwd().c_str() );
    mafLogMessage("Assembling dictionaries...");
  }

  m_SubXMLDictionaryFileName = this->GetXMLDictionaryFileName(m_SubXMLDictionaryFilePrefix);
  if (m_SubXMLDictionaryFileName == "NOT FOUND")
  {
    return MAF_ERROR;
  }

  // get manual tags
  wxString command2execute;
  command2execute.Clear();
  if (m_DebugMode)
    command2execute = m_PythonExe.GetCStr();
  else
    command2execute = m_PythonwExe.GetCStr();;

  command2execute.Append(" lhpXMLDictionariesBuilder.py ");
  command2execute.Append(m_MasterXMLDictionaryFileName);
  command2execute.Append(" ");
  command2execute.Append(m_SubXMLDictionaryFileName);
  command2execute.Append(" ");
  command2execute.Append(m_SubDictionaryBuildingCommand);
  command2execute.Append(" ");
  command2execute.Append(m_AssembledXMLDictionaryFileName);

  if (m_DebugMode)
    mafLogMessage( _T("Executing command: '%s'"), command2execute.c_str() );

  wxArrayString output;
  wxArrayString errors;
  long pid = -1;
  if (pid = wxExecute(command2execute, output, errors, wxEXEC_SYNC) != 0)
  {
    wxMessageBox("Error in lhpXMLDictionariesBuilder.py. Uploading stopped", wxMessageBoxCaptionStr, wxSTAY_ON_TOP | wxOK);
    return MAF_ERROR;
  }
  
  if (m_DebugMode)
  {
    mafLogMessage("Command Output Messages:");
    for (int i = 0; i < output.size(); i++)
    {
      mafLogMessage(output[i]);
    }

    mafLogMessage("Command Errors Messages:");
    for (int i = 0; i < errors.size(); i++)
    {
      mafLogMessage(errors[i]);
    }
  }

  wxSetWorkingDirectory(oldDir);
  if (m_DebugMode)
    mafLogMessage( _T("Current working directory is: '%s' "), wxGetCwd().c_str() );

  return MAF_OK;
}