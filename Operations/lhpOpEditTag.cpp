/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: lhpOpEditTag.cpp,v $
Language:  C++
Date:      $Date: 2008-10-31 18:25:08 $
Version:   $Revision: 1.26.2.4 $
Authors:   Roberto Mucci , Stefano Perticoni
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

#include "lhpOpEditTag.h"

#include "mafTagArray.h"

#include "mafGUI.h"
#include "lhpUser.h"
#include "mafNode.h"
#include "mafVMEGenericAbstract.h"
#include "mafVMEStorage.h"
#include "mafVMERoot.h"

#include "lhpFactoryTagHandler.h"
#include "vtkPolyData.h"

#include <list>
#include <string>
#include <istream>
#include <ostream>

const bool DEBUG_TAGS_PROPAGATION = false;

//----------------------------------------------------------------------------
mafCxxTypeMacro(lhpOpEditTag);
//----------------------------------------------------------------------------
//static variables
mafString lhpOpEditTag::m_CacheSubdir = "0";

enum  m_SubdictionaryId_VALUES
{
  NO_SUBDICTIONARY = 0,
  MOTION_ANALYSIS_SUBDICTIONARY = 1,
  DICOM_SUBDICTIONARY = 2,
  MICROCT_SUBDICTIONARY = 3,
};

enum lhpOpUploadVME_ID
{
  ID_SUBDICTIONARY = MINID, 
  ID_METADATA_EDITOR,
  ID_USEFADICTIONARY,
  ID_PROPAGATE,
};

//----------------------------------------------------------------------------
lhpOpEditTag::lhpOpEditTag(wxString label) :
mafOp(label)
//----------------------------------------------------------------------------
{
	m_OpType  = OPTYPE_OP;
	m_Canundo = false;
  m_HasLink = false;
  m_LinkNode.clear();
  m_LinkName.clear();
  m_User = NULL;

  //m_PythonExe ="C:\\Python25\\python.exe ";
  m_PythonExe ="python.exe ";
  m_PythonwExe ="pythonw.exe ";
  m_CacheDir = (mafGetApplicationDirectory() + "\\VMEUploaderDownloader\\UploadCache\\").c_str();
  m_OutgoingDir = (mafGetApplicationDirectory() + "\\VMEUploaderDownloader\\Outgoing\\").c_str();

  m_PythonUploadFullPath  = (mafGetApplicationDirectory() + "\\VMEUploaderDownloader\\").c_str();
  m_FileName = "";

  m_MsfDir = "";

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
  m_ProxyPort = "";

  m_MetadataEditorId = 0;
  m_UseFADictionary = 0;
  m_DictionaryToProcessFileName = "UNDEFINED";
}

//----------------------------------------------------------------------------
lhpOpEditTag::~lhpOpEditTag()
//----------------------------------------------------------------------------
{
  m_LinkNode.clear();
  m_LinkName.clear();
}
//----------------------------------------------------------------------------
mafOp* lhpOpEditTag::Copy()
//----------------------------------------------------------------------------
{
	/** return a copy of itself, needs to put it into the undo stack */
	return new lhpOpEditTag(m_Label);
}
//----------------------------------------------------------------------------
bool lhpOpEditTag::Accept(mafNode* vme)
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
void lhpOpEditTag::OpRun()
//----------------------------------------------------------------------------
{
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
  event.SetSender(this);
  event.SetId(ID_REQUEST_PROXY);
  mafEventMacro(event);

  if(event.GetString())
  {
    mafString port;
    port << event.GetArg();
    m_ProxyURL = *event.GetString();
    m_ProxyPort = port;
  }

  int result = OP_RUN_CANCEL;

  bool upToDate = false;
  
  if (DEBUG_TAGS_PROPAGATION)
  {
    upToDate = true;
  }
  else
  {
    upToDate = this->IsLHPBuilderVersionUpToDate();
  }
  
  if(upToDate)
  {
    CreateGui();
    ShowGui();
  }
  else
  {
    OpStop(result);
  }  
}

//----------------------------------------------------------------------------
void lhpOpEditTag::LoadConnectionConfigurationFile()
//----------------------------------------------------------------------------
{
  wxString oldDir = wxGetCwd();
  mafLogMessage( _T("Current working directory is: '%s' "), wxGetCwd().c_str() );
  wxSetWorkingDirectory(m_PythonUploadFullPath.GetCStr());
  mafLogMessage( _T("Now current working directory is: '%s' "), wxGetCwd().c_str() );

  // open auto tags file and try to handle tags using tags factory 
  ifstream configurationFile;

  configurationFile.open(m_ConnectionConfigurationFileName.GetCStr());
  if (!configurationFile) {
    wxString message = m_ConnectionConfigurationFileName.GetCStr();
    message.Append(" not found! Unable to open connection configuration file: default values will be used");
    mafLogMessage(message.c_str());
  }
  else
  {
    std::string tmp;

    configurationFile >> tmp;
    m_ProxyURL = tmp.c_str();
    
    configurationFile >> tmp;
    m_ProxyPort = tmp.c_str();
     
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
void lhpOpEditTag::OnEvent(mafEventBase *maf_event) 
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {
    case ID_SUBDICTIONARY:
    {
    //    // nothing to do for the moment...
      mafLogMessage("You choosed dictionary number %i", m_SubdictionaryId);
    }
    break;
    
    case ID_PROPAGATE:
    {
      PropagateTagsToChoosedVMES();
      return;
    }
    break;

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
void lhpOpEditTag::OpDo()   
//----------------------------------------------------------------------------
{
  if (DEBUG_TAGS_PROPAGATION)
  {
    // continue
  }
  else
  {
    if (EditTags()== MAF_ERROR)
    {
      return;
    }
  }
}
//-------------------------------------------------------------------
int lhpOpEditTag::EditTags()
//-------------------------------------------------------------------
{
  m_HasLink = false;
  if (m_Input->GetNumberOfLinks() != 0)
  {
    m_HasLink = true;
    SaveLinkInfo();

    //remove links that will be linked after in ImportMSF()
    m_Input->RemoveAllLinks();
    mafEventMacro(mafEvent(this, MENU_FILE_SAVE));
  }

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

  if (m_MsfDir == "")
  {
    wxMessageBox("Can't edit VME tags: msf must be saved locally");
    return MAF_ERROR;
  }

  int ret = this->GeneratesTagsListsFromXMLDictionary();
  if (ret == MAF_ERROR)
  {
    wxMessageBox("Problems generating tags list! Exiting...");
    return MAF_ERROR;
  } 

  //If doesn't exist yet, append a TagArray:
  if (m_Input->IsA("mafVMERoot") && m_Input->GetTagArray() == NULL)
  {
    mafTagItem rootTag;
    rootTag.SetName("ROOT_TAG");
    m_Input->GetTagArray()->SetTag(rootTag);
  }

  wxString oldDir = wxGetCwd();
  mafLogMessage( _T("Current working directory is: '%s' "), wxGetCwd().c_str() );
  wxSetWorkingDirectory(m_PythonUploadFullPath.GetCStr());
  wxBusyCursor wait;

  //PROCESS EXIST, ONLY CALL CLIENT
  wxString command2execute;
  command2execute = m_PythonExe;
  // script for client
  m_FileName = "lhpEditVMETag.py ";
  command2execute.Append(m_FileName.GetCStr());

  //workaround to understanding directory argument
  wxString directoryWorkAround = m_MsfDir;
  directoryWorkAround.Replace(" ", "?");
  command2execute.Append(wxString::Format("%s ",directoryWorkAround)); //cache directory
  command2execute.Append(wxString::Format("%d ",m_Input->GetId())); //vme id
  command2execute.Append(wxString::Format("%s", m_CsvName.c_str())); //manualTagFile

  mafLogMessage( _T("Executing command: '%s'"), command2execute.c_str() );
  long pid = -1;
  if (pid = wxExecute(command2execute, wxEXEC_SYNC) != 0)
  {
    wxMessageBox("Error in lhpEditVMETag.py");
    mafLogMessage(_T("SYNC Command process '%s' terminated with exit code %d."),
      command2execute.c_str(), pid);
    return MAF_ERROR;
  }

  mafLogMessage(_T("ASYNC Command process '%s' terminated with exit code %d."),
    command2execute.c_str(), pid);

  ImportMSF();
  mafEventMacro(mafEvent(this, MENU_FILE_SAVE));

  // show dialog for tag propagation...
  int propagate = wxMessageBox(wxString::Format("Propagate edited tags to other VMEs?"),\
  "Propagate Tags", wxOK | wxCANCEL | wxCENTRE | wxICON_QUESTION);
  
  if (propagate == wxOK)
  {
    PropagateTagsToChoosedVMES();
  } 
  else if (propagate == wxCANCEL)
  {
    std::ostringstream stringStream;
    stringStream << "Skipping propagation..."  << std::endl;
    mafLogMessage(stringStream.str().c_str());
  }
  return MAF_OK;
}

//-------------------------------------------------------------------
int lhpOpEditTag::ImportMSF()
//-------------------------------------------------------------------
{
  //msf name created by phyton tag editor is standard: OutputMSF.lhp
  mafString msfPythonFileName;
  mafString msfCompletePath;
  msfPythonFileName.Append(m_MsfDir);
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
      m_Input->SetLink(m_LinkName[i].GetCStr(), m_LinkNode[i]);
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
void lhpOpEditTag::SaveLinkInfo()   
//----------------------------------------------------------------------------
{
  m_LinkNode.clear();
  m_LinkName.clear();
  for (mafNode::mafLinksMap::iterator i = m_Input->GetLinks()->begin(); i != m_Input->GetLinks()->end(); i++)
  {
    if (i->second.m_Node != NULL)
    {
      mafNode *link = i->second.m_Node;
      m_LinkNode.push_back(link);
      m_LinkName.push_back(i->first);
    }
  }
}

//----------------------------------------------------------------------------
void lhpOpEditTag::SetDictionary(int subDictionary)   
//----------------------------------------------------------------------------
{
  m_SubdictionaryId = subDictionary;
}
//----------------------------------------------------------------------------
void lhpOpEditTag::OpStop(int result)   
//----------------------------------------------------------------------------
{
  HideGui();
	mafEventMacro(mafEvent(this,result));
}

//----------------------------------------------------------------------------
int lhpOpEditTag::GeneratesTagsListsFromXMLDictionary()
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
  
  m_DictionaryToProcessFileName = m_MasterXMLDictionaryFileName;

  // handle sub dictionaries creation...
  if (m_SubdictionaryId == DICOM_SUBDICTIONARY)
  {
    // build dicom
    m_SubXMLDictionaryFilePrefix = "lhpXMLDicomSourceSubdictionary_";
    m_SubDictionaryBuildingCommand = "dicom";
    if (this->AssembleMasterWithSubdictionary() == MAF_ERROR)
    {
      return MAF_ERROR;
    }
    m_DictionaryToProcessFileName = m_AssembledXMLDictionaryFileName;
  } 
  else if (m_SubdictionaryId == MOTION_ANALYSIS_SUBDICTIONARY)
  {
    // build motion analysis
    m_SubXMLDictionaryFilePrefix = "lhpXMLMotionAnalysisSourceSubdictionary_";
    m_SubDictionaryBuildingCommand = "motion_analysis";
    // build sub dictionary code
    if (this->AssembleMasterWithSubdictionary() == MAF_ERROR)
    {
      return MAF_ERROR;
    }

    m_DictionaryToProcessFileName = m_AssembledXMLDictionaryFileName;
  }
  else if (m_SubdictionaryId == MICROCT_SUBDICTIONARY)
  {
    // build micro ct
    m_SubXMLDictionaryFilePrefix = "lhpXMLMicroCTSourceSubdictionary_";
    m_SubDictionaryBuildingCommand = "micro_ct";
    // build sub dictionary code
    if (this->AssembleMasterWithSubdictionary() == MAF_ERROR)
    {
      return MAF_ERROR;
    }

    m_DictionaryToProcessFileName = m_AssembledXMLDictionaryFileName;
  }
  else if (m_SubdictionaryId == NO_SUBDICTIONARY)
  {
    m_DictionaryToProcessFileName = m_MasterXMLDictionaryFileName;
    std::ostringstream stringStream;
    stringStream << "Not using subdictionaries..."  << std::endl;
    mafLogMessage(stringStream.str().c_str());
    // nothing to do...continue...
  }  
  else
  {
    mafLogMessage("this case is not handled...");
    return MAF_ERROR;
  }
  
  if (m_UseFADictionary)
  {
    int result = AppendFADictionary();

    if (result == MAF_ERROR)
    {
      return MAF_ERROR;
    } 
  } 
  else
  {
      std::ostringstream stringStream;
      stringStream << "Not using FA dictionary..."  << std::endl;
      mafLogMessage(stringStream.str().c_str());
  }

  // get auto tags
  wxString command2execute;
  command2execute.Append(m_PythonExe.GetCStr());
  command2execute.Append(" lhpXMLDictionaryParser.py ");
  command2execute.Append(m_DictionaryToProcessFileName.GetCStr());
  command2execute.Append(" auto_tags ");
  command2execute.Append(m_AutoTagsListFromXMLDictionaryFileName.GetCStr());
  
  mafLogMessage( _T("Executing command: '%s'"), command2execute.c_str() );

  long pid = -1;
  if (pid = wxExecute(command2execute, wxEXEC_SYNC) != 0)
  {
    wxMessageBox("Error in lhpXMLDictionaryParser.py. Uploading stopped");
    mafLogMessage(_T("SYNC Command process '%s' terminated with exit code %d."),
      command2execute.c_str(), pid);
    return MAF_ERROR;
  }

  if ( !command2execute )
    return MAF_ERROR;

  mafLogMessage(_T("SYNC Command process '%s' terminated with exit code %d."),
    command2execute.c_str(), pid);

  // get manual tags
  command2execute.Clear();
  command2execute = m_PythonExe;
  
  command2execute.Append(" lhpXMLDictionaryParser.py ");
  command2execute.Append(m_DictionaryToProcessFileName.GetCStr());
  command2execute.Append(" manual_tags ");
  command2execute.Append(m_ManualTagsListFromXMLDictionaryFileName.GetCStr());

  mafLogMessage( _T("Executing command: '%s'"), command2execute.c_str() );

  pid = -1;
  if (pid = wxExecute(command2execute, wxEXEC_SYNC) != 0)
  {
    wxMessageBox("Error in lhpXMLDictionaryParser.py. Uploading stopped");
    mafLogMessage(_T("SYNC Command process '%s' terminated with exit code %d."),
      command2execute.c_str(), pid);
    return MAF_ERROR;
  }

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
  parametersCargo->SetInputUser(m_User);
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
        tagValue.Append("\",\"");
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
  m_CsvName.Replace(" ", "?"); //replace blank spaces in VME name
  m_CsvName.Replace("?", "_");
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
        unhandledPlusManualTagsFile << "\"" << tagName.GetCStr() << "\",\"" << tagValue.GetCStr() << "\"" << std::endl ;
        tagFound = true;
        break;
      }
    }
    if (!tagFound)
      unhandledPlusManualTagsFile << "\"" << tagName.GetCStr() << "\",\"ANNOTATE ME!\"" << std::endl ;
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
      unhandledPlusManualTagsFile << "\"" << tagName.GetCStr() << "\",\"ANNOTATE ME!\"" << std::endl ;
  }

  unhandledPlusManualTagsFile.close();
  
  if (m_MetadataEditorId == 0)
  {
    // launch new editor
    command2execute.Clear();
    command2execute.Append(m_PythonExe.GetCStr());
    command2execute.Append(" lhpMetadataEditor.py ");
    command2execute.Append(m_CsvName.c_str()); 
    command2execute.Append(" ");
    command2execute.Append(m_DictionaryToProcessFileName.GetCStr());
  } 
  else
  {
    // launch old editor
    command2execute.Clear();
    command2execute.Append(m_PythonExe.GetCStr());
    command2execute.Append(" CSVOMATIC.py ");
    command2execute.Append(m_CsvName.c_str());
  }
  
  
  mafLogMessage( _T("Executing command: '%s'"), command2execute.c_str() );

  pid = wxExecute(command2execute, wxEXEC_SYNC);

  if ( !command2execute )
    return MAF_ERROR;

  mafLogMessage(_T("SYNC Command process '%s' terminated with exit code %d."),
    command2execute.c_str(), pid);

  wxSetWorkingDirectory(oldDir);
  mafLogMessage( _T("Current working directory is: '%s' "), wxGetCwd().c_str() );

  return MAF_OK;
}

//----------------------------------------------------------------------------
bool lhpOpEditTag::IsLHPBuilderVersionUpToDate()
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

  wxArrayString output;
  wxArrayString errors;
  long pid = -1;
  if (pid = wxExecute(command2execute, output, errors, wxEXEC_SYNC) != 0)
  {
    wxMessageBox("Error in lhpDictionaryVersionChecker.py. Uploading stopped");
    mafLogMessage(_T("SYNC Command process '%s' terminated with exit code %d."),
      command2execute.c_str(), pid);
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
mafString lhpOpEditTag::GetXMLDictionaryFileName( mafString dictionaryFileNamePrefix )
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

  wxDir::GetAllFiles(wxGetWorkingDirectory(), &files, filePattern, wxDIR_FILES);
  
  if (files.size() != 1)
  {
    std::ostringstream stringStream;
    
    for (int i = 0; i < files.size(); i++) 
    { 
      stringStream << "found dictionary: " << files[i].c_str()  << std::endl;      
    }
    mafLogMessage(stringStream.str().c_str());
    mafLogMessage("Too much dictionaries found! exiting...");
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
// widget id's
//----------------------------------------------------------------------------

void lhpOpEditTag::CreateGui()
{
  m_Gui = new mafGUI(this);
  
  if (DEBUG_TAGS_PROPAGATION)
  {
    m_Gui->Divider(2);
    m_Gui->Button(ID_PROPAGATE,"test propagate");
  }
  
  m_Gui->Divider(2);
  const wxString metadataEditor[] = {"Metadata Editor","CSV Editor"};
  m_Gui->Label("Choose editor");
  m_Gui->Combo(ID_METADATA_EDITOR, "", &m_MetadataEditorId, 2, metadataEditor);

  m_Gui->Divider(2);
  m_Gui->Label("Use subdictionary");
  wxString subDictionariesList[4] = {"none", "motionAnalysis", "dicom","microCT"};
  m_Gui->Combo(ID_SUBDICTIONARY,"",&m_SubdictionaryId,4,subDictionariesList);

  m_Gui->Divider(2);
  m_Gui->Label("Use FA ontology");
  m_Gui->Bool(ID_USEFADICTIONARY,_(""),&m_UseFADictionary);
  m_Gui->Divider();
  m_Gui->Divider();

  m_Gui->OkCancel(); 
  m_Gui->Label("");
  m_Gui->Update();

}

//----------------------------------------------------------------------------
int lhpOpEditTag::AssembleMasterWithSubdictionary()
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

  wxArrayString output;
  wxArrayString errors;
  long pid = -1;
  if (pid = wxExecute(command2execute, output, errors, wxEXEC_SYNC) != 0)
  {
    wxMessageBox("Error in lhpXMLDictionariesBuilder.py. Uploading stopped");
    mafLogMessage(_T("SYNC Command process '%s' terminated with exit code %d."),
      command2execute.c_str(), pid);
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

int lhpOpEditTag::AppendFADictionary()
{
  mafString faDictionaryFilePrefix = "lhpXMLFASourceSubdictionary_";
  mafString faDictionaryFileName = this->GetXMLDictionaryFileName(faDictionaryFilePrefix).GetCStr();
  if (faDictionaryFileName == "NOT FOUND")
  { 
    std::ostringstream stringStream;
    stringStream << "No FA dictionary found. Exiting..."  << std::endl;
    mafLogMessage(stringStream.str().c_str());
    return MAF_ERROR;
  }

  wxString command2execute;
  command2execute = m_PythonExe;

  mafString assembledWithFaDictionaryFileName = "assembledWithFA.xml";
  command2execute.Append(" lhpXMLDictionariesBuilder.py ");
  command2execute.Append(m_DictionaryToProcessFileName);
  command2execute.Append(" ");
  command2execute.Append(faDictionaryFileName);
  command2execute.Append(" ");
  command2execute.Append("functional_anatomy");
  command2execute.Append(" ");
  command2execute.Append(assembledWithFaDictionaryFileName);

  m_DictionaryToProcessFileName = assembledWithFaDictionaryFileName;
  mafLogMessage( _T("Executing command: '%s'"), command2execute.c_str() );

  wxArrayString output;
  wxArrayString errors;
  long pid = -1;
  if (pid = wxExecute(command2execute, output, errors, wxEXEC_SYNC) != 0)
  {
    wxMessageBox("Error in lhpXMLDictionariesBuilder.py. Uploading stopped");
    mafLogMessage(_T("SYNC Command process '%s' terminated with exit code %d."),
      command2execute.c_str(), pid);
    return MAF_ERROR;
  }

  return MAF_OK;
}

void lhpOpEditTag::PropagateTagsToChoosedVMES()
{
  mafString s(_("Choose target VMEs"));
  mafEvent e(this,VME_CHOOSE, &s);
  e.SetBool(true); //true to create dialog with VME multiselect
  mafEventMacro(e);
  std::vector<mafNode *> nodeVector;
  nodeVector = e.GetVmeVector();
  int size = nodeVector.size();

  std::ostringstream stringStream;
  stringStream << "Vector size: "<< size  << std::endl;
  mafLogMessage(stringStream.str().c_str());

  // for each vme different from the input one
  // copy input edited tags into it

  // get L000 tags from the vme tag array
  std::vector<std::string> tagNamesVector;
  std::vector<std::string>::iterator tagNamesVectorIterator;

  std::map<std::string, std::string> tagsToBeCopiedDictionary;
  std::map<std::string, std::string>::iterator tagsToBeCopiedDictionaryIterator;

  mafTagArray *inputTagArray = m_Input->GetTagArray();
  inputTagArray->GetTagList(tagNamesVector); 

  size = tagNamesVector.size();

  for (int i = 0; i < size; i++) 
  { 
    std::string tagName = tagNamesVector[i];
    std::string stringToSearch = "L0000";

    bool found = false;
    std::ostringstream stringStream;

    int foundPos = -1;
    foundPos = tagName.find(stringToSearch);
    std::string foundTxt;


    if (foundPos == 0)
    {
      foundTxt = "Found";
      std::string tagValue;

      tagValue = inputTagArray->GetTag(tagName.c_str())->GetValue();
      tagsToBeCopiedDictionary[tagName] = tagValue;
    } 
    else
    {
      foundTxt = "NOT Found";
    }

    stringStream << foundTxt << " " << stringToSearch << " in "<< tagName <<  std::endl;
    mafLogMessage(stringStream.str().c_str());
  }

  std::vector<mafNode *>::iterator nodeVectorIterator = nodeVector.begin();
  stringStream.clear();
  stringStream << "The following vme were checked: " << std::endl;
  mafLogMessage(stringStream.str().c_str());


  // for each target vme excluding the input
  while( nodeVectorIterator != nodeVector.end() )
  {
    mafVME *targetVme = mafVME::SafeDownCast(*nodeVectorIterator);
    assert(targetVme);
    std::ostringstream stringStream;
    stringStream << "vme name: " << targetVme->GetName()  << std::endl;
    mafLogMessage(stringStream.str().c_str());

    if (targetVme == m_Input)
    {
      std::ostringstream stringStream;
      stringStream << "Skipping input vme!"  << std::endl;
      mafLogMessage(stringStream.str().c_str());
    } 
    else
    {
      std::ostringstream stringStream;
      stringStream << "Copying to "  << targetVme->GetName() << std::endl;
      mafLogMessage(stringStream.str().c_str());

      // get the target vme tag array
      mafTagArray *targetTagArray = targetVme->GetTagArray();

      tagsToBeCopiedDictionaryIterator = tagsToBeCopiedDictionary.begin();

      // for every tag in source:
      while (tagsToBeCopiedDictionaryIterator != tagsToBeCopiedDictionary.end()) 
      { 
        std::string key = tagsToBeCopiedDictionaryIterator->first;
        std::string val = tagsToBeCopiedDictionaryIterator->second;
        std::ostringstream stringStream;
        stringStream << "Copying " << key << " " << val << " to " << targetVme->GetName() << std::endl;
        mafLogMessage(stringStream.str().c_str());
        targetTagArray->SetTag(key.c_str(), val.c_str());
        tagsToBeCopiedDictionaryIterator++;
      }

    }

    nodeVectorIterator++;
  }
  return;
}