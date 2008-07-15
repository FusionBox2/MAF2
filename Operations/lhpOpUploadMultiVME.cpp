/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: lhpOpUploadMultiVME.cpp,v $
Language:  C++
Date:      $Date: 2008-07-15 15:23:10 $
Version:   $Revision: 1.9 $
Authors:   Roberto Mucci
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

#include "lhpOpUploadMultiVME.h"
#include "lhpOpUploadVME.h"

#include "mafVMELandmarkCloud.h"
#include "mmgGui.h"
#include "lhpUser.h"

#include "mafNode.h"
#include "mafNodeIterator.h"

#include "lhpFactoryTagHandler.h"
#include "vtkPolyData.h"

#include <string>
#include <istream>
#include <ostream>

//----------------------------------------------------------------------------
mafCxxTypeMacro(lhpOpUploadMultiVME);
//----------------------------------------------------------------------------
//static variables
long lhpOpUploadMultiVME::m_Pid = -1;
mafString lhpOpUploadMultiVME::m_CacheSubdir = "0";

enum lhpOpUploadMultiVME_ID
{
  ID_SUBDICTIONARY = MINID, 
};

//----------------------------------------------------------------------------
lhpOpUploadMultiVME::lhpOpUploadMultiVME(wxString label) :
mafOp(label)
//----------------------------------------------------------------------------
{
	m_OpType  = OPTYPE_OP;
	m_Canundo = false;
  m_LinkURI.clear();

  m_PythonExe ="python.exe ";
  m_PythonUploadFullPath  = (mafGetApplicationDirectory() + "\\VMEUploaderDownloader\\").c_str();

  m_MasterXMLDictionaryFileName = "UNDEFINED";
  m_SubXMLDictionaryFilePrefix = "UNDEFINED" ;
  m_SubXMLDictionaryFileName = "UNDEFINED";
  m_AssembledXMLDictionaryFileName = "assembledXMLDictionary.xml";
  m_SubDictionaryBuildingCommand = "UNDEFINED";
  m_listURIFileName = " ";

  m_SubdictionaryId = 0; // NO_SUBDICTIONARY; 
  m_ConnectionConfigurationFileName = "vmeUploaderConnectionConfiguration.conf" ;
  SetListener(this);
  m_NodeCounter = 0;
}

//----------------------------------------------------------------------------
lhpOpUploadMultiVME::~lhpOpUploadMultiVME()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
mafOp* lhpOpUploadMultiVME::Copy()
//----------------------------------------------------------------------------
{
	/** return a copy of itself, needs to put it into the undo stack */
	return new lhpOpUploadMultiVME(m_Label);
}

//----------------------------------------------------------------------------
void lhpOpUploadMultiVME::OpRun()
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

  mafString s(_("Upload VMEs"));
  mafEvent e(this,VME_CHOOSE, &s);
  e.SetBool(true); //true to create dialog with VME multiselect
  mafEventMacro(e);
  m_NodeVector = e.GetVmeVector();
  int size = m_NodeVector.size();
  if (size == 0)
  {
    OpStop(result);
    return;
  }
  
  m_UploadVME = new lhpOpUploadVME("vmeUploader");
  m_UploadVME->SetListener(this->GetListener());

  bool upToDate = false;
  m_UploadVME->SetProxyPort(m_ProxyPort);
  m_UploadVME->SetProxyURL(m_ProxyURL);

  if(m_UploadVME->CheckLogin()) 
  {
    upToDate = this->IsLHPBuilderVersionUpToDate();
  }
  else
  {
    OpStop(result);
    return;
  } 

  if (upToDate)
  {
    mafEventMacro(mafEvent(this, MENU_FILE_SAVE));
    m_UploadingNode = m_NodeVector[m_NodeCounter];
    bool hasLink = (m_UploadingNode->GetNumberOfLinks() != 0);
    this->MultiGui();
  }
  else
  {
    OpStop(result);
  }
}
//------------------------------------------------------------
void lhpOpUploadMultiVME::SaveConnectionConfigurationFile()
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
void lhpOpUploadMultiVME::CreateGui()
//----------------------------------------------------------------------------
{
  m_Gui = new mmgGui(this);

  m_Gui->Divider(2);
  
  m_Gui->Label("VME Name:", true);
  m_Gui->Label(m_UploadingNode->GetName());

  m_Gui->Label("use subdictionary", true);
  wxString subDictionariesList[3] = {"none", "motionAnalysis", "dicom"};
  m_Gui->Combo(ID_SUBDICTIONARY,"",&m_SubdictionaryId,3,subDictionariesList);

  m_Gui->Divider(2);

  m_Gui->OkCancel(); 
  m_Gui->Label("");
  m_Gui->Update();
}
//----------------------------------------------------------------------------
int lhpOpUploadMultiVME::AssembleDictionaries()
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
  command2execute = m_PythonExe;

  command2execute.Append(" lhpXMLDictionariesBuilder.py ");
  command2execute.Append(m_MasterXMLDictionaryFileName);
  command2execute.Append(" ");
  command2execute.Append(m_SubXMLDictionaryFileName);
  command2execute.Append(" ");
  command2execute.Append(m_SubDictionaryBuildingCommand);
  command2execute.Append(" ");
  command2execute.Append(m_AssembledXMLDictionaryFileName);

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

  wxSetWorkingDirectory(oldDir);
  mafLogMessage( _T("Current working directory is: '%s' "), wxGetCwd().c_str() );

  return MAF_OK;
}
//----------------------------------------------------------------------------
bool lhpOpUploadMultiVME::isBinaryDataPresent(mafNode *node) 
//----------------------------------------------------------------------------
{
  bool ret = false;
  //create cache: logic comunicate the msf directory
  mafEvent event;
  event.SetSender(this);
  event.SetId(ID_MSF_DATA_CACHE);
  mafEventMacro(event);

  wxString temp;
  temp.Append((*event.GetString()).GetCStr());
  mafString msfFile = temp;  
  

  wxString oldDir = wxGetCwd();
  mafLogMessage( _T("Current working directory is: '%s' "), wxGetCwd().c_str() );
  wxSetWorkingDirectory(m_PythonUploadFullPath.GetCStr());

  // get manual tags
  wxString command2execute;
  command2execute.Clear();
  command2execute = m_PythonExe;

  command2execute.Append(" lhpCheckBinaryName.py ");
  command2execute.Append("\"");
  command2execute.Append(msfFile.GetCStr());
  command2execute.Append("\"");
  command2execute.Append(" ");
  command2execute.Append(wxString::Format("%d ",node->GetId()));

  long pid = wxExecute(command2execute, wxEXEC_SYNC);

  wxArrayString output;
  wxArrayString errors;

  pid = wxExecute(command2execute, output, errors);
  wxString result = output[output.size() - 1];

  //if result == "", no binary data has been found
  if (result != "")
  {
    ret = true;
  }
  return ret;
}

//----------------------------------------------------------------------------
void lhpOpUploadMultiVME::OnEvent(mafEventBase *maf_event) 
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {
    case ID_SUBDICTIONARY:
    {
      // nothing to do for the moment...
      mafLogMessage("You chosed dictionary number %i", m_SubdictionaryId);
      m_UploadVME->SetDictionary(m_SubdictionaryId);
    }
    break;

    case wxOK:
      {
        UploadMultiVME();
      }
      break;

    case wxCANCEL:
      {        
        HideGui();
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
void lhpOpUploadMultiVME::UploadMultiVME()   
//----------------------------------------------------------------------------
{  
  m_LinkURI.clear();
  bool hasBinary = false;
  mafString URI;
  if ((m_NodeCounter + 1) < m_NodeVector.size())
  {
    m_UploadingNode = m_NodeVector[m_NodeCounter];
    if (m_UploadingNode->GetNumberOfLinks() != 0)
    {
      if (UploadVMELinks(m_UploadingNode) == MAF_ERROR)
      {
        HideGui();
        this->OpStop(OP_RUN_CANCEL);
        return;
      }
    }
    hasBinary = isBinaryDataPresent(m_UploadingNode);
    m_UploadVME->SetInput(m_UploadingNode);
    if (SaveListURIFile() == MAF_ERROR)
    {
      wxMessageBox("Unable to write list of link binary URI. Uploading stopped.");
      return;
    }
    if (m_UploadVME->UploadVME(URI, hasBinary) == MAF_ERROR)
    {
      HideGui();
      this->OpStop(OP_RUN_CANCEL);
      return;
    }

    m_NodeCounter++;
    this->HideGui();
    this->MultiGui();
  }
  else
  {
    m_UploadingNode = m_NodeVector[m_NodeCounter];
    if (m_UploadingNode->GetNumberOfLinks() != 0)
    {
      if (UploadVMELinks(m_UploadingNode) == MAF_ERROR)
      {
        HideGui();
        this->OpStop(OP_RUN_CANCEL);
        return;
      }
    }

    hasBinary = isBinaryDataPresent(m_UploadingNode);
    m_UploadVME->SetInput(m_UploadingNode);

    if (SaveListURIFile() == MAF_ERROR)
    {
      wxMessageBox("Unable to write list of link binary URI. Uploading stopped.");
      return;
    }
    if (m_UploadVME->UploadVME(URI, hasBinary) == MAF_ERROR)
    {
      HideGui();
      this->OpStop(OP_RUN_CANCEL);
      return;
    }
    this->OpStop(OP_RUN_OK);
    return;
  }

}
//----------------------------------------------------------------------------
int lhpOpUploadMultiVME::UploadVMELinks(mafNode *derived)   
//----------------------------------------------------------------------------
{  
    for (mafNode::mafLinksMap::iterator i = m_UploadingNode->GetLinks()->begin(); i != m_UploadingNode->GetLinks()->end(); i++)
  {
    
    bool hasBinary = false;
    mafString URI;
    if (i->second.m_Node != NULL)
    {
      mafNode *link = i->second.m_Node;
      if (link->IsA("mafVMELandmarkCloud") && i->second.m_NodeSubId != -1)
      {
        ((mafVMELandmarkCloud *)link)->Open();
        link = (mafNode*)((mafVMELandmarkCloud *)link)->GetLandmark(i->second.m_NodeSubId);
      }
      hasBinary = isBinaryDataPresent(link);
      wxMessageBox(wxString::Format("Link found! Upload VME: %s", link->GetName()));

      m_UploadVME->SetInput(link);
      if (m_UploadVME->UploadVME(URI, hasBinary) == MAF_ERROR || (hasBinary == true && URI == ""))
      {
        return MAF_ERROR;
      }
      m_LinkURI.push_back(URI);
    }
  }
  return MAF_OK;
}

//------------------------------------------------------------
int lhpOpUploadMultiVME::SaveListURIFile()
//------------------------------------------------------------
{
  wxString oldDir = wxGetCwd();
  mafLogMessage( _T("Current working directory is: '%s' "), wxGetCwd().c_str() );
  wxSetWorkingDirectory(m_PythonUploadFullPath.GetCStr());
  mafLogMessage( _T("Now current working directory is: '%s' "), wxGetCwd().c_str() );

  m_listURIFileName = "listURI";
  wxString lockPath = m_PythonUploadFullPath;
  lockPath += m_listURIFileName.GetCStr();
  //if file exists , delete it
  if (wxFileExists(lockPath))
    wxRemoveFile(lockPath);

  // open auto tags file and try to handle tags using tags factory 
  ofstream listURIFile;

  listURIFile.open(lockPath);
  if (!listURIFile)
  {

    return MAF_ERROR;
  }
  else
  {
    for (int n = 0; n < m_LinkURI.size(); n++)
    {
      listURIFile << m_LinkURI[n];
      listURIFile << "\n";
    }
    listURIFile.close();
  }

  wxSetWorkingDirectory(oldDir);
  mafLogMessage( _T("Current working directory is: '%s' "), wxGetCwd().c_str() );
  return MAF_OK;
}

//----------------------------------------------------------------------------
void lhpOpUploadMultiVME::OpDo()   
//----------------------------------------------------------------------------
{
  HideGui();
}

//----------------------------------------------------------------------------
void lhpOpUploadMultiVME::MultiGui()   
//----------------------------------------------------------------------------
{
  m_UploadingNode = m_NodeVector[m_NodeCounter];
  m_SubdictionaryId = 0;
  CreateGui();
  ShowGui();
}
//----------------------------------------------------------------------------
void lhpOpUploadMultiVME::OpStop(int result)   
//----------------------------------------------------------------------------
{
	mafEventMacro(mafEvent(this,result));
}

//----------------------------------------------------------------------------
bool lhpOpUploadMultiVME::IsLHPBuilderVersionUpToDate()
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
  command2execute = m_PythonExe;

  command2execute.Append(" lhpDictionaryVersionChecker.py ");
  command2execute.Append(" ");
  command2execute.Append(m_ProxyURL.GetCStr());
  command2execute.Append(" ");
  command2execute.Append(m_ProxyPort.GetCStr());

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
  
}/*
//----------------------------------------------------------------------------
bool lhpOpUploadMultiVME::CheckLogin()
//----------------------------------------------------------------------------
{
  bool result = false;

  m_User.SetProxyPort(m_ProxyPort);
  m_User.SetProxyURL(m_ProxyURL);

  result = m_User.CheckUserCredentials();
  if (result)
  {
    int remember = m_User.GetRememberUserCredentials();
    result = m_User.SetCredentials(m_User.GetName(), m_User.GetPwd(), remember);
  }
  return result;
}*/
//--------------------------------------------------------------------------------------------
mafString lhpOpUploadMultiVME::GetXMLDictionaryFileName( mafString dictionaryFileNamePrefix )
//--------------------------------------------------------------------------------------------
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


