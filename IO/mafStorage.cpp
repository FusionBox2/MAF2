/*=========================================================================

 Program: MAF2
 Module: mafStorage
 Authors: Marco Petrone m.petrone@cineca.it
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "mafStorage.h"
#include "mafStorable.h"
#include "mafDirectory.h"
#include "mafXMLStorage.h"

//------------------------------------------------------------------------------
mafCxxTypeMacro(mafStorage);
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
mafStorage::mafStorage()
//------------------------------------------------------------------------------
{
  m_TmpFileId       = 0;
  m_ErrorCode       = 0;
  m_TmpFolder       = mafWxToString(wxGetCwd());
  m_Parser          = mafXMLParser::New();
}

//------------------------------------------------------------------------------
mafStorage::~mafStorage()
//------------------------------------------------------------------------------
{
  mafDEL(m_Parser);
}

//------------------------------------------------------------------------------
int mafStorage::Store()
//------------------------------------------------------------------------------
{
  SetErrorCode(0);
  // extract the path substring
  mafString dir_path=m_URL;
  int last_slash=dir_path.FindLastChr('/');
  if (last_slash>=0)
  { 
    dir_path.Erase(last_slash);
  }
  else
  {
    dir_path.Clear();
  }

  //open the directory index
  if (OpenDirectory(dir_path)==MAF_ERROR)
  {
    mafErrorMessage(_M("I/O Error: stored failed because path not found!"));
    return MAF_ERROR;
  }

  // store the content
  int ret=InternalStore();
  
  // set the new filename as current
  m_ParserURL=m_URL; 

  // here I should add a call for packing/sending files
  
  return ret;
}
//------------------------------------------------------------------------------
int mafStorage::Restore()
//------------------------------------------------------------------------------
{
  SetErrorCode(0);
  m_ParserURL = m_URL; // set the new filename as current
  return InternalRestore();
}

//------------------------------------------------------------------------------
void mafStorage::ForceParserURL()
//------------------------------------------------------------------------------
{
  m_ParserURL = m_URL; // set the new filename as current
}

//------------------------------------------------------------------------------
const mafString& mafStorage::GetURL()
//------------------------------------------------------------------------------
{
  return m_URL;
}

//------------------------------------------------------------------------------
const mafString& mafStorage::GetParserURL()
//------------------------------------------------------------------------------
{
  return m_ParserURL;
}

//----------------------------------------------------------------------------
bool mafStorage::IsFileInDirectory(const mafString& filename)
//----------------------------------------------------------------------------
{
  return m_FilesDictionary.find(filename)!=m_FilesDictionary.end();
}

//------------------------------------------------------------------------------
void mafStorage::SetDocument(mafStorable *doc)
//------------------------------------------------------------------------------
{
  if(m_Parser)
    m_Parser->SetDocument(doc);
}
//------------------------------------------------------------------------------
mafStorable *mafStorage::GetDocument()
//------------------------------------------------------------------------------
{
  if(!m_Parser)
    return NULL;
  return m_Parser->GetDocument();
}
//------------------------------------------------------------------------------
const mafString& mafStorage::GetTmpFolder()
//------------------------------------------------------------------------------
{
  if (m_TmpFolder.IsEmpty())
  {
    mafString path=mafPathOnly(m_URL);
    if (!path.IsEmpty())
    {
      m_DefaultTmpFolder=path;
      m_DefaultTmpFolder+=_R("/");
    }
    else
    {
      m_DefaultTmpFolder.Clear();
    }

    return m_DefaultTmpFolder;
  }
  else
  {
    return m_TmpFolder;
  }
}

//----------------------------------------------------------------------------
int mafStorage::OpenDirectory(const mafString& pathname)
//----------------------------------------------------------------------------
{
  mafDirectory dir;
  if (pathname.IsEmpty())
  {
    if (!dir.Load(_R(".")))
      return MAF_ERROR;
  }
  else
  {
    if (!dir.Load(pathname))
      return MAF_ERROR;
  }

  m_FilesDictionary.clear();

  for (int i=0;i<dir.GetNumberOfFiles();i++)
  {
    mafString filename = dir.GetFile(i).BaseName();
    m_FilesDictionary.insert(filename);
  }

  return MAF_OK;
}

//------------------------------------------------------------------------------
void mafStorage::SetURL(const mafString& name)
//------------------------------------------------------------------------------
{
  if (m_URL!=name)
  {
    // when saving to a new file or loading a different file
    // simply clear the list of URLs to be released.
    m_GarbageCollector.clear();
    m_URL=name; // force copying the const char reference
  }
}


//------------------------------------------------------------------------------
void mafStorage::GetTmpFile(mafString &filename)
//------------------------------------------------------------------------------
{
  mafString tmpfname=GetTmpFolder();
  tmpfname+=_R("#tmp.");
  do 
  {
    tmpfname+=mafToString(m_TmpFileId++);	
  } while(m_TmpFileNames.find(tmpfname)!=m_TmpFileNames.end());
  
  filename=tmpfname;
  m_TmpFileNames.insert(filename);
}
//------------------------------------------------------------------------------
void mafStorage::ReleaseTmpFile(const mafString& filename)
//------------------------------------------------------------------------------
{
  std::set<mafString>::iterator it=m_TmpFileNames.find(filename);
  if (it!=m_TmpFileNames.end())
  {
    m_TmpFileNames.erase(it);
  }

  // remove file from disk if present
  mafFileRemove(filename);
}
//------------------------------------------------------------------------------
int mafStorage::ResolveInputURL(const mafString& url, mafString &filename, mafBaseEventHandler *observer)
//------------------------------------------------------------------------------
{
  // currently no real URL support
  mafString path;
  path=mafPathOnly(url);
  if (path.IsEmpty())
  {
    mafString base_path;
    base_path=mafPathOnly(m_ParserURL);

    filename=base_path;

    if (!base_path.IsEmpty())
      filename+=_R("/");

    filename+=url;
  }
  else
  {
    filename=url;
  }

  bool file_exist = mafFileExists(filename);
  return file_exist ? MAF_OK : MAF_WAIT;
}
//------------------------------------------------------------------------------
int mafStorage::StoreToURL(const mafString& filename, const mafString& url)
//------------------------------------------------------------------------------
{
  if(url.IsEmpty())
  {
    assert(false);
    return MAF_ERROR;
  }
  // currently no real URL support
  mafString path;
  path=mafPathOnly(url);

  if (path.IsEmpty())
  {
    // if local file prepend base_path
    mafString base_path,fullpathname;
    base_path=mafPathOnly(m_URL);
    if (!base_path.IsEmpty())
    {
      fullpathname=base_path+_R("/")+url;
    }
    else
    {
      fullpathname=url;
    }

    if (IsFileInDirectory(url)) // IsFileInDirectory accepts URL specifications
    {
      // remove old file if present
      DeleteURL(url);
    }

    // currently only local files are supported
    return mafFileRename(filename,fullpathname)?MAF_OK:MAF_ERROR;
  }
  else
  {
    // remove old file if present
    mafFileRemove(url);
    // currently only local files are supported
    return mafFileRename(filename,url)?MAF_OK:MAF_ERROR;
  } 
}

//------------------------------------------------------------------------------
int mafStorage::ReleaseURL(const mafString& url)
//------------------------------------------------------------------------------
{
  // add to list of files to be deleted
  m_GarbageCollector.insert(url);
  return MAF_OK;
}


//------------------------------------------------------------------------------
int mafStorage::DeleteURL(const mafString& url)
//------------------------------------------------------------------------------
{
  // currently no real URL support
  mafString path;
  path=mafPathOnly(url);

  if (path.IsEmpty())
  {
    // if local file prepend base_path
    mafString base_path,fullpathname;
    base_path=mafPathOnly(m_URL);
    fullpathname=base_path+_R("/")+url;

    if (IsFileInDirectory(url))
    {
      // remove old file if present
      mafFileRemove(fullpathname);
      return MAF_OK;
    }

    return MAF_ERROR;
  }
  else
  {
    return (mafFileRemove(url)?MAF_OK:MAF_ERROR);
  }

}

//------------------------------------------------------------------------------
void mafStorage::EmptyGarbageCollector()
//------------------------------------------------------------------------------
{
  for (std::set<mafString>::iterator it=m_GarbageCollector.begin();it!=m_GarbageCollector.end();it++)
  {
    DeleteURL(*it);
  }
  m_GarbageCollector.clear();
}
//------------------------------------------------------------------------------
int mafStorage::InternalStore()
//------------------------------------------------------------------------------
{
  mafString filename;

  // initially store to a tmp file
  GetTmpFile(filename);

  m_Parser->SetURL(filename);
  int errorCode = m_Parser->Store();

  // move to destination URL
  if (errorCode==0)
  {
    if (StoreToURL(filename,m_URL)!=MAF_OK)
    {
      mafErrorMessage(_M(_R("Unable to resolve URL for output XML file, a copy of the file can be found in: ") + filename));
      errorCode = 4;
    }
    else
    {
      //
      // clean the storage file directory
      //

      ReleaseTmpFile(filename); // remove the storage tmp file

      EmptyGarbageCollector();
    }
  }    
  return errorCode;
}
//------------------------------------------------------------------------------
int mafStorage::InternalRestore()
//------------------------------------------------------------------------------
{
  mafString filename;
  // here I should resolve the XML file name
  if (ResolveInputURL(m_ParserURL,filename) == MAF_ERROR)
  {
    mafErrorMessage(_M("Unable to resolve URL for input XML file"));
    return IO_WRONG_URL;
  }

  m_Parser->SetURL(filename);
  return m_Parser->Restore();
}
