/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: lhpOpUploadMultiVME.cpp,v $
Language:  C++
Date:      $Date: 2008-09-10 15:11:51 $
Version:   $Revision: 1.14 $
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
#include "mafGUI.h"

#include "lhpUser.h"

#include "mafNode.h"
#include "mafStorageElement.h"
#include "mafNodeIterator.h"
#include "mafTagArray.h"

#include "lhpFactoryTagHandler.h"
#include "vtkPolyData.h"


#include "mafNodeIterator.h"

#include <string>
#include <istream>
#include <ostream>

//----------------------------------------------------------------------------
mafCxxTypeMacro(lhpOpUploadMultiVME);
//----------------------------------------------------------------------------
//static variables
long lhpOpUploadMultiVME::m_Pid = -1;
mafString lhpOpUploadMultiVME::m_CacheSubdir = "0";
lhpUser lhpOpUploadMultiVME::m_User = lhpUser();

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
  m_WithChild = false;
  m_UploadedURIVector.clear();
  m_UploadedNodeVector.clear();
  m_EmptyNodeVector.clear();
  m_FileCreatedVector.clear();
  m_NodeDerivedId.clear();
 

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
}

//----------------------------------------------------------------------------
lhpOpUploadMultiVME::~lhpOpUploadMultiVME()
//----------------------------------------------------------------------------
{
  m_UploadedURIVector.clear();
  m_UploadedNodeVector.clear();
  m_EmptyNodeVector.clear();
  m_FileCreatedVector.clear();
  m_NodeDerivedId.clear();
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
    result = OP_RUN_OK;
    mafEventMacro(mafEvent(this, MENU_FILE_SAVE));
    OpStop(result); 
  }
  else
  {
    OpStop(result);
  }
}

//----------------------------------------------------------------------------
void lhpOpUploadMultiVME::OpDo()   
//----------------------------------------------------------------------------
{
  wxMessageBox("All Vmes will be uploaded with their metadata.\nPlease check before uploading. Completion of curation can be done in the sandbox.");
  
  for (int i = 0; i < m_NodeVector.size(); i++)
  {
    if (m_NodeVector[i]->IsA("mafVMERoot"))
    {
      UploadTree(m_NodeVector[i]);
    }
    else
    {
      UploadMultiVME(m_NodeVector[i]);
    }
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
void lhpOpUploadMultiVME::UploadTree(mafNode *node)   
//----------------------------------------------------------------------------
{
  std::vector<const mafNode::mafChildrenVector*> childrenVector;
  childrenVector.clear();
  bool hasBinary = false;
  bool alreadyUploaded = false;
  bool emptyNode = false;
  mafString URI;
  int numVmeChild = 0;
  int i = 0;
  mafNode *childToUpload = NULL;
  
  const mafNode::mafChildrenVector *children;
  children = node->GetChildren();
  childrenVector.push_back(children);
  numVmeChild = children->size();
  if (numVmeChild != 0)
    childToUpload = children->at(0);
  while (numVmeChild != 0)
  {
    i = 0;
    for ( i ; i < children->size() ; i++)
    {
      numVmeChild = children->size();
      alreadyUploaded = false;
      emptyNode = false;
      
      //Check if VME children has been already uploaded
      for (int c = 0; c < m_EmptyNodeVector.size(); c++)
      {
        if (m_EmptyNodeVector[c]->Equals(children->at(i)) && m_EmptyNodeVector[c]->GetId() == children->at(i)->GetId())
        {
          emptyNode = true;
          break;
        }
      }
      children = children->at(i)->GetChildren();

      if (children->size() == 0 || emptyNode)
      {
        children = childrenVector.back();
        childToUpload = children->at(i);

        //Check if VME has been already uploaded
        for (int c = 0; c < m_UploadedNodeVector.size(); c++)
        {
          if (m_UploadedNodeVector[c]->Equals(children->at(i)) && m_UploadedNodeVector[c]->GetId() == children->at(i)->GetId())
          {
            alreadyUploaded = true;
            break;
          }
        }

        if (!alreadyUploaded)
        {
          if (childToUpload->GetNumberOfLinks() != 0)
          {
            m_NodeDerivedId.push_back(childToUpload->GetId());
          }
          hasBinary = isBinaryDataPresent(childToUpload);
          m_UploadVME->SetInput(childToUpload);
          URI = "";
          if (m_UploadVME->UploadVME(URI, hasBinary, childToUpload->GetNumberOfChildren()!=0) == MAF_ERROR || (URI == ""))
          {
          this->OpStop(OP_RUN_CANCEL);
          return;
          }
          m_UploadedNodeVector.push_back(childToUpload);
          m_EmptyNodeVector.push_back(childToUpload);
          m_UploadedURIVector.push_back(URI);
          SaveChildURIFile(childToUpload, URI);
        }
      }
      else
      {
        childrenVector.push_back(children);
        i = -1;
      }
    }
    m_EmptyNodeVector.push_back(childToUpload->GetParent());

    if (childrenVector.size() > 1)
      childrenVector.pop_back();
    else
      break;

    children = childrenVector.back();
    numVmeChild = children->size();
  }

  //upload VME Root
  m_UploadVME->SetInput(node);
  URI = "";
  if (m_UploadVME->UploadVME(URI, false, node->GetNumberOfChildren()!=0) == MAF_ERROR)
  {
    this->OpStop(OP_RUN_CANCEL);
    return;
  }
  m_UploadedNodeVector.push_back(node);
  m_UploadedURIVector.push_back(URI);
  SetVMELinks(node);
}

//----------------------------------------------------------------------------
void lhpOpUploadMultiVME::UploadMultiVME(mafNode *node)   
//----------------------------------------------------------------------------
{  
  bool hasBinary = false;
  mafString URI;

  if (node->GetNumberOfLinks() != 0)
  {
    if (UploadVMELinks(node) == MAF_ERROR)
    {
      this->OpStop(OP_RUN_CANCEL);
      return;
    }
  }
  hasBinary = isBinaryDataPresent(node);
  m_UploadVME->SetInput(node);
  URI = "";
  if (m_UploadVME->UploadVME(URI, hasBinary, false) == MAF_ERROR)
  {
    this->OpStop(OP_RUN_CANCEL);
    return;
  }
}
//----------------------------------------------------------------------------
void lhpOpUploadMultiVME::SetVMELinks(mafNode *node)   
//----------------------------------------------------------------------------
{  
  wxString vmeURI;
  mafString listURI;
  mafNode *derived;
  for (int n = 0; n < m_NodeDerivedId.size(); n++)
  {
    derived = node->FindInTreeById(m_NodeDerivedId[n]);
    //find URI of this VME
    for (int c = 0; c < m_UploadedNodeVector.size(); c++)
    {
      if (m_UploadedNodeVector[c]->Equals(derived) && m_UploadedNodeVector[c]->GetId() == derived->GetId())
      {
        vmeURI = m_UploadedURIVector[c];
        int count = vmeURI.find_first_of("'");
        vmeURI.erase(0, count+1);
        count = vmeURI.find_first_of("'");
        vmeURI = (vmeURI.substr(0, count)).c_str();
        break;
      }
    }

    for (mafNode::mafLinksMap::iterator i = derived->GetLinks()->begin(); i != derived->GetLinks()->end(); i++)
    {
      if (i->second.m_Node != NULL)
      {
        mafNode *link = i->second.m_Node;
        for (int c = 0; c < m_UploadedNodeVector.size(); c++)
        {
          if (m_UploadedNodeVector[c]->Equals(link) && m_UploadedNodeVector[c]->GetId() == link->GetId())
          {
            m_UploadedURIVector[c];
            listURI.Append(m_UploadedURIVector[c]);
            break;
          }
        }
      }
    }

    wxString oldDir = wxGetCwd();
    mafString path  = (mafGetApplicationDirectory() + "\\VMEUploaderDownloader").c_str();
    wxSetWorkingDirectory(path.GetCStr());
    mafLogMessage( _T("Now current working directory is: '%s' "), wxGetCwd().c_str() );

    //Add URI tag to link VME uploaded
    wxString command2execute;
    command2execute.Clear();
    command2execute = m_PythonExe;

    command2execute.Append("lhpEditRemoteTag.py ");
    command2execute.Append(m_User.GetName());
    command2execute.Append(" ");
    command2execute.Append(m_User.GetPwd());
    command2execute.Append(" ");
    command2execute.Append(vmeURI.c_str());
    command2execute.Append(",");
    command2execute.Append("L0000_resource_MAF_Procedural_VMElinkURI1");
    command2execute.Append(",");
    command2execute.Append(listURI.GetCStr());
    mafLogMessage( _T("Executing command: '%s'"), command2execute.c_str() );

    long pid = wxExecute(command2execute, wxEXEC_SYNC);

    wxSetWorkingDirectory(oldDir);
    mafLogMessage( _T("Current working directory is: '%s' "), wxGetCwd().c_str() );
  }

}
//----------------------------------------------------------------------------
int lhpOpUploadMultiVME::UploadVMELinks(mafNode *derived)   
//----------------------------------------------------------------------------
{  
 // m_UncompletedTagNode.clear();
 // m_UncompletedTagURI.clear();
  bool emptyNode = false;
  bool alreadyUploaded = false;
  bool hasBinary = false;
  std::vector<mafString> linkURI;
  linkURI.clear();
  m_UncompletedTagNode.clear();

  for (mafNode::mafLinksMap::iterator i = derived->GetLinks()->begin(); i != derived->GetLinks()->end(); i++)
  {
    hasBinary = false;
    alreadyUploaded = false;
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

      //Verify if the link has some link!!
      if (link->GetNumberOfLinks() != 0)
      {
        if (UploadVMELinks(link) == MAF_ERROR)
        {
          return MAF_ERROR;
        }
      }
      m_UploadVME->SetInput(link);

      //Check if VME has been already uploaded
     /* int counterVec = 0;
      for (counterVec = 0; counterVec < m_UploadedNodeVector.size(); counterVec++)
      {
        if (m_UploadedNodeVector[counterVec]->Equals(link))
        {
          alreadyUploaded = true;
          break;
        }
      }*/

      //if (!alreadyUploaded)
      //{
        //Check if VME children has been already uploaded
     /*   for (int c = 0; c < m_EmptyNodeVector.size(); c++)
        {
          if (m_EmptyNodeVector[c]->Equals(link))
          {
            emptyNode = true;
            break;
          }
        }*/

        URI = "";
        //if (m_UploadVME->UploadVME(URI, hasBinary, link->GetNumberOfChildren()!=0) == MAF_ERROR || (URI == ""))
        if (m_UploadVME->UploadVME(URI, hasBinary, false) == MAF_ERROR || (URI == ""))
        {
          return MAF_ERROR;
        }
        m_UploadedNodeVector.push_back(link);
        //SaveChildURIFile(link, URI);
        linkURI.push_back(URI);
       // m_UncompletedTagURI.push_back(URI);
       // m_UncompletedTagNode.push_back(link);

     // }
     // else
     /* {
        URI = m_UploadedURIVector.at(counterVec);
        linkURI.push_back(URI);
      }*/
    }
  }
  if (SaveLinkURIFile(derived, linkURI) == MAF_ERROR)
  {
    wxMessageBox("Unable to write list of link binary URI. Uploading stopped.");
    return MAF_ERROR;
  }
  return MAF_OK;
}

//------------------------------------------------------------
int lhpOpUploadMultiVME::SaveLinkURIFile(mafNode *node, std::vector<mafString> linkURI)
//------------------------------------------------------------
{
  wxString oldDir = wxGetCwd();
  mafLogMessage( _T("Current working directory is: '%s' "), wxGetCwd().c_str() );
  wxSetWorkingDirectory(m_PythonUploadFullPath.GetCStr());
  mafLogMessage( _T("Now current working directory is: '%s' "), wxGetCwd().c_str() );

  m_listURIFileName = node->GetName();
  m_listURIFileName.Append(wxString::Format("%d",node->GetId()));
  m_listURIFileName.Append(".linkURI");
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
    for (int n = 0; n < linkURI.size(); n++)
    {
      listURIFile << linkURI[n];
      listURIFile << "\n";
    }
    listURIFile.close();
  }

  wxSetWorkingDirectory(oldDir);
  mafLogMessage( _T("Current working directory is: '%s' "), wxGetCwd().c_str() );
  return MAF_OK;
}

//------------------------------------------------------------
int lhpOpUploadMultiVME::SaveChildURIFile(mafNode* node, mafString URI)
//------------------------------------------------------------
{
  wxString oldDir = wxGetCwd();
  mafLogMessage( _T("Current working directory is: '%s' "), wxGetCwd().c_str() );
  wxSetWorkingDirectory(m_PythonUploadFullPath.GetCStr());
  mafLogMessage( _T("Now current working directory is: '%s' "), wxGetCwd().c_str() );

  m_listURIFileName = node->GetParent()->GetName();
  m_listURIFileName.Append(wxString::Format("%d",node->GetParent()->GetId()));
  m_listURIFileName.Append(".childURI");
  wxString lockPath = m_PythonUploadFullPath;
  lockPath += m_listURIFileName.GetCStr();

  //Check if file named "lockPath" has been created by this operation
  bool myFile = false;
  for (int n = 0; n < m_FileCreatedVector.size(); n++)
  {
    if (m_FileCreatedVector[n].Equals(lockPath))
    {
      myFile = true;
      break;
    }
  }

   //if file exists and is not created by this operation, delete it
  if (!myFile)
  {
    if (wxFileExists(lockPath))
      wxRemoveFile(lockPath);
  }

  m_FileCreatedVector.push_back(lockPath);

  // open auto tags file and try to handle tags using tags factory 
  ofstream listURIFile;

  listURIFile.open(lockPath, fstream::in | fstream::out | fstream::app);
  if (!listURIFile)
  {
    return MAF_ERROR;
  }
  else
  {
    listURIFile << URI;
    listURIFile << "\n";
  }
    listURIFile.close();

  wxSetWorkingDirectory(oldDir);
  mafLogMessage( _T("Current working directory is: '%s' "), wxGetCwd().c_str() );
  return MAF_OK;
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
  
}

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


