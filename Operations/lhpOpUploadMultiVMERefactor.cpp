/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: lhpOpUploadMultiVMERefactor.cpp,v $
Language:  C++
Date:      $Date: 2009-05-22 10:49:42 $
Version:   $Revision: 1.1.2.16 $
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

#include "lhpOpUploadMultiVMERefactor.h"
#include "lhpOpUploadVMERefactor.h"

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
mafCxxTypeMacro(lhpOpUploadMultiVMERefactor);
//----------------------------------------------------------------------------
//static variables
mafString lhpOpUploadMultiVMERefactor::m_CacheSubdir = "0";

enum lhpOpUploadMultiVME_ID
{
  ID_SUBDICTIONARY = MINID, 
};

//----------------------------------------------------------------------------
lhpOpUploadMultiVMERefactor::lhpOpUploadMultiVMERefactor(wxString label) :
mafOp(label)
//----------------------------------------------------------------------------
{
	m_OpType  = OPTYPE_OP;
	m_Canundo = false;
  m_WithChild = false;
  m_DebugMode = false;
  m_AlreadyUploadedXMLURIVector.clear();
  m_AlreadyUploadedVMEVector.clear();
  m_EmptyNodeVector.clear();
  m_FileCreatedVector.clear();
  m_DerivedVMEsIdVector.clear();
  m_VMEsToUploadIdsVector.clear();
  m_OpUploadVME = NULL;
  //m_ServiceURL = "http://devel.fec.cineca.it:12680/town/biomed_town/LHDL/users/repository/lhprepository2/";
  m_ServiceURL = "https://www.biomedtown.org/biomed_town/LHDL/users/repository/lhprepository2/";
 
  m_PythonExe = "python.exe_UNDEFINED";

  m_VMEUploaderDownloaderABSFolder  = (lhpUtils::lhpGetApplicationDirectory() + "\\VMEUploaderDownloaderRefactor\\").c_str();

  m_MasterXMLDictionaryFileName = "UNDEFINED";
  m_SubXMLDictionaryFilePrefix = "UNDEFINED" ;
  m_SubXMLDictionaryFileName = "UNDEFINED";
  m_AssembledXMLDictionaryFileName = "assembledXMLDictionary.xml";
  m_SubDictionaryBuildingCommand = "UNDEFINED";

  m_SubdictionaryId = 0; // NO_SUBDICTIONARY; 
  m_ConnectionConfigurationFileName = "vmeUploaderConnectionConfiguration.conf" ;
  SetListener(this);
}

//----------------------------------------------------------------------------
lhpOpUploadMultiVMERefactor::~lhpOpUploadMultiVMERefactor()
//----------------------------------------------------------------------------
{
  m_AlreadyUploadedXMLURIVector.clear();
  m_AlreadyUploadedVMEVector.clear();
  m_EmptyNodeVector.clear();
  m_FileCreatedVector.clear();
  m_DerivedVMEsIdVector.clear();
  
  // this is crashing the app
  // cppDEL(m_OpUploadVME);
}

//----------------------------------------------------------------------------
bool lhpOpUploadMultiVMERefactor::Accept(mafNode* vme)
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
mafOp* lhpOpUploadMultiVMERefactor::Copy()
//----------------------------------------------------------------------------
{
	/** return a copy of itself, needs to put it into the undo stack */
	return new lhpOpUploadMultiVMERefactor(m_Label);
}

//----------------------------------------------------------------------------
void lhpOpUploadMultiVMERefactor::OpRun()
//----------------------------------------------------------------------------
{
  if (GetTestMode() == true)
  {
    m_OpUploadVME = new lhpOpUploadVMERefactor("vmeUploader");
  } 
  else
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
    m_ProxyPort = m_User->GetProxyPort();
    // load the connection configuration file:
    this->SaveConnectionConfigurationFile();
  }
  else
  {
    wxString oldDir = wxGetCwd();
    if (m_DebugMode)
      mafLogMessage( _T("Current working directory is: '%s' "), wxGetCwd().c_str() );
    wxSetWorkingDirectory(m_VMEUploaderDownloaderABSFolder.GetCStr());
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

  mafString DebugPath = m_VMEUploaderDownloaderABSFolder;
  DebugPath.Append("\\Debug.py");
  if (wxFileExists(DebugPath.GetCStr()))
  {
    ifstream debugFile;
    debugFile.open(DebugPath.GetCStr());
    if (!debugFile) {
      mafLogMessage("Unable to open Debug.py file");
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

  mafString s(_("Upload VMEs"));
  mafEvent e(this,VME_CHOOSE, &s);
  e.SetBool(true); //true to create dialog with VME multiselect
  mafEventMacro(e);
  m_VMEToBeUploadedVector = e.GetVmeVector();
  int size = m_VMEToBeUploadedVector.size();
  if (size == 0)
  {
    OpStop(result);
    return;
  }
  
  m_OpUploadVME = new lhpOpUploadVMERefactor("vmeUploader");
  m_OpUploadVME->SetDebugMode(m_DebugMode);
  m_OpUploadVME->SetListener(this->GetListener());

  bool upToDate = false;
  upToDate = m_OpUploadVME->IsClientSoftwareVersionUpToDate();

  if (upToDate)
  {
    mafEventMacro(mafEvent(this, MENU_FILE_SAVE));
    //logic comunicate the msf directory
    mafEvent event;
    event.SetSender(this);
    event.SetId(ID_MSF_DATA_CACHE);
    mafEventMacro(event);

    wxString temp;
    temp.Append((*event.GetString()).GetCStr());
    m_OpenMSFFileNameFullPath = temp;
    temp = temp.BeforeLast('/');
    mafString openMsfABSFolder = temp;  

    if (openMsfABSFolder == "")
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
  
}

//----------------------------------------------------------------------------
void lhpOpUploadMultiVMERefactor::OpDo()   
//----------------------------------------------------------------------------
{
  Upload();
  return;
}
//------------------------------------------------------------
void lhpOpUploadMultiVMERefactor::SaveConnectionConfigurationFile()
//------------------------------------------------------------
{
  wxString oldDir = wxGetCwd();
  if (m_DebugMode)
    mafLogMessage( _T("Current working directory is: '%s' "), wxGetCwd().c_str() );
  wxSetWorkingDirectory(m_VMEUploaderDownloaderABSFolder.GetCStr());
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


int lhpOpUploadMultiVMERefactor::UploadVMEWithItsChildren( mafNode *vme )
{
  //creates a file with a list of uploaded xml dataresources, to be used by
  //python to remove xml uploaded in case of msf upload error
  
  int number = 0;
  mafString rollBackLocalFileName = "msfList";
  mafString xmlDataResourcesRollBackLocalFileName = rollBackLocalFileName;

  while(wxFileExists(\
  m_VMEUploaderDownloaderABSFolder + xmlDataResourcesRollBackLocalFileName.GetCStr() + ".lhp"))
  {
    xmlDataResourcesRollBackLocalFileName = rollBackLocalFileName;
    xmlDataResourcesRollBackLocalFileName << number;
    number++;
  }

  mafString msfFilePath = m_VMEUploaderDownloaderABSFolder + xmlDataResourcesRollBackLocalFileName.GetCStr() + ".lhp";
  FILE *file = fopen(msfFilePath,"w");
  fclose(file);
  xmlDataResourcesRollBackLocalFileName << ".lhp";

  std::vector<const mafNode::mafChildrenVector*> childrenVector;
  childrenVector.clear();
  bool hasBinaryData = false;
  bool alreadyUploaded = false;
  bool emptyNode = false;

  int numVmeChild = 0;
  int i = 0;
  mafNode *childToUpload = NULL;
  
  const mafNode::mafChildrenVector *children;
  children = vme->GetChildren();
  childrenVector.push_back(children);
  numVmeChild = children->size();
  if (numVmeChild != 0)
    childToUpload = children->at(0);

    std::ostringstream stringStream;
    stringStream << "childToUpload:" << childToUpload->GetName() << std::endl;
    mafLogMessage(stringStream.str().c_str());
          
  // upload children...
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

        //Check if children has been already uploaded:
        for (int c = 0; c < m_AlreadyUploadedVMEVector.size(); c++) 
        {
          if (m_AlreadyUploadedVMEVector[c]->Equals(children->at(i)) && \
              m_AlreadyUploadedVMEVector[c]->GetId() == children->at(i)->GetId())
          {
            alreadyUploaded = true;
            break;
          }
        }
        
        // if children not uploaded already:
        if (!alreadyUploaded)
        {
          if (childToUpload->GetNumberOfLinks() != 0)
          {
            m_DerivedVMEsIdVector.push_back(childToUpload->GetId());
          }

          m_OpUploadVME->SetInput(childToUpload);

          if (GetUploadError())
          {
            RemoveAlreadyUploadedXMLResources(m_AlreadyUploadedXMLURIVector);
            return MAF_ERROR;
          }
          
          m_OpUploadVME->SetWithChild(childToUpload->GetNumberOfChildren()!=0);
          m_OpUploadVME->SetXMLUploadedResourcesRollBackLocalFileName(xmlDataResourcesRollBackLocalFileName);
          m_OpUploadVME->SetIsLast(false);

          if (m_OpUploadVME->Upload()\
          == MAF_ERROR)
          {
            RemoveAlreadyUploadedXMLResources(m_AlreadyUploadedXMLURIVector);
            return MAF_ERROR;
          }

          m_AlreadyUploadedVMEVector.push_back(childToUpload);
          m_EmptyNodeVector.push_back(childToUpload);
          m_AlreadyUploadedXMLURIVector.push_back(m_OpUploadVME->GetRemoteXMLResourceURI());
          if (SaveChildrenURIFile(childToUpload, m_OpUploadVME->GetRemoteXMLResourceURI()) \
          == MAF_ERROR)
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

  // finally upload VME Root
  m_OpUploadVME->SetInput(vme);

  if (GetUploadError())
  {
    RemoveAlreadyUploadedXMLResources(m_AlreadyUploadedXMLURIVector);
    return MAF_ERROR;
  }
    
  m_OpUploadVME->SetWithChild(false);
  m_OpUploadVME->SetXMLUploadedResourcesRollBackLocalFileName(xmlDataResourcesRollBackLocalFileName);
  m_OpUploadVME->SetIsLast(true);

  if (m_OpUploadVME->Upload() == MAF_ERROR)
  {
    RemoveAlreadyUploadedXMLResources(m_AlreadyUploadedXMLURIVector);
    return MAF_ERROR;
  }

  m_AlreadyUploadedVMEVector.push_back(vme);
  m_AlreadyUploadedXMLURIVector.push_back(m_OpUploadVME->GetRemoteXMLResourceURI());
  
  if (AddLinksURIToDerivedVMEMetadata(vme) == MAF_ERROR)
  {
    RemoveAlreadyUploadedXMLResources(m_AlreadyUploadedXMLURIVector);
    return MAF_ERROR;
  }
  
  return MAF_OK;
  
}

int lhpOpUploadMultiVMERefactor::UploadVMEWithItsLinks( mafNode *vme, bool isLast )
{  
  bool hasBinaryData = false;
  mafString xmlURI;

  if (vme->GetNumberOfLinks() != 0)
  {
    if (UploadVMELinks(vme) == MAF_ERROR)
    {
      return MAF_ERROR;
    }
  }

  m_OpUploadVME->SetInput(vme);
  xmlURI = "";

  m_OpUploadVME->SetWithChild(false);
  m_OpUploadVME->SetXMLUploadedResourcesRollBackLocalFileName("noMSF");
  m_OpUploadVME->SetIsLast(true);

  if (m_OpUploadVME->Upload() == MAF_ERROR)
  {
    m_AlreadyUploadedXMLURIVector.clear();
    m_AlreadyUploadedXMLURIVector.push_back(m_OpUploadVME->GetRemoteXMLResourceURI());
    RemoveAlreadyUploadedXMLResources(m_AlreadyUploadedXMLURIVector);
    return MAF_ERROR;
  }

  return MAF_OK;
}

bool lhpOpUploadMultiVMERefactor::GetUploadError()   
{
  bool errorFound = false;
  if (wxFileExists(m_VMEUploaderDownloaderABSFolder + "ErrorFound.lhp"))
  {
    mafString errorMessage;
    std::ifstream errorFile(m_VMEUploaderDownloaderABSFolder + "ErrorFound.lhp", std::ios::in);
    if (errorFile)
    {
      std::string buf;
      getline(errorFile, buf);
      errorMessage.Append(buf.c_str());
      errorMessage.Append("\n");
      while (!errorFile.eof())
      {
        getline(errorFile, buf);
        errorMessage.Append(buf.c_str());
      }

      wxMessageBox(wxString::Format("Error in MSF upload.\n%s\nUpload MSF stopped.\nVME already uploaded \
will be removed from repository.",errorMessage.GetCStr()), wxMessageBoxCaptionStr, wxSTAY_ON_TOP | wxOK);
      errorFile.close();
      wxRemoveFile(m_VMEUploaderDownloaderABSFolder + "ErrorFound.lhp");
      errorFound = true;
    }
  }
  return errorFound;
}

//----------------------------------------------------------------------------
bool lhpOpUploadMultiVMERefactor::RemoveAlreadyUploadedXMLResources(std::vector<mafString> xmlURIVector)   
//----------------------------------------------------------------------------
{
  wxString oldDir = wxGetCwd();
  if (m_DebugMode)
    mafLogMessage( _T("Current working directory is: '%s' "), wxGetCwd().c_str() );
  wxSetWorkingDirectory(m_VMEUploaderDownloaderABSFolder.GetCStr());

  wxString command2execute;

  //WAIT MESSAGE:
  wxInfoFrame *wait;
  if(!m_TestMode)
  {
    wait = new wxInfoFrame(NULL, "Please wait, removing uploaded VME");
    wait->SetWindowStyleFlag(wxSTAY_ON_TOP); //to keep wait message on top
    wait->Show(true);
    wait->Refresh();
    wait->Update();
  }

  for (int i = 0; i < xmlURIVector.size(); i++)
  {
    command2execute.Clear();
    command2execute = "pythonw.exe ";

    // remove XML resource from repository
    command2execute.Append(" lhpRemoveXMLResource.py ");
    command2execute.Append(m_User->GetName());
    command2execute.Append(" ");
    command2execute.Append(m_User->GetPwd());
    command2execute.Append(" ");
    command2execute.Append(xmlURIVector[i].GetCStr());

    if (m_DebugMode)
      mafLogMessage( _T("Executing command: '%s'"), command2execute.c_str() );

    long pid = -1;
    if (pid = wxExecute(command2execute, wxEXEC_SYNC) != 0)
    {
      wxMessageBox("Error in lhpRemoveXMLResource. Can not remove uploaded resource", wxMessageBoxCaptionStr, wxSTAY_ON_TOP | wxOK);
      if (m_DebugMode)
        mafLogMessage(_T("ASYNC Command process '%s' terminated with exit code %d."),
        command2execute.c_str(), pid);
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

  return MAF_OK;
}

//----------------------------------------------------------------------------
int lhpOpUploadMultiVMERefactor::AddLinksURIToDerivedVMEMetadata( mafNode *derived )
{  
  wxString vmeURI;
  mafString listURI;
  mafNode *currentDerivedVME;

  for (int i = 0; i < m_DerivedVMEsIdVector.size(); i++)
  {
    currentDerivedVME = derived->FindInTreeById(m_DerivedVMEsIdVector[i]);

    //find URI of this VME
    for (int c = 0; c < m_AlreadyUploadedVMEVector.size(); c++)
    {
      if (m_AlreadyUploadedVMEVector[c]->Equals(currentDerivedVME) && \
          m_AlreadyUploadedVMEVector[c]->GetId() == currentDerivedVME->GetId())
      {
        vmeURI = m_AlreadyUploadedXMLURIVector[c];
        int count = vmeURI.find_first_of("'");
        vmeURI.erase(0, count+1);
        count = vmeURI.find_first_of("'");
        vmeURI = (vmeURI.substr(0, count)).c_str();
        break;
      }
    }

    for (mafNode::mafLinksMap::iterator i = currentDerivedVME->GetLinks()->begin(); \
      i != currentDerivedVME->GetLinks()->end(); i++)
    {
      if (i->second.m_Node != NULL)
      {
        mafNode *link = i->second.m_Node;
        for (int c = 0; c < m_AlreadyUploadedVMEVector.size(); c++)
        {
          if (m_AlreadyUploadedVMEVector[c]->Equals(link) && \
              m_AlreadyUploadedVMEVector[c]->GetId() == link->GetId())
          {
            m_AlreadyUploadedXMLURIVector[c];
            listURI.Append(m_AlreadyUploadedXMLURIVector[c]);
            listURI.Append(" ");
            break;
          }
        }
      }
    }

    wxString oldDir = wxGetCwd();
    wxSetWorkingDirectory(m_VMEUploaderDownloaderABSFolder.GetCStr());
    if (m_DebugMode)
      mafLogMessage( _T("Now current working directory is: '%s' "), wxGetCwd().c_str() );

    //Add URI tag to link VME uploaded
    wxString command2execute;
    command2execute.Clear();
    command2execute = m_PythonExe.GetCStr();

    command2execute.Append("lhpEditRemoteTag.py ");
    command2execute.Append(m_User->GetName());
    command2execute.Append(" ");
    command2execute.Append(m_User->GetPwd());
    command2execute.Append(" ");
    command2execute.Append(m_ServiceURL.GetCStr());
    command2execute.Append(" ");
    command2execute.Append(vmeURI.c_str());
    command2execute.Append(",");
    command2execute.Append("L0000_resource_MAF_Procedural_VMElinkURI1");
    command2execute.Append(",");
    command2execute.Append(listURI.GetCStr());
    if (m_DebugMode)
      mafLogMessage( _T("Executing command: '%s'"), command2execute.c_str() );

    long pid = -1;
    if (pid = wxExecute(command2execute, wxEXEC_SYNC) != 0)
    {
      wxMessageBox("Error in lhpEditRemoteTag.py. Uploading stopped", wxMessageBoxCaptionStr, wxSTAY_ON_TOP | wxOK);
      mafLogMessage(_T("SYNC Command process '%s' terminated with exit code %d."),
        command2execute.c_str(), pid);
      return MAF_ERROR;
    }

    wxSetWorkingDirectory(oldDir);
    if (m_DebugMode)
      mafLogMessage( _T("Current working directory is: '%s' "), wxGetCwd().c_str() );
  }
  return MAF_OK;

}
//----------------------------------------------------------------------------
int lhpOpUploadMultiVMERefactor::UploadVMELinks( mafNode *derived )
{  
  bool emptyNode = false;
  bool alreadyUploaded = false;
  bool hasBinaryData = false;
  std::vector<mafString> linkURI;
  linkURI.clear();


  for (mafNode::mafLinksMap::iterator i = derived->GetLinks()->begin(); i != derived->GetLinks()->end(); i++)
  {
    hasBinaryData = false;
    alreadyUploaded = false;
    mafString xmlURI;
    if (i->second.m_Node != NULL)
    {
      mafNode *link = i->second.m_Node;
      if (link->IsA("mafVMELandmarkCloud") && i->second.m_NodeSubId != -1)
      {
        ((mafVMELandmarkCloud *)link)->Open();
        link = (mafNode*)((mafVMELandmarkCloud *)link)->GetLandmark(i->second.m_NodeSubId);
      }

      wxMessageBox(wxString::Format("Link found! Upload VME: %s", link->GetName()), wxMessageBoxCaptionStr, wxSTAY_ON_TOP | wxOK);

      //Verify if the link has some link!!
      if (link->GetNumberOfLinks() != 0)
      {
        if (UploadVMELinks(link) == MAF_ERROR)
        {
          return MAF_ERROR;
        }
      }
      m_OpUploadVME->SetInput(link);

      xmlURI = "";

      m_OpUploadVME->SetWithChild(false);
      m_OpUploadVME->SetXMLUploadedResourcesRollBackLocalFileName("noMsf");
      m_OpUploadVME->SetIsLast(false);

      if (m_OpUploadVME->Upload() == MAF_ERROR)
      { 

        m_AlreadyUploadedXMLURIVector.clear();
        m_AlreadyUploadedXMLURIVector.push_back(m_OpUploadVME->GetRemoteXMLResourceURI());
        RemoveAlreadyUploadedXMLResources(m_AlreadyUploadedXMLURIVector);
        return MAF_ERROR;
      }
      m_AlreadyUploadedVMEVector.push_back(link);
      linkURI.push_back(xmlURI);
    }
  }
  if (SaveLinksURIFile(derived, linkURI) == MAF_ERROR)
  {
    wxMessageBox("Unable to write list of link binary URI. Uploading stopped.", wxMessageBoxCaptionStr, wxSTAY_ON_TOP | wxOK);
    return MAF_ERROR;
  }
  return MAF_OK;
}

//------------------------------------------------------------
int lhpOpUploadMultiVMERefactor::SaveLinksURIFile(mafNode *node, std::vector<mafString> linkURI)
//------------------------------------------------------------
{
  wxString listURIFileName;
  wxString oldDir = wxGetCwd();
  if (m_DebugMode)
    mafLogMessage( _T("Current working directory is: '%s' "), wxGetCwd().c_str() );
  wxSetWorkingDirectory(m_VMEUploaderDownloaderABSFolder.GetCStr());
  if (m_DebugMode)
    mafLogMessage( _T("Now current working directory is: '%s' "), wxGetCwd().c_str() );

  listURIFileName = node->GetName();
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
  wxString lockPath = m_VMEUploaderDownloaderABSFolder;
  lockPath += listURIFileName.c_str();
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
  if (m_DebugMode)
    mafLogMessage( _T("Current working directory is: '%s' "), wxGetCwd().c_str() );
  return MAF_OK;
}

//------------------------------------------------------------
int lhpOpUploadMultiVMERefactor::SaveChildrenURIFile(mafNode* node, mafString URI)
//------------------------------------------------------------
{
  wxString listURIFileName;
  wxString oldDir = wxGetCwd();
  if (m_DebugMode)
    mafLogMessage( _T("Current working directory is: '%s' "), wxGetCwd().c_str() );
  wxSetWorkingDirectory(m_VMEUploaderDownloaderABSFolder.GetCStr());
  if (m_DebugMode)
    mafLogMessage( _T("Now current working directory is: '%s' "), wxGetCwd().c_str() );

  listURIFileName = node->GetParent()->GetName();
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

  wxString lockPath = m_VMEUploaderDownloaderABSFolder;
  lockPath += listURIFileName.c_str();
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
    if (wxFileExists(lockPath.c_str()))
      wxRemoveFile(lockPath.c_str());
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
  if (m_DebugMode)
    mafLogMessage( _T("Current working directory is: '%s' "), wxGetCwd().c_str() );
  return MAF_OK;
}

//----------------------------------------------------------------------------
void lhpOpUploadMultiVMERefactor::OpStop(int result)   
//----------------------------------------------------------------------------
{
	mafEventMacro(mafEvent(this,result));
}

//--------------------------------------------------------------------------------------------
mafString lhpOpUploadMultiVMERefactor::GetXMLDictionaryFileName( mafString dictionaryFileNamePrefix )
//--------------------------------------------------------------------------------------------
{
  mafString dictionaryFileName = "NOT FOUND";
  wxString oldDir = wxGetCwd();

  if (m_DebugMode)
    mafLogMessage( _T("Current working directory is: '%s' "), wxGetCwd().c_str() );
  wxSetWorkingDirectory(m_VMEUploaderDownloaderABSFolder.GetCStr());
  if (m_DebugMode)
    mafLogMessage( _T("Now current working directory is: '%s' "), wxGetCwd().c_str() );

  wxArrayString files;
  wxString filePattern = dictionaryFileNamePrefix ;
  filePattern.Append("*.xml");

  wxDir::GetAllFiles(wxGetWorkingDirectory(), &files, filePattern, wxDIR_FILES);
  
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

void lhpOpUploadMultiVMERefactor::Upload()
{
  
  bool rootSelected = false;

  if (GetTestMode() == true)
  {
    mafLogMessage("All Vmes will be uploaded with their metadata.\nPlease check before uploading. Completion of curation can be done in the sandbox.");
  }
  else
  {
    int res = wxMessageBox("All Vmes will be uploaded with their metadata.\nPlease check before uploading. Completion of curation can be done in the sandbox.", wxMessageBoxCaptionStr, wxOK | wxCANCEL);
    //returns 4 for OK, 16 for CANCEL
    if (res == 16)
    {
      return;
    }
  }

  wxInfoFrame *wait;
  if(!m_TestMode)
  {
    wait = new wxInfoFrame(NULL, "Please wait, uploading VME");
    wait->SetWindowStyleFlag(wxSTAY_ON_TOP); //to keep wait message on top
    wait->Show(true);
    wait->Refresh();
    wait->Update();
  }

  //Check if VMERoot has been chosen, than upload only the root with its children
  
  // root selected?
  for (int i = 0; i < m_VMEToBeUploadedVector.size(); i++)
  {
    if (m_VMEToBeUploadedVector[i]->IsA("mafVMERoot"))
    {
      rootSelected = true;
      //if exists, remove error file form python
      if (wxFileExists(m_VMEUploaderDownloaderABSFolder + "ErrorFound.lhp"))
      {
        wxRemoveFile(m_VMEUploaderDownloaderABSFolder + "ErrorFound.lhp");
      }

      //Check if VME without name are present
      mafNode *vmeWithNoName = NULL;

      vmeWithNoName = m_VMEToBeUploadedVector[i]->FindInTreeByName("");

      if  (vmeWithNoName != NULL)
      {
        wxMessageBox("Tree contains VME without name. Upload Stopped.", wxMessageBoxCaptionStr, wxSTAY_ON_TOP | wxOK);
        return;
      }

      wxString rootName = m_VMEToBeUploadedVector[i]->GetName();
      if  (rootName.CompareTo("root") == 0)
      {
        wxString path, newRootName, ext;
        wxString msfFullPathName = m_OpenMSFFileNameFullPath;
        wxSplitPath(msfFullPathName, &path, &newRootName, &ext);
        wxMessageBox(wxString::Format("Root name will be modified with the name as the MSF: %s ",newRootName), wxMessageBoxCaptionStr, wxSTAY_ON_TOP | wxOK);
        m_VMEToBeUploadedVector[i]->SetName(newRootName.c_str());
        m_VMEToBeUploadedVector[i]->Modified();
        ((mafVMERoot *)m_VMEToBeUploadedVector[i])->Update();

        mafEvent ev(this,VME_MODIFIED,m_VMEToBeUploadedVector[i]);
        mafEventMacro(ev);
      }
      
      // upload root with its children vmes
      if (UploadVMEWithItsChildren(m_VMEToBeUploadedVector[i]) == MAF_ERROR)
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

  // root not selected  =>
  if (!rootSelected)
  {
    bool isLast = false;
    // for each selected vme:
    for (int i = 0; i < m_VMEToBeUploadedVector.size(); i++)
    {
      if (strcmp(m_VMEToBeUploadedVector[i]->GetName(), "") == 0)
      {
        wxMessageBox("Can not upload VME without name.", wxMessageBoxCaptionStr, wxSTAY_ON_TOP | wxOK);
        return;
      }

      //check if is last VME
      if (i+1 == m_VMEToBeUploadedVector.size())
        isLast = true;

      if (UploadVMEWithItsLinks(m_VMEToBeUploadedVector[i], isLast) == MAF_ERROR)
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

 // cppDEL(m_OpUploadVME);
}

int lhpOpUploadMultiVMERefactor::LoadVMEsToUploadIdsVectorFromFile(const char *vmeIdsFileName)
{
  std::ifstream inputFile(vmeIdsFileName, std::ios::in);

  if (!inputFile) {
    std::cerr << "Error opening " << vmeIdsFileName << "\n";
    assert(false);
    return MAF_ERROR;
  }

  int buf;
  
  m_VMEsToUploadIdsVector.clear();

  while(inputFile >> buf)
  {
    m_VMEsToUploadIdsVector.push_back(buf);
  }

  inputFile.close();

  assert(m_Input);

  mafNode *root = m_Input->GetRoot();
  assert(root);

  m_VMEToBeUploadedVector.clear();

  for (int i = 0; i < m_VMEsToUploadIdsVector.size(); i++) 
  {
    mafNode *node = root->FindInTreeById(m_VMEsToUploadIdsVector[i]);
    m_VMEToBeUploadedVector.push_back(node);
    assert(node);
    if (node == NULL)
    {
      std::ostringstream stringStream;
      stringStream << "Node with id: " << m_VMEToBeUploadedVector[i] << \
        " does not exist in VME tree. Exiting with MAF_ERROR" << std::endl;
      mafLogMessage(stringStream.str().c_str());
      m_VMEToBeUploadedVector.clear();
      return MAF_ERROR;
    }
  }


  return MAF_OK ;
}

int lhpOpUploadMultiVMERefactor::SetVMEsToUploadIdsVector( std::vector<int> vmeIDsVector )
{

  m_VMEsToUploadIdsVector.clear();

  m_VMEsToUploadIdsVector = vmeIDsVector;

  assert(m_Input);

  mafNode *root = m_Input->GetRoot();
  assert(root);

  m_VMEToBeUploadedVector.clear();

  for (int i = 0; i < m_VMEsToUploadIdsVector.size(); i++) 
  {
    mafNode *node = root->FindInTreeById(m_VMEsToUploadIdsVector[i]);
    m_VMEToBeUploadedVector.push_back(node);
    assert(node);
    if (node == NULL)
    {
        std::ostringstream stringStream;
        stringStream << "Node with id: " << m_VMEToBeUploadedVector[i] << \
        " does not exist in VME tree. Exiting with MAF_ERROR" << std::endl;
        mafLogMessage(stringStream.str().c_str());
        m_VMEToBeUploadedVector.clear();
        return MAF_ERROR;
    }
  }

  return MAF_OK ;
}
