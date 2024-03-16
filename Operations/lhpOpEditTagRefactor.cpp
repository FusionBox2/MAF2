/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: lhpOpEditTagRefactor.cpp,v $
Language:  C++
Date:      $Date: 2009-05-15 16:12:23 $
Version:   $Revision: 1.1.2.6 $
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
#include "lhpUtils.h"

#include <wx/process.h>
#include <wx/dir.h>
#include <wx/file.h>
#include <wx/busyinfo.h>

#include "lhpOpEditTagRefactor.h"
#include "lhpTagHandler.h"

#include "mafTagArray.h"

#include "mafGUI.h"
#include "lhpUser.h"
#include "mafNode.h"
#include "mafNodeManager.h"
#include "mafVMEGenericAbstract.h"
#include "mafVMEStorage.h"
#include "mafVMERoot.h"

#include "lhpFactoryTagHandler.h"
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
mafString lhpOpEditTagRefactor::m_CacheSubdir = _R("0");

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
lhpOpEditTagRefactor::lhpOpEditTagRefactor(const mafString& label) : Superclass(label)
//----------------------------------------------------------------------------
{
	m_OpType  = OPTYPE_OP;
	m_Canundo = false;
  m_HasLink = false;
  m_DebugMode = false;
  m_LinkNode.clear();
  m_LinkName.clear();
  m_User = NULL;

  m_PythonExe = _R("python.exe_UNDEFINED");
  m_PythonwExe = _R("pythonw.exe_UNDEFINED");  

  m_CacheDir = lhpUtils::lhpGetApplicationDirectory() + _R("\\VMEUploaderDownloaderRefactor\\UploadCache\\");
  m_OutgoingDir = lhpUtils::lhpGetApplicationDirectory() + _R("\\VMEUploaderDownloaderRefactor\\Outgoing\\");

  m_VMEUploaderDownloaderDir  = lhpUtils::lhpGetApplicationDirectory() + _R("\\VMEUploaderDownloaderRefactor\\");
  m_FileName = _R("");
  m_UnhandledPlusManualTagsLocalFileName = _R("manualTagFile.csv");
  m_MsfDir = _R("");


  m_MasterXMLDictionaryFilePrefix = _R("lhpXMLDictionary_");
  m_MasterXMLDictionaryFileName = _R("UNDEFINED");
  m_SubXMLDictionaryFilePrefix = _R("UNDEFINED") ;
  m_SubXMLDictionaryFileName = _R("UNDEFINED");
  m_AssembledXMLDictionaryFileName = _R("assembledXMLDictionary.xml");
  m_SubDictionaryBuildingCommand = _R("UNDEFINED");

  m_AutoTagsListFromXMLDictionaryFileName = _R("autoTagsList.txt");
  m_ManualTagsListFromXMLDictionaryFileName = _R("manualTagsList.txt");
  m_HandledAutoTagsLocalFileName = _R("handledAutoTagsList.csv");

  m_HandledAutoTagsListFromFactory.Clear();
  m_UnhandledAutoTagsListFromFactory.Clear();
  m_AutoTagsList.Clear();
  m_ManualTagsList.Clear();
  
  m_SubdictionaryId = NO_SUBDICTIONARY; // default to none

  m_MetadataEditorId = 0;
  m_UseFADictionary = 0;
  m_DictionaryToProcessFileName = _R("UNDEFINED");

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
	return new lhpOpEditTagRefactor(GetLabel());
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
  DebugPath.Append(_R("\\Debug.py"));
  if (mafFileExists(DebugPath))
  {
      std::ifstream debugFile;
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
      m_PythonwExe = m_PythonExe;
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
        mafLogMessage(_M(_R("You choosed dictionary number ") + mafToString(m_SubdictionaryId)));
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
  temp.Append((*event.GetString()).toWx());
  m_MsfFile = mafWxToString(temp);
  temp = temp.BeforeLast('/');
  m_MsfDir = mafWxToString(temp);

  if (m_MsfDir.IsEmpty())
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
    rootTag.SetName(_R("ROOT_TAG"));
    m_Input->GetTagArray()->SetTag(rootTag);
  }

  wxString oldDir = wxGetCwd();
  if (m_DebugMode)
    mafLogMessage(_M(_R("Current working directory is: '") + mafWxToString(wxGetCwd()) + _R("' ")));
  wxSetWorkingDirectory(m_VMEUploaderDownloaderDir.toWx());
  wxBusyCursor wait;

  //PROCESS EXIST, ONLY CALL CLIENT
  mafString command2execute;
  command2execute = m_PythonwExe;
  // script for client
  m_FileName = _R("lhpEditVMETag.py ");
  command2execute.Append(m_FileName);

  //workaround to understand directory argument
  wxString directoryWorkAround = m_CurrentCache.toWx();
  directoryWorkAround.Replace(" ", "??");
  command2execute.Append(mafWxToString(directoryWorkAround)); //cache directory
  command2execute.Append(_R(" "));

  //workaround to understand directory argument
  directoryWorkAround = m_MsfDir.toWx();
  directoryWorkAround.Replace(" ", "??");

  command2execute.Append(mafWxToString(directoryWorkAround)); //cache directory
  command2execute.Append(mafToString(m_Input->GetId())); //vme id
  command2execute.Append(_R(" "));
  command2execute.Append(m_UnhandledPlusManualTagsLocalFileName); //manualTagFile
  command2execute.Append(m_HandledAutoTagsLocalFileName); //autoTagFile


  if (m_DebugMode)
    mafLogMessage(_M(_R("Executing command: '") + command2execute + _R("'")));
  long pid = -1;
  if (pid = wxExecute(command2execute.toWx(), wxEXEC_SYNC) != 0)
  {
    wxMessageBox("Error in lhpEditVMETag.py");
    mafLogMessage(_M(_R("SYNC Command process '") + command2execute + _R("' terminated with exit code ") + mafToString(pid) + _R(".")));
    return MAF_ERROR;
  }

  if (m_DebugMode)
    mafLogMessage(_M(_R("SYNC Command process '") + command2execute + _R("' terminated with exit code ") + mafToString(pid) + _R(".")));

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
        mafLogMessage(_M(stringStream.str().c_str()));
    }
  } 
  else if (save == wxNO)
  {
    std::ostringstream stringStream;
    stringStream << "Skipping saving..."  << std::endl;
    if (m_DebugMode)
      mafLogMessage(_M(stringStream.str().c_str()));
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
      m_Input->SetLink(m_LinkName[i], m_LinkNode[i]);
    }
  }

  //remove csv file
  mafString lockPath = m_VMEUploaderDownloaderDir;
  lockPath += m_UnhandledPlusManualTagsLocalFileName;
  if (mafFileExists(lockPath))
    mafFileRemove(lockPath); //fileName

  //remove msf created by phyton tag editor
  remove(msfCompletePath.GetCStr());

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
    mafLogMessage(_M(_R("Current working directory is: '") + mafWxToString(wxGetCwd()) + _R("' ")));
  wxSetWorkingDirectory(m_VMEUploaderDownloaderDir.toWx());
  if (m_DebugMode)
    mafLogMessage(_M(_R("Now current working directory is: '") + mafWxToString(wxGetCwd()) + _R("' ")));

  mafString outputDict = _R("UNDEFINED");
  BuildXMLEditorInputDictionary(outputDict);
  
  // get auto tags
  mafString command2execute;
  command2execute.Append(m_PythonExe);
  command2execute.Append(_R(" lhpXMLDictionaryParser.py "));
  command2execute.Append(outputDict);
  command2execute.Append(_R(" auto_tags "));
  command2execute.Append(m_AutoTagsListFromXMLDictionaryFileName);
  
  if (m_DebugMode)
    mafLogMessage(_M(_R("Executing command: '") + command2execute + _R("'")));

  long pid = -1;
  if (pid = wxExecute(command2execute.toWx(), wxEXEC_SYNC) != 0)
  {
    wxMessageBox("Error in lhpXMLDictionaryParser.py");
    mafLogMessage(_M(_R("SYNC Command process '") + command2execute + _R("' terminated with exit code ") + mafToString(pid) + _R(".")));
    return MAF_ERROR;
  }

  if ( !command2execute.GetCStr() )
    return MAF_ERROR;

  if (m_DebugMode)
    mafLogMessage(_M(_R("SYNC Command process '") + command2execute + _R("' terminated with exit code ") + mafToString(pid) + _R(".")));

  // get manual tags
  command2execute.Clear();
  command2execute = m_PythonExe;  
  command2execute.Append(_R(" lhpXMLDictionaryParser.py "));
  command2execute.Append(m_DictionaryToProcessFileName);
  command2execute.Append(_R(" manual_tags "));
  command2execute.Append(m_ManualTagsListFromXMLDictionaryFileName);

  if (m_DebugMode)
    mafLogMessage(_M(_R("Executing command: '") + command2execute + _R("'")));

  pid = -1;
  if (pid = wxExecute(command2execute.toWx(), wxEXEC_SYNC) != 0)
  {
    wxMessageBox("Error in lhpXMLDictionaryParser.py");
    mafLogMessage(_M(_R("SYNC Command process '") + command2execute + _R("' terminated with exit code ") + mafToString(pid) + _R(".")));
    return MAF_ERROR;
  }

  if ( !command2execute.GetCStr() )
    return MAF_ERROR;

  if (m_DebugMode)
    mafLogMessage(_M(_R("SYNC Command process '") + command2execute + _R("' terminated with exit code ") + mafToString(pid) + _R(".")));

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
        tagValue.Append(tagName.toWx());
        tagValue.Append("\",\"");
        tagValue.Append(parametersCargo->GetTagHandlerGeneratedString().toWx());
        tagValue.Append('\"');
        m_HandledAutoTagsListFromFactory.Add(tagValue.c_str());
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

  handledAutoTagsFile.open(m_HandledAutoTagsLocalFileName.GetCStr());

  for (int i = 0; i < m_HandledAutoTagsListFromFactory.size(); i++)
  {
    tagName = mafWxToString(m_HandledAutoTagsListFromFactory[i]);
    handledAutoTagsFile << tagName.GetCStr() << std::endl ;
  }
  
  handledAutoTagsFile.close();

  
  // generates manual tag file 
  // open auto tags file and try to handle tags using tags factory 
  std::ofstream unhandledPlusManualTagsFile;

  unhandledPlusManualTagsFile.open((m_CurrentCache + m_UnhandledPlusManualTagsLocalFileName).GetCStr());

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


  
  if (m_MetadataEditorId == 0)
  {
    // launch new editor
    command2execute.Clear();
    command2execute.Append(m_PythonwExe);
    command2execute.Append(_R(" lhpMetadataEditor.py "));
    command2execute.Append(m_UnhandledPlusManualTagsLocalFileName); 
    command2execute.Append(_R(" "));
    command2execute.Append(m_DictionaryToProcessFileName);
    command2execute.Append(_R(" "));
    command2execute.Append(m_CurrentCache); //manualTagFile
  } 
  else
  {
    // launch old editor
    command2execute.Clear();
    command2execute.Append(m_PythonExe);
    command2execute.Append(_R(" CSVOMATIC.py "));
    command2execute.Append(m_UnhandledPlusManualTagsLocalFileName);
  }
  
  
    // if (m_DebugMode)
  mafLogMessage(_M(_R("Executing command: '") + command2execute + _R("'")));

  pid = wxExecute(command2execute.toWx(), wxEXEC_SYNC);

  if ( !command2execute.GetCStr())
    return MAF_ERROR;

  if (m_DebugMode)
    mafLogMessage(_M(_R("SYNC Command process '") + command2execute + _R("' terminated with exit code ") + mafToString(pid) + _R(".")));

  wxSetWorkingDirectory(oldDir);
  if (m_DebugMode)
    mafLogMessage(_M(_R("Current working directory is: '") + mafWxToString(wxGetCwd()) + _R("' ")));

  return MAF_OK;
}


//----------------------------------------------------------------------------
mafString lhpOpEditTagRefactor::GetXMLDictionaryFileName( mafString dictionaryFileNamePrefix )
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
  wxString filePattern = dictionaryFileNamePrefix.toWx() ;
  filePattern.Append("*.xml");

  wxDir::GetAllFiles(wxGetCwd(), &files, filePattern, wxDIR_FILES);
  
  if (files.size() == 0)
  {
    mafLogMessage(_M(dictionaryFileNamePrefix));
    mafLogMessage(_M("dictionary not found! exiting..."));
  }
  else if (files.size() > 1)
  {
    std::ostringstream stringStream;
    
    for (int i = 0; i < files.size(); i++) 
    { 
      stringStream << "found dictionary: " << files[i].c_str()  << std::endl;      
    }
    mafLogMessage(_M(stringStream.str().c_str()));
    mafLogMessage(_M("Too much dictionaries found! exiting..."));

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
// widget id's
//----------------------------------------------------------------------------

void lhpOpEditTagRefactor::CreateGui()
{
  m_Gui = new mafGUI(this);
  
  if (DEBUG_TAGS_PROPAGATION)
  {
    m_Gui->Divider(2);
    m_Gui->Button(ID_PROPAGATE,_R("test propagate"));
  }
  
  // Default editor is Metadata Editor
  /*m_Gui->Divider(2);
  const wxString metadataEditor[] = {"Metadata Editor","CSV Editor"};
  m_Gui->Label("Choose editor");
  m_Gui->Combo(ID_METADATA_EDITOR, "", &m_MetadataEditorId, 2, metadataEditor);*/

  m_Gui->Divider(2);
  m_Gui->Label(_R("Use Dicom subdictionary"));
  m_Gui->Bool(ID_USE_DICOM_SUBDICTIONARY, _R(""), &m_UseDicomSubdictionary);
  m_Gui->Divider(2);
  m_Gui->Label(_R("Use Motion Analysis subdictionary"));
  m_Gui->Bool(ID_USE_MA_SUBDICTIONARY, _R(""), &m_UseMASubdictionary);
  m_Gui->Divider(2);
  m_Gui->Label(_R("Use MicroCT subdictionary"));
  m_Gui->Bool(ID_USE_MICROCT_SUBDICTIONARY, _R(""), &m_UseMicroCTSubdictionary);
  m_Gui->Divider(2);
  m_Gui->Label(_R("Use Functional Anatomy subdictionary"));
  m_Gui->Bool(ID_USE_FA_SUBDICTIONARY, _R(""), &m_UseFASubdictionary);
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

  mafString command2execute;
  command2execute = m_PythonwExe;
  
  command2execute.Append(_R(" lhpXMLDictionariesBuilder.py "));
  command2execute.Append(_R(sourceXMLDictionaryFileName));
  command2execute.Append(_R(" "));
  command2execute.Append(_R(childXMLDictionaryToAppendFileName));
  command2execute.Append(_R(" "));
  command2execute.Append(_R(pythonString));
  command2execute.Append(_R(" "));
  command2execute.Append(_R(outputXMLFN));

  m_DictionaryToProcessFileName = _R(outputXMLFN);
  // if (m_DebugMode)
    mafLogMessage(_M(_R("Executing command: '") + command2execute + _R("'")));

  wxArrayString output;
  wxArrayString errors;
  long pid = -1;
  if (pid = wxExecute(command2execute.toWx(), output, errors, wxEXEC_SYNC) != 0)
  {
    wxMessageBox("Error in lhpXMLDictionariesBuilder.py");
    mafLogMessage(_M(_R("SYNC Command process '") + command2execute + _R("' terminated with exit code ") + mafToString(pid) + _R(".")));
    return MAF_ERROR;
  }

  return MAF_OK;
}

void lhpOpEditTagRefactor::PropagateTagsToChoosedVMES()
{
  mafString s(_L("Choose target VMEs"));
  mafEvent e(this,VME_CHOOSE, &s);
  e.SetBool(true); //true to create dialog with VME multiselect
  mafEventMacro(e);
  std::vector<mafNode *> nodeVector;
  nodeVector = e.GetVmeVector();
  int size = nodeVector.size();

  std::ostringstream stringStream;
  stringStream << "Vector size: "<< size  << std::endl;
  if (m_DebugMode)
      mafLogMessage(_M(stringStream.str().c_str()));

  // for each vme different from the input one
  // copy input edited tags into it

  // get L000 tags from the vme tag array
  std::vector<mafString> tagNamesVector;
  std::vector<mafString>::iterator tagNamesVectorIterator;

  std::map<std::string, std::string> tagsToBeCopiedDictionary;
  std::map<std::string, std::string>::iterator tagsToBeCopiedDictionaryIterator;

  mafTagArray *inputTagArray = m_Input->GetTagArray();
  inputTagArray->GetTagList(tagNamesVector); 

  size = tagNamesVector.size();

  for (int i = 0; i < size; i++) 
  { 
    std::string tagName = tagNamesVector[i].GetCStr();
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

      tagValue = inputTagArray->GetTag(_R(tagName.c_str()))->GetValue().toStd();
      tagsToBeCopiedDictionary[tagName] = tagValue;
    } 
    else
    {
      foundTxt = "NOT Found";
    }

    stringStream << foundTxt << " " << stringToSearch << " in "<< tagName <<  std::endl;
    if (m_DebugMode)
        mafLogMessage(_M(stringStream.str().c_str()));
  }

  std::vector<mafNode *>::iterator nodeVectorIterator = nodeVector.begin();
  stringStream.clear();
  stringStream << "The following vme were checked: " << std::endl;
  if (m_DebugMode)
      mafLogMessage(_M(stringStream.str().c_str()));


  // for each target vme excluding the input
  while( nodeVectorIterator != nodeVector.end() )
  {
    mafVME *targetVme = mafVME::SafeDownCast(*nodeVectorIterator);
    assert(targetVme);
    std::ostringstream stringStream;
    stringStream << "vme name: " << targetVme->GetName().GetCStr() << std::endl;
    if (m_DebugMode)
        mafLogMessage(_M(stringStream.str().c_str()));

    if (targetVme == m_Input)
    {
      std::ostringstream stringStream;
      stringStream << "Skipping input vme!"  << std::endl;
      if (m_DebugMode)
          mafLogMessage(_M(stringStream.str().c_str()));
    } 
    else
    {
      std::ostringstream stringStream;
      stringStream << "Copying to "  << targetVme->GetName().GetCStr() << std::endl;
      if (m_DebugMode)
          mafLogMessage(_M(stringStream.str().c_str()));

      // get the target vme tag array
      mafTagArray *targetTagArray = targetVme->GetTagArray();

      tagsToBeCopiedDictionaryIterator = tagsToBeCopiedDictionary.begin();

      // for every tag in source:
      while (tagsToBeCopiedDictionaryIterator != tagsToBeCopiedDictionary.end()) 
      { 
        std::string key = tagsToBeCopiedDictionaryIterator->first;
        std::string val = tagsToBeCopiedDictionaryIterator->second;
        std::ostringstream stringStream;
        stringStream << "Copying " << key << " " << val << " to " << targetVme->GetName().GetCStr() << std::endl;
        if (m_DebugMode)
            mafLogMessage(_M(stringStream.str().c_str()));
        targetTagArray->SetTag(mafTagItem(_R(key.c_str()), _R(val.c_str())));
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
  if (m_MasterXMLDictionaryFileName == _R("NOT FOUND"))
  {
    return MAF_ERROR;
  }

  m_DictionaryToProcessFileName = m_MasterXMLDictionaryFileName;

  mafString inputDict = m_MasterXMLDictionaryFileName;
  mafString outputDict = inputDict;

  if (m_UseDicomSubdictionary == 1)
  { 

    mafString dicomSubDictionaryAppendingCommand = _R("dicom");   
    mafString dicomSubDictionaryFilePrefix = _R("lhpXMLDicomSourceSubdictionary_");
    mafString dicomSubDictionaryFileName = this->GetXMLDictionaryFileName(dicomSubDictionaryFilePrefix);

    outputDict = _R("assembledWithDicom.xml");

    if (this->\
      AppendChildDictionary(inputDict.GetCStr(),dicomSubDictionaryFileName.GetCStr(),
          dicomSubDictionaryAppendingCommand.GetCStr(), outputDict.GetCStr())
      == MAF_ERROR)
    {
      return MAF_ERROR;
    }

    inputDict = outputDict;
  } 

  if (m_UseMASubdictionary == 1)
  { 
    mafString maSubDictionaryAppendingCommand = _R("motion_analysis");   
    mafString maSubDictionaryFilePrefix = _R("lhpXMLMotionAnalysisSourceSubdictionary_");
    mafString maSubDictionaryFileName = this->GetXMLDictionaryFileName(maSubDictionaryFilePrefix);

    outputDict = _R("assembledWithMA.xml");

    if (this->\
      AppendChildDictionary(inputDict.GetCStr(),maSubDictionaryFileName.GetCStr(),
          maSubDictionaryAppendingCommand.GetCStr(), outputDict.GetCStr())
      == MAF_ERROR)
    {
      return MAF_ERROR;
    }  

    inputDict = outputDict;
  }

  if (m_UseMicroCTSubdictionary == 1)
  {
    // build micro ct

    mafString microCTSubDictionaryBuildingCommand = _R("micro_ct");

    mafString microCTSubDictionaryFilePrefix = _R("lhpXMLMicroCTSourceSubdictionary_");
    mafString microCTSubDictionaryFileName = this->GetXMLDictionaryFileName(microCTSubDictionaryFilePrefix);

    outputDict = _R("assembledWithMicroCT.xml");

    if (this->\
      AppendChildDictionary(inputDict.GetCStr(),microCTSubDictionaryFileName.GetCStr(),
          microCTSubDictionaryBuildingCommand.GetCStr(), outputDict.GetCStr())
      == MAF_ERROR)
    {
      return MAF_ERROR;
    }

    inputDict = outputDict;
  }

  if (m_UseFASubdictionary == 1)
  {
    mafString faSubDictionaryFilePrefix = _R("lhpXMLFASourceSubdictionary_");
    mafString faSubDictionaryFileName = this->GetXMLDictionaryFileName(faSubDictionaryFilePrefix);

    mafString outputDict = _R("assembledWithFA.xml");
    mafString s = _R("functional_anatomy");

    int result = AppendChildDictionary(m_DictionaryToProcessFileName.GetCStr(),
      faSubDictionaryFileName.GetCStr(), s.GetCStr(), outputDict.GetCStr());

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

  m_Input->GetTagArray()->GetTag(_R("USE_DICOM_SUBDICTIONARY"), tag);
  mafString value;
  value = tag.GetValue();
  m_UseDicomSubdictionary = value == _R("1") ? 1 : 0 ;

  m_Input->GetTagArray()->GetTag(_R("USE_FA_SUBDICTIONARY"), tag);
  value = tag.GetValue();
  m_UseFASubdictionary = value == _R("1") ? 1 : 0 ;

  m_Input->GetTagArray()->GetTag(_R("USE_MA_SUBDICTIONARY"), tag);
  value = tag.GetValue();
  m_UseMASubdictionary = value == _R("1") ? 1 : 0 ;
  
  m_Input->GetTagArray()->GetTag(_R("USE_MICROCT_SUBDICTIONARY"), tag);
  value = tag.GetValue();
  m_UseMicroCTSubdictionary = value == _R("1") ? 1 : 0 ;
}

void lhpOpEditTagRefactor::StoreUsedDictionariesToTags()
{
  mafString value;
  value = m_UseDicomSubdictionary  == 1 ? _R("1") : _R("0") ;
  m_Input->GetTagArray()->SetTag(mafTagItem(_R("USE_DICOM_SUBDICTIONARY"), value));
  
  value = m_UseFASubdictionary  == 1 ? _R("1") : _R("0") ;
  m_Input->GetTagArray()->SetTag(mafTagItem(_R("USE_FA_SUBDICTIONARY"),value));

  value = m_UseMASubdictionary  == 1 ? _R("1") : _R("0") ;
  m_Input->GetTagArray()->SetTag(mafTagItem(_R("USE_MA_SUBDICTIONARY"),value));

  value = m_UseMicroCTSubdictionary  == 1 ? _R("1") : _R("0") ;
  m_Input->GetTagArray()->SetTag(mafTagItem(_R("USE_MICROCT_SUBDICTIONARY"),value));
}