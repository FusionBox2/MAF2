/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: lhpOpUploadMultiVME.cpp,v $
Language:  C++
Date:      $Date: 2009-05-13 15:09:51 $
Version:   $Revision: 1.27.2.17 $
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
#include "lhpUtils.h"

#include <wx/process.h>
#include <wx/dir.h>
#include <wx/file.h>
#include <wx/busyinfo.h>

#include "lhpOpUploadMultiVME.h"
#include "lhpOpUploadVME.h"

#include "mafVMERoot.h"
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
mafString lhpOpUploadMultiVME::m_CacheSubdir = _R("0");

enum lhpOpUploadMultiVME_ID
{
  ID_SUBDICTIONARY = MINID, 
};

//----------------------------------------------------------------------------
lhpOpUploadMultiVME::lhpOpUploadMultiVME(const mafString& label) : Superclass(label)
//----------------------------------------------------------------------------
{
	m_OpType  = OPTYPE_OP;
	m_Canundo = false;
  m_WithChild = false;
  m_DebugMode = false;
  m_User = NULL;
  m_UploadedURIVector.clear();
  m_UploadedNodeVector.clear();
  m_EmptyNodeVector.clear();
  m_FileCreatedVector.clear();
  m_NodeDerivedId.clear();

  //m_ServiceURL = _R("http://devel.fec.cineca.it:12680/town/biomed_town/LHDL/users/repository/lhprepository2/");
  m_ServiceURL = _R("https://www.biomedtown.org/biomed_town/LHDL/users/repository/lhprepository2/");
 
  m_PythonExe = _R("python.exe_UNDEFINED");

  m_VMEUploaderDownloaderDir  = lhpUtils::lhpGetApplicationDirectory() + _R("\\VMEUploaderDownloader\\");

  m_MasterXMLDictionaryFileName = _R("UNDEFINED");
  m_SubXMLDictionaryFilePrefix = _R("UNDEFINED") ;
  m_SubXMLDictionaryFileName = _R("UNDEFINED");
  m_AssembledXMLDictionaryFileName = _R("assembledXMLDictionary.xml");
  m_SubDictionaryBuildingCommand = _R("UNDEFINED");

  m_SubdictionaryId = 0; // NO_SUBDICTIONARY; 
  m_ConnectionConfigurationFileName = _R("vmeUploaderConnectionConfiguration.conf") ;
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
bool lhpOpUploadMultiVME::Accept(mafNode* vme)
//----------------------------------------------------------------------------
{
  lhpUser *user = NULL;
  //Get User values
  mafEvent event;
  event.SetSender(this);
  event.SetId(ID_REQUEST_USER);
  mafEventMacro(event);
  if(event.GetMafObject() != NULL) //if proxy string contains something != ""
  {
    user = (lhpUser*)event.GetMafObject();
  }
  return (user != NULL && user->IsAuthenticated() && vme != NULL);
}

//----------------------------------------------------------------------------
mafOp* lhpOpUploadMultiVME::Copy()
//----------------------------------------------------------------------------
{
	/** return a copy of itself, needs to put it into the undo stack */
	return new lhpOpUploadMultiVME(GetLabel());
}

//----------------------------------------------------------------------------
void lhpOpUploadMultiVME::OpRun()
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

  //Get User values
  mafEvent event;
  event.SetSender(this);
  event.SetId(ID_REQUEST_USER);
  mafEventMacro(event);
  if(event.GetMafObject() != NULL) //if proxy string contains something != ""
  {
    m_User = (lhpUser*)event.GetMafObject();
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

  mafString DebugPath = m_VMEUploaderDownloaderDir;
  DebugPath.Append(_R("\\Debug.py"));
  if (mafFileExists(DebugPath))
  {
    ifstream debugFile;
    debugFile.open(DebugPath.GetCStr());
    if (!debugFile) {
      mafLogMessage(_M("Unable to open Debug.py file"));
    }

    std::string isDebug;
    debugFile >> isDebug;
    int pos = isDebug.find_last_of('=');
    isDebug = isDebug.substr(pos+1);
    if (!isDebug.compare("1") || !isDebug.compare("True"))
    {
      m_DebugMode = true;
    }
    debugFile.close();
  }

  mafString s(_L("Upload VMEs"));
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
  
  m_UploadVME = new lhpOpUploadVME(_R("vmeUploader"));
  m_UploadVME->SetDebugMode(m_DebugMode);
  m_UploadVME->SetListener(this->GetListener());

  bool upToDate = false;
  upToDate = this->IsSoftwareVersionUpToDate();
  if (upToDate)
  {
    mafEventMacro(mafEvent(this, MENU_FILE_SAVE));
    //logic comunicate the msf directory
    mafEvent event;
    event.SetSender(this);
    event.SetId(ID_MSF_DATA_CACHE);
    mafEventMacro(event);

    wxString temp;
    temp.Append((*event.GetString()).toWx());
    m_MsfFile = mafWxToString(temp);
    temp = temp.BeforeLast('/');
    mafString msfDir = mafWxToString(temp);

    if (msfDir.IsEmpty())
      wxMessageBox("Msf must be saved locally. Upload stopped", wxMessageBoxCaptionStr, wxSTAY_ON_TOP | wxOK);
    else
      result = OP_RUN_OK;

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
  bool rootChosen = false;
  int res = wxMessageBox("All Vmes will be uploaded with their metadata.\nPlease check before uploading. Completion of curation can be done in the sandbox.", wxMessageBoxCaptionStr, wxOK | wxCANCEL);
  //returns 4 for OK, 16 for CANCEL
  if (res == 16)
  {
    return;
  }

  wxBusyInfo *wait;
  if(!m_TestMode)
  {
    wait = new wxBusyInfo("Please wait, uploading VME");
  }

  //If VMERoot has been chosen, than upload only itself with its children
  for (int i = 0; i < m_NodeVector.size(); i++)
  {
    if (m_NodeVector[i]->IsA("mafVMERoot"))
    {
      rootChosen = true;
      //if exists, remove error file form python
      if (mafFileExists(m_VMEUploaderDownloaderDir + _R("ErrorFound.lhp")))
      {
        mafFileRemove(m_VMEUploaderDownloaderDir + _R("ErrorFound.lhp"));
      }
      //Check if are present VME without name
      mafNode *nodeNoName = NULL;
      nodeNoName = m_NodeVector[i]->FindInTreeByName(_R(""));
      if  (nodeNoName != NULL)
      {
        wxMessageBox("Tree contains VME without name. Upload Stopped.", wxMessageBoxCaptionStr, wxSTAY_ON_TOP | wxOK);
        return;
      }
      //Check if Root VME has a name different from "Root"
      mafString RootName = m_NodeVector[i]->GetName();
      if  (RootName == _R("root"))
      {
        mafString path, name, ext;
        mafString newRootName = m_MsfFile;
        mafSplitPath(newRootName, &path, &name, &ext);
        wxMessageBox(wxString::Format("Root name will be modified with the name of the MSF: ") + name.toWx(), wxMessageBoxCaptionStr, wxSTAY_ON_TOP | wxOK);
        m_NodeVector[i]->SetName(name);
        m_NodeVector[i]->Modified();
        ((mafVMERoot *)m_NodeVector[i])->Update();

        mafEvent ev(this,VME_MODIFIED,m_NodeVector[i]);
        mafEventMacro(ev);

        //m_NodeVector[i]->Update();
      }

      if (UploadTree(m_NodeVector[i]) == MAF_ERROR)
      {
        if(!m_TestMode)
        {
          delete wait;
        }
        return;
      }
      break;
    }
  }
  
  if (!rootChosen)
  {
    bool isLast = false;
    for (int i = 0; i < m_NodeVector.size(); i++)
    {
      if (strcmp(m_NodeVector[i]->GetName().GetCStr(), "") == 0)
      {
        wxMessageBox("Can not upload VME without name.", wxMessageBoxCaptionStr, wxSTAY_ON_TOP | wxOK);
        return;
      }

      //check if is last VME
      if (i+1 == m_NodeVector.size())
        isLast = true;

      if (UploadMultiVME(m_NodeVector[i], isLast) == MAF_ERROR)
      {
        if(!m_TestMode)
        {
          delete wait;
        }
        return;
      }
    }
  }

  if(!m_TestMode)
  {
    delete wait;
  }
}
//------------------------------------------------------------
void lhpOpUploadMultiVME::SaveConnectionConfigurationFile()
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
  ofstream configurationFile;

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
int lhpOpUploadMultiVME::AssembleDictionaries()
//----------------------------------------------------------------------------
{
  wxString oldDir = wxGetCwd();
  if (m_DebugMode)
    mafLogMessage(_M(_R("Current working directory is: '") + mafWxToString(wxGetCwd()) + _R("' ")));
  wxSetWorkingDirectory(m_VMEUploaderDownloaderDir.toWx());
  if (m_DebugMode)
    mafLogMessage(_M(_R("Now current working directory is: '") + mafWxToString(wxGetCwd()) + _R("' ")));

  if (m_DebugMode)
    mafLogMessage(_M("Assembling dictionaries..."));

  m_SubXMLDictionaryFileName = this->GetXMLDictionaryFileName(m_SubXMLDictionaryFilePrefix);
  if (m_SubXMLDictionaryFileName == _R("NOT FOUND"))
  {
    return MAF_ERROR;
  }

  // get manual tags
  mafString command2execute;
  command2execute.Clear();
  command2execute = m_PythonExe;

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

  wxSetWorkingDirectory(oldDir);
  if (m_DebugMode)
    mafLogMessage(_M(_R("Current working directory is: '") + mafWxToString(wxGetCwd()) + _R("' ")));

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

  mafString temp;
  temp.Append(*event.GetString());
  mafString msfFile = temp;  
  

  wxString oldDir = wxGetCwd();
  if (m_DebugMode)
    mafLogMessage(_M(_R("Current working directory is: '") + mafWxToString(wxGetCwd()) + _R("' ")));
  wxSetWorkingDirectory(m_VMEUploaderDownloaderDir.toWx());

  // get manual tags
  mafString command2execute;
  command2execute.Clear();
  command2execute = m_PythonExe;

  command2execute.Append(_R(" lhpVMEBinaryDataChecker.py "));
  command2execute.Append(_R("\""));
  command2execute.Append(msfFile);
  command2execute.Append(_R("\""));
  command2execute.Append(_R(" "));
  command2execute.Append(mafToString(node->GetId()));
  command2execute.Append(_R(" "));

  wxArrayString output;
  wxArrayString errors;
  long pid = -1;
  if (pid = wxExecute(command2execute.toWx(), output, errors, wxEXEC_SYNC) != 0)
  {
    wxMessageBox("Error in lhpVMEBinaryDataChecker.py. Uploading stopped", wxMessageBoxCaptionStr, wxSTAY_ON_TOP | wxOK);
    mafLogMessage(_M(_R("SYNC Command process '") + command2execute + _R("' terminated with exit code ") + mafToString(pid) + _R(".")));
    return MAF_ERROR;
  }


  wxString result = output[output.size() - 1];

  //if result == "", no binary data has been found
  if (result != "")
  {
    ret = true;
  }
  return ret;
}


//----------------------------------------------------------------------------
int lhpOpUploadMultiVME::UploadTree(mafNode *node)   
//----------------------------------------------------------------------------
{
  //creates a file with a list of upload xml, to be used by
  //python to remove xml uploaded in case of msf upload error
  int number = 0;
  mafString TmpFileName = _R("msfList");
  mafString msfFileName = TmpFileName;

  while(mafFileExists(m_VMEUploaderDownloaderDir + msfFileName + _R(".lhp")))
  {
    msfFileName = TmpFileName;
    msfFileName += mafToString(number);
    number++;
  }
  mafString msfFilePath = m_VMEUploaderDownloaderDir + msfFileName + _R(".lhp");
  FILE *file = fopen(msfFilePath.GetCStr(),"w");
  fclose(file);
  msfFileName += _R(".lhp");

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
          URI = _R("");

          if (GetUploadError())
          {
            RemoveResources(m_UploadedURIVector);
            return MAF_ERROR;
          }
          

          if (m_UploadVME->UploadVME(URI, hasBinary, childToUpload->GetNumberOfChildren()!=0, msfFileName, false) == MAF_ERROR)
          {
            RemoveResources(m_UploadedURIVector);
            return MAF_ERROR;
          }
          m_UploadedNodeVector.push_back(childToUpload);
          m_EmptyNodeVector.push_back(childToUpload);
          m_UploadedURIVector.push_back(URI);
          if (SaveChildURIFile(childToUpload, URI) == MAF_ERROR)
          {
            
            return MAF_ERROR;
          }
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
  URI = _R("");

  if (GetUploadError())
  {
    RemoveResources(m_UploadedURIVector);
    return MAF_ERROR;
  }

  if (m_UploadVME->UploadVME(URI, false, node->GetNumberOfChildren()!=0, msfFileName, true) == MAF_ERROR)
  {
    RemoveResources(m_UploadedURIVector);
    return MAF_ERROR;
  }
  m_UploadedNodeVector.push_back(node);
  m_UploadedURIVector.push_back(URI);
  if (SetVMELinks(node) == MAF_ERROR)
  {
    RemoveResources(m_UploadedURIVector);
    return MAF_ERROR;
  }
  return MAF_OK;
  
}

//----------------------------------------------------------------------------
int lhpOpUploadMultiVME::UploadMultiVME(mafNode *node, bool isLast)   
//----------------------------------------------------------------------------
{  
  bool hasBinary = false;
  mafString URI;

  if (node->GetNumberOfLinks() != 0)
  {
    if (UploadVMELinks(node) == MAF_ERROR)
    {
      return MAF_ERROR;
    }
  }

  hasBinary = isBinaryDataPresent(node);

  m_UploadVME->SetInput(node);
  URI = _R("");
  if (m_UploadVME->UploadVME(URI, hasBinary, false, _R("noMsf"), isLast) == MAF_ERROR)
  {
    m_UploadedURIVector.clear();
    m_UploadedURIVector.push_back(URI);
    RemoveResources(m_UploadedURIVector);
    return MAF_ERROR;
  }
  return MAF_OK;
}
//----------------------------------------------------------------------------
bool lhpOpUploadMultiVME::GetUploadError()   
//----------------------------------------------------------------------------
{
  bool errorFound = false;
  if (mafFileExists(m_VMEUploaderDownloaderDir + _R("ErrorFound.lhp")))
  {
    mafString errorMessage;
    std::ifstream errorFile((m_VMEUploaderDownloaderDir + _R("ErrorFound.lhp")).GetCStr(), std::ios::in);
    if (errorFile)
    {
      std::string buf;
      getline(errorFile, buf);
      errorMessage.Append(_R(buf.c_str()));
      errorMessage.Append(_R("\n"));
      while (!errorFile.eof())
      {
        getline(errorFile, buf);
        errorMessage.Append(_R(buf.c_str()));
      }

      wxMessageBox(wxString::Format("Error in MSF upload.\n%s\nUpload MSF stopped.\nVME already uploaded will be removed from repository.",errorMessage.GetCStr()), wxMessageBoxCaptionStr, wxSTAY_ON_TOP | wxOK);
      errorFile.close();
      mafFileRemove(m_VMEUploaderDownloaderDir + _R("ErrorFound.lhp"));
      errorFound = true;
    }
  }
  return errorFound;
}

//----------------------------------------------------------------------------
bool lhpOpUploadMultiVME::RemoveResources(std::vector<mafString> vectorURI)   
//----------------------------------------------------------------------------
{
  wxString oldDir = wxGetCwd();
  if (m_DebugMode)
    mafLogMessage(_M(_R("Current working directory is: '") + mafWxToString(wxGetCwd()) + _R("' ")));
  wxSetWorkingDirectory(m_VMEUploaderDownloaderDir.toWx());

  mafString command2execute;

  //WAIT MESSAGE:
  wxBusyInfo *wait;
  if(!m_TestMode)
  {
    wait = new wxBusyInfo("Please wait, removing uploaded VME");
  }

  for (int i = 0; i < m_UploadedURIVector.size(); i++)
  {
    command2execute.Clear();
    command2execute = _R("pythonw.exe ");

    // script for client
    command2execute.Append(_R(" lhpRemoveResource.py "));
    command2execute.Append(m_User->GetName());
    command2execute.Append(_R(" "));
    command2execute.Append(m_User->GetPwd());
    command2execute.Append(_R(" "));
    command2execute.Append(m_UploadedURIVector[i]);

    if (m_DebugMode)
      mafLogMessage(_M(_R("Executing command: '") + command2execute + _R("'")));

    long pid = -1;
    if (pid = wxExecute(command2execute.toWx(), wxEXEC_SYNC) != 0)
    {
      wxMessageBox("Error in lhpRemoveResource. Can not remove uploaded resource", wxMessageBoxCaptionStr, wxSTAY_ON_TOP | wxOK);
      if (m_DebugMode)
        mafLogMessage(_M(_R("ASYNC Command process '") + command2execute + _R("' terminated with exit code ") + mafToString(pid) + _R(".")));
      if(!m_TestMode)
      {
        delete wait;
      }
      return MAF_ERROR;
    }
  } 
  if(!m_TestMode)
  {
    delete wait;
  }
}

//----------------------------------------------------------------------------
int lhpOpUploadMultiVME::SetVMELinks(mafNode *node)   
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
        vmeURI = m_UploadedURIVector[c].toWx();
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
            listURI.Append(_R(" "));
            break;
          }
        }
      }
    }

    wxString oldDir = wxGetCwd();
    wxSetWorkingDirectory(m_VMEUploaderDownloaderDir.toWx());
    if (m_DebugMode)
      mafLogMessage(_M(_R("Now current working directory is: '") + mafWxToString(wxGetCwd()) + _R("' ")));

    //Add URI tag to link VME uploaded
    mafString command2execute;
    command2execute.Clear();
    command2execute = m_PythonExe;

    command2execute.Append(_R("lhpEditRemoteTag.py "));
    command2execute.Append(m_User->GetName());
    command2execute.Append(_R(" "));
    command2execute.Append(m_User->GetPwd());
    command2execute.Append(_R(" "));
    command2execute.Append(m_ServiceURL);
    command2execute.Append(_R(" "));
    command2execute.Append(mafWxToString(vmeURI));
    command2execute.Append(_R(","));
    command2execute.Append(_R("L0000_resource_MAF_Procedural_VMElinkURI1"));
    command2execute.Append(_R(","));
    command2execute.Append(listURI);
    if (m_DebugMode)
      mafLogMessage(_M(_R("Executing command: '") + command2execute + _R("'")));

    long pid = -1;
    if (pid = wxExecute(command2execute.toWx(), wxEXEC_SYNC) != 0)
    {
      wxMessageBox("Error in lhpEditRemoteTag.py. Uploading stopped", wxMessageBoxCaptionStr, wxSTAY_ON_TOP | wxOK);
      mafLogMessage(_M(_R("SYNC Command process '") + command2execute + _R("' terminated with exit code ") + mafToString(pid) + _R(".")));
      return MAF_ERROR;
    }

    wxSetWorkingDirectory(oldDir);
    if (m_DebugMode)
      mafLogMessage(_M(_R("Current working directory is: '") + mafWxToString(wxGetCwd()) + _R("' ")));
  }
  return MAF_OK;

}
//----------------------------------------------------------------------------
int lhpOpUploadMultiVME::UploadVMELinks(mafNode *derived)   
//----------------------------------------------------------------------------
{  
  bool emptyNode = false;
  bool alreadyUploaded = false;
  bool hasBinary = false;
  std::vector<mafString> linkURI;
  linkURI.clear();


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
      wxMessageBox(wxString::Format("Link found! Upload VME: %s", link->GetName()), wxMessageBoxCaptionStr, wxSTAY_ON_TOP | wxOK);

      //Verify if the link has some link!!
      if (link->GetNumberOfLinks() != 0)
      {
        if (UploadVMELinks(link) == MAF_ERROR)
        {
          return MAF_ERROR;
        }
      }
      m_UploadVME->SetInput(link);

      URI = _R("");
      if (m_UploadVME->UploadVME(URI, hasBinary, false, _R("noMsf"), false) == MAF_ERROR)
      {
        m_UploadedURIVector.clear();
        m_UploadedURIVector.push_back(URI);
        RemoveResources(m_UploadedURIVector);
        return MAF_ERROR;
      }
      m_UploadedNodeVector.push_back(link);
      linkURI.push_back(URI);
    }
  }
  if (SaveLinkURIFile(derived, linkURI) == MAF_ERROR)
  {
    wxMessageBox("Unable to write list of link binary URI. Uploading stopped.", wxMessageBoxCaptionStr, wxSTAY_ON_TOP | wxOK);
    return MAF_ERROR;
  }
  return MAF_OK;
}

//------------------------------------------------------------
int lhpOpUploadMultiVME::SaveLinkURIFile(mafNode *node, std::vector<mafString> linkURI)
//------------------------------------------------------------
{
  wxString listURIFileName;
  wxString oldDir = wxGetCwd();
  if (m_DebugMode)
    mafLogMessage(_M(_R("Current working directory is: '") + mafWxToString(wxGetCwd()) + _R("' ")));
  wxSetWorkingDirectory(m_VMEUploaderDownloaderDir.toWx());
  if (m_DebugMode)
    mafLogMessage(_M(_R("Now current working directory is: '") + mafWxToString(wxGetCwd()) + _R("' ")));

  listURIFileName = node->GetName().toWx();
  listURIFileName.Append(wxString::Format("%d",node->GetId()));
  listURIFileName.Replace("/","_");
  listURIFileName.Replace("\\","_");
  listURIFileName.Replace(":","_");
  listURIFileName.Replace("*","_");
  listURIFileName.Replace("?","_");
  listURIFileName.Replace("\"","_");
  listURIFileName.Replace("<","_");
  listURIFileName.Replace(">","_");
  listURIFileName.Replace("!","_");
  listURIFileName.Append(".linkURI");
  mafString lockPath = m_VMEUploaderDownloaderDir;
  lockPath += mafWxToString(listURIFileName);
  //if file exists , delete it
  if (mafFileExists(lockPath))
    mafFileRemove(lockPath);

  // open auto tags file and try to handle tags using tags factory 
  ofstream listURIFile;

  listURIFile.open(lockPath.GetCStr());
  if (!listURIFile)
  {
    return MAF_ERROR;
  }
  else
  {
    for (int n = 0; n < linkURI.size(); n++)
    {
      listURIFile << linkURI[n].GetCStr();
      listURIFile << "\n";
    }
    listURIFile.close();
  }

  wxSetWorkingDirectory(oldDir);
  if (m_DebugMode)
    mafLogMessage(_M(_R("Current working directory is: '") + mafWxToString(wxGetCwd()) + _R("' ")));
  return MAF_OK;
}

//------------------------------------------------------------
int lhpOpUploadMultiVME::SaveChildURIFile(mafNode* node, mafString URI)
//------------------------------------------------------------
{
  wxString listURIFileName;
  wxString oldDir = wxGetCwd();
  if (m_DebugMode)
    mafLogMessage(_M(_R("Current working directory is: '") + mafWxToString(wxGetCwd()) + _R("' ")));
  wxSetWorkingDirectory(m_VMEUploaderDownloaderDir.toWx());
  if (m_DebugMode)
    mafLogMessage(_M(_R("Now current working directory is: '") + mafWxToString(wxGetCwd()) + _R("' ")));

  listURIFileName = node->GetParent()->GetName().toWx();
  listURIFileName.Append(wxString::Format("%d",node->GetParent()->GetId()));
  listURIFileName.Replace("/","_");
  listURIFileName.Replace("\\","_");
  listURIFileName.Replace(":","_");
  listURIFileName.Replace("*","_");
  listURIFileName.Replace("?","_");
  listURIFileName.Replace("\"","_");
  listURIFileName.Replace("<","_");
  listURIFileName.Replace(">","_");
  listURIFileName.Replace("!","_");
  listURIFileName.Replace("|","_");
  listURIFileName.Append(".childURI");

  mafString lockPath = m_VMEUploaderDownloaderDir;
  lockPath += mafWxToString(listURIFileName);
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
    if (mafFileExists(lockPath))
      mafFileRemove(lockPath);
  }

  m_FileCreatedVector.push_back(lockPath);

  // open auto tags file and try to handle tags using tags factory 
  ofstream listURIFile;

  listURIFile.open(lockPath.GetCStr(), fstream::in | fstream::out | fstream::app);
  if (!listURIFile)
  {
    return MAF_ERROR;
  }
  else
  {
    listURIFile << URI.GetCStr();
    listURIFile << "\n";
  }
    listURIFile.close();

  wxSetWorkingDirectory(oldDir);
  if (m_DebugMode)
    mafLogMessage(_M(_R("Current working directory is: '") + mafWxToString(wxGetCwd()) + _R("' ")));
  return MAF_OK;
}

//----------------------------------------------------------------------------
void lhpOpUploadMultiVME::OpStop(int result)   
//----------------------------------------------------------------------------
{
	mafEventMacro(mafEvent(this,result));
}

//----------------------------------------------------------------------------
bool lhpOpUploadMultiVME::IsSoftwareVersionUpToDate()
//----------------------------------------------------------------------------
{
  wxBusyInfo("Checking if  your software is up-to-date in order to upload, please wait...");
  wxString oldDir = wxGetCwd();
  if (m_DebugMode)
    mafLogMessage(_M(_R("Current working directory is: '") + mafWxToString(wxGetCwd()) + _R("' ")));
  wxSetWorkingDirectory(m_VMEUploaderDownloaderDir.toWx());
  if (m_DebugMode)
    mafLogMessage(_M(_R("Now current working directory is: '") + mafWxToString(wxGetCwd()) + _R("' ")));

  // get application name
  mafEvent eventGetApplicationName;
  eventGetApplicationName.SetSender(this);
  eventGetApplicationName.SetId(ID_REQUEST_APPLICATION_NAME);
  mafEventMacro(eventGetApplicationName);
  mafString appName = *eventGetApplicationName.GetString();  

  // get manual tags
  mafString command2execute;
  command2execute.Clear();
  command2execute = m_PythonExe;

  command2execute.Append(_R(" lhpDictionaryVersionChecker.py "));
  command2execute.Append(appName);
  command2execute.Append(_R(" "));
  command2execute.Append(m_ProxyURL);
  command2execute.Append(_R(" "));
  command2execute.Append(m_ProxyPort);

  if (m_DebugMode)
    mafLogMessage(_M(_R("Executing command: '") + command2execute + _R("'")));


  wxArrayString output;
  wxArrayString errors;
  long pid = -1;
  if (pid = wxExecute(command2execute.toWx(), output, errors, wxEXEC_SYNC) != 0)
  {
    wxMessageBox("Error in lhpDictionaryVersionChecker.py. Uploading stopped", wxMessageBoxCaptionStr, wxSTAY_ON_TOP | wxOK);
    mafLogMessage(_M(_R("Command process '") + command2execute + _R("' terminated with exit code ") + mafToString(pid) + _R(".")));
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

//--------------------------------------------------------------------------------------------
mafString lhpOpUploadMultiVME::GetXMLDictionaryFileName( mafString dictionaryFileNamePrefix )
//--------------------------------------------------------------------------------------------
{
  mafString dictionaryFileName = _R("NOT FOUND");
  wxString oldDir = wxGetCwd();

  if (m_DebugMode)
    mafLogMessage(_M(_R("Current working directory is: '") + mafWxToString(wxGetCwd()) + _R("' ")));
  wxSetWorkingDirectory(m_VMEUploaderDownloaderDir.toWx());
  if (m_DebugMode)
    mafLogMessage(_M(_R("Now current working directory is: '") + mafWxToString(wxGetCwd()) + _R("' ")));

  wxArrayString files;
  wxString filePattern = dictionaryFileNamePrefix.toWx() ;
  filePattern.Append("*.xml");

  wxDir::GetAllFiles(wxGetCwd(), &files, filePattern, wxDIR_FILES);
  
  if (files.size() != 1)
  {
    mafLogMessage(_M("lhpXMLDictionary_*.xml not found! exiting"));
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