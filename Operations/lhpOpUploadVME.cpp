/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: lhpOpUploadVME.cpp,v $
Language:  C++
Date:      $Date: 2008-10-02 10:33:40 $
Version:   $Revision: 1.90 $
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

#include <wx/process.h>
#include <wx/dir.h>
#include <wx/file.h>
#include <wx/busyinfo.h>

#include "lhpOpUploadVME.h"

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
mafCxxTypeMacro(lhpOpUploadVME);
//----------------------------------------------------------------------------
//static variables
long lhpOpUploadVME::m_Pid = -1;
mafString lhpOpUploadVME::m_CacheSubdir = "0";
lhpUser lhpOpUploadVME::m_User = lhpUser();

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
lhpOpUploadVME::lhpOpUploadVME(wxString label) :
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

  //m_PythonExe ="C:\\Python25\\python.exe ";
  m_PythonExe ="python.exe ";
  m_PythonwExe ="pythonw.exe ";
  m_CacheDir = (mafGetApplicationDirectory() + "\\VMEUploaderDownloader\\UploadCache\\").c_str();
  m_OutgoingDir = (mafGetApplicationDirectory() + "\\VMEUploaderDownloader\\Outgoing\\").c_str();

  m_PythonUploadFullPath  = (mafGetApplicationDirectory() + "\\VMEUploaderDownloader\\").c_str();
  m_FileName = "";

  m_MsfDir = "";

  m_ServiceURL = "http://devel.fec.cineca.it:12680/town/biomed_town/LHDL/users/repository/lhprepository2/";
  //"https://www.biomedtown.org/biomed_town/LHDL/users/repository/lhprepository2/"


  m_MasterXMLDictionaryFilePrefix = "lhpXMLDictionary_";
  m_MasterXMLDictionaryFileName = "UNDEFINED";
  m_SubXMLDictionaryFilePrefix = "UNDEFINED" ;
  m_SubXMLDictionaryFileName = "UNDEFINED";
  m_AssembledXMLDictionaryFileName = "assembledXMLDictionary.xml";
  m_SubDictionaryBuildingCommand = "UNDEFINED";

  m_AutoTagsListFromXMLDictionaryFileName = "autoTagsList.txt";
  m_ManualTagsListFromXMLDictionaryFileName = "manualTagsList.txt";
  m_HandledAutoTagsFileName = "handledAutoTagsList.csv";

  m_HandledAutoTagsListFromFactory.Clear();
  m_UnhandledAutoTagsListFromFactory.Clear();
  m_AutoTagsList.Clear();
  m_ManualTagsList.Clear();
  
  m_SubdictionaryId = NO_SUBDICTIONARY; // default to none
  m_ConnectionConfigurationFileName = "vmeUploaderConnectionConfiguration.conf" ;

  m_ProxyURL = "";
  m_ProxyPort = "0";
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
	return (vme != NULL && (!vme->IsMAFType(mafVMERoot)));
}
//----------------------------------------------------------------------------
void lhpOpUploadVME::OpRun()
//----------------------------------------------------------------------------
{
  //Get Proxy values
  mafEvent event;
  event.SetSender(this);
  event.SetId(ID_REQUEST_PROXY);
  mafEventMacro(event);

  if(event.GetString()) //if proxy string contains something != ""
  {
    mafString port;
    port << event.GetArg();
    m_ProxyURL = *event.GetString();
    m_ProxyPort = port;

    // load the connection configuration file:
    this->SaveConnectionConfigurationFile();
  }
  else
  {
    wxString oldDir = wxGetCwd();
    mafLogMessage( _T("Current working directory is: '%s' "), wxGetCwd().c_str() );
    wxSetWorkingDirectory(m_PythonUploadFullPath.GetCStr());
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
  if(CheckLogin())
  {
    upToDate = this->IsLHPBuilderVersionUpToDate();
  }
  else
  {
    OpStop(result);
    return;
  }

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
  mafLogMessage( _T("Current working directory is: '%s' "), wxGetCwd().c_str() );
  wxSetWorkingDirectory(m_PythonUploadFullPath.GetCStr());
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

    mafLogMessage(message.c_str());

    configurationFile.close();
  }

  wxSetWorkingDirectory(oldDir);
  mafLogMessage( _T("Current working directory is: '%s' "), wxGetCwd().c_str() );
}

//----------------------------------------------------------------------------
int lhpOpUploadVME::UploadVME(mafString &XMLURI, bool isBinaryDataPresent, bool withChild, mafString msfListFile)   
//----------------------------------------------------------------------------
{
  //check if vme has a name
  if(strcmp(m_Input->GetName(), "") == 0)
  {
    wxMessageBox("Can not upload VME without name.");
    return MAF_ERROR;
  }

  mafString hasLink = "false";
  mafString uploadWithChild = "false";
  m_HasLink = false;

  if (withChild)
  {
    uploadWithChild = "true";
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
  mafLogMessage( _T("Current working directory is: '%s' "), wxGetCwd().c_str() );
  wxSetWorkingDirectory(m_PythonUploadFullPath.GetCStr());

  //if already exist file with binary URI, remove it
  wxString fileName = m_Input->GetName();
  fileName << wxString::Format("%d", m_Input->GetId());
  wxString lockPath = m_PythonUploadFullPath;
  lockPath += fileName;
  if (wxFileExists(lockPath))
    wxRemoveFile(lockPath); //fileName

  //logic comunicate the msf directory
  mafEvent event;
  event.SetSender(this);
  event.SetId(ID_MSF_DATA_CACHE);
  mafEventMacro(event);

  wxString temp;
  temp.Append((*event.GetString()).GetCStr());
  m_MsfFile = temp;
  temp = temp.BeforeLast('/');
  m_MsfDir = temp;  

  if (m_MsfDir == "")
  {
    wxMessageBox("Can't edit VME tags: msf must be saved locally. Uploading stopped");
    return MAF_ERROR;
  }

  int ret = this->GeneratesTagsListsFromXMLDictionary();
  if (ret == MAF_ERROR)
  {
    wxMessageBox("Problems generating tags list! Uploading stopped");
    return ret;
  } 

  //If doesn't exist yet, append a TagArray:
  if (m_Input->IsA("mafVMERoot") && m_Input->GetTagArray() == NULL)
  {
    mafTagItem rootTag;
    rootTag.SetName("ROOT_TAG");
    m_Input->GetTagArray()->SetTag(rootTag);
  }

  if(!CreateBaseCacheAndOutgoingDirectories())
  {
    wxMessageBox("Unable to create Cache Base Directory. Uploading stopped");
    return MAF_ERROR;
  }

  if(!CreateCache())
  {
    wxMessageBox("Unable to create a temporary cache, remember that msf must be saved locally. Uploading stopped");
    return MAF_ERROR;
  }

  //------Edit Tag----------------------------//
  wxString command2execute;
  command2execute = m_PythonExe;
  // script for client
  m_FileName = "lhpEditVMETag.py ";
  command2execute.Append(m_FileName.GetCStr());

  //workaround to understanding directory argument
  wxString directoryWorkAround = m_CurrentCache;
  directoryWorkAround.Replace(" ", "??");
  command2execute.Append(wxString::Format("%s ",directoryWorkAround)); //cache directory
  command2execute.Append(wxString::Format("%d ",m_CacheVme->GetId())); //vme id
  command2execute.Append(wxString::Format("%s", m_CsvName.c_str())); //manualTagFile

  //mafLogMessage( _T("Executing command: '%s'"), command2execute.c_str() );
  long pid = -1;
  if (pid = wxExecute(command2execute, wxEXEC_SYNC) != 0)
  {
    wxMessageBox("Can't edit MSF. Uploading stopped");
    //mafLogMessage(_T("ASYNC Command process '%s' terminated with exit code %d."),
    //  command2execute.c_str(), pid);
    return MAF_ERROR;
  }
  //------Edit Tag----------------------------//
 
  //import msf generated by python to get tag edited
  ImportMSF();

  //remove cache created to edit tags
  mafString filesInCache = m_CurrentCache;
  filesInCache.Append("*.*");
  wxString f = wxFindFirstFile(filesInCache.GetCStr());
  while ( !f.empty() )
  {
    wxRemoveFile(f);
    f = wxFindNextFile();
  }
   wxRmdir(m_CurrentCache.GetCStr());


  mafEventMacro(mafEvent(this, MENU_FILE_SAVE));
  if(!CreateCache())
  {
    wxMessageBox("Unable to create a temporary cache, remember that msf must be saved locally. Uploading stopped");
    return MAF_ERROR;
  }

  command2execute.Clear();
  command2execute = m_PythonExe;
  command2execute.Append("lhpGetXMLURI.py ");
  command2execute.Append(m_User.GetName());
  command2execute.Append(" ");
  command2execute.Append(m_User.GetPwd());
  command2execute.Append(" ");
  command2execute.Append(m_ServiceURL.GetCStr());
  //mafLogMessage( _T("Executing command: '%s'"), command2execute.c_str() );

  wxArrayString output;
  wxArrayString errors;
  pid = -1;
  if (pid = wxExecute(command2execute, output, errors, wxEXEC_SYNC) != 0)
  {
    wxMessageBox("Error in lhpGetXMLURI.py. Uploading stopped");
    //mafLogMessage(_T("SYNC Command process '%s' terminated with exit code %d."),
    //  command2execute.c_str(), pid);
    return MAF_ERROR;
  }

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

  XMLURI = output[output.size() - 1];

  wxSetWorkingDirectory(oldDir);
  mafLogMessage( _T("Current working directory is: '%s' "), wxGetCwd().c_str());
 
  if ( ExistsRunningProcess() )
  {
    //PROCESS EXIST, ONLY CALL CLIENT
    wxString command2execute;
    command2execute = m_PythonwExe;
    m_FileName = "Client.py ";
    command2execute.Append(m_FileName.GetCStr());
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
    wxString directoryWorkAround = m_CurrentCache;
    directoryWorkAround.Replace(" ", "??");
    command2execute.Append(wxString::Format("%s ",directoryWorkAround)); //cache directory
    command2execute.Append(wxString::Format("%s ",m_User.GetName())); //user
    command2execute.Append(wxString::Format("%s ",m_User.GetPwd())); //pwd
    command2execute.Append(wxString::Format("%s ",m_ServiceURL.GetCStr())); //dev repository
    command2execute.Append(wxString::Format("%d ",m_Input->GetId())); //id in original tree
    command2execute.Append(wxString::Format("%s ", hasLink.GetCStr())); //has link?
    command2execute.Append(wxString::Format("%s ", uploadWithChild.GetCStr())); //upload with children?
    command2execute.Append(wxString::Format("%s ", msfListFile.GetCStr())); //file to be used for rollback operation, in case of error in msf upload
    command2execute.Append(wxString::Format("%s ", XMLURI.GetCStr())); //XML resource URI
    m_NodeName.Replace(" ", "??");
    command2execute.Append(wxString::Format("%s ",m_NodeName)); //vme name

    m_Pid = wxExecute(command2execute, wxEXEC_ASYNC);

    //mafLogMessage(_T("ASYNC Command process '%s' terminated with exit code %d."),
    //command2execute.c_str(), m_Pid);
  }
  else
  {
    //PROCESS NOT EXIST, CREATE SERVER AND CALL CLIENT
    wxString command2execute;
    command2execute = m_PythonExe;
    m_FileName = "ThreadedClient.py ";
    command2execute.Append(m_FileName.GetCStr());
    command2execute.Append("50000");
    //command2execute.Append(" > log.txt"); //logme
    //mafLogMessage( _T("Executing command: '%s'"), command2execute.c_str() );

    m_Pid = wxExecute(command2execute, wxEXEC_ASYNC);

    //mafLogMessage(_T("ASYNC Command process '%s' terminated with exit code %d."),
    //  command2execute.c_str(), m_Pid);

    mafSleep(5000);
    command2execute.clear();
    command2execute = m_PythonwExe;
    m_FileName = "Client.py ";
    command2execute.Append(m_FileName.GetCStr());
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
    wxString directoryWorkAround = m_CurrentCache;
    directoryWorkAround.Replace(" ", "??");
    command2execute.Append(wxString::Format("%s ",directoryWorkAround)); //cache directory
    command2execute.Append(wxString::Format("%s ",m_User.GetName())); //user
    command2execute.Append(wxString::Format("%s ",m_User.GetPwd())); //pwd
    command2execute.Append(wxString::Format("%s ",m_ServiceURL.GetCStr())); //dev repository
    command2execute.Append(wxString::Format("%d ",m_Input->GetId())); //id in original tree
    command2execute.Append(wxString::Format("%s ", hasLink.GetCStr())); //has link?
    command2execute.Append(wxString::Format("%s ", uploadWithChild.GetCStr())); //upload with children?
    command2execute.Append(wxString::Format("%s ", msfListFile.GetCStr())); //file to be used for rollback operation, in case of error in msf upload
    command2execute.Append(wxString::Format("%s ", XMLURI.GetCStr())); //XML resource URI
    m_NodeName.Replace(" ", "??");
    command2execute.Append(wxString::Format("%s ", m_NodeName)); //vme name

    m_Pid = wxExecute(command2execute, wxEXEC_ASYNC);
    
    //mafLogMessage(_T("ASYNC Command process '%s' terminated with exit code %d."),
    //    command2execute.c_str(), m_Pid);
  }

  //remove csv file with tags
  wxRemoveFile(m_CsvName);
  wxSetWorkingDirectory(oldDir);


  return MAF_OK;
}
//----------------------------------------------------------------------------
void lhpOpUploadVME::OpDo()   
//----------------------------------------------------------------------------
{
  mafString URI;

  if (UploadVME(URI, "", false, "noMsf") == MAF_ERROR)
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
  msfPythonFileName.Append(m_CurrentCache.GetCStr());
  msfPythonFileName.Append("/");
  msfPythonFileName.Append("OutputMSF");
  int fileNumber = 0;
  msfCompletePath = msfPythonFileName;

  while(wxFileExists(msfCompletePath.Append(".msf").GetCStr()))
  {
    msfCompletePath = msfPythonFileName;
    msfCompletePath << fileNumber;
    fileNumber++;   
  }
  msfPythonFileName.Append(".lhp");
  int result = rename(msfPythonFileName, msfCompletePath);
  if ( result != 0 )
    return MAF_ERROR;

  mafVMEStorage *storage;
  storage = mafVMEStorage::New();
  storage->SetURL(msfCompletePath.GetCStr());

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

  //remove csv file
  wxString lockPath = m_PythonUploadFullPath;
  lockPath += m_CsvName.c_str();
  if (wxFileExists(lockPath))
    wxRemoveFile(lockPath); //fileName

  //remove msf created by phyton tag editor
  remove(msfCompletePath);
  mafDEL(storage);
  return MAF_OK;
}

//----------------------------------------------------------------------------
void lhpOpUploadVME::SaveLinkInfo()   
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
  bool copied = false;
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
  m_CurrentCache = currentSubdir;

  //If VME is ExternalData, copy the external file
  if (m_Input->IsA("mafVMEExternalData"))
  {
    wxString externalFileName = ((mafVMEExternalData *)m_Input)->GetFileName();
    externalFileName.append(".");
    externalFileName.append(((mafVMEExternalData *)m_Input)->GetExtension());
    wxString externalFilePath = ((mafVMEExternalData *)m_Input)->GetAbsoluteFileName();
    wxString externalCopiedName =  currentSubdir + "\\" + externalFileName;
    if(wxFileExists(externalFilePath))
    {
      copied = wxCopyFile(externalFilePath, externalCopiedName);
    }
  }

  wxString oldDir = wxGetCwd();
  mafLogMessage( _T("Current working directory is: '%s' "), wxGetCwd().c_str() );
  wxSetWorkingDirectory(currentSubdir.GetCStr());

  currentSubdir = m_Input->GetName();
  currentSubdir.Append(".msf");

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
  storage->SetURL(currentSubdir.GetCStr());

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
int lhpOpUploadVME::GeneratesTagsListsFromXMLDictionary()
//----------------------------------------------------------------------------
{
  wxString oldDir = wxGetCwd();
  mafLogMessage( _T("Current working directory is: '%s' "), wxGetCwd().c_str() );
  wxSetWorkingDirectory(m_PythonUploadFullPath.GetCStr());
  mafLogMessage( _T("Now current working directory is: '%s' "), wxGetCwd().c_str() );

  m_MasterXMLDictionaryFileName = this->GetXMLDictionaryFileName(m_MasterXMLDictionaryFilePrefix);
  if (m_MasterXMLDictionaryFileName == "NOT FOUND")
  {
    return MAF_ERROR;
  }
  
  mafString dictionaryToProcessFileName;

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
    dictionaryToProcessFileName = m_AssembledXMLDictionaryFileName;
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

    dictionaryToProcessFileName = m_AssembledXMLDictionaryFileName;
  }
  else if (m_SubdictionaryId == NO_SUBDICTIONARY)
  {
    dictionaryToProcessFileName = m_MasterXMLDictionaryFileName;
    // nothing to do...continue...
  }  
  else
  {
    mafLogMessage("this case is not handled...");
    return MAF_ERROR;
  }

  // get auto tags
  wxString command2execute;
  command2execute.Append(m_PythonExe.GetCStr());
  command2execute.Append(" lhpXMLDictionaryParser.py ");
  command2execute.Append(dictionaryToProcessFileName.GetCStr());
  command2execute.Append(" auto_tags ");
  command2execute.Append(m_AutoTagsListFromXMLDictionaryFileName.GetCStr());
  
  //mafLogMessage( _T("Executing command: '%s'"), command2execute.c_str() );

  long pid = -1;
  if (pid = wxExecute(command2execute, wxEXEC_SYNC) != 0)
  {
    wxMessageBox("Error in lhpXMLDictionaryParser.py. Uploading stopped");
   // mafLogMessage(_T("SYNC Command process '%s' terminated with exit code %d."),
   //   command2execute.c_str(), pid);
    return MAF_ERROR;
  }
  if ( !command2execute )
    return MAF_ERROR;

  // get manual tags
  command2execute.Clear();
  command2execute = m_PythonExe;
  
  command2execute.Append(" lhpXMLDictionaryParser.py ");
  command2execute.Append(dictionaryToProcessFileName.GetCStr());
  command2execute.Append(" manual_tags ");
  command2execute.Append(m_ManualTagsListFromXMLDictionaryFileName.GetCStr());

  //mafLogMessage( _T("Executing command: '%s'"), command2execute.c_str() );

  pid = -1;
  if (pid = wxExecute(command2execute, wxEXEC_SYNC) != 0)
  {
    wxMessageBox("Error in lhpXMLDictionaryParser.py. Uploading stopped");
    //mafLogMessage(_T("SYNC Command process '%s' terminated with exit code %d."),
    //  command2execute.c_str(), pid);
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

  inManualTagsFile.open(m_ManualTagsListFromXMLDictionaryFileName.GetCStr());
  if (!inManualTagsFile) {
    wxString message = m_ManualTagsListFromXMLDictionaryFileName.GetCStr();
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
  
  lhpTagHandlerInputOutputParametersCargo *parametersCargo = lhpTagHandlerInputOutputParametersCargo::New();
  parametersCargo->SetInputVme(mafVME::SafeDownCast(m_Input));
  parametersCargo->SetInputUser(&m_User);
	parametersCargo->SetInputMSF(m_MsfFile);

  for (int i = 0; i < m_AutoTagsList.size(); i++)
  {
    tagName = m_AutoTagsList[i].c_str();

    if (tagName != "")
    {
      
      lhpFactoryTagHandler *tagsFactory  = lhpFactoryTagHandler::GetInstance();
      assert(tagsFactory!=NULL);
      lhpTagHandler *obj = NULL;
      obj = tagsFactory->CreateTagHandlerInstance("lhpTagHandler_" + tagName);
      
      if (obj)
      {
        obj->HandleAutoTag(parametersCargo);
        wxString tagValue = "\"";
        tagValue.Append(tagName.GetCStr());
        tagValue.Append("\" , \"");
        tagValue.Append(parametersCargo->GetTagHandlerGeneratedString());
        tagValue.Append('\"');
        m_HandledAutoTagsListFromFactory.Add(tagValue.c_str());
      }
      else
      {
        m_UnhandledAutoTagsListFromFactory.Add(tagName.GetCStr());
        mafLogMessage(_("Cannot handle \"%s\" tag!, this tag will become manual"),tagName.GetCStr());
      }      
    }
  }
  
  // clean up
  parametersCargo->Delete();

  // generates handled auto file
  // open auto tags file and try to handle tags using tags factory 
  ofstream handledAutoTagsFile;

  handledAutoTagsFile.open(m_HandledAutoTagsFileName.GetCStr());

  for (int i = 0; i < m_HandledAutoTagsListFromFactory.size(); i++)
  {
    tagName = m_HandledAutoTagsListFromFactory[i].c_str();
    handledAutoTagsFile << tagName.GetCStr() << std::endl ;
  }
  
  handledAutoTagsFile.close();

  // generates manual tag file 
  // open auto tags file and try to handle tags using tags factory 
  ofstream unhandledPlusManualTagsFile;

  m_CsvName = m_Input->GetName();
  m_NodeName = m_CsvName;
  //m_CsvName.Replace(" ", "?"); //replace blank spaces in VME name
  m_CsvName.Replace(" ", "_");
  m_CsvName << "_id";
  m_CsvName << wxString::Format("%d",m_Input->GetId());
  m_CsvName << "_tag.csv";
  
  unhandledPlusManualTagsFile.open(m_CsvName.c_str());

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
        unhandledPlusManualTagsFile << "\"" << tagName.GetCStr() << "\" , \"" << tagValue.GetCStr() << "\"" << std::endl ;
        tagFound = true;
        break;
      }
    }
    if (!tagFound)
      unhandledPlusManualTagsFile << "\"" << tagName.GetCStr() << "\" , \"ANNOTATE ME!\"" << std::endl ;
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
        unhandledPlusManualTagsFile << "\"" << tagName.GetCStr() << "\" , \"" << tagValue.GetCStr() << "\"" << std::endl ;
        tagFound = true;
        break; 
      }
    }
    if (!tagFound)
      unhandledPlusManualTagsFile << "\"" << tagName.GetCStr() << "\" , \"ANNOTATE ME!\"" << std::endl ;
  }
  unhandledPlusManualTagsFile.close();
  wxSetWorkingDirectory(oldDir);
  mafLogMessage( _T("Current working directory is: '%s' "), wxGetCwd().c_str() );
  return MAF_OK;
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

//----------------------------------------------------------------------------
bool lhpOpUploadVME::IsLHPBuilderVersionUpToDate()
//----------------------------------------------------------------------------
{
  wxBusyInfo("Checking if  your software is up-to-date in order to upload, please wait...");
  wxString oldDir = wxGetCwd();
  mafLogMessage( _T("Current working directory is: '%s' "), wxGetCwd().c_str() );
  wxSetWorkingDirectory(m_PythonUploadFullPath.GetCStr());
  mafLogMessage( _T("Now current working directory is: '%s' "), wxGetCwd().c_str() );

  // get manual tags
  wxString command2execute;
  command2execute.Clear();
  command2execute = m_PythonwExe;

  command2execute.Append(" lhpDictionaryVersionChecker.py ");
  command2execute.Append(" ");
  if( !m_ProxyURL.Equals("") )
  {
    command2execute.Append(m_ProxyURL.GetCStr());
    command2execute.Append(" ");
    command2execute.Append(m_ProxyPort.GetCStr());
  }

  //mafLogMessage( _T("Executing command: '%s'"), command2execute.c_str() );

  wxArrayString output;
  wxArrayString errors;
  long pid = -1;
  if (pid = wxExecute(command2execute, output, errors, wxEXEC_SYNC) != 0)
  {
    wxMessageBox("Error in lhpDictionaryVersionChecker.py. Uploading stopped");
    return false;
  }

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


  wxString result = output[output.size() - 1];

  wxSetWorkingDirectory(oldDir);
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
bool lhpOpUploadVME::CheckLogin()
//----------------------------------------------------------------------------
{
  bool result = false;

  m_User.SetProxyPort(m_ProxyPort);
  m_User.SetProxyURL(m_ProxyURL);

  result = m_User.CheckUserCredentials();
  return result;
}
//----------------------------------------------------------------------------
mafString lhpOpUploadVME::GetXMLDictionaryFileName( mafString dictionaryFileNamePrefix )
//----------------------------------------------------------------------------
{
  mafString dictionaryFileName = "NOT FOUND";
  wxString oldDir = wxGetCwd();

  mafLogMessage( _T("Current working directory is: '%s' "), wxGetCwd().c_str() );
  wxSetWorkingDirectory(m_PythonUploadFullPath.GetCStr());
  mafLogMessage( _T("Now current working directory is: '%s' "), wxGetCwd().c_str() );

  wxArrayString files;
  wxString filePattern = dictionaryFileNamePrefix ;
  filePattern.Append("*.xml");

  wxDir::GetAllFiles(wxGetWorkingDirectory(), &files, filePattern);
  
  if (files.size() != 1)
  {
    mafLogMessage("lhpXMLDictionary_*.xml not found! exiting");
    return dictionaryFileName;
  }
  else
  {
    assert(files.size() == 1);
    dictionaryFileName = files[0];
    int pos = dictionaryFileName.FindLast("\\");
    dictionaryFileName.Erase(0, pos);
    mafLogMessage("Found dictionary!");
    mafLogMessage(dictionaryFileName.GetCStr());
  }
  
  wxSetWorkingDirectory(oldDir);
  mafLogMessage( _T("Current working directory is: '%s' "), wxGetCwd().c_str() );
  
  return dictionaryFileName;
}

//----------------------------------------------------------------------------
int lhpOpUploadVME::AssembleDictionaries()
//----------------------------------------------------------------------------
{
  wxString oldDir = wxGetCwd();
  mafLogMessage( _T("Current working directory is: '%s' "), wxGetCwd().c_str() );
  wxSetWorkingDirectory(m_PythonUploadFullPath.GetCStr());
  mafLogMessage( _T("Now current working directory is: '%s' "), wxGetCwd().c_str() );

  mafLogMessage("Assembling dictionaries...");

  m_SubXMLDictionaryFileName = this->GetXMLDictionaryFileName(m_SubXMLDictionaryFilePrefix);
  if (m_SubXMLDictionaryFileName == "NOT FOUND")
  {
    return MAF_ERROR;
  }

  // get manual tags
  wxString command2execute;
  command2execute.Clear();
  command2execute = m_PythonwExe;

  command2execute.Append(" lhpXMLDictionariesBuilder.py ");
  command2execute.Append(m_MasterXMLDictionaryFileName);
  command2execute.Append(" ");
  command2execute.Append(m_SubXMLDictionaryFileName);
  command2execute.Append(" ");
  command2execute.Append(m_SubDictionaryBuildingCommand);
  command2execute.Append(" ");
  command2execute.Append(m_AssembledXMLDictionaryFileName);

  //mafLogMessage( _T("Executing command: '%s'"), command2execute.c_str() );

  wxArrayString output;
  wxArrayString errors;
  long pid = -1;
  if (pid = wxExecute(command2execute, output, errors, wxEXEC_SYNC) != 0)
  {
    wxMessageBox("Error in lhpXMLDictionariesBuilder.py. Uploading stopped");
    return MAF_ERROR;
  }
  
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

  wxSetWorkingDirectory(oldDir);
  mafLogMessage( _T("Current working directory is: '%s' "), wxGetCwd().c_str() );

  return MAF_OK;
}
