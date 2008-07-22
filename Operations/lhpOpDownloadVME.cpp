/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: lhpOpDownloadVME.cpp,v $
Language:  C++
Date:      $Date: 2008-07-22 12:54:15 $
Version:   $Revision: 1.23 $
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

#include "lhpOpDownloadVME.h"

#include "mmgGui.h"
#include "lhpUser.h"
#include "mafNode.h"
#include "mafVMEGenericAbstract.h"
#include "mafOpImporterMSF.h"
#include "mafVMELandmarkCloud.h"
#include "mafTagArray.h"

#include "mafVMEStorage.h"
#include "mafVMERoot.h"
#include "mafVMEGroup.h"


#include "lhpFactoryTagHandler.h"
#include "vtkPolyData.h"

#include <string>
#include <istream>
#include <ostream>

//----------------------------------------------------------------------------
mafCxxTypeMacro(lhpOpDownloadVME);
//----------------------------------------------------------------------------

//static variables
long lhpOpDownloadVME::m_Pid = -1;
mafString lhpOpDownloadVME::m_CacheSubdir = "0";
lhpUser lhpOpDownloadVME::m_User = lhpUser();

enum lhpOpDownloadVME_ID
{
  ID_TEST = MINID, 
};

//----------------------------------------------------------------------------
lhpOpDownloadVME::lhpOpDownloadVME(wxString label) :
mafOp(label)
//----------------------------------------------------------------------------
{
	m_OpType  = OPTYPE_OP;
	m_Canundo = false;
  m_FillLinkVector = false;
  m_DerivedNodeVector.clear();
  m_LinkNodeVector.clear();
  m_Group = NULL;

  //m_PythonExe ="C:\\Python25\\python.exe ";
  m_PythonExe ="python.exe ";
  m_PythonwExe ="pythonw.exe ";
  m_CurrentCache = m_CacheDir = (mafGetApplicationDirectory() + "\\VMEUploaderDownloader\\UploadCache\\").c_str();
  m_IncomingDir = (mafGetApplicationDirectory() + "\\VMEUploaderDownloader\\Incoming\\").c_str();

  m_PythonUploadFullPath  = (mafGetApplicationDirectory() + "\\VMEUploaderDownloader\\").c_str();
  m_FileName = "";

  m_MsfDir = "";
  m_IncomingCompletePath = "";
  
  m_BasketListFileName = "ToDownload.txt" ;
  m_ConnectionConfigurationFileName = "vmeUploaderConnectionConfiguration.conf" ;

  m_BinaryRealName = "";

  m_URISRBFile = "";
  m_URISRBFileSize = "0";

  m_ProxyURL = "";
  m_ProxyPort = "0";
}

//----------------------------------------------------------------------------
lhpOpDownloadVME::~lhpOpDownloadVME()
//----------------------------------------------------------------------------
{
  m_DerivedNodeVector.clear();
  m_LinkNodeVector.clear();
  mafDEL(m_Group);
}
//----------------------------------------------------------------------------
mafOp* lhpOpDownloadVME::Copy()
//----------------------------------------------------------------------------
{
	/** return a copy of itself, needs to put it into the undo stack */
	return new lhpOpDownloadVME(m_Label);
}
//----------------------------------------------------------------------------
bool lhpOpDownloadVME::Accept(mafNode* vme)
//----------------------------------------------------------------------------
{
	return (vme != NULL);
}
//----------------------------------------------------------------------------
void lhpOpDownloadVME::OpRun()
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
    if(this->CreateFileListFromBasket() == MAF_OK)
    {
      result = OP_RUN_OK;
      mafEventMacro(mafEvent(this,result));
    }
    else
    {
      OpStop(result);
      return;
    }
  }
  else
  {
    OpStop(result);
    return;
  }

}
//------------------------------------------------------------
void lhpOpDownloadVME::SaveConnectionConfigurationFile()
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
void lhpOpDownloadVME::OnEvent(mafEventBase *maf_event) 
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {
    case wxOK:
      {
        this->OpStop(OP_RUN_OK);
        return;
      }
      break;

    case wxCANCEL:
      {
        
        this->OpStop(OP_RUN_CANCEL);
        return;
      }
      break;

    default:
      mafEventMacro(*e);
      break;
    }	
  }
}
//----------------------------------------------------------------------------
void lhpOpDownloadVME::OpDo()   
//----------------------------------------------------------------------------
{
  if(RetrieveInformationFromBasketListFile() != MAF_OK)
  {
    wxMessageBox("Unable to read what are the choosen vme");
    return;
  }
  //Download VME form the basket
  if (DownloadVME(m_BasketList) != MAF_OK)
  {
    return;
  }

  if (m_ListLinkURI.size() != 0)
  {
    m_FillLinkVector = true;

    //Download VME link
    if (DownloadVME(m_ListLinkURI) != MAF_OK)
    {
      return;
    }
    int counter = 0;
    for (int n = 0; n < m_DerivedNodeVector.size(); n++)
    {
      int subId = -1;
      mafString linkName;
      for (mafNode::mafLinksMap::iterator i = m_DerivedNodeVector[n]->GetLinks()->begin(); i != m_DerivedNodeVector[n]->GetLinks()->end(); i++)
      {
        linkName = i->first;
        if (m_LinkNodeVector[counter]->IsA("mafVMELandmarkCloud") && m_LinkNodeVector[counter]->GetNumberOfChildren() != 0)
        {
          //set subId to 0, because it is the first landmark of the cloud
          subId = 0;
        }
        m_DerivedNodeVector[n]->SetLink(linkName.GetCStr(), m_LinkNodeVector[counter], subId); 
        counter++;
      }
    }
  }
}
//----------------------------------------------------------------------------
int lhpOpDownloadVME::DownloadVME(wxArrayString listVME)   
//----------------------------------------------------------------------------
{
	if(!CreateIncomingDirectory())
	{
		wxMessageBox("Unable to create Incoming Directory");
		return MAF_ERROR;
	}
  
	if(!CreateIncomingCache())
	{
		wxMessageBox("Unable to create a temporary cache, remember that msf must be saved locally");
		return MAF_ERROR;
	}

  for (int i = 0; i < listVME.size(); i++)
  {
    if(DownloadSelectedXMLFromBasket(listVME[i]) != MAF_OK)
    {
      wxMessageBox("Unable to download xml");
      return MAF_ERROR;
    }

    //reconstruct msf
    if(ReconstructMSF(listVME[i]) != MAF_OK)
    {
      wxMessageBox("Unable to reconstruct msf");
      return MAF_ERROR;
    }

    wxString oldDir = wxGetCwd();
    mafLogMessage( _T("Current working directory is: '%s' "), wxGetCwd().c_str() );
    wxSetWorkingDirectory(m_PythonUploadFullPath.GetCStr());
    //mafLogMessage( _T("Current working directory is: '%s' "), wxGetCwd().c_str() );
    wxBusyCursor wait;


    if (m_URISRBFile.Equals("NOT PRESENT"))
      m_URISRBFile = ".";

    if ( ExistsRunningProcess() )
    {
      //PROCESS EXIST, ONLY CALL CLIENT
      wxString command2execute;
      command2execute = m_PythonwExe;
      //command2execute.Append(m_PythonUploadFullPath.GetCStr());
      // script for client
      m_FileName = "Client.py ";
      command2execute.Append(m_FileName.GetCStr());
      command2execute.Append("127.0.0.1 "); //server address (localhost)
      command2execute.Append("50000 "); //port address (50000)
      command2execute.Append(wxString::Format("DOWNLOAD ")); //Download command
      command2execute.Append(wxString::Format("%s ",m_URISRBFileSize)); //file size

      //workaround to understanding directory argument
      wxString directoryWorkAround = m_IncomingCompletePath;
      directoryWorkAround.Replace(" ", "?");
      command2execute.Append(wxString::Format("%s ",directoryWorkAround)); //cache directory
      command2execute.Append(wxString::Format("%s ",m_User.GetName())); //user
      command2execute.Append(wxString::Format("%s ",m_User.GetPwd())); //pwd
      command2execute.Append(wxString::Format("%s ","https://www.biomedtown.org/biomed_town/LHDL/users/repository/lhprepository2")); //dev repository
      //http://www.biomedtown.org/biomed_town/LHDL/users/repository/lhprepository2 prod
      command2execute.Append(wxString::Format("%s ", "none")); //set "none" for download
      command2execute.Append(wxString::Format("%s ", "false")); //has link? (set "false" for download")
      command2execute.Append(wxString::Format("%s ",m_URISRBFile.GetCStr())); //DATA DOWNLOAD NAME
      //command2execute.Append(" > log.txt"); //logme

      m_Pid = wxExecute(command2execute, wxEXEC_ASYNC);
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
      command2execute.Append(wxString::Format("DOWNLOAD ")); //Download command
      command2execute.Append(wxString::Format("%s ",m_URISRBFileSize)); //file size

      //workaround to understanding directory argument
      wxString directoryWorkAround = m_IncomingCompletePath;
      directoryWorkAround.Replace(" ", "?");
      command2execute.Append(wxString::Format("%s ",directoryWorkAround)); //cache directory
      command2execute.Append(wxString::Format("%s ",m_User.GetName())); //user
      command2execute.Append(wxString::Format("%s ",m_User.GetPwd())); //pwd
      command2execute.Append(wxString::Format("%s ","https://www.biomedtown.org/biomed_town/LHDL/users/repository/lhprepository2")); //repository

      command2execute.Append(wxString::Format("%s ", "none")); //set "false" for download
      command2execute.Append(wxString::Format("%s ", "false")); //has link? (set "none" for download")
      command2execute.Append(wxString::Format("%s ",m_URISRBFile.GetCStr())); //DATA DOWNLOAD NAME

      mafLogMessage( _T("Executing command: '%s'"), command2execute.c_str() );
      m_Pid = wxExecute(command2execute, wxEXEC_ASYNC);
    }
    wxSetWorkingDirectory(oldDir);

    //import msf in the current tree
    if(ImportMSF() != MAF_OK)
    {
      wxMessageBox("Unable to import msf");
      return MAF_ERROR;;
    }
  }
  return MAF_OK;
}
//----------------------------------------------------------------------------
void lhpOpDownloadVME::OpStop(int result)   
//----------------------------------------------------------------------------
{
  //HideGui();
	mafEventMacro(mafEvent(this,result));
}
//----------------------------------------------------------------------------
bool lhpOpDownloadVME::CreateIncomingCache()
//----------------------------------------------------------------------------
{
  bool result = true;
  //create cache: logic comunicate the msf directory
  mafEvent event;
  event.SetSender(this);
  event.SetId(ID_MSF_DATA_CACHE);
  mafEventMacro(event);

  wxString temp;
  temp.Append((*event.GetString()).GetCStr());
  m_MsfFile = temp;
  temp = temp.BeforeLast('/');
  m_MsfDir = temp;

  wxDir dir(m_MsfDir.GetCStr());
  wxString exist = m_MsfDir.GetCStr();
  if ( !wxDirExists(exist) || !dir.IsOpened())
  {
    // deal with the error here - wxDir would already log an error message
    // explaining the exact reason of the failure
    return false;
  }

  //control cache subdir
  mafString currentSubdir;
  currentSubdir = m_IncomingDir + m_CacheSubdir.GetCStr();
  while(wxDirExists(currentSubdir))
  {
    int number = atoi(m_CacheSubdir.GetCStr());
    number += 1;
    m_CacheSubdir = "";
    m_CacheSubdir << number;
    currentSubdir = m_IncomingDir + m_CacheSubdir.GetCStr();
  }
  currentSubdir = currentSubdir + "\\";
  wxMkDir(currentSubdir);
  m_IncomingCompletePath = currentSubdir;
  
  //wxMessageBox(m_MsfDir.GetCStr());
  return result;
  
}
//----------------------------------------------------------------------------
bool lhpOpDownloadVME::ExistsRunningProcess()
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
bool lhpOpDownloadVME::CreateIncomingDirectory()
//----------------------------------------------------------------------------
{
  bool resultIncoming = false;

  
  wxString existIncoming = m_IncomingDir.GetCStr();
  if ( wxDirExists(existIncoming) )
  {
    resultIncoming = true;
  }
  else
  {
    wxMkDir(existIncoming);
    if ( wxDirExists(existIncoming) ) resultIncoming = true;
  }

  return resultIncoming;
}


//----------------------------------------------------------------------------
bool lhpOpDownloadVME::CheckLogin()
//----------------------------------------------------------------------------
{
  bool result = false;

  m_User.SetProxyPort(m_ProxyPort);
  m_User.SetProxyURL(m_ProxyURL);

  result = m_User.CheckUserCredentials();
  return result;
}
//----------------------------------------------------------------------------
// widget id's
//----------------------------------------------------------------------------
void lhpOpDownloadVME::CreateGui()
//----------------------------------------------------------------------------
{
  /*m_Gui = new mmgGui(this);

  m_Gui->Divider(2);

  m_Gui->Label("Download", true);
  wxString subDictionariesList[3] = {"none", "motionAnalysis", "dicom"};
  //m_Gui->Combo(ID_SUBDICTIONARY,"",&m_SubdictionaryId,3,subDictionariesList);

  m_Gui->Divider(2);
 
  m_Gui->OkCancel(); 
  m_Gui->Label("");
  m_Gui->Update();*/

}
//----------------------------------------------------------------------------
int lhpOpDownloadVME::CreateFileListFromBasket()
//----------------------------------------------------------------------------
{
  wxString oldDir = wxGetCwd();
  mafLogMessage( _T("Current working directory is: '%s' "), wxGetCwd().c_str() );
  wxSetWorkingDirectory(m_PythonUploadFullPath.GetCStr());
  mafLogMessage( _T("Now current working directory is: '%s' "), wxGetCwd().c_str() );

  // gui for selecting vme
  wxString command2execute;
  command2execute.Clear();
  command2execute = m_PythonwExe;

  command2execute.Append(" downloadSelectorApp.py ");
  command2execute.Append(m_User.GetName());
  command2execute.Append(" ");
  command2execute.Append(m_User.GetPwd());
  /*command2execute.Append(" ");
  command2execute.Append(m_ProxyURL.GetCStr());
  command2execute.Append(" ");
  command2execute.Append(m_ProxyPort.GetCStr());*/
  //mafLogMessage( _T("Executing command: '%s'"), command2execute.c_str() );

  long pid = wxExecute(command2execute, wxEXEC_SYNC);


  wxSetWorkingDirectory(oldDir);
  mafLogMessage( _T("Current working directory is: '%s' "), wxGetCwd().c_str() );

  return MAF_OK;
}
//-------------------------------------------------------------------
int lhpOpDownloadVME::RetrieveInformationFromBasketListFile()
//-------------------------------------------------------------------
{
  wxString oldDir = wxGetCwd();
  mafLogMessage( _T("Current working directory is: '%s' "), wxGetCwd().c_str() );
  wxSetWorkingDirectory(m_PythonUploadFullPath.GetCStr());
  mafLogMessage( _T("Now current working directory is: '%s' "), wxGetCwd().c_str() );

  // open auto tags file and try to handle tags using tags factory 
  ifstream inBasketListFile;

  inBasketListFile.open(m_BasketListFileName.GetCStr());
  if (!inBasketListFile) {
    mafLogMessage("Unable to open file");
    return MAF_ERROR; // terminate with error
  }

  std::string idName;

  while (inBasketListFile >> idName) 
  {
    m_BasketList.Add(idName.c_str());
  }
  inBasketListFile.close();

  wxSetWorkingDirectory(oldDir);
  mafLogMessage( _T("Current working directory is: '%s' "), wxGetCwd().c_str() );

  return MAF_OK;
}
//-------------------------------------------------------------------
int lhpOpDownloadVME::DownloadSelectedXMLFromBasket(mafString  xmlFile)
//-------------------------------------------------------------------
{
  wxString oldDir = wxGetCwd();
  mafLogMessage( _T("Current working directory is: '%s' "), wxGetCwd().c_str() );
  wxSetWorkingDirectory(m_PythonUploadFullPath.GetCStr());
  mafLogMessage( _T("Now current working directory is: '%s' "), wxGetCwd().c_str() );

  // get manual tags
  wxString command2execute;
  command2execute.Clear();
  command2execute = m_PythonwExe;

  command2execute.Append(" downloadSingleXML.py ");
  command2execute.Append(m_User.GetName());
  command2execute.Append(" ");
  command2execute.Append(m_User.GetPwd());
  command2execute.Append(" ");

  command2execute.Append(xmlFile.GetCStr());
  command2execute.Append(" ");

  wxString directoryWorkAround = m_IncomingCompletePath;
  directoryWorkAround.Replace(" ", "?");
  command2execute.Append(directoryWorkAround);


  long pid = wxExecute(command2execute, wxEXEC_SYNC);

  wxArrayString output;
  wxArrayString errors;

  m_Pid = wxExecute(command2execute, output, errors);


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

  
  if(output.size() < 3)
  {
    return MAF_ERROR;
  }

  m_URISRBFileSize = output[output.size() - 1];
  m_URISRBFile = output[output.size() - 2];
  

  wxSetWorkingDirectory(oldDir);
  mafLogMessage( _T("Current working directory is: '%s' "), wxGetCwd().c_str() );

  return MAF_OK;
}
//-------------------------------------------------------------------
int lhpOpDownloadVME::ReconstructMSF(mafString xmlFile)
//-------------------------------------------------------------------
{
  wxString oldDir = wxGetCwd();
  mafLogMessage( _T("Current working directory is: '%s' "), wxGetCwd().c_str() );
  wxSetWorkingDirectory(m_PythonUploadFullPath.GetCStr());
  mafLogMessage( _T("Now current working directory is: '%s' "), wxGetCwd().c_str() );

  wxString command2execute;
  command2execute.Clear();
  command2execute = m_PythonwExe;

  command2execute.Append(" msfReconstructor.py ");

  wxString directoryWorkAround = m_IncomingCompletePath;
  directoryWorkAround.Replace(" ", "?");

  command2execute.Append(directoryWorkAround);
  command2execute.Append(" ");

  command2execute.Append(xmlFile);
  command2execute.Append(" ");

  wxString directoryWorkAroundMSF = m_MsfDir;
  directoryWorkAroundMSF.Append("/");
  directoryWorkAroundMSF.Replace(" ", "?");

  command2execute.Append(directoryWorkAroundMSF);
  command2execute.Append(" ");
  /*command2execute.Append(" ");
  command2execute.Append(m_ProxyURL.GetCStr());
  command2execute.Append(" ");
  command2execute.Append(m_ProxyPort.GetCStr());*/
  mafLogMessage( _T("Executing command: '%s'"), command2execute.c_str() );

  long pid = wxExecute(command2execute, wxEXEC_SYNC);

  
  wxSetWorkingDirectory(oldDir);
  mafLogMessage( _T("Current working directory is: '%s' "), wxGetCwd().c_str() );

  return MAF_OK;
}
//-------------------------------------------------------------------
void lhpOpDownloadVME::GetLinkURI()
//-------------------------------------------------------------------
{
  wxString name;
  int count, count2;
  std::string listURI = m_NodeDownloaded->GetTagArray()->GetTag("L0000_resource_MAF_Procedural_VMElinkURI1")->GetValue();

  count = listURI.find_first_of("'");
  listURI.erase(0, count+1);

  while (listURI.find_first_of("'") != -1)
  {
    count2 = listURI.find_first_of("'");
    name = (listURI.substr(0, count2)).c_str();
    if (name != " ")
    {
      m_ListLinkURI.Add(name);
    }
    listURI.erase(0, count2+1);
  }
}
//-------------------------------------------------------------------
int lhpOpDownloadVME::ImportMSF()
//-------------------------------------------------------------------
{
  //msf name is standard: outputMSF.msf
  mafString msfFileName;
  msfFileName.Append(m_IncomingCompletePath);
  msfFileName.Append("outputMAF.msf");

  mafVMEStorage *storage;
  storage = mafVMEStorage::New();
  storage->SetURL(msfFileName.GetCStr());

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
  m_NodeDownloaded = root->GetFirstChild();

  if (m_NodeDownloaded->GetNumberOfLinks() != 0)
  {
    wxMessageBox(wxString::Format("Link found! VME link will be downloaded"));
    GetLinkURI();
    m_DerivedNodeVector.push_back(m_NodeDownloaded);    
  }

  if (m_FillLinkVector)
  {
    m_LinkNodeVector.push_back(m_NodeDownloaded);
  }

  if (m_Group == NULL)
  {
    mafNEW(m_Group);
    m_Group->SetName("Downloaded from repository");
    m_Group->ReparentTo(m_Input);
  }
   
  m_NodeDownloaded->ReparentTo(m_Group);

  mafDEL(storage);
  return MAF_OK;
}
//----------------------------------------------------------------------------
bool lhpOpDownloadVME::IsLHPBuilderVersionUpToDate()
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

  mafLogMessage( _T("Executing command: '%s'"), command2execute.c_str() );

  long pid = wxExecute(command2execute, wxEXEC_SYNC);

  wxArrayString output;
  wxArrayString errors;

  m_Pid = wxExecute(command2execute, output, errors);


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