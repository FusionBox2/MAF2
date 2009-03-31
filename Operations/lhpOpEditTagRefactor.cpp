/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: lhpOpEditTagRefactor.cpp,v $
Language:  C++
Date:      $Date: 2009-03-31 16:43:23 $
Version:   $Revision: 1.1.2.2 $
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

#include "lhpOpEditTagRefactor.h"
#include "lhpTagHandlerRefactor.h"

#include "mafTagArray.h"

#include "mafGUI.h"
#include "lhpUser.h"
#include "mafNode.h"
#include "mafVMEGenericAbstract.h"
#include "mafVMEStorage.h"
#include "mafVMERoot.h"

#include "lhpFactoryTagHandlerRefactor.h"
#include "vtkPolyData.h"
#include "mafTagArray.h"

#include <list>
#include <string>
#include <istream>
#include <ostream>

const bool DEBUG_TAGS_PROPAGATION = false;

//----------------------------------------------------------------------------
mafCxxTypeMacro(lhpOpEditTagRefactor);
//----------------------------------------------------------------------------
//static variables
mafString lhpOpEditTagRefactor::m_CacheSubdir = "0";

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
  ID_USE_DICOM_SUBDICTIONARY,
  ID_USE_FA_SUBDICTIONARY,
  ID_USE_MA_SUBDICTIONARY,
  ID_USE_MICROCT_SUBDICTIONARY,
};

//----------------------------------------------------------------------------
lhpOpEditTagRefactor::lhpOpEditTagRefactor(wxString label) :
mafOp(label)
//----------------------------------------------------------------------------
{
	m_OpType  = OPTYPE_OP;
	m_Canundo = false;
  m_HasLink = false;
  m_DebugMode = false;
  m_LinkNode.clear();
  m_LinkName.clear();
  m_User = NULL;

  m_PythonExe = "python.exe_UNDEFINED";
  m_PythonwExe = "pythonw.exe_UNDEFINED";  

  m_CacheDir = (lhpUtils::lhpGetApplicationDirectory() + "\\VMEUploaderDownloaderRefactor\\UploadCache\\").c_str();
  m_OutgoingDir = (lhpUtils::lhpGetApplicationDirectory() + "\\VMEUploaderDownloaderRefactor\\Outgoing\\").c_str();

  m_VMEUploaderDownloaderDir  = (lhpUtils::lhpGetApplicationDirectory() + "\\VMEUploaderDownloaderRefactor\\").c_str();
  m_FileName = "";
  m_CsvName = "manualTagFile.csv";
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

  m_MetadataEditorId = 0;
  m_UseFADictionary = 0;
  m_DictionaryToProcessFileName = "UNDEFINED";

  m_UseDicomSubdictionary = 0;
  m_UseFASubdictionary = 0;
  m_UseMicroCTSubdictionary = 0;
  m_UseMASubdictionary = 0;

}

//----------------------------------------------------------------------------
lhpOpEditTagRefactor::~lhpOpEditTagRefactor()
//----------------------------------------------------------------------------
{
  m_LinkNode.clear();
  m_LinkName.clear();
}
//----------------------------------------------------------------------------
mafOp* lhpOpEditTagRefactor::Copy()
//----------------------------------------------------------------------------
{
	/** return a copy of itself, needs to put it into the undo stack */
	return new lhpOpEditTagRefactor(m_Label);
}
//----------------------------------------------------------------------------
bool lhpOpEditTagRefactor::Accept(mafNode* vme)
//----------------------------------------------------------------------------
{
  //lhpUser *user = NULL;
  ////Get User values
  //mafEvent event;
  //event.SetSender(this);
  //event.SetId(ID_REQUEST_USER);
  //mafEventMacro(event);
  //if(event.GetMafObject() != NULL) //if proxy string contains something != ""
  //{
  //  user = (lhpUser*)event.GetMafObject();
  //}
  return (vme != NULL);
}

//----------------------------------------------------------------------------
void lhpOpEditTagRefactor::OpRun()
//----------------------------------------------------------------------------
{ 
  LoadUsedDictionariesFromTags();

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


  //Get User values
  mafEvent eventGetUser;
  eventGetUser.SetSender(this);
  eventGetUser.SetId(ID_REQUEST_USER);
  mafEventMacro(eventGetUser);
  if(eventGetUser.GetMafObject() != NULL) 
  {
    m_User = (lhpUser*)eventGetUser.GetMafObject();
  }

  mafString DebugPath = m_VMEUploaderDownloaderDir;
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
      m_PythonwExe = m_PythonExe.GetCStr();
      m_DebugMode = true;
    }
    debugFile.close();
  }
  mafEventMacro(mafEvent(this, MENU_FILE_SAVE));
  
  CreateGui();
  ShowGui();

}

//----------------------------------------------------------------------------
void lhpOpEditTagRefactor::OnEvent(mafEventBase *maf_event) 
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {
    case ID_SUBDICTIONARY:
    {
      if (m_DebugMode)
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
void lhpOpEditTagRefactor::OpDo()   
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
int lhpOpEditTagRefactor::EditTags()
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

  if(!CreateBaseCacheDirectories())
  {
    wxMessageBox("Unable to create Cache Base Directory.");
    return MAF_ERROR;
  }

  if(!CreateCache())
  {
    wxMessageBox("Unable to create a temporary cache. Editing stopped");
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
  if (m_DebugMode)
    mafLogMessage( _T("Current working directory is: '%s' "), wxGetCwd().c_str() );
  wxSetWorkingDirectory(m_VMEUploaderDownloaderDir.GetCStr());
  wxBusyCursor wait;

  //PROCESS EXIST, ONLY CALL CLIENT
  wxString command2execute;
  command2execute = m_PythonwExe.GetCStr();
  // script for client
  m_FileName = "lhpEditVMETag.py ";
  command2execute.Append(m_FileName.GetCStr());

  //workaround to understanding directory argument
  wxString directoryWorkAround = m_CurrentCache;
  directoryWorkAround.Replace(" ", "??");
  command2execute.Append(wxString::Format("%s ",directoryWorkAround)); //cache directory

  //workaround to understanding directory argument
  directoryWorkAround = m_MsfDir;
  directoryWorkAround.Replace(" ", "??");

  command2execute.Append(wxString::Format("%s ",directoryWorkAround)); //cache directory
  command2execute.Append(wxString::Format("%d ",m_Input->GetId())); //vme id
  command2execute.Append(wxString::Format("%s", m_CsvName.c_str())); //manualTagFile

  if (m_DebugMode)
    mafLogMessage( _T("Executing command: '%s'"), command2execute.c_str() );
  long pid = -1;
  if (pid = wxExecute(command2execute, wxEXEC_SYNC) != 0)
  {
    wxMessageBox("Error in lhpEditVMETag.py");
    mafLogMessage(_T("SYNC Command process '%s' terminated with exit code %d."),
      command2execute.c_str(), pid);
    return MAF_ERROR;
  }

  if (m_DebugMode)
    mafLogMessage(_T("SYNC Command process '%s' terminated with exit code %d."),
    command2execute.c_str(), pid);

  int save = wxMessageBox(wxString::Format("Do you want to save your changes?"),\
    "Save changes", wxYES | wxNO | wxCENTRE | wxICON_QUESTION);

  if (save == wxYES)
  {
    CopyEditorTagsIntoOriginalMSF();
    StoreUsedDictionariesToTags();
    mafEventMacro(mafEvent(this, MENU_FILE_SAVE));

    // show dialog for tag propagation...
    int propagate = wxMessageBox(wxString::Format("Propagate edited tags to other VMEs?"),\
      "Propagate Tags", wxYES | wxNO | wxCENTRE | wxICON_QUESTION);

    if (propagate == wxYES)
    {
      PropagateTagsToChoosedVMES();
    } 
    else if (propagate == wxNO)
    {
      std::ostringstream stringStream;
      stringStream << "Skipping propagation..."  << std::endl;
      if (m_DebugMode)
        mafLogMessage(stringStream.str().c_str());
    }
  } 
  else if (save == wxNO)
  {
    std::ostringstream stringStream;
    stringStream << "Skipping saving..."  << std::endl;
    if (m_DebugMode)
      mafLogMessage(stringStream.str().c_str());
  }

  return MAF_OK;
  
}

//-------------------------------------------------------------------
int lhpOpEditTagRefactor::CopyEditorTagsIntoOriginalMSF()
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
  wxString lockPath = m_VMEUploaderDownloaderDir;
  lockPath += m_CsvName.c_str();
  if (wxFileExists(lockPath))
    wxRemoveFile(lockPath); //fileName

  //remove msf created by phyton tag editor
  remove(msfCompletePath); 

  mafDEL(storage);
  return MAF_OK;
}

//----------------------------------------------------------------------------
bool lhpOpEditTagRefactor::CreateBaseCacheDirectories()
//----------------------------------------------------------------------------
{
  bool resultCache = false;

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

  return resultCache;
}

//----------------------------------------------------------------------------
bool lhpOpEditTagRefactor::CreateCache()
//----------------------------------------------------------------------------
{
  bool result = false;
  //control cache subdir
  wxString currentSubdir;
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
  if(wxMkDir(currentSubdir) == 0)
    result = true;

  m_CurrentCache = currentSubdir;
  return result;
}

//----------------------------------------------------------------------------
void lhpOpEditTagRefactor::SaveLinkInfo()   
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
void lhpOpEditTagRefactor::SetDictionary(int subDictionary)   
//----------------------------------------------------------------------------
{
  m_SubdictionaryId = subDictionary;
}
//----------------------------------------------------------------------------
void lhpOpEditTagRefactor::OpStop(int result)   
//----------------------------------------------------------------------------
{
  HideGui();
	mafEventMacro(mafEvent(this,result));
}

//----------------------------------------------------------------------------
int lhpOpEditTagRefactor::GeneratesTagsListsFromXMLDictionary()
//----------------------------------------------------------------------------
{
  
  wxString oldDir = wxGetCwd();
  if (m_DebugMode)
    mafLogMessage( _T("Current working directory is: '%s' "), wxGetCwd().c_str() );
  wxSetWorkingDirectory(m_VMEUploaderDownloaderDir.GetCStr());
  if (m_DebugMode)
    mafLogMessage( _T("Now current working directory is: '%s' "), wxGetCwd().c_str() );

  mafString outputDict = "UNDEFINED";
  BuildXMLEditorInputDictionary(outputDict);
  
  // get auto tags
  wxString command2execute;
  command2execute.Append(m_PythonExe.GetCStr());
  command2execute.Append(" lhpXMLDictionaryParser.py ");
  command2execute.Append(outputDict.GetCStr());
  command2execute.Append(" auto_tags ");
  command2execute.Append(m_AutoTagsListFromXMLDictionaryFileName.GetCStr());
  
  if (m_DebugMode)
    mafLogMessage( _T("Executing command: '%s'"), command2execute.c_str() );

  long pid = -1;
  if (pid = wxExecute(command2execute, wxEXEC_SYNC) != 0)
  {
    wxMessageBox("Error in lhpXMLDictionaryParser.py");
    mafLogMessage(_T("SYNC Command process '%s' terminated with exit code %d."),
      command2execute.c_str(), pid);
    return MAF_ERROR;
  }

  if ( !command2execute )
    return MAF_ERROR;

  if (m_DebugMode)
    mafLogMessage(_T("SYNC Command process '%s' terminated with exit code %d."),
    command2execute.c_str(), pid);

  // get manual tags
  command2execute.Clear();
  command2execute = m_PythonExe.GetCStr();  
  command2execute.Append(" lhpXMLDictionaryParser.py ");
  command2execute.Append(m_DictionaryToProcessFileName.GetCStr());
  command2execute.Append(" manual_tags ");
  command2execute.Append(m_ManualTagsListFromXMLDictionaryFileName.GetCStr());

  if (m_DebugMode)
    mafLogMessage( _T("Executing command: '%s'"), command2execute.c_str() );

  pid = -1;
  if (pid = wxExecute(command2execute, wxEXEC_SYNC) != 0)
  {
    wxMessageBox("Error in lhpXMLDictionaryParser.py");
    mafLogMessage(_T("SYNC Command process '%s' terminated with exit code %d."),
      command2execute.c_str(), pid);
    return MAF_ERROR;
  }

  if ( !command2execute )
    return MAF_ERROR;

  if (m_DebugMode)
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
  
  lhpTagHandlerInputOutputParametersCargoRefactor *parametersCargo = lhpTagHandlerInputOutputParametersCargoRefactor::New();
  parametersCargo->SetInputVme(mafVME::SafeDownCast(m_Input));
  parametersCargo->SetInputUser(m_User);
	parametersCargo->SetInputMSF(m_MsfFile);

  for (int i = 0; i < m_AutoTagsList.size(); i++)
  {
    tagName = m_AutoTagsList[i].c_str();

    if (tagName != "")
    {
      
      lhpFactoryTagHandlerRefactor *tagsFactory  = lhpFactoryTagHandlerRefactor::GetInstance();
      assert(tagsFactory!=NULL);
      lhpTagHandlerRefactor *tagHandler = NULL;
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

  unhandledPlusManualTagsFile.open(m_CurrentCache + m_CsvName.c_str());

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


  
  if (m_MetadataEditorId == 0)
  {
    // launch new editor
    command2execute.Clear();
    command2execute.Append(m_PythonwExe.GetCStr());
    command2execute.Append(" lhpMetadataEditor.py ");
    command2execute.Append(m_CsvName.c_str()); 
    command2execute.Append(" ");
    command2execute.Append(m_DictionaryToProcessFileName.GetCStr());
    command2execute.Append(wxString::Format(" %s", m_CurrentCache.GetCStr())); //manualTagFile
  } 
  else
  {
    // launch old editor
    command2execute.Clear();
    command2execute.Append(m_PythonExe.GetCStr());
    command2execute.Append(" CSVOMATIC.py ");
    command2execute.Append(m_CsvName.c_str());
  }
  
  
    // if (m_DebugMode)
    mafLogMessage( _T("Executing command: '%s'"), command2execute.c_str() );

  pid = wxExecute(command2execute, wxEXEC_SYNC);

  if ( !command2execute )
    return MAF_ERROR;

  if (m_DebugMode)
    mafLogMessage(_T("SYNC Command process '%s' terminated with exit code %d."),
    command2execute.c_str(), pid);

  wxSetWorkingDirectory(oldDir);
  if (m_DebugMode)
    mafLogMessage( _T("Current working directory is: '%s' "), wxGetCwd().c_str() );

  return MAF_OK;
}


//----------------------------------------------------------------------------
mafString lhpOpEditTagRefactor::GetXMLDictionaryFileName( mafString dictionaryFileNamePrefix )
//----------------------------------------------------------------------------
{
  mafString dictionaryFileName = "NOT FOUND";
  wxString oldDir = wxGetCwd();

  if (m_DebugMode)
    mafLogMessage( _T("Current working directory is: '%s' "), wxGetCwd().c_str() );
  wxSetWorkingDirectory(m_VMEUploaderDownloaderDir.GetCStr());
  if (m_DebugMode)
    mafLogMessage( _T("Now current working directory is: '%s' "), wxGetCwd().c_str() );

  wxArrayString files;
  wxString filePattern = dictionaryFileNamePrefix ;
  filePattern.Append("*.xml");

  wxDir::GetAllFiles(wxGetWorkingDirectory(), &files, filePattern, wxDIR_FILES);
  
  if (files.size() == 0)
  {
    mafLogMessage(dictionaryFileNamePrefix.GetCStr());
    mafLogMessage("dictionary not found! exiting...");
  }
  else if (files.size() > 1)
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
// widget id's
//----------------------------------------------------------------------------

void lhpOpEditTagRefactor::CreateGui()
{
  m_Gui = new mafGUI(this);
  
  if (DEBUG_TAGS_PROPAGATION)
  {
    m_Gui->Divider(2);
    m_Gui->Button(ID_PROPAGATE,"test propagate");
  }
  
  // Default editor is Metadata Editor
  /*m_Gui->Divider(2);
  const wxString metadataEditor[] = {"Metadata Editor","CSV Editor"};
  m_Gui->Label("Choose editor");
  m_Gui->Combo(ID_METADATA_EDITOR, "", &m_MetadataEditorId, 2, metadataEditor);*/

  m_Gui->Divider(2);
  m_Gui->Label("Use Dicom subdictionary");
  m_Gui->Bool(ID_USE_DICOM_SUBDICTIONARY, "", &m_UseDicomSubdictionary);
  m_Gui->Divider(2);
  m_Gui->Label("Use Motion Analysis subdictionary");
  m_Gui->Bool(ID_USE_MA_SUBDICTIONARY, "", &m_UseMASubdictionary);
  m_Gui->Divider(2);
  m_Gui->Label("Use MicroCT subdictionary");
  m_Gui->Bool(ID_USE_MICROCT_SUBDICTIONARY, "", &m_UseMicroCTSubdictionary);
  m_Gui->Divider(2);
  m_Gui->Label("Use Functional Anatomy subdictionary");
  m_Gui->Bool(ID_USE_FA_SUBDICTIONARY, "", &m_UseFASubdictionary);
  m_Gui->Divider(2);
  m_Gui->Divider();
  m_Gui->Divider();

  m_Gui->OkCancel(); 
  
  m_Gui->Update();

}
int lhpOpEditTagRefactor::AppendChildDictionary( const char *sourceXMLDictionaryFileName,\
    const char *childXMLDictionaryToAppendFileName, const char *pythonString,\
    const char *outputXMLFN )
{

  wxString command2execute;
  command2execute = m_PythonwExe.GetCStr();
  
  command2execute.Append(" lhpXMLDictionariesBuilder.py ");
  command2execute.Append(sourceXMLDictionaryFileName);
  command2execute.Append(" ");
  command2execute.Append(childXMLDictionaryToAppendFileName);
  command2execute.Append(" ");
  command2execute.Append(pythonString);
  command2execute.Append(" ");
  command2execute.Append(outputXMLFN);

  m_DictionaryToProcessFileName = outputXMLFN;
  // if (m_DebugMode)
    mafLogMessage( _T("Executing command: '%s'"), command2execute.c_str() );

  wxArrayString output;
  wxArrayString errors;
  long pid = -1;
  if (pid = wxExecute(command2execute, output, errors, wxEXEC_SYNC) != 0)
  {
    wxMessageBox("Error in lhpXMLDictionariesBuilder.py");
    mafLogMessage(_T("SYNC Command process '%s' terminated with exit code %d."),
      command2execute.c_str(), pid);
    return MAF_ERROR;
  }

  return MAF_OK;
}

void lhpOpEditTagRefactor::PropagateTagsToChoosedVMES()
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
  if (m_DebugMode)
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
    if (m_DebugMode)
      mafLogMessage(stringStream.str().c_str());
  }

  std::vector<mafNode *>::iterator nodeVectorIterator = nodeVector.begin();
  stringStream.clear();
  stringStream << "The following vme were checked: " << std::endl;
  if (m_DebugMode)
    mafLogMessage(stringStream.str().c_str());


  // for each target vme excluding the input
  while( nodeVectorIterator != nodeVector.end() )
  {
    mafVME *targetVme = mafVME::SafeDownCast(*nodeVectorIterator);
    assert(targetVme);
    std::ostringstream stringStream;
    stringStream << "vme name: " << targetVme->GetName()  << std::endl;
    if (m_DebugMode)
      mafLogMessage(stringStream.str().c_str());

    if (targetVme == m_Input)
    {
      std::ostringstream stringStream;
      stringStream << "Skipping input vme!"  << std::endl;
      if (m_DebugMode)
        mafLogMessage(stringStream.str().c_str());
    } 
    else
    {
      std::ostringstream stringStream;
      stringStream << "Copying to "  << targetVme->GetName() << std::endl;
      if (m_DebugMode)
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
        if (m_DebugMode)
          mafLogMessage(stringStream.str().c_str());
        targetTagArray->SetTag(key.c_str(), val.c_str());
        tagsToBeCopiedDictionaryIterator++;
      }

    }

    nodeVectorIterator++;
  }
  return;
}

int lhpOpEditTagRefactor::BuildXMLEditorInputDictionary( mafString &generatedXMLDictionaryFileName )
{
  m_MasterXMLDictionaryFileName = this->GetXMLDictionaryFileName(m_MasterXMLDictionaryFilePrefix);
  if (m_MasterXMLDictionaryFileName == "NOT FOUND")
  {
    return MAF_ERROR;
  }

  m_DictionaryToProcessFileName = m_MasterXMLDictionaryFileName;

  mafString inputDict = m_MasterXMLDictionaryFileName;
  mafString outputDict = inputDict;

  if (m_UseDicomSubdictionary == 1)
  { 

    mafString dicomSubDictionaryAppendingCommand = "dicom";   
    mafString dicomSubDictionaryFilePrefix = "lhpXMLDicomSourceSubdictionary_";
    mafString dicomSubDictionaryFileName = this->GetXMLDictionaryFileName(dicomSubDictionaryFilePrefix).GetCStr();

    outputDict = "assembledWithDicom.xml";

    if (this->\
      AppendChildDictionary(inputDict,dicomSubDictionaryFileName\
      , dicomSubDictionaryAppendingCommand, outputDict)
      == MAF_ERROR)
    {
      return MAF_ERROR;
    }

    inputDict = outputDict;
  } 

  if (m_UseMASubdictionary == 1)
  { 
    mafString maSubDictionaryAppendingCommand = "motion_analysis";   
    mafString maSubDictionaryFilePrefix = "lhpXMLMotionAnalysisSourceSubdictionary_";
    mafString maSubDictionaryFileName = this->GetXMLDictionaryFileName(maSubDictionaryFilePrefix).GetCStr();

    outputDict = "assembledWithMA.xml";

    if (this->\
      AppendChildDictionary(inputDict,maSubDictionaryFileName\
      , maSubDictionaryAppendingCommand, outputDict)
      == MAF_ERROR)
    {
      return MAF_ERROR;
    }  

    inputDict = outputDict;
  }

  if (m_UseMicroCTSubdictionary == 1)
  {
    // build micro ct

    mafString microCTSubDictionaryBuildingCommand = "micro_ct";

    mafString microCTSubDictionaryFilePrefix = "lhpXMLMicroCTSourceSubdictionary_";
    mafString microCTSubDictionaryFileName = this->GetXMLDictionaryFileName(microCTSubDictionaryFilePrefix).GetCStr();

    outputDict = "assembledWithMicroCT.xml";

    if (this->\
      AppendChildDictionary(inputDict,microCTSubDictionaryFileName\
      , microCTSubDictionaryBuildingCommand, outputDict)
      == MAF_ERROR)
    {
      return MAF_ERROR;
    }

    inputDict = outputDict;
  }

  if (m_UseFASubdictionary == 1)
  {
    mafString faSubDictionaryFilePrefix = "lhpXMLFASourceSubdictionary_";
    mafString faSubDictionaryFileName = this->GetXMLDictionaryFileName(faSubDictionaryFilePrefix).GetCStr();

    mafString outputDict = "assembledWithFA.xml";
    mafString s = "functional_anatomy";

    int result = AppendChildDictionary(m_DictionaryToProcessFileName, \
      faSubDictionaryFileName.GetCStr(), s, outputDict);

    if (result == MAF_ERROR)
    {
      return MAF_ERROR;
    } 

    inputDict = outputDict;
  }

  generatedXMLDictionaryFileName = inputDict;
  return MAF_OK;
}

void lhpOpEditTagRefactor::LoadUsedDictionariesFromTags()
{
  mafTagItem tag;

  m_Input->GetTagArray()->GetTag("USE_DICOM_SUBDICTIONARY", tag);
  mafString value;
  value = tag.GetValue();
  m_UseDicomSubdictionary = value == "1" ? 1 : 0 ;

  m_Input->GetTagArray()->GetTag("USE_FA_SUBDICTIONARY", tag);
  value = tag.GetValue();
  m_UseFASubdictionary = value == "1" ? 1 : 0 ;

  m_Input->GetTagArray()->GetTag("USE_MA_SUBDICTIONARY", tag);
  value = tag.GetValue();
  m_UseMASubdictionary = value == "1" ? 1 : 0 ;
  
  m_Input->GetTagArray()->GetTag("USE_MICROCT_SUBDICTIONARY", tag);
  value = tag.GetValue();
  m_UseMicroCTSubdictionary = value == "1" ? 1 : 0 ;
}

void lhpOpEditTagRefactor::StoreUsedDictionariesToTags()
{
  mafString value;
  value = m_UseDicomSubdictionary  == 1 ? "1" : "0" ;
  m_Input->GetTagArray()->SetTag("USE_DICOM_SUBDICTIONARY", value.GetCStr());
  
  value = m_UseFASubdictionary  == 1 ? "1" : "0" ;
  m_Input->GetTagArray()->SetTag("USE_FA_SUBDICTIONARY",value.GetCStr());

  value = m_UseMASubdictionary  == 1 ? "1" : "0" ;
  m_Input->GetTagArray()->SetTag("USE_MA_SUBDICTIONARY",value.GetCStr());

  value = m_UseMicroCTSubdictionary  == 1 ? "1" : "0" ;
  m_Input->GetTagArray()->SetTag("USE_MICROCT_SUBDICTIONARY",value.GetCStr());
}