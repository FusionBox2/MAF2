/*=========================================================================

 Program: MAF2
 Module: mafOpGarbageCollectMSFDir
 Authors: Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------


#include "mafOpGarbageCollectMSFDir.h"
#include "mafDecl.h"
#include "mafEvent.h"
#include "mafGUI.h"

#include "mafVMEStorage.h"
#include "mafEventIO.h"
#include "mafDataVector.h"
#include "mafVMEItem.h"
#include "mafVMEGenericAbstract.h"
#include "mafVMERoot.h"
#include "mafNodeIterator.h"
#include "mafVMEExternalData.h"
#include "vtkDirectory.h"
#include <algorithm>
#include <iterator> 
#include "mafOpValidateTree.h"
#include "mafEventIO.h"

//----------------------------------------------------------------------------
// Constants :
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafOpGarbageCollectMSFDir);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafOpGarbageCollectMSFDir::mafOpGarbageCollectMSFDir(const mafString& label) : Superclass(label)
//----------------------------------------------------------------------------
{
  m_OpType	= OPTYPE_OP;
  m_Canundo = false;
}
//----------------------------------------------------------------------------
mafOpGarbageCollectMSFDir::~mafOpGarbageCollectMSFDir()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
mafOp* mafOpGarbageCollectMSFDir::Copy()   
//----------------------------------------------------------------------------
{
	return new mafOpGarbageCollectMSFDir(GetLabel());
}
//----------------------------------------------------------------------------
void mafOpGarbageCollectMSFDir::OpRun()
//----------------------------------------------------------------------------
{
  wxString msg = "This operation will remove garbage data from your msf dir and \
clear the Undo stack. \
 \
Before continuing please make sure you have a working msf backup. \
Removed files will be written to Log Area. \
Proceed?";

  if (m_TestMode == false)
  {
    int answer = wxMessageBox(msg,_("Confirm"),wxYES_NO|wxCANCEL|wxICON_QUESTION,mafGetFrame()); // ask user if will save msf before closing
    if(answer == wxCANCEL || answer == wxNO)
    {
      mafEventMacro(mafEvent(this,OP_RUN_CANCEL));
      return;
    }
    else if(answer == wxYES)
    {
      // continue
    }
  }
 
  this->GarbageCollect();

  mafEventMacro(mafEvent(this,OP_RUN_OK));
}

int mafOpGarbageCollectMSFDir::GetFilesToRemove(std::set<std::string> &filesToRemoveSet )
{
  assert(m_Input);
  mafOpValidateTree* validateTree = new mafOpValidateTree();
  validateTree->SetInput(m_Input);

  // the input msf must be valid
  int result = validateTree->ValidateTree();
  
  if (result == mafOpValidateTree::VALIDATE_SUCCESS)
  {
    // continue
  } 
  else
  {
    mafString msg = _R("MSF Tree is invalid, exiting");

    if (!m_TestMode)
    {
      mafErrorMessage(_M(msg));
    }
    mafLogMessage(_M("MSF Tree is invalid, exiting"));
    return MAF_ERROR;
  }
  
  std::set<std::string> msfTreeFiles;
  result = validateTree->GetMSFTreeABSFileNamesSet(msfTreeFiles);

  if (result == MAF_OK)
  {
    std::set<std::string> msfDirFiles = this->GetMSFDirABSFileNamesSet();

    // add the msf file name:
    mafString msfXMLFileABSFileName = GetMSFXMLFileAbsFileName(m_Input);

    assert(mafFileExists(msfXMLFileABSFileName));
    msfTreeFiles.insert(msfXMLFileABSFileName.toStd());

    // add the backup file if present:
#pragma message("actually no replacement needed, go straight with mafString")
    wxString msfXMLBackupFile = msfXMLFileABSFileName.Append(_R(".bak")).toWx();
    msfXMLBackupFile.Replace("/","\\");

    if (mafFileExists(mafWxToString(msfXMLBackupFile)))
    {
      msfTreeFiles.insert(mafWxToString(msfXMLBackupFile).toStd());
#pragma message("a bit hacky")
    }

    filesToRemoveSet = SetDifference(msfDirFiles, msfTreeFiles);

    PrintSet(filesToRemoveSet);
  }
  else
  {
    return MAF_ERROR;
  }

  cppDEL(validateTree);
  return MAF_OK;
}

void mafOpGarbageCollectMSFDir::PrintSet( std::set<std::string> inputSet )
{
  auto iter = inputSet.begin();

  std::ostringstream stringStream;
  stringStream << std::endl;

  while( iter != inputSet.end() ) 
  {
    stringStream << *iter << std::endl << std::endl;
    ++iter;
  }

  mafLogMessage(_M(stringStream.str().c_str()));
}

std::set<std::string> mafOpGarbageCollectMSFDir::GetMSFDirABSFileNamesSet()
{
  assert(m_Input);
  mafString msfABSPath = GetMSFDirAbsPath(m_Input);
  
  assert(msfABSPath.IsEmpty() == false);
  assert(mafDirExists(msfABSPath));

  m_MSFDirABSFileNamesSet.clear();

  vtkDirectory *directoryReader = vtkDirectory::New();
  directoryReader->Open(msfABSPath.GetCStr());
  for (int i=0; i < directoryReader->GetNumberOfFiles(); i++) // skipping "." and ".." dir
  {
    mafString localFileName = _R(directoryReader->GetFile(i));

    wxString absFileName = (msfABSPath +  _R("/") + localFileName).toWx();

    absFileName.Replace("/","\\");

    if (localFileName == _R(".") || localFileName == _R("..")) 
    {
      continue;
    }
    else if (mafFileExists(mafWxToString(absFileName)))
    {     
      m_MSFDirABSFileNamesSet.insert(mafWxToString(absFileName).toStd());
#pragma message("a bit hacky")
    }
  }
  
  vtkDEL(directoryReader);
  return m_MSFDirABSFileNamesSet;
}

std::set<std::string> mafOpGarbageCollectMSFDir::SetDifference( std::set<std::string> &s1, std::set<std::string> &s2 )
{
  std::set<std::string> result;
  set_difference(s1.begin(), s1.end(), \
    s2.begin(), s2.end(),
    inserter(result, result.end()));

  return result;
}

mafString mafOpGarbageCollectMSFDir::GetMSFDirAbsPath(mafNode *anyTreeNode)
{
  mafVMERoot* root = mafVMERoot::SafeDownCast(anyTreeNode->GetRoot());
  assert(root);

  mafEventIO es(this,NODE_GET_STORAGE);
  root->OnEvent(&es);
  mafVMEStorage *storage = mafVMEStorage::SafeDownCast(es.GetStorage());
  assert(storage);

  mafString msfABSPath = _R(""); // empty by default
 
  if (storage != NULL)
  {  
    msfABSPath = storage->GetURL();
    msfABSPath.ExtractPathName();
  }

  wxString tmp = msfABSPath.toWx();
  tmp.Replace("/","\\");

  return mafWxToString(tmp);
}

mafString mafOpGarbageCollectMSFDir::GetMSFXMLFileAbsFileName(mafNode *anyTreeNode)
{
  mafVMERoot* root = mafVMERoot::SafeDownCast(anyTreeNode->GetRoot());
  assert(root);

  mafEventIO es(this,NODE_GET_STORAGE);
  root->OnEvent(&es);
  mafVMEStorage *storage = mafVMEStorage::SafeDownCast(es.GetStorage());
  assert(storage);

  wxString msfXMLFileAbsFileName = ""; // empty by default

  if (storage != NULL)
  {  
    msfXMLFileAbsFileName = storage->GetURL().toWx();
  }

  msfXMLFileAbsFileName.Replace("/","\\");

  return mafWxToString(msfXMLFileAbsFileName);
}

int mafOpGarbageCollectMSFDir::GarbageCollect()
{
  std::set<std::string> filesToRemove;
  int errorCode = GetFilesToRemove(filesToRemove);
  
  auto iter = filesToRemove.begin();

  std::ostringstream stringStream;
  stringStream << endl;

  while( iter != filesToRemove.end() ) 
  {
    std::string fileToRemove = *iter;
    stringStream << "Deleting: " << fileToRemove << endl;
    
    bool result = mafFileRemove(_R(fileToRemove.c_str()));
    
    if (result == false)
    {
      return MAF_ERROR;  
    }
      
    stringStream << "OK" << endl;
    mafLogMessage(_M(stringStream.str().c_str()));

    ++iter;
  }
 
  // Clear UnDo stack if successful
  mafEventMacro(mafEvent(this, CLEAR_UNDO_STACK));

  return MAF_OK;
}