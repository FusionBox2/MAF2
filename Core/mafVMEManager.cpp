/*=========================================================================

 Program: MAF2
 Module: mafVMEManager
 Authors: Silvano Imboden
 
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


#include "mafVMEManager.h"

#include <wx/busyinfo.h>
#include <wx/zipstrm.h>
#include <wx/zstream.h>
#include <wx/ffile.h>
#include <wx/wfstream.h>
#include <wx/fs_zip.h>
#include <wx/dir.h>
#include <wx/filename.h>

#include "mafDecl.h"
#include "mafNode.h"
#include "mafVMEStorage.h"
#include "mafRemoteStorage.h"
#include "mmdRemoteFileManager.h"
#include "mafVMEGenericAbstract.h"
#include "mafDataVector.h"

#include "mafNodeIterator.h"
#include "mafTagArray.h"

#include <fstream>

#include "mafFilesDirs.h"

//----------------------------------------------------------------------------
mafVMEManager::mafVMEManager()
//----------------------------------------------------------------------------
{
  m_Modified    = false;
	m_MakeBakFile = true;
	m_Storage     = NULL;
  m_Crypting    = false;
  m_LoadingFlag = false;
  m_FileHistoryIdx = -1;

  mafString msfDir = mafGetApplicationDirectory();
  msfDir.ParsePathName();
	m_MSFDir   = msfDir;
	m_MSFFile  = "";
	m_ZipFile  = "";
  m_TmpDir   = "";
  m_file_extension = "msf";

  m_SingleBinaryFile = false;

  m_Config = wxConfigBase::Get();

  m_TestMode = false;
}
//----------------------------------------------------------------------------
mafVMEManager::~mafVMEManager()
//----------------------------------------------------------------------------
{
  if(m_Storage) 
    NotifyRemove( m_Storage->GetRoot() ); // //SIL. 11-4-2005:  - cast root to node -- maybe to be removed

  m_AppStamp.clear();
  SetListener(NULL);// m_Listener = NULL;
  
  mafDEL(m_Storage);
  cppDEL(m_Config);
}
//----------------------------------------------------------------------------
void mafVMEManager::SetSingleBinaryFile(bool singleFile)
//----------------------------------------------------------------------------
{
  m_SingleBinaryFile = singleFile;
}
//----------------------------------------------------------------------------
void mafVMEManager::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (maf_event->GetChannel()==MCH_UP)
  {
    if (maf_event->GetId() == mafDataVector::GetSingleFileDataId())
    {
      ((mafEvent *)maf_event)->SetBool(m_SingleBinaryFile);
      return;
    }
    // events coming from the tree...
    switch (maf_event->GetId())
    {
      case NODE_ATTACHED_TO_TREE:
      {
        if (!m_LoadingFlag)
        {
          NotifyAdd((mafNode *)maf_event->GetSender());
          MSFModified(true);
        }
      }
      break;
      case NODE_DETACHED_FROM_TREE:
      {
        if (!m_LoadingFlag)
        {
          NotifyRemove((mafNode *)maf_event->GetSender());
          MSFModified(true);
        }
      }
      break;
      default:
        mafEventMacro(*maf_event);
    }
  }
}

//----------------------------------------------------------------------------
mafVMERoot *mafVMEManager::GetRoot()
//----------------------------------------------------------------------------
{
  return (m_Storage?m_Storage->GetRoot():NULL);
}
//----------------------------------------------------------------------------
mafVMEStorage *mafVMEManager::GetStorage()
//----------------------------------------------------------------------------
{
  return m_Storage;
}
//----------------------------------------------------------------------------
void mafVMEManager::SetApplicationStamp(mafString &appstamp)
//----------------------------------------------------------------------------
{
  // Add a single application stamp; this is done automatically while creating the application with the application name
  m_AppStamp.push_back(appstamp);
}
//----------------------------------------------------------------------------
void mafVMEManager::SetApplicationStamp(std::vector<mafString> appstamp)
//----------------------------------------------------------------------------
{
  // Add a vector of time stamps; this can be done manually for adding compatibility with other applications. 
  // The application name itself must not be included since it was already added with the other call (see function above).
  for (int i=0; i<appstamp.size();i++)
  {
    m_AppStamp.push_back(appstamp.at(i));
  }
}
//----------------------------------------------------------------------------
void mafVMEManager::SetLocalCacheFolder(mafString cache_folder)
//----------------------------------------------------------------------------
{
  if (m_Storage)
  {
    // Set the local cache directory
    m_Storage->SetTmpFolder(cache_folder.GetCStr());
  }
}
//----------------------------------------------------------------------------
void mafVMEManager::MSFNew(bool notify_root_creation)
//----------------------------------------------------------------------------
{
  if (m_Storage)
  {
    m_Storage->GetRoot()->CleanTree();
    mafEventMacro(mafEvent(this,CLEAR_UNDO_STACK)); // ask logic to clear the undo stack
  }

  mafRemoveDirectory(m_TmpDir); // remove the temporary directory
  m_TmpDir = "";

  m_Modified = false;

  m_LoadingFlag = true; // set the loading flag to prevent the manager
                        // listen to NODE_ATTACHED_TO_TREE and NODE_DETACHED_TO_TREE events

  mafEvent e(this,CREATE_STORAGE,m_Storage); // ask logic to create a new storage
  mafEventMacro(e);
  m_Storage = (mafVMEStorage *)e.GetMafObject();
  m_LoadingFlag = false;

  if(notify_root_creation)
	{
		//Add the application stamps
		mafTagItem tag_appstamp;
		tag_appstamp.SetName("APP_STAMP");
		tag_appstamp.SetValue(this->m_AppStamp.at(0).GetCStr());
		m_Storage->GetRoot()->GetTagArray()->SetTag(tag_appstamp); // set the appstamp tag for the root
    AddCreationDate(m_Storage->GetRoot());
		mafEventMacro(mafEvent(this,VME_ADDED,m_Storage->GetRoot())); // raise notification events
		mafEventMacro(mafEvent(this,VME_SELECTED,m_Storage->GetRoot()));
	}

  m_MSFFile = ""; //next MSFSave will ask for a filename
  m_ZipFile = ""; 
}

//----------------------------------------------------------------------------
void mafVMEManager::AddCreationDate(mafNode *vme)
//----------------------------------------------------------------------------
{
  wxString dateAndTime;
  wxDateTime time = wxDateTime::UNow(); // get time with millisecond precision
  dateAndTime  = wxString::Format("%02d/%02d/%02d %02d:%02d:%02d",time.GetDay(), time.GetMonth()+1, time.GetYear(), time.GetHour(), time.GetMinute(),time.GetSecond());
 
  mafTagItem tag_creationDate;
  tag_creationDate.SetName("Creation_Date");
  tag_creationDate.SetValue(dateAndTime);
  vme->GetTagArray()->SetTag(tag_creationDate); // set creation date tag for the specified vme
}

//----------------------------------------------------------------------------
int  mafVMEManager::MSFOpen(int file_id)
//----------------------------------------------------------------------------
{
  m_FileHistoryIdx = file_id - wxID_FILE1;
	mafString file = m_FileHistory.GetHistoryFile(m_FileHistoryIdx).c_str(); // get the filename from history
	return MSFOpen(file);
}
//----------------------------------------------------------------------------
int mafVMEManager::MSFOpen(mafString filename)
//----------------------------------------------------------------------------
{
  wxWindowDisabler *disableAll;
  wxBusyCursor *wait_cursor;

  if(!m_TestMode) // Losi 02/16/2010 for test class
  {
    disableAll = new wxWindowDisabler();
    wait_cursor = new wxBusyCursor();
  }
  
  mafString protocol = "";
  bool remote_file = IsRemote(filename, protocol); // check if the specified path refers to a remote location
  
  // open a local msf
  if(!remote_file && !::wxFileExists(filename.GetCStr()))
	{
		mafString msg;
    msg = _("File ");
		msg << filename;
		msg << _(" not found!");
		mafWarningMessage(msg, _("Warning"));
    if(m_FileHistoryIdx != -1)
    {
      m_FileHistory.RemoveFileFromHistory(m_FileHistoryIdx); // remove filename to history
      m_FileHistory.Save(*m_Config); // Save file history to registry
      m_FileHistoryIdx = -1;
    }

    if(!m_TestMode) // Losi 02/16/2010 for test class
    {
      cppDEL(disableAll);
      cppDEL(wait_cursor);
    }
		return MAF_ERROR; 
	}

  MSFNew(false); // insert and select the root - reset m_MSFFile - delete the old storage and create a new one
  
  mafString unixname = filename;
  if (remote_file)
  {
    // set parameters for remote storage according to the remote file.
    ((mafRemoteStorage *)m_Storage)->SetHostName(m_Host);
    ((mafRemoteStorage *)m_Storage)->SetRemotePort(m_Port);
    ((mafRemoteStorage *)m_Storage)->SetUsername(m_User);
    ((mafRemoteStorage *)m_Storage)->SetPassword(m_Pwd);
  }
  
  wxString path, name, ext;
  wxSplitPath(filename.GetCStr(),&path,&name,&ext);
  //if(ext == "zmsf")
  if(ext.IsSameAs("z" + m_file_extension,true)) 
  {
    if (remote_file) // download remote zmsf
    {
      // Download the file if it is not present into the cache
      // we are using the remote storage!!
      mafString local_filename, remote_filename;
      remote_filename = filename;
      local_filename = m_Storage->GetTmpFolder();
      local_filename += "\\";
      local_filename += name;
   //   local_filename += ".zmsf";
	  local_filename += ".z";
	  local_filename += m_file_extension;
      ((mafRemoteStorage *)m_Storage)->GetRemoteFileManager()->DownloadRemoteFile(remote_filename, local_filename); // download the remote file in the download cache
      filename = local_filename;
    }
    m_ZipFile = filename;
    unixname = mafOpenZIP(filename, m_Storage->GetTmpFolder(), m_TmpDir); // open the zmsf archive and extract it to the temporary directory
    if(unixname.IsEmpty())
    {
    	mafMessage(_("Bad or corrupted z" + m_file_extension + " file!"));
      m_Modified = false;
      m_Storage->Delete();
      m_Storage = NULL;
      MSFNew();
      if(!m_TestMode) // Losi 02/16/2010 for test class
      {
        cppDEL(disableAll);
        cppDEL(wait_cursor);
      }
      return MAF_ERROR;
    }
    wxSetWorkingDirectory(m_TmpDir.GetCStr());
  }
  
  unixname.ParsePathName();

  m_MSFFile = unixname; 
  m_Storage->SetURL(m_MSFFile.GetCStr());
 
  m_LoadingFlag = true; // set m_LoadingFlag to prevent the manager
                        // listen to NODE_ATTACHED_TO_TREE and NODE_DETACHED_TO_TREE events
  int res = m_Storage->Restore(); // restore the tree
  if (res != MAF_OK)
  {
    mafErrorMessage(_("Errors during file parsing! Look the log area for error messages.")); // if some problems occurred during import give feedback to the user
  }
  m_LoadingFlag = false;

  mafTimeStamp b[2];
  mafVMERoot *root_node = m_Storage->GetRoot();
  root_node->GetOutput()->GetTimeBounds(b);
  root_node->SetTreeTime(b[0]); // Set tree time to the starting time
  
	////////////////////////////////  Application Stamp managing ////////////////////
	if(!root_node->GetTagArray()->GetTag("APP_STAMP"))
	{
		//update the old data files to support Application Stamp
		mafTagItem tag_appstamp;
		tag_appstamp.SetName("APP_STAMP");
		tag_appstamp.SetValue(this->m_AppStamp.at(0).GetCStr());
		root_node->GetTagArray()->SetTag(tag_appstamp); // set appstamp tag of the root
	}
	
	mafString app_stamp;
  if(mafTagItem *ti = root_node->GetTagArray()->GetTag("APP_STAMP"))
    app_stamp = root_node->GetTagArray()->GetTag("APP_STAMP")->GetValue();
  // First check for compatibility with all stored App stamps
  bool stamp_found = false;
  bool stamp_data_manager_found = false;
  bool stamp_open_all_found = false;
  for (int k=0; k<m_AppStamp.size(); k++)
  {
    // Check with the Application name
    if (app_stamp.Equals(m_AppStamp.at(k).GetCStr()))
    {
      stamp_found = true;
    }
    // Check with the "Data Manager" tag
    if (m_AppStamp.at(k).Equals("DataManager"))
    {
      stamp_data_manager_found = true;
    }
    // Check with the "OPEN_ALL_DATA" tag
    if (m_AppStamp.at(k).Equals("OPEN_ALL_DATA"))
    {
      stamp_open_all_found = true;
    }
  }
	/*if(app_stamp.Equals("INVALID") || ((!stamp_found) && (!stamp_data_manager_found) && (!stamp_open_all_found))) 
	{
		//Application stamp not valid
		mafMessage(_("File not valid for this application!"), _("Warning"));
		m_Modified = false;
		m_Storage->Delete();
		m_Storage = NULL;
    
		MSFNew();
    if(!m_TestMode) // Losi 02/16/2010 for test class
    {
      cppDEL(disableAll);
      cppDEL(wait_cursor);
    }
		return MAF_ERROR;
	}*/
	///////////////////////////////////////////////////////////////////////////////// 
  NotifyAdd(root_node); // add the storage root (the tree) with events notification

	mafEventMacro(mafEvent(this,VME_SELECTED, root_node)); // raise notification events (to logic)
  mafEventMacro(mafEvent(this,CAMERA_RESET)); 
  
	if (m_TmpDir != "")
	{
    m_FileHistory.AddFileToHistory(m_ZipFile.GetCStr()); // add the zmsf file to the history
	}
  else if(/*!remote_file && */res == MAF_OK)
  {
    m_FileHistory.AddFileToHistory(m_MSFFile.GetCStr()); // add the msf file to the history
  }
  else if(res != MAF_OK && m_FileHistoryIdx != -1)
  {
    m_FileHistory.RemoveFileFromHistory(m_FileHistoryIdx); // if something get wrong retoring the file remove it from istory
  }
	m_FileHistory.Save(*m_Config); // save file history to registry
  m_FileHistoryIdx = -1;

  mafEventMacro(mafEvent(this,LAYOUT_LOAD)); // ask logic to load the layout

  if(!m_TestMode) // Losi 02/16/2010 for test class
  {
    cppDEL(disableAll);
    cppDEL(wait_cursor);
  }
}

//----------------------------------------------------------------------------
int mafVMEManager::MSFSave()
//----------------------------------------------------------------------------
{
  wxBusyInfo *wait;
  int ret=MAF_OK;

  if(m_MSFFile.IsEmpty()) 
  {
    // new file to save: ask to the application which is the default
    // modality to save binary files.
    mafEvent e(this,mafDataVector::GetSingleFileDataId());
    mafEventMacro(e);
    SetSingleBinaryFile(e.GetBool()); // set the save modality for time-varying vme
    
    // ask for the new file name.
    wxString wildc = _("MAF Storage Format file (*."+ m_file_extension +")|*."
		              + m_file_extension +"|Compressed file (*.z"+ m_file_extension +")|*.z" + m_file_extension + "");
    mafString file = mafGetSaveFile(m_MSFDir, wildc.c_str());
    if(file.IsEmpty())
      return MAF_ERROR;
   
    wxString path, name, ext, file_dir;
    wxSplitPath(file.GetCStr(),&path,&name,&ext);

    if(!wxFileExists(file.GetCStr()))
		{
			file_dir = path + "/" + name;  //Put the file in a folder with the same name
			if(!wxDirExists(file_dir))
				wxMkdir(file_dir);
      //if (ext == "zmsf")
	  if(ext.IsSameAs("z"+m_file_extension,true))
      {
        m_ZipFile = file;
        m_TmpDir = path + "/" + name;
        //ext = "msf";
		ext = m_file_extension;
      }
			file = file_dir + "/" + name + "." + ext;
		}

    file.ParsePathName();

    m_MSFFile = file.GetCStr();
  }

  if(wxFileExists(m_MSFFile.GetCStr()) && m_MakeBakFile) // an msf with the same name exists
	{
    mafString bak_filename = m_MSFFile + ".bak";                // create the backup for the saved msf
    wxRenameFile(m_MSFFile.GetCStr(), bak_filename.GetCStr());  // renaming the founded one
	}
	
  if(!m_TestMode) 
    wait=new wxBusyInfo(_("Saving MSF: Please wait"));
  
  m_Storage->SetURL(m_MSFFile.GetCStr());
  if (m_Storage->Store() != MAF_OK) // store the tree
  {
    ret=false;
    mafLogMessage(_("Error during MSF saving"));
  }
  // add the msf (or zmsf) to the history
  if (!m_ZipFile.IsEmpty())
  {
    mafZIPSave(m_ZipFile, m_TmpDir);
    m_FileHistory.AddFileToHistory(m_ZipFile.GetCStr()); // add the zmsf to the file history
  }
  else
  {
    m_FileHistory.AddFileToHistory(m_MSFFile.GetCStr()); // add the msf to the file history
  }
	m_FileHistory.Save(*m_Config);
  m_Modified = false;

  if(!m_TestMode)
    delete wait;

  return ret;
}
//----------------------------------------------------------------------------
int mafVMEManager::MSFSaveAs()   
//----------------------------------------------------------------------------
{
  int retValue;
  mafString oldFileName;
  oldFileName=m_MSFFile;
  m_MSFFile = ""; // set filenames to empty so the MSFSave method will ask for them
  m_ZipFile = "";
  m_MakeBakFile = false;
  retValue=MSFSave();
  //if the user cancel save operation the name will be empty 
  if (m_MSFFile=="")
    m_MSFFile=oldFileName;
  
  return retValue;
}
//----------------------------------------------------------------------------
void mafVMEManager::Upload(mafString local_file, mafString remote_file)
//----------------------------------------------------------------------------
{
  if (m_Storage == NULL)
  {
    mafMessage(_("Some problem occourred, MAF storage is NULL!!"), _("Warning"));
    return;
  }
  mafRemoteStorage *storage = (mafRemoteStorage *)m_Storage;
  if (storage->GetRemoteFileManager()->UploadLocalFile(local_file, remote_file) != MAF_OK) // Upload the local file to the remote repository
  {

  }
  /*
  if (upload_flag == UPLOAD_TREE)
  {
    wxString local_dir = wxPathOnly(m_MSFFile);
    wxString remote_dir = remote_file.GetCStr();
    remote_dir = wxPathOnly(remote_dir);
    wxString upload_file;

    wxArrayString files;
    wxDir::GetAllFiles(local_dir,&files); // get all files in the temporary directory

    wxString path, short_name, ext, local_file;
    for (size_t i = 0; i < files.GetCount(); i++) 
    {
      local_file = files.Item(i);
      wxSplitPath(local_file.c_str(), &path, &short_name, &ext);
      upload_file = remote_dir + "/" + short_name + "." + ext;
      if (storage->UploadLocalFile(local_file, upload_file) != MAF_OK) // Upload the local file to the remote repository
      {
        break;
      }
    }
  }*/
}
//----------------------------------------------------------------------------
void mafVMEManager::VmeAdd(mafNode *n)
//----------------------------------------------------------------------------
{
  if(n != NULL)
  {
    // check the node's parent
    mafNode *vp = n->GetParent();  
    assert( vp == NULL || m_Storage->GetRoot()->IsInTree(vp) );
    if(vp == NULL) 
			n->ReparentTo(m_Storage->GetRoot()); // reparent the node to the root

    m_Modified = true;
  }
}
//----------------------------------------------------------------------------
void mafVMEManager::VmeRemove(mafNode *n)
//----------------------------------------------------------------------------
{
  if(n != NULL && m_Storage->GetRoot() /*&& m_Storage->GetRoot()->IsInTree(n)*/) 
  {
    assert(m_Storage->GetRoot()->IsInTree(n));
    n->ReparentTo(NULL);
    m_Modified = true;
  }
}
//----------------------------------------------------------------------------
void mafVMEManager::TimeSet(double time)
//----------------------------------------------------------------------------
{
  if(m_Storage->GetRoot()) m_Storage->GetRoot()->SetTreeTime(time); // set the tree time
}
//----------------------------------------------------------------------------
void mafVMEManager::TimeGetBounds(double *min, double *max)
//----------------------------------------------------------------------------
{
  mafTimeStamp b[2];
  if(m_Storage->GetRoot()) 
  {
    m_Storage->GetRoot()->GetOutput()->GetTimeBounds(b); // get the root's time bounds
    *min = b[0];
    *max = b[1];
  }
  else
  {
    // no msf
    *min = 0;
    *max = 0;
  }
}
//----------------------------------------------------------------------------
void mafVMEManager::NotifyRemove(mafNode *n)
//----------------------------------------------------------------------------
{
  mafNodeIterator *iter = n->NewIterator();
  iter->IgnoreVisibleToTraverse(true); // ignore visible to traverse flag and visits all nodes
  iter->SetTraversalModeToPostOrder(); // traverse is: first the subtree left to right, then the root
  for (mafNode *node = iter->GetFirstNode(); node; node = iter->GetNextNode())
		mafEventMacro(mafEvent(this,VME_REMOVING,node)); // raise notification event (to logic)
  iter->Delete();
}
//----------------------------------------------------------------------------
void mafVMEManager::NotifyAdd(mafNode *n)
//----------------------------------------------------------------------------
{
  bool checkSingleFile = n->IsMAFType(mafVMERoot);
  
  mafNodeIterator *iter = n->NewIterator();
  iter->IgnoreVisibleToTraverse(true); // ignore visible to traverse flag and visits all nodes
  for (mafNode *node = iter->GetFirstNode(); node; node = iter->GetNextNode())
  {
    mafVMERoot *root = GetRoot();
    if (root != NULL)
    {
      mafVME *vme = mafVME::SafeDownCast(node);
      if (vme != NULL)
      {
        // Update the new VME added to the tree with the current time-stamp
        // present in the tree.
        vme->SetTimeStamp(root->GetTimeStamp());
      }
    }

    mafEventMacro(mafEvent(this,VME_ADDED,node)); // raise notification event (to logic)
    if (checkSingleFile)
    {
      // if checkSingleFile == true this method is called by the MSFOpen
      // so we have to check if the msf has been saved in single binary file or not.
      mafVMEGenericAbstract *vmeWithDataVector = mafVMEGenericAbstract::SafeDownCast(node);
      if (vmeWithDataVector)
      {
        mafDataVector *dv = vmeWithDataVector->GetDataVector();
        if (dv != NULL)
        {
          SetSingleBinaryFile(dv->GetSingleFileMode());
          checkSingleFile = false;
        }
      }
    }
  }
  iter->Delete();
  m_Modified = true;
}
//----------------------------------------------------------------------------
void mafVMEManager::SetFileHistoryMenu(wxMenu *menu)
//----------------------------------------------------------------------------
{
  m_FileHistory.UseMenu(menu);
	m_FileHistory.Load(*m_Config); // Loads file history from registry
}
//----------------------------------------------------------------------------
bool mafVMEManager::AskConfirmAndSave()
//----------------------------------------------------------------------------
{
  bool go = true;
	if (m_Modified) // check if the msf has been modified
	{
		int answer = wxMessageBox(_("your work is modified, would you like to save it?"),_("Confirm"),wxYES_NO|wxCANCEL|wxICON_QUESTION,mafGetFrame()); // ask user if will save msf before closing
		if(answer == wxCANCEL) go = false;
		if(answer == wxYES)    MSFSave();
	}
	return go;
}
//----------------------------------------------------------------------------
void mafVMEManager::UpdateFromTag(mafNode *n)
//----------------------------------------------------------------------------
{
  /*
  if (n)
  {
    mafVmeData *vd = (mafVmeData *)n->GetClientData();
    if (vd)
      vd->UpdateFromTag();
  }
  else
  {
    mafNodeIterator *iter = m_Storage->GetRoot()->NewIterator();
    for (mafNode *node = iter->GetFirstNode(); node; node = iter->GetNextNode())
    {
      mafVmeData *vd = (mafVmeData *)node->GetClientData();
      if (vd)
        vd->UpdateFromTag();
    }
    iter->Delete();
  }
  */
}
