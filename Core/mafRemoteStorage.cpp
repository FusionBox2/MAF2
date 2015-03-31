/*=========================================================================

 Program: MAF2
 Module: mafRemoteStorage
 Authors: Paolo Quadrani
 
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

#include <wx/tokenzr.h>

#include "mafRemoteStorage.h"
#include "mmdRemoteFileManager.h"

#include "mafDirectory.h"
#include "mafCurlUtility.h"
#include "mafEvent.h"
#include "mafFilesDirs.h"

//------------------------------------------------------------------------------
mafCxxTypeMacro(mafRemoteStorage)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
mafRemoteStorage::mafRemoteStorage()
//------------------------------------------------------------------------------
{
  m_HostName = "";
  m_LocalMSFFolder = "";
  m_RemoteRepository = "";
  m_RemoteMSF = "";
  m_IsRemoteMSF = false;
  mafNEW(m_RemoteFileManager);
}
//------------------------------------------------------------------------------
void mafRemoteStorage::Initialize()
//------------------------------------------------------------------------------
{
  mafEventMacro(mafEvent(this,DEVICE_ADD,m_RemoteFileManager));
}
//------------------------------------------------------------------------------
mafRemoteStorage::~mafRemoteStorage()
//------------------------------------------------------------------------------
{
  mafEventMacro(mafEvent(this,DEVICE_REMOVE,m_RemoteFileManager));
  mafSleep(100);
  mafDEL(m_RemoteFileManager);
}
//------------------------------------------------------------------------------
void mafRemoteStorage::SetUsername(const mafString &usr)
//------------------------------------------------------------------------------
{
  m_RemoteFileManager->SetUsername(usr);
}
//------------------------------------------------------------------------------
void mafRemoteStorage::SetPassword(const mafString &pwd)
//------------------------------------------------------------------------------
{
  m_RemoteFileManager->SetPassword(pwd);
}
//------------------------------------------------------------------------------
void mafRemoteStorage::SetHostName(const mafString &host)
//------------------------------------------------------------------------------
{
  m_HostName = host;
}
//------------------------------------------------------------------------------
void mafRemoteStorage::SetRemotePort(const int &port)
//------------------------------------------------------------------------------
{
  m_RemoteFileManager->SetRemotePort(port);
}
//------------------------------------------------------------------------------
int mafRemoteStorage::ResolveInputURL(const mafString& url, mafString &filename, mafBaseEventHandler *observer)
//------------------------------------------------------------------------------
{
  int res = MAF_OK;
  mafString path;
  path = mafPathOnly(url);
  if (path.IsEmpty())
  {
    mafString base_path;
    base_path = mafPathOnly(m_ParserURL);

    filename = base_path;

    if (!base_path.IsEmpty())
      filename << "/";

    filename << url;
  }
  else
  {
    filename = url;
  }

  mafString protocol = "";
  m_IsRemoteMSF = IsRemote(filename,protocol);
  m_RemoteFileManager->EnableAuthentication(protocol.Equals("https"));

  if (m_IsRemoteMSF)
  {
    // Download the file if it is not present into the cache
    mafString local_filename;
    //res = DownloadRemoteFile(filename, local_filename);
    //---------------------------------- Build folder into the local cache to put the downloaded file
    mafString path, name, ext, tmpFolder;
    mafString baseName = mafFileNameFromPath(filename);
    mafSplitPath(baseName,&path,&name,&ext);

    if (ext == "msf")
    {
      m_RemoteMSF = filename;
      tmpFolder = GetTmpFolder();//m_LocalCacheFolder;
      tmpFolder += "\\";
      tmpFolder += name;

      if (!mafDirExists(tmpFolder))
      {
        mafDirMake(tmpFolder);
      }
      m_LocalMSFFolder = tmpFolder;
    }

    local_filename = m_LocalMSFFolder;
    local_filename += "\\";
    local_filename += mafFileNameFromPath(filename);

    if (mafFileExists(local_filename))
    {
      return MAF_OK;
    }
    //-----------------------------------------------
    res = m_RemoteFileManager->DownloadRemoteFile(filename, local_filename);
    m_RemoteRepository = mafPathOnly(filename);
    filename = local_filename;
  }
  else
  {
    m_RemoteRepository = "";
  }

  return res;
}
//------------------------------------------------------------------------------
int mafRemoteStorage::StoreToURL(const mafString& filename, const mafString& url)
//------------------------------------------------------------------------------
{
  if (!m_IsRemoteMSF)
    return Superclass::StoreToURL(filename, url);
  int save_res = MAF_ERROR;
  if(url.IsEmpty())
  {
    assert(false);
    return MAF_ERROR;
  }

  // currently no real URL support
  mafString path, base_path, fullpathname;
  path = mafPathOnly(url);

  if (path.IsEmpty())
  {
    // if local file prepends base_path
    base_path = m_LocalMSFFolder;
    if (!base_path.IsEmpty())
    {
      fullpathname = base_path + "/" + url;
    }
    else
    {
      fullpathname = url;
    }

    if (IsFileInDirectory(url)) // IsFileInDirectory accepts URL specifications
    {
      // remove old file if present.
      // With remote files should be sufficient overwrite it by uploading the new one.
      if (IsFileInLocalDirectory(url))
      {
        DeleteURL(url);
      }
    }

    // currently only local files are supported
    save_res = mafFileRename(filename, fullpathname) ? MAF_OK : MAF_ERROR;
  }
  else
  {
    mafString baseName = mafFileNameFromPath(url);
    fullpathname = m_LocalMSFFolder;
    fullpathname += "\\";
    fullpathname += baseName;
    // remove old file if present
    mafFileRemove(fullpathname);
    // currently only local files are supported
    save_res = mafFileRename(filename, fullpathname) ? MAF_OK : MAF_ERROR;
  }
  if (save_res == MAF_OK && !m_RemoteRepository.IsEmpty())
  {
    mafString remote_file = m_RemoteRepository + "/" + mafFileNameFromPath(url);
    save_res = m_RemoteFileManager->UploadLocalFile(fullpathname, remote_file);
  }
  return save_res;
}
//----------------------------------------------------------------------------
bool mafRemoteStorage::IsFileInLocalDirectory(const mafString& filename)
//----------------------------------------------------------------------------
{
  return m_LocalFilesDictionary.find(filename)!=m_LocalFilesDictionary.end();
}

//----------------------------------------------------------------------------
int mafRemoteStorage::OpenDirectory(const mafString& pathname)
//----------------------------------------------------------------------------
{
  if (!m_IsRemoteMSF)
    return Superclass::OpenDirectory(pathname);
  mafString baseName = wxFileNameFromPath(m_RemoteMSF.GetCStr());
  mafString path, name, ext;
  mafString query_string;
  mafSplitPath(baseName,&path,&name,&ext);
  query_string = m_RemoteRepository;
  query_string += "/";
  query_string += "msfList?prefix=";
  query_string += name;

  struct msfTreeSearchReult chunk;
  chunk.memory=NULL; // we expect realloc(NULL, size) to work 
  chunk.size = 0;    // no data at this point 

  int res = m_RemoteFileManager->ListRemoteDirectory(query_string, chunk);
  if (res == MAF_OK)
  {
    m_FilesDictionary.clear();
    wxString msf_list = chunk.memory;
    wxStringTokenizer tkz(msf_list, "\n");
    while (tkz.HasMoreTokens())
    {
      m_FilesDictionary.insert(tkz.GetNextToken().c_str());
    }
  }
  else
    return MAF_ERROR;
  return OpenLocalMSFDirectory();
}
//------------------------------------------------------------------------------
int mafRemoteStorage::OpenLocalMSFDirectory()
//------------------------------------------------------------------------------
{
  mafDirectory dir;
  if (!dir.Load(m_LocalMSFFolder))
    return MAF_ERROR;

  m_LocalFilesDictionary.clear();

  for (int i=0;i<dir.GetNumberOfFiles();i++)
  {
    const char *fullname=dir.GetFile(i);  
    const char *filename=mafString::BaseName(fullname);
    m_LocalFilesDictionary.insert(filename);
  }
  return MAF_OK;
}
//------------------------------------------------------------------------------
const mafString& mafRemoteStorage::GetTmpFolder()
//------------------------------------------------------------------------------
{
  if(!m_IsRemoteMSF)
    return Superclass::GetTmpFolder();
  if(!m_TmpFolder.IsEmpty())
    return Superclass::GetTmpFolder();
  m_DefaultTmpFolder = m_LocalMSFFolder;
  m_DefaultTmpFolder << "/";
  return m_DefaultTmpFolder;
}
