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
mafCxxAbstractTypeMacro(mafParser);
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
mafStorage::mafStorage()
//------------------------------------------------------------------------------
{
  m_TmpFileId       = 0;
  m_ErrorCode       = 0;
  m_TmpFolder       = wxGetCwd().c_str();
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
    dir_path="";
  }

  //open the directory index
  if (OpenDirectory(dir_path)==MAF_ERROR)
  {
    mafErrorMessage("I/O Error: stored failed because path not found!");
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
const char *mafStorage::GetURL()
//------------------------------------------------------------------------------
{
  return m_URL;
}

//------------------------------------------------------------------------------
const char *mafStorage::GetPareserURL()
//------------------------------------------------------------------------------
{
  return m_ParserURL;
}

//----------------------------------------------------------------------------
bool mafStorage::IsFileInDirectory(const char *filename)
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
const char* mafStorage::GetTmpFolder()
//------------------------------------------------------------------------------
{
  if (m_TmpFolder.IsEmpty())
  {
    wxString path=wxPathOnly(m_URL.GetCStr());
    if (!path.IsEmpty())
    {
      m_DefaultTmpFolder=path;
      m_DefaultTmpFolder<<"/";
    }
    else
    {
      m_DefaultTmpFolder="";
    }

    return m_DefaultTmpFolder;
  }
  else
  {
    return m_TmpFolder;
  }
}

//----------------------------------------------------------------------------
int mafStorage::OpenDirectory(const char *pathname)
//----------------------------------------------------------------------------
{
  mafDirectory dir;
  if (mafString::IsEmpty(pathname))
  {
    if (!dir.Load("."))
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
    const char *fullname=dir.GetFile(i);  
    const char *filename=mafString::BaseName(fullname);
    m_FilesDictionary.insert(filename);
  }

  return MAF_OK;
}

//------------------------------------------------------------------------------
void mafStorage::SetURL(const char *name)
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
  tmpfname<<"#tmp.";
  do 
  {
    tmpfname<<mafString(m_TmpFileId++);	
  } while(m_TmpFileNames.find(tmpfname)!=m_TmpFileNames.end());
  
  filename=tmpfname;
  m_TmpFileNames.insert(filename);
}
//------------------------------------------------------------------------------
void mafStorage::ReleaseTmpFile(const char *filename)
//------------------------------------------------------------------------------
{
  std::set<mafString>::iterator it=m_TmpFileNames.find(filename);
  if (it!=m_TmpFileNames.end())
  {
    m_TmpFileNames.erase(it);
  }

#ifdef MAF_USE_WX
  // remove file from disk if present
  wxRemoveFile(filename);
#endif

}
//------------------------------------------------------------------------------
int mafStorage::ResolveInputURL(const char * url, mafString &filename, mafBaseEventHandler *observer)
//------------------------------------------------------------------------------
{
  // currently no real URL support
  wxString path;
  path=wxPathOnly(url);
  if (path.IsEmpty())
  {
    wxString base_path;
    base_path=wxPathOnly(m_ParserURL.GetCStr());

    filename=base_path;

    if (!base_path.IsEmpty())
      filename<<"/";

    filename<<url;
  }
  else
  {
    filename=url;
  }

  bool file_exist = wxFileExists(filename.GetCStr());
  return file_exist ? MAF_OK : MAF_WAIT;
}
//------------------------------------------------------------------------------
int mafStorage::StoreToURL(const char * filename, const char * url)
//------------------------------------------------------------------------------
{
  assert(url); // NULL url not yet supported

  // currently no real URL support
  wxString path;
  path=wxPathOnly(url);

  if (path.IsEmpty())
  {
    // if local file prepend base_path
    wxString base_path,fullpathname;
    base_path=wxPathOnly(m_URL.GetCStr());
    if (!base_path.IsEmpty())
    {
      fullpathname=base_path+"/"+url;
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
    return wxRenameFile(filename,fullpathname)?MAF_OK:MAF_ERROR;
  }
  else
  {
    // remove old file if present
    wxRemoveFile(url);
    // currently only local files are supported
    return wxRenameFile(filename,url)?MAF_OK:MAF_ERROR;
  } 
}

//------------------------------------------------------------------------------
int mafStorage::ReleaseURL(const char *url)
//------------------------------------------------------------------------------
{
  // add to list of files to be deleted
  m_GarbageCollector.insert(url);
  return MAF_OK;
}


//------------------------------------------------------------------------------
int mafStorage::DeleteURL(const char *url)
//------------------------------------------------------------------------------
{
  // currently no real URL support
  wxString path;
  path=wxPathOnly(url);

  if (path.IsEmpty())
  {
    // if local file prepend base_path
    wxString base_path,fullpathname;
    base_path=wxPathOnly(m_URL.GetCStr());
    fullpathname=base_path+"/"+url;

    if (IsFileInDirectory(url))
    {
      // remove old file if present
      wxRemoveFile(fullpathname);
      return MAF_OK;
    }

    return MAF_ERROR;
  }
  else
  {
    return (wxRemoveFile(url)?MAF_OK:MAF_ERROR);
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
      mafErrorMessage("Unable to resolve URL for output XML file, a copy of the file can be found in: %s",filename.GetCStr());
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
    mafErrorMessage("Unable to resolve URL for input XML file");
    return IO_WRONG_URL;
  }

  m_Parser->SetURL(filename);
  return m_Parser->Restore();
}
//------------------------------------------------------------------------------
mafParser::mafParser()
//------------------------------------------------------------------------------
{
  m_Document        = NULL;
  m_NeedsUpgrade    = false;
  m_ErrorCode       = 0;
  m_Version         = "1.1";
}
//------------------------------------------------------------------------------
int mafParser::Store()
//------------------------------------------------------------------------------
{
  SetErrorCode(0);
  return InternalStore();
}
//------------------------------------------------------------------------------
int mafParser::Restore()
//------------------------------------------------------------------------------
{
  SetErrorCode(0);
  return InternalRestore();
}
