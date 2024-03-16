/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: lhpOpUploadVME.cpp,v $
Language:  C++
Date:      $Date: 2009-04-10 13:50:21 $
Version:   $Revision: 1.94.2.22 $
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
#include "lhpUtils.h"
#include "lhpBuilderDecl.h"

#include <wx/process.h>
#include <wx/dir.h>
#include <wx/file.h>
#include <wx/busyinfo.h>

#include "lhpOpUploadVME.h"

#include "mafGUI.h"
#include "lhpUser.h"
#include "mafNode.h"
#include "mafNodeManager.h"
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
mafCxxTypeMacro(lhpOpUploadVME);
//----------------------------------------------------------------------------
//static variables
long lhpOpUploadVME::m_Pid = -1;
mafString lhpOpUploadVME::m_CacheSubdir = _R("0");

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
lhpOpUploadVME::lhpOpUploadVME(const mafString& label) : Superclass(label)
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

  m_PythonExe = _R("python.exe_UNDEFINED");
  m_PythonwExe = _R("pythonw.exe_UNDEFINED");  

  m_CacheDir = lhpUtils::lhpGetApplicationDirectory() + _R("\\VMEUploaderDownloader\\UploadCache\\");
  m_OutgoingDir = lhpUtils::lhpGetApplicationDirectory() + _R("\\VMEUploaderDownloader\\Outgoing\\");

  m_VMEUploaderDownloaderDir  = lhpUtils::lhpGetApplicationDirectory() + _R("\\VMEUploaderDownloader\\");
  m_FileName = _R("");

  m_MsfDir = _R("");
  m_CsvName = "manualTagFile.csv";

  //m_ServiceURL = _R("http://devel.fec.cineca.it:12680/town/biomed_town/LHDL/users/repository/lhprepository2/");
  m_ServiceURL =_R("https://www.biomedtown.org/biomed_town/LHDL/users/repository/lhprepository2/");


  m_MasterXMLDictionaryFilePrefix = _R("lhpXMLDictionary_");
  m_MasterXMLDictionaryFileName = _R("UNDEFINED");
  m_SubXMLDictionaryFilePrefix = _R("UNDEFINED") ;
  m_SubXMLDictionaryFileName = _R("UNDEFINED");
  m_AssembledXMLDictionaryFileName = _R("assembledXMLDictionary.xml");
  m_SubDictionaryBuildingCommand = _R("UNDEFINED");

  m_AutoTagsListFromXMLDictionaryFileName = _R("autoTagsList.txt");
  m_ManualTagsListFromXMLDictionaryFileName = _R("manualTagsList.txt");
  m_HandledAutoTagsFileName = _R("handledAutoTagsList.csv");

  m_HandledAutoTagsListFromFactory.Clear();
  m_UnhandledAutoTagsListFromFactory.Clear();
  m_AutoTagsList.Clear();
  m_ManualTagsList.Clear();
  
  m_SubdictionaryId = NO_SUBDICTIONARY; // default to none
  m_ConnectionConfigurationFileName = _R("vmeUploaderConnectionConfiguration.conf") ;

  m_ProxyURL = _R("");
  m_ProxyPort = _R("0");
}

//----------------------------------------------------------------------------
lhpOpUploadVME::~lhpOpUploadVME()
//----------------------------------------------------------------------------
{
  cppDEL (m_User);
}
//----------------------------------------------------------------------------
mafOp* lhpOpUploadVME::Copy()
//----------------------------------------------------------------------------
{
	/** return a copy of itself, needs to put it into the undo stack */
	return new lhpOpUploadVME(GetLabel());
}
//----------------------------------------------------------------------------
bool lhpOpUploadVME::Accept(mafNode* vme)
//----------------------------------------------------------------------------
{
	return (vme != NULL && (!vme->IsMAFType(mafVMERoot)));
}
//----------------------------------------------------------------------------
void lhpOpUploadVME::OpRun()
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

  //Get Proxy values
  if(m_User->GetProxyFlag())
  {
    m_ProxyURL = m_User->GetProxyHost();
    m_ProxyPort = mafToString(m_User->GetProxyPort());
    // load the connection configuration file:
    this->SaveConnectionConfigurationFile();
  }
  else
  {
    wxString oldDir = wxGetCwd();
    if (m_DebugMode)
      mafLogMessage(_M(_R("Current working directory is: '") + mafWxToString(wxGetCwd()) + _R("' ")));
    wxSetWorkingDirectory(m_VMEUploaderDownloaderDir.toWx());
    if (m_DebugMode)
      mafLogMessage(_M(_R("Now current working directory is: '") + mafWxToString(wxGetCwd()) + _R("' ")));

    //if file exists , delete it
    if(mafFileExists(m_ConnectionConfigurationFileName))
    {
      mafFileRemove(m_ConnectionConfigurationFileName);
    }

    wxSetWorkingDirectory(oldDir);
  }

  int result = OP_RUN_CANCEL;

  bool upToDate = false;

  upToDate = this->IsLHPBuilderVersionUpToDate();
  if(upToDate)
  {
    mafEventMacro(mafEvent(this, MENU_FILE_SAVE));
    this->OpStop(OP_RUN_OK);
  }
}
//------------------------------------------------------------
void lhpOpUploadVME::SaveConnectionConfigurationFile()
//------------------------------------------------------------
{
  wxString oldDir = wxGetCwd();
  if (m_DebugMode)
    mafLogMessage(_M(_R("Current working directory is: '") + mafWxToString(wxGetCwd()) + _R("' ")));
  wxSetWorkingDirectory(m_VMEUploaderDownloaderDir.toWx());
  if (m_DebugMode)
    mafLogMessage(_M(_R("Now current working directory is: '") + mafWxToString(wxGetCwd()) + _R("' ")));

  //if file exists , delete it
  if(mafFileExists(m_ConnectionConfigurationFileName))
  {
    mafFileRemove(m_ConnectionConfigurationFileName);
  }

  // open auto tags file and try to handle tags using tags factory 
  std::ofstream configurationFile;

  configurationFile.open(m_ConnectionConfigurationFileName.GetCStr());
  if (!configurationFile) {
    mafString message = m_ConnectionConfigurationFileName;
    message.Append(_R(" not found! Unable to write configuration connection file"));
    mafLogMessage(_M(message));
  }
  else
  {
    configurationFile << m_ProxyURL.GetCStr();
    configurationFile << "\n";
    configurationFile << m_ProxyPort.GetCStr();

    mafString message = m_ConnectionConfigurationFileName;
    message.Append(_R("Found connection configuration file: using connection parameters"));
    message.Append(_R("m_ProxyURL: "));
    message.Append(m_ProxyURL);
    message.Append(_R("m_ProxyPort: "));
    message.Append(m_ProxyPort);

    if (m_DebugMode)
      mafLogMessage(_M(message));

    configurationFile.close();
  }

  wxSetWorkingDirectory(oldDir);
  if (m_DebugMode)
    mafLogMessage(_M(_R("Current working directory is: '") + mafWxToString(wxGetCwd()) + _R("' ")));
}

//----------------------------------------------------------------------------
int lhpOpUploadVME::UploadVME(mafString &XMLURI, bool isBinaryDataPresent, bool withChild, mafString msfListFile, bool isLast)   
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
  if(strcmp(m_Input->GetName().GetCStr(), "") == 0)
  {
    wxMessageBox("Can not upload VME without name.", wxMessageBoxCaptionStr, wxSTAY_ON_TOP | wxOK);
    return MAF_ERROR;
  }

  mafString hasLink = _R("false");
  mafString isLastResource = _R("true");
  mafString uploadWithChild = _R("false");
  m_HasLink = false;

  if (withChild)
  {
    uploadWithChild = _R("true");
  }
  if (!isLast)
  {
    isLastResource = _R("false");
  }  

  
  if (m_Input->GetNumberOfLinks() != 0)
  {
    m_HasLink = true;
    hasLink = _R("true");
    SaveLinkInfo();

    //remove links that will be linked again after
    m_Input->RemoveAllLinks();
    mafEventMacro(mafEvent(this, MENU_FILE_SAVE));
  }

  wxString oldDir = wxGetCwd();
  if (m_DebugMode)
    mafLogMessage(_M(_R("Current working directory is: '") + mafWxToString(wxGetCwd()) + _R("' ")));
  wxSetWorkingDirectory(m_VMEUploaderDownloaderDir.toWx());

  //if already exist file with binary URI, remove it
  mafString fileName = m_Input->GetName();
  fileName += mafToString(m_Input->GetId());
  mafString lockPath = m_VMEUploaderDownloaderDir;
  lockPath += fileName;
  if (mafFileExists(lockPath))
    mafFileRemove(lockPath); //fileName

  //logic comunicate the msf directory
  event.SetSender(this);
  event.SetId(ID_MSF_DATA_CACHE);
  mafEventMacro(event);

  wxString temp;
  temp.Append((*event.GetString()).toWx());
  m_MsfFile = mafWxToString(temp);
  temp = temp.BeforeLast('/');
  m_MsfDir = mafWxToString(temp);

  if (m_MsfDir.IsEmpty())
  {
    wxMessageBox("Can't edit VME tags: msf must be saved locally. Uploading stopped", wxMessageBoxCaptionStr, wxSTAY_ON_TOP | wxOK);
    return MAF_ERROR;
  }

  if(!CreateBaseCacheAndOutgoingDirectories())
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
    rootTag.SetName(_R("ROOT_TAG"));
    m_Input->GetTagArray()->SetTag(rootTag);
  }

  //------Edit Tag----------------------------//
  mafString command2execute;

  if (m_DebugMode)
    command2execute = m_PythonExe;
  else
    command2execute = m_PythonwExe;
  // script for client
  m_FileName = _R("lhpEditVMETag.py ");
  command2execute.Append(m_FileName);

  //workaround to understanding directory argument
  wxString directoryWorkAround = m_CurrentCache;
  directoryWorkAround.Replace(" ", "??");
  command2execute.Append(mafWxToString(directoryWorkAround)); //cache directory
  command2execute.Append(_R(" "));

  //workaround to understanding directory argument
  directoryWorkAround = m_MsfDir.toWx();
  directoryWorkAround.Replace(" ", "??");

  command2execute.Append(mafWxToString(directoryWorkAround)); //MSF directory
  command2execute.Append(_R(" "));
  command2execute.Append(mafToString(m_Input->GetId())); //vme id
  command2execute.Append(_R(" "));
  command2execute.Append(mafWxToString(m_CsvName)); //manualTagFile

  if (m_DebugMode)
      mafLogMessage(_M(_R("Executing command: '") + command2execute + _R("'")));
  long pid = -1;
  if (pid = wxExecute(command2execute.toWx(), wxEXEC_SYNC) != 0)
  {
    wxMessageBox("Can't edit MSF. Uploading stopped", wxMessageBoxCaptionStr, wxSTAY_ON_TOP | wxOK);
    if (m_DebugMode)
        mafLogMessage(_M(_R("ASYNC Command process '") + command2execute + _R("' terminated with exit code ") + mafToString(pid) + _R(".")));
    return MAF_ERROR;
  }
  //------Edit Tag----------------------------//
 
  //import msf generated by python to get tag edited
  ImportMSF();

  mafEventMacro(mafEvent(this, MENU_FILE_SAVE));
  if(!CopyInCache())
  {
    wxMessageBox("Unable to create a temporary cache, remember that msf must be saved locally. Uploading stopped", wxMessageBoxCaptionStr, wxSTAY_ON_TOP | wxOK);
    return MAF_ERROR;
  }

  command2execute.Clear();
  command2execute = m_PythonExe;
  command2execute.Append(_R("lhpGetXMLURI.py "));
  command2execute.Append(m_User->GetName());
  command2execute.Append(_R(" "));
  command2execute.Append(m_User->GetPwd());
  command2execute.Append(_R(" "));
  command2execute.Append(m_ServiceURL);
  if (m_DebugMode)
      mafLogMessage(_M(_R("Executing command: '") + command2execute + _R("'")));

  wxArrayString output;
  wxArrayString errors;
  pid = -1;
  if (pid = wxExecute(command2execute.toWx(), output, errors, wxEXEC_SYNC) != 0)
  {
    wxMessageBox("Error in lhpGetXMLURI.py. Uploading stopped", wxMessageBoxCaptionStr, wxSTAY_ON_TOP | wxOK);
    if (m_DebugMode)
        mafLogMessage(_M(_R("SYNC Command process '") + command2execute + _R("' terminated with exit code ") + mafToString(pid) + _R(".")));
    return MAF_ERROR;
  }

  if (m_DebugMode)
  {
     mafLogMessage(_M("Command Output Messages:"));
    for (int i = 0; i < output.size(); i++)
    {
        mafLogMessage(_M(mafWxToString(output[i])));
    }

    mafLogMessage(_M("Command Errors Messages:"));
    for (int i = 0; i < errors.size(); i++)
    {
        mafLogMessage(_M(mafWxToString(errors[i])));
    }
  }

  XMLURI = mafWxToString(output[output.size() - 1]);

  if (XMLURI == _R("OverQuota"))
  {
    wxMessageBox("Over Quota!. Uploading stopped.", wxMessageBoxCaptionStr, wxSTAY_ON_TOP | wxOK);
    return MAF_ERROR;
  }

  wxSetWorkingDirectory(oldDir);
  if (m_DebugMode)
      mafLogMessage(_M(_R("Current working directory is: '") + mafWxToString(wxGetCwd()) + _R("' ")));

  if ( ExistsRunningProcess() )
  {
    //PROCESS EXIST, ONLY CALL CLIENT
    mafString command2execute;
    if (m_DebugMode)
      command2execute = m_PythonExe;
    else
      command2execute = m_PythonwExe;
  
    m_FileName = _R("Client.py ");
    command2execute.Append(m_FileName);
    command2execute.Append(_R("127.0.0.1 ")); //server address (localhost)
    command2execute.Append(_R("50000 ")); //port address (50000)
    command2execute.Append(_R("UPLOAD ")); //UPLOAD command

    //Get the right id
    if (m_Input->IsA("mafVMELandmark"))
    {
      command2execute.Append(mafToString(m_CacheVme->GetFirstChild()->GetId())); //vme id
      command2execute.Append(_R(" "));
    }
    else
    {
        command2execute.Append(mafToString(m_CacheVme->GetId())); //vme id
        command2execute.Append(_R(" "));
    }
   
    //workaround to understanding directory argument
    wxString directoryWorkAround = m_CurrentCache;
    directoryWorkAround.Replace(" ", "??");
    command2execute.Append(mafWxToString(directoryWorkAround)); //cache directory
    command2execute.Append(_R(" "));
    command2execute.Append(m_User->GetName()); //user
    command2execute.Append(_R(" "));
    command2execute.Append(m_User->GetPwd()); //pwd
    command2execute.Append(_R(" "));
    command2execute.Append(m_ServiceURL); //dev repository
    command2execute.Append(_R(" "));
    command2execute.Append(mafToString(m_Input->GetId())); //id in original tree
    command2execute.Append(_R(" "));
    command2execute.Append(hasLink); //has link?
    command2execute.Append(_R(" "));
    command2execute.Append(uploadWithChild); //upload with children?
    command2execute.Append(_R(" "));
    command2execute.Append(msfListFile); //file to be used for rollback operation, in case of error in msf upload
    command2execute.Append(_R(" "));
    command2execute.Append(XMLURI); //XML resource URI
    command2execute.Append(_R(" "));
    command2execute.Append(isLastResource); //true if is last VME to be uploaded
    command2execute.Append(_R(" "));
    wxString name = m_Input->GetName().toWx();
    name.Replace(" ", "??");
    command2execute.Append(mafWxToString(name)); //vme name
    command2execute.Append(_R(" "));

    m_Pid = wxExecute(command2execute.toWx(), wxEXEC_ASYNC);

    if (m_DebugMode)
        mafLogMessage(_M(_R("ASYNC Command process '") + command2execute + _R("' terminated with exit code ") + mafToString(m_Pid) + _R(".")));
  }
  else
  {
    //PROCESS NOT EXIST, CREATE SERVER AND CALL CLIENT
    mafString command2execute;
    if (m_DebugMode)
      command2execute = m_PythonExe;
    else
      command2execute = m_PythonwExe;

    m_FileName = _R("ThreadedClient.py ");
    command2execute.Append(m_FileName);
    command2execute.Append(_R("50000"));
    if (m_DebugMode)
        mafLogMessage(_M(_R("Executing command: '") + command2execute + _R("'")));

    m_Pid = wxExecute(command2execute.toWx(), wxEXEC_ASYNC);

    if (m_DebugMode)
        mafLogMessage(_M(_R("ASYNC Command process '") + command2execute + _R("' terminated with exit code ") + mafToString(m_Pid) + _R(".")));

    mafSleep(5000);
    command2execute.Clear();
    if (m_DebugMode)
      command2execute = m_PythonExe;
    else
      command2execute = m_PythonwExe;

    m_FileName = _R("Client.py ");
    command2execute.Append(m_FileName);
    command2execute.Append(_R("127.0.0.1 ")); //server address (localhost)
    command2execute.Append(_R("50000 ")); //port address (50000)
    command2execute.Append(_R("UPLOAD ")); //UPLOAD command

    //Get the right id
    if (m_Input->IsA("mafVMELandmark"))
    {
        command2execute.Append(mafToString(m_CacheVme->GetFirstChild()->GetId())); //vme id
        command2execute.Append(_R(" "));
    }
    else
    {
        command2execute.Append(mafToString(m_CacheVme->GetId())); //vme id
        command2execute.Append(_R(" "));
    }

    //workaround to understanding directory argument
    wxString directoryWorkAround = m_CurrentCache;
    directoryWorkAround.Replace(" ", "??");
    command2execute.Append(mafWxToString(directoryWorkAround)); //cache directory
    command2execute.Append(_R(" "));
    command2execute.Append(m_User->GetName()); //user
    command2execute.Append(_R(" "));
    command2execute.Append(m_User->GetPwd()); //pwd
    command2execute.Append(_R(" "));
    command2execute.Append(m_ServiceURL); //dev repository
    command2execute.Append(_R(" "));
    command2execute.Append(mafToString(m_Input->GetId())); //id in original tree
    command2execute.Append(_R(" "));
    command2execute.Append(hasLink); //has link?
    command2execute.Append(_R(" "));
    command2execute.Append(uploadWithChild); //upload with children?
    command2execute.Append(_R(" "));
    command2execute.Append(msfListFile); //file to be used for rollback operation, in case of error in msf upload
    command2execute.Append(_R(" "));
    command2execute.Append(XMLURI); //XML resource URI
    command2execute.Append(_R(" "));
    command2execute.Append(isLastResource); //true if is last VME to be uploaded
    command2execute.Append(_R(" "));
    wxString name = m_Input->GetName().toWx();
    name.Replace(" ", "??");
    command2execute.Append(mafWxToString(name)); //vme name
    command2execute.Append(_R(" "));

    m_Pid = wxExecute(command2execute.toWx(), wxEXEC_ASYNC);
    
    if (m_DebugMode)
        mafLogMessage(_M(_R("ASYNC Command process '") + command2execute + _R("' terminated with exit code ") + mafToString(m_Pid) + _R(".")));
  }


  wxSetWorkingDirectory(oldDir);


  return MAF_OK;
}
//----------------------------------------------------------------------------
void lhpOpUploadVME::OpDo()   
//----------------------------------------------------------------------------
{
  mafString URI;

  if (UploadVME(URI, "", false, _R("noMsf"), true) == MAF_ERROR)
  {
    return;
  }
}

//-------------------------------------------------------------------
int lhpOpUploadVME::ImportMSF()
//-------------------------------------------------------------------
{
  //msf name created by phyton tag editor is standard: OutputMSF.lhp
  mafString msfPythonFileName;
  mafString msfCompletePath;
  msfPythonFileName.Append(m_MsfDir);
  msfPythonFileName.Append(_R("/"));
  msfPythonFileName.Append(_R("OutputMSF"));
  int fileNumber = 0;
  msfCompletePath = msfPythonFileName;

  while(mafFileExists(msfCompletePath.Append(_R(".msf"))))
  {
    msfCompletePath = msfPythonFileName;
    msfCompletePath += mafToString(fileNumber);
    fileNumber++;   
  }
  msfPythonFileName.Append(_R(".lhp"));
  int result = rename(msfPythonFileName.GetCStr(), msfCompletePath.GetCStr());
  if ( result != 0 )
    return MAF_ERROR;

  mafVMEStorage *storage;
  mafNodeManager manager;
  storage = mafVMEStorage::New();
  storage->SetManager(&manager);
  storage->SetURL(msfCompletePath);

  int res = storage->Restore();
  if (res != MAF_OK)
  {
    // if some problems occurred during import give feedback to the user
    if (!m_TestMode)
      mafErrorMessage(_M(mafString(_L("Errors during file parsing! Look the log area for error messages."))));
    return MAF_ERROR;
  }
  mafVMERoot *root = mafVMERoot::SafeDownCast(manager.GetRoot());
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
      m_Input->SetLink(m_LinkName[i], m_LinkNode[i], m_SubId);
    }
  }

  //remove msf created by phyton tag editor
  remove(msfCompletePath.GetCStr());
  mafDEL(storage);
  return MAF_OK;
}

//----------------------------------------------------------------------------
void lhpOpUploadVME::SaveLinkInfo()   
//----------------------------------------------------------------------------
{
  m_LinkNode.clear();
  m_LinkName.clear();
  for (auto i = m_Input->GetLinks()->begin(); i != m_Input->GetLinks()->end(); ++i)
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
void lhpOpUploadVME::SetDictionary(int subDictionary)   
//----------------------------------------------------------------------------
{
  m_SubdictionaryId = subDictionary;
}
//----------------------------------------------------------------------------
void lhpOpUploadVME::OpStop(int result)   
//----------------------------------------------------------------------------
{
  if(m_Gui) HideGui();
	mafEventMacro(mafEvent(this,result));
}
//----------------------------------------------------------------------------
bool lhpOpUploadVME::CreateCache()
//----------------------------------------------------------------------------
{
  bool result = false;
  //control cache subdir
  mafString currentSubdir;
  currentSubdir = m_CacheDir + m_CacheSubdir;
  while(mafDirExists(currentSubdir))
  {
    int number = atoi(m_CacheSubdir.GetCStr());
    number += 1;
    m_CacheSubdir = _R("");
    m_CacheSubdir += mafToString(number);
    currentSubdir = m_CacheDir + m_CacheSubdir;
  }
  currentSubdir = currentSubdir + _R("\\");
  if(mafDirMake(currentSubdir) == 0)
    result = true;

  m_CurrentCache = currentSubdir.toWx();
  return result;
}
//----------------------------------------------------------------------------
bool lhpOpUploadVME::CopyInCache()
//----------------------------------------------------------------------------
{
  bool copied = false;
  //If VME is ExternalData, copy the external file
  if (m_Input->IsA("mafVMEExternalData"))
  {
    mafString externalFileName = _R(((mafVMEExternalData *)m_Input)->GetFileName());
    externalFileName.Append(_R("."));
    externalFileName.Append(_R(((mafVMEExternalData *)m_Input)->GetExtension()));
    mafString externalFilePath = ((mafVMEExternalData *)m_Input)->GetAbsoluteFileName();
    mafString externalCopiedName =  mafWxToString(m_CurrentCache) + _R("\\") + externalFileName;
    if(mafFileExists(externalFilePath))
    {
      copied = mafFileCopy(externalFilePath, externalCopiedName);
    }
  }

  wxString oldDir = wxGetCwd();
  if (m_DebugMode)
      mafLogMessage(_M(_R("Current working directory is: '") + mafWxToString(wxGetCwd()) + _R("' ")));
  wxSetWorkingDirectory(m_CurrentCache.c_str());

  mafString msfname = m_CacheSubdir;
  msfname.Append(_R(".msf"));

  // restore due attributes
  mafString typeVme;
  typeVme = _R(m_Input->GetTypeName());

  if (typeVme.Equals(_R("mafVMELandmark")))
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
    m_CacheVme = factory->CreateVMEInstance(typeVme.GetCStr());
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
      m_CacheVme->SetLink(m_LinkName[i], fakeLinkNode);
    }
  }

  mafVMEStorage *storage;
  mafNodeManager manager;
  storage = mafVMEStorage::New();
  //Substitute character
  storage->SetManager(&manager);
  storage->SetURL(msfname);

  mafVMERoot *root;
  mafNEW(root);
  root->Initialize();
  root->SetName(_R("Root"));
  manager.SetRoot(root);

  if (m_CacheVme->IsA("mafVMERoot"))
    root->DeepCopy(m_CacheVme);
  else
    root->AddChild(m_CacheVme);
  
  storage->Store();
  wxSetWorkingDirectory(oldDir);

  mafDEL(storage);
  mafDEL(root);
  copied = true;
  return copied;
}
//----------------------------------------------------------------------------
bool lhpOpUploadVME::ExistsRunningProcess()
//----------------------------------------------------------------------------
{
  bool result = false;
  
  wxFile lockFile;
  mafString lockpath = m_VMEUploaderDownloaderDir;
  lockpath += _R("activeLock.lhp");
  if (mafFileExists(lockpath))
  {
    result = lockFile.Open(lockpath.toWx());
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
      mafFileRemove(lockpath);
    }
  }
  return result;
}
//----------------------------------------------------------------------------
int lhpOpUploadVME::GeneratesTagsListsFromXMLDictionary()
//----------------------------------------------------------------------------
{
  wxString oldDir = wxGetCwd();
  if (m_DebugMode)
      mafLogMessage(_M(_R("Current working directory is: '") + mafWxToString(wxGetCwd()) + _R("' ")));
  wxSetWorkingDirectory(m_VMEUploaderDownloaderDir.toWx());
  if (m_DebugMode)
      mafLogMessage(_M(_R("Now current working directory is: '") + mafWxToString(wxGetCwd()) + _R("' ")));

  m_MasterXMLDictionaryFileName = this->GetXMLDictionaryFileName(m_MasterXMLDictionaryFilePrefix);
  if (m_MasterXMLDictionaryFileName == _R("NOT FOUND"))
  {
    return MAF_ERROR;
  }
  
  mafString dictionaryToProcessFileName;

  // handle sub dictionaries creation...
  if (m_SubdictionaryId == DICOM_SUBDICTIONARY)
  {
    // build dicom
    m_SubXMLDictionaryFilePrefix = _R("lhpXMLDicomSourceSubdictionary_");
    m_SubDictionaryBuildingCommand = _R("dicom");
    if (this->AssembleDictionaries() == MAF_ERROR)
    {
      return MAF_ERROR;
    }
    dictionaryToProcessFileName = m_AssembledXMLDictionaryFileName;
  } 
  else if (m_SubdictionaryId == MOTION_ANALYSIS_SUBDICTIONARY)
  {
    // build motion analysis
    m_SubXMLDictionaryFilePrefix = _R("lhpXMLMotionAnalysisSourceSubdictionary_");
    m_SubDictionaryBuildingCommand = _R("motion_analysis");
    // build sub dictionary code
    if (this->AssembleDictionaries() == MAF_ERROR)
    {
      return MAF_ERROR;
    }

    dictionaryToProcessFileName = m_AssembledXMLDictionaryFileName;
  }
  else if (m_SubdictionaryId == NO_SUBDICTIONARY)
  {
    dictionaryToProcessFileName = m_MasterXMLDictionaryFileName;
    // nothing to do...continue...
  }  
  else
  {
    mafLogMessage(_M("this case is not handled..."));
    return MAF_ERROR;
  }

  // get auto tags
  mafString command2execute;
  command2execute = m_PythonExe;
  command2execute.Append(_R(" lhpXMLDictionaryParser.py "));
  command2execute.Append(dictionaryToProcessFileName);
  command2execute.Append(_R(" auto_tags "));
  command2execute.Append(m_AutoTagsListFromXMLDictionaryFileName);
  
  if (m_DebugMode)
      mafLogMessage(_M(_R("Executing command: '") + command2execute + _R("'")));

  long pid = -1;
  if (pid = wxExecute(command2execute.toWx(), wxEXEC_SYNC) != 0)
  {
    wxMessageBox("Error in lhpXMLDictionaryParser.py. Uploading stopped", wxMessageBoxCaptionStr, wxSTAY_ON_TOP | wxOK);
    if (m_DebugMode)
        mafLogMessage(_M(_R("SYNC Command process '") + command2execute + _R("' terminated with exit code ") + mafToString(pid) + _R(".")));
    return MAF_ERROR;
  }
  if ( !command2execute.GetCStr() )
    return MAF_ERROR;

  // get manual tags
  command2execute.Clear();
  command2execute = m_PythonExe;
  command2execute.Append(_R(" lhpXMLDictionaryParser.py "));
  command2execute.Append(dictionaryToProcessFileName);
  command2execute.Append(_R(" manual_tags "));
  command2execute.Append(m_ManualTagsListFromXMLDictionaryFileName);

  if (m_DebugMode)
      mafLogMessage(_M(_R("Executing command: '") + command2execute + _R("'")));

  pid = -1;
  if (pid = wxExecute(command2execute.toWx(), wxEXEC_SYNC) != 0)
  {
    wxMessageBox("Error in lhpXMLDictionaryParser.py. Uploading stopped", wxMessageBoxCaptionStr, wxSTAY_ON_TOP | wxOK);
    if (m_DebugMode)
        mafLogMessage(_M(_R("SYNC Command process '") + command2execute + _R("' terminated with exit code ") + mafToString(pid) + _R(".")));
    return MAF_ERROR;
  }
  if ( !command2execute.GetCStr() )
    return MAF_ERROR;


  // cleanup
  m_AutoTagsList.Clear();
  m_ManualTagsList.Clear();
  m_UnhandledAutoTagsListFromFactory.Clear();

  // open auto tags file and try to handle tags using tags factory 
  std::ifstream inManualTagsFile;

  inManualTagsFile.open(m_ManualTagsListFromXMLDictionaryFileName.GetCStr());
  if (!inManualTagsFile) {
    mafString message = m_ManualTagsListFromXMLDictionaryFileName;
    message.Append(_R(" not found! Unable to open XML dictionary file"));
    mafLogMessage(_M(message));
    return MAF_ERROR; // terminate with error
  }

  std::string mtag;

  while (inManualTagsFile >> mtag) 
  {
    m_ManualTagsList.Add(mtag.c_str());
  }
  inManualTagsFile.close();

  // open auto tags file and try to handle tags using tags factory 
  std::ifstream inAutoTagsFile;

  inAutoTagsFile.open(m_AutoTagsListFromXMLDictionaryFileName.GetCStr());
  if (!inAutoTagsFile) {
    mafLogMessage(_M("Unable to open file"));
    return MAF_ERROR; // terminate with error
  }

  std::string atag;
  while (inAutoTagsFile >> atag) 
  {
    m_AutoTagsList.Add(atag.c_str());
  }
  inAutoTagsFile.close();
  mafString tagName = _R("");
  
  lhpTagHandlerInputOutputParametersCargo *parametersCargo = lhpTagHandlerInputOutputParametersCargo::New();
  parametersCargo->SetInputVme(mafVME::SafeDownCast(m_Input));
  parametersCargo->SetInputUser(m_User);
	parametersCargo->SetInputMSF(m_MsfFile.GetCStr());

  for (int i = 0; i < m_AutoTagsList.size(); i++)
  {
    tagName = mafWxToString(m_AutoTagsList[i]);

    if (!tagName.IsEmpty())
    {
      
      lhpFactoryTagHandler *tagsFactory  = lhpFactoryTagHandler::GetInstance();
      assert(tagsFactory!=NULL);
      lhpTagHandler *tagHandler = NULL;
      tagHandler = tagsFactory->CreateTagHandlerInstance((_R("lhpTagHandler_") + tagName).GetCStr());
      
      if (tagHandler)
      {
        tagHandler->SetPythonExe(m_PythonExe.GetCStr());
        tagHandler->SetPythonwExe(m_PythonwExe.GetCStr());
        tagHandler->HandleAutoTag(parametersCargo);
        wxString tagValue = "\"";
        tagValue.Append(tagName.GetCStr());
        tagValue.Append("\",\"");
        tagValue.Append(parametersCargo->GetTagHandlerGeneratedString().toWx());
        tagValue.Append('\"');
        m_HandledAutoTagsListFromFactory.Add(tagValue);
      }
      else
      {
        m_UnhandledAutoTagsListFromFactory.Add(tagName.toWx());
        if (m_DebugMode)
          mafLogMessage(_M(_L("Cannot handle \"") + tagName + _L("\" tag!, this tag will become manual")));
      }
    }
  }
  
  // clean up
  parametersCargo->Delete();

  // generates handled auto file
  // open auto tags file and try to handle tags using tags factory 
  std::ofstream handledAutoTagsFile;

  handledAutoTagsFile.open(m_HandledAutoTagsFileName.GetCStr());

  for (int i = 0; i < m_HandledAutoTagsListFromFactory.size(); i++)
  {
    tagName = mafWxToString(m_HandledAutoTagsListFromFactory[i]);
    handledAutoTagsFile << tagName.GetCStr() << std::endl ;
  }
  
  handledAutoTagsFile.close();

  // generates manual tag file 
  // open auto tags file and try to handle tags using tags factory 
  std::ofstream unhandledPlusManualTagsFile;

  unhandledPlusManualTagsFile.open(mafWxToString(m_CurrentCache + m_CsvName).toStd());

  if (!unhandledPlusManualTagsFile) {
    mafLogMessage(_M("Unable to create file"));
    return MAF_ERROR; // terminate with error
  }

  std::vector<mafString> tagList;
  m_Input->GetTagArray()->GetTagList(tagList);

  bool tagFound;
  mafString tagValue = _R("");
  // write unhandled auto
  for (int i = 0; i < m_UnhandledAutoTagsListFromFactory.size(); i++)
  {
    tagFound = false;
    tagName = mafWxToString(m_UnhandledAutoTagsListFromFactory[i]);

    for (int n = 0; n < tagList.size(); n++)
    {
      if (tagName.Equals(tagList[n]))
      {
        tagValue =  m_Input->GetTagArray()->GetTag(tagList[n])->GetValue();
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
    tagName = mafWxToString(m_ManualTagsList[i]);

    for (int n = 0; n < tagList.size(); n++)
    {
      if (tagName.Equals(tagList[n]))
      {
        tagValue =  m_Input->GetTagArray()->GetTag(tagList[n])->GetValue();
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
      mafLogMessage(_M(_R("Current working directory is: '") + mafWxToString(wxGetCwd()) + _R("' ")));
  return MAF_OK;
}


//----------------------------------------------------------------------------
bool lhpOpUploadVME::CreateBaseCacheAndOutgoingDirectories()
//----------------------------------------------------------------------------
{
  bool resultCache = false, resultOutgoing = false;

  mafString existCache = m_CacheDir;
  if ( mafDirExists(existCache) )
  {
     resultCache = true;
  }
  else
  {
    mafDirMake(existCache);
    if ( mafDirExists(existCache) ) resultCache = true;
  }

  mafString existOutgoing = m_OutgoingDir;
  if ( mafDirExists(existOutgoing) )
  {
    resultOutgoing = true;
  }
  else
  {
    mafDirMake(existOutgoing);
    if ( mafDirExists(existOutgoing) ) resultOutgoing = true;
  }

  return resultCache && resultOutgoing;
}

//----------------------------------------------------------------------------
bool lhpOpUploadVME::IsLHPBuilderVersionUpToDate()
//----------------------------------------------------------------------------
{
  wxBusyInfo("Checking if  your software is up-to-date in order to upload, please wait...");
  wxString oldDir = wxGetCwd();
  if (m_DebugMode)
      mafLogMessage(_M(_R("Current working directory is: '") + mafWxToString(wxGetCwd()) + _R("' ")));
  wxSetWorkingDirectory(m_VMEUploaderDownloaderDir.toWx());
  if (m_DebugMode)
      mafLogMessage(_M(_R("Now current working directory is: '") + mafWxToString(wxGetCwd()) + _R("' ")));

  // get manual tags
  mafString command2execute;
  command2execute.Clear();
  if (m_DebugMode)
    command2execute = m_PythonExe;
  else
    command2execute = m_PythonwExe;

  // get application name
  mafEvent eventGetApplicationName;
  eventGetApplicationName.SetSender(this);
  eventGetApplicationName.SetId(ID_REQUEST_APPLICATION_NAME);
  mafEventMacro(eventGetApplicationName);
  mafString appName = *eventGetApplicationName.GetString();  

  command2execute.Append(_R(" lhpDictionaryVersionChecker.py "));
  command2execute.Append(appName);
  command2execute.Append(_R(" "));
  if( !m_ProxyURL.IsEmpty() )
  {
    command2execute.Append(m_ProxyURL);
    command2execute.Append(_R(" "));
    command2execute.Append(m_ProxyPort);
  }

  if (m_DebugMode)
      mafLogMessage(_M(_R("Executing command: '") + command2execute + _R("'")));

  wxArrayString output;
  wxArrayString errors;
  long pid = -1;
  if (pid = wxExecute(command2execute.toWx(), output, errors, wxEXEC_SYNC) != 0)
  {
    wxMessageBox("Error in lhpDictionaryVersionChecker.py. Uploading stopped", wxMessageBoxCaptionStr, wxSTAY_ON_TOP | wxOK);
    return false;
  }

  if (m_DebugMode)
  {
    mafLogMessage(_M("Command Output Messages:"));
    for (int i = 0; i < output.size(); i++)
    {
        mafLogMessage(_M(mafWxToString(output[i])));
    }

    mafLogMessage(_M("Command Errors Messages:"));
    for (int i = 0; i < errors.size(); i++)
    {
        mafLogMessage(_M(mafWxToString(errors[i])));
    }
  }

  wxString result = output[output.size() - 1];

  wxSetWorkingDirectory(oldDir);
  if (m_DebugMode)
      mafLogMessage(_M(_R("Current working directory is: '") + mafWxToString(wxGetCwd()) + _R("' ")));

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
mafString lhpOpUploadVME::GetXMLDictionaryFileName( mafString dictionaryFileNamePrefix )
//----------------------------------------------------------------------------
{
  mafString dictionaryFileName = _R("NOT FOUND");
  wxString oldDir = wxGetCwd();

  if (m_DebugMode)
      mafLogMessage(_M(_R("Current working directory is: '") + mafWxToString(wxGetCwd()) + _R("' ")));
  wxSetWorkingDirectory(m_VMEUploaderDownloaderDir.toWx());
  if (m_DebugMode)
      mafLogMessage(_M(_R("Now current working directory is: '") + mafWxToString(wxGetCwd()) + _R("' ")));

  wxArrayString files;
  wxString filePattern = dictionaryFileNamePrefix.toWx();
  filePattern.Append("*.xml");

  wxDir::GetAllFiles(wxGetCwd(), &files, filePattern, wxDIR_FILES);
  
  if (files.size() == 0)
  {
    std::ostringstream stringStream;
    mafLogMessage(_M(stringStream.str().c_str()));
    mafLogMessage(_M("lhpXMLDictionary_*.xml not found! exiting"));

    return dictionaryFileName;
  }
  if (files.size() != 1)
  {
    std::ostringstream stringStream;
    stringStream << "Found " << files.size() << " dictionaries!"  << std::endl;    
    mafLogMessage(_M(stringStream.str().c_str()));
    mafLogMessage(_M("Too many lhpXMLDictionary_*.xml! exiting"));

    return dictionaryFileName;
  }
  else
  {
    assert(files.size() == 1);
    dictionaryFileName = mafWxToString(files[0]);
    int pos = dictionaryFileName.FindLast(_R("\\"));
    dictionaryFileName.Erase(0, pos);
    if (m_DebugMode)
    {
      mafLogMessage(_M("Found dictionary!"));
      mafLogMessage(_M(dictionaryFileName));
    }
  }
  
  wxSetWorkingDirectory(oldDir);
  if (m_DebugMode)
      mafLogMessage(_M(_R("Current working directory is: '") + mafWxToString(wxGetCwd()) + _R("' ")));

  return dictionaryFileName;
}

//----------------------------------------------------------------------------
int lhpOpUploadVME::AssembleDictionaries()
//----------------------------------------------------------------------------
{
  wxString oldDir = wxGetCwd();
  if (m_DebugMode)
      mafLogMessage(_M(_R("Current working directory is: '") + mafWxToString(wxGetCwd()) + _R("' ")));
  wxSetWorkingDirectory(m_VMEUploaderDownloaderDir.toWx());
  if (m_DebugMode)
  {
      mafLogMessage(_M(_R("Now current working directory is: '") + mafWxToString(wxGetCwd()) + _R("' ")));
      mafLogMessage(_M("Assembling dictionaries..."));
  }

  m_SubXMLDictionaryFileName = this->GetXMLDictionaryFileName(m_SubXMLDictionaryFilePrefix);
  if (m_SubXMLDictionaryFileName == _R("NOT FOUND"))
  {
    return MAF_ERROR;
  }

  // get manual tags
  mafString command2execute;
  command2execute.Clear();
  if (m_DebugMode)
    command2execute = m_PythonExe;
  else
    command2execute = m_PythonwExe;

  command2execute.Append(_R(" lhpXMLDictionariesBuilder.py "));
  command2execute.Append(m_MasterXMLDictionaryFileName);
  command2execute.Append(_R(" "));
  command2execute.Append(m_SubXMLDictionaryFileName);
  command2execute.Append(_R(" "));
  command2execute.Append(m_SubDictionaryBuildingCommand);
  command2execute.Append(_R(" "));
  command2execute.Append(m_AssembledXMLDictionaryFileName);

  if (m_DebugMode)
      mafLogMessage(_M(_R("Executing command: '") + command2execute + _R("'")));

  wxArrayString output;
  wxArrayString errors;
  long pid = -1;
  if (pid = wxExecute(command2execute.toWx(), output, errors, wxEXEC_SYNC) != 0)
  {
    wxMessageBox("Error in lhpXMLDictionariesBuilder.py. Uploading stopped", wxMessageBoxCaptionStr, wxSTAY_ON_TOP | wxOK);
    return MAF_ERROR;
  }
  
  if (m_DebugMode)
  {
    mafLogMessage(_M("Command Output Messages:"));
    for (int i = 0; i < output.size(); i++)
    {
        mafLogMessage(_M(mafWxToString(output[i])));
    }

    mafLogMessage(_M("Command Errors Messages:"));
    for (int i = 0; i < errors.size(); i++)
    {
        mafLogMessage(_M(mafWxToString(errors[i])));
    }
  }

  wxSetWorkingDirectory(oldDir);
  if (m_DebugMode)
      mafLogMessage(_M(_R("Current working directory is: '") + mafWxToString(wxGetCwd()) + _R("' ")));

  return MAF_OK;
}