/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafFilesDirs.cpp,v $
  Language:  C++
  Date:      $Date: 2005-01-11 17:34:57 $
  Version:   $Revision: 1.4 $
  Authors:   Based on itkDirectory (www.itk.org), adapted by Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#include "mafDefines.h"
#include "mafDecl.h"
#include "mafFilesDirs.h"
#include <list>

#include <fstream>


#include <wx/zipstrm.h>
#include <wx/zstream.h>
#include <wx/ffile.h>
#include <wx/wfstream.h>
#include <wx/fs_zip.h>
#include <wx/filename.h>
#include "wx/dir.h"
#include "mafDirectory.h"

mafString mafStripMenuCodes(const mafString& com)
{
  return mafWxToString(wxStripMenuCodes(com.toWx()));
}


bool mafDirMake(const mafString& directory)
{
  return wxMkdir(directory.toWx());
}
bool mafDirRemove(const mafString& directory)
{
  return ::wxRmdir(directory.toWx());
}
bool mafDirExists(const mafString& directory)
{
  return ::wxDirExists(directory.toWx());
}
bool mafFileRemove(const mafString& file)
{
  return ::wxRemoveFile(file.toWx());
}
bool mafFileRename(const mafString& files, const mafString& filet)
{
  return wxRenameFile(files.toWx(), filet.toWx());
}
bool mafFileCopy(const mafString& files, const mafString& filet, bool overwrite)
{
  return wxCopyFile(files.toWx(), filet.toWx(), overwrite);
}
bool mafFileExists(const mafString& file)
{
  return ::wxFileExists(file.toWx());
}
mafString mafFindFirstFile(const mafString& spec, bool dir)
{
  return mafWxToString(wxFindFirstFile(spec.toWx(), dir ? wxDIR : wxFILE));
}
mafString mafFindNextFile()
{
  return mafWxToString(wxFindNextFile());
}
mafString mafPathOnly(const mafString& fullname)
{
  return mafWxToString(wxPathOnly(fullname.toWx()));
}
mafString mafFileNameFromPath(const mafString& fullname)
{
  return mafWxToString(wxFileNameFromPath(fullname.toWx()));
}


mafString mafCreateTempFileName(const mafString& base)
{
  mafString name;
  name = mafWxToString(wxFileName::CreateTempFileName(base.toWx())); // used to get a valid temporary name for cache directory
  mafFileRemove(name);
  name.ParsePathName();
  return name;
}
void mafSplitPath(const mafString& fullname, mafString* path, mafString* name, mafString* ext)
{
  wxString wpath, wname, wext;
  wxFileName::SplitPath(fullname.toWx(),&wpath,&wname,&wext);
  if(path)
    *path = mafWxToString(wpath);
  if(name)
    *name = mafWxToString(wname);
  if(ext)
    *ext = mafWxToString(wext);
}
void mafSplitPath(const mafString& fullname, mafString* path, mafString* nameext)
{
  mafString name, ext;
  mafSplitPath(fullname, path, &name, &ext);
  if(nameext)
    *nameext = name + _R(".") + ext;
}


//----------------------------------------------------------------------------
void mafRemoveDirectory(const mafString& directory)
//----------------------------------------------------------------------------
{
  if(directory.IsEmpty())
    return;
  wxSetWorkingDirectory(mafGetApplicationDirectory().toWx());
  if(!mafDirExists(directory))
    return;
  mafString file_match = directory + _R("/*.*");
  mafString f = mafFindFirstFile(file_match);
  while (!f.IsEmpty())
  {
    mafFileRemove(f);
    f = mafFindNextFile();
  }
  mafDirRemove(directory);
}

//----------------------------------------------------------------------------
mafString mafOpenZIP(const mafString& filename, const mafString& stor_tmp, mafString& tmpDir)
//----------------------------------------------------------------------------
{
  wxZipFSHandler      *zipHandler;      ///< Handler for zip archive (used to open zmsf files)
  wxFileSystem        *fileSystem;      ///< File system manager
  mafString           MSFFile;

  fileSystem = new wxFileSystem();
  zipHandler = new wxZipFSHandler();
  fileSystem->AddHandler(zipHandler);

  mafString path, name, ext;

  mafString zip_cache = mafPathOnly(filename); // get the directory
  if (zip_cache.IsEmpty())
    zip_cache = stor_tmp;

  zip_cache += _R("/");
  zip_cache = mafCreateTempFileName(zip_cache); // used to get a valid temporary name for cache directory
  mafFileRemove(zip_cache);
  mafSplitPath(zip_cache,&path,&name,&ext);
  zip_cache = path + _R("/") + name + ext;
  zip_cache.ParsePathName();

  if (!mafDirExists(zip_cache))
    mafDirMake(zip_cache); // create a temporary directory in which extract the archive
  tmpDir = zip_cache;

  mafString complete_name, zfile, out_file;
  mafSplitPath(filename,&path,&name,&ext);
  complete_name = name + _R(".") + ext;

  wxFSFile *zfileStream;
  wxZlibInputStream *zip_is;
  mafString pkg = _R("#zip:");
  mafString header_name = complete_name + pkg;
  int length_header_name = header_name.Length();
  bool enable_mid = false;

  fileSystem->ChangePathTo(filename.toWx());
  // extract filename from the zip archive
  zfile = mafWxToString(fileSystem->FindFirst((complete_name+pkg+name+_R("\\*.*")).toWx()));
  if (zfile.IsEmpty())
  {
    enable_mid = true;
    // no files found: try to search inside the archive without filename
    zfile = mafWxToString(fileSystem->FindFirst((complete_name+pkg+_R("\\*.*")).toWx()));
  }
  if (zfile.IsEmpty())
  {
    fileSystem->RemoveHandler(zipHandler);
    cppDEL(zipHandler);
    cppDEL(fileSystem);
    mafRemoveDirectory(tmpDir); // remove the temporary directory
    return mafString();
  }

  char *buf;
  int s_size;
  std::ofstream out_file_stream;

  for (;!zfile.IsEmpty(); zfile = mafWxToString(fileSystem->FindNext()))
  {
    zfileStream = fileSystem->OpenFile(zfile.toWx());
    if (zfileStream == NULL) // unable to open the file
    {
      fileSystem->RemoveHandler(zipHandler);
      cppDEL(zipHandler);
      cppDEL(fileSystem);
      mafRemoveDirectory(tmpDir); // remove the temporary directory
      return mafString();
    }
    zip_is = (wxZlibInputStream *)zfileStream->GetStream();
    mafSplitPath(zfile,&path,&name,&ext);
    complete_name = name + _R(".") + ext;
    if (enable_mid)
      complete_name = mafWxToString(complete_name.toWx().Mid(length_header_name));
    out_file = tmpDir + _R("\\") + complete_name;
    if(ext == _R("msf"))
    {
      MSFFile = out_file; // The file to extract is an msf
      out_file_stream.open(out_file.GetCStr(), std::ios_base::out);
    }
    else
      out_file_stream.open(out_file.GetCStr(), std::ios_base::binary); // The file to extract is a binary
    s_size = zip_is->GetSize();
    buf = new char[s_size];
    zip_is->Read(buf,s_size);
    out_file_stream.write(buf, s_size);
    out_file_stream.close();
    delete[] buf;
    zfileStream->UnRef();
    delete zfileStream;
  }

  fileSystem->ChangePathTo(tmpDir.toWx(), TRUE);

  if (MSFFile.IsEmpty()) // msf file not extracted
  {
    mafErrorMessage(_M(mafString(_L("compressed archive is not a valid msf file!"))));
    return mafString();
  }
  fileSystem->RemoveHandler(zipHandler);
  cppDEL(zipHandler);
  cppDEL(fileSystem);

  // return the extracted msf filename
  return MSFFile;
}
//----------------------------------------------------------------------------
void mafOpenZIP(const mafString& filename, const mafString& temp_directory)
//----------------------------------------------------------------------------
{
  wxZipFSHandler      *zipHandler = NULL;      ///< Handler for zip archive (used to open zmsf files)
  wxFileSystem        *fileSystem = NULL;      ///< File system manager

  mafString path, name, ext;

  mafString zipFile = filename;
  std::list<mafString> extractedFiles;

  mafString complete_name, zfile, out_file;
  mafSplitPath(zipFile,&path,&name,&ext);
  complete_name = name + _R(".") + ext;

  wxFSFile *zfileStream;
  wxZlibInputStream *zip_is;
  mafString pkg = _R("#zip:");
  mafString header_name = complete_name + pkg;
  int length_header_name = header_name.Length();
  bool enable_mid = false;
  if(fileSystem == NULL)
    fileSystem = new wxFileSystem();

  if(zipHandler == NULL)
  {
    zipHandler = new wxZipFSHandler();
    fileSystem->AddHandler(zipHandler); // add the handler that manage zip protocol
    // (the handler to manage the local files protocol is already added to wxFileSystem)
  }

  fileSystem->ChangePathTo(zipFile.toWx());
  // extract filename from the zip archive
  zfile = mafWxToString(fileSystem->FindFirst((complete_name+pkg+name+_R("\\*.*")).toWx()));
  if (zfile.IsEmpty())
  {
    enable_mid = true;
    // no files found: try to search inside the archive without filename
    zfile = mafWxToString(fileSystem->FindFirst((complete_name+pkg+_R("\\*.*")).toWx()));
  }
  /*if (zfile == "")
    return;*/
  for(; !zfile.IsEmpty(); zfile = mafWxToString(fileSystem->FindNext()))
  {
    zfileStream = fileSystem->OpenFile(zfile.toWx());
    if (zfileStream == NULL) // unable to open the file
    {
      for(auto it = extractedFiles.begin(); it != extractedFiles.end(); ++it)
      {
        mafFileRemove(*it);
      }
      break;
    }
    mafSplitPath(zfile,&path,&name,&ext);
    complete_name = name + _R(".") + ext;
    if (enable_mid)
      complete_name = mafWxToString(complete_name.toWx().Mid(length_header_name));
    zip_is = (wxZlibInputStream *)zfileStream->GetStream();
    out_file = temp_directory + _R("\\") + complete_name;
    char *buf;
    int s_size;
    std::ofstream out_file_stream;
    out_file_stream.open(out_file.GetCStr(), std::ios_base::binary); // the file to extract is a binary
    s_size = zip_is->GetSize();
    buf = new char[s_size];
    zip_is->Read(buf,s_size);
    out_file_stream.write(buf, s_size);
    out_file_stream.close();
    delete[] buf;
    zfileStream->UnRef();
    delete zfileStream;
    extractedFiles.push_back(out_file);
  }
  fileSystem->ChangePathTo(temp_directory.toWx(), TRUE);
  fileSystem->RemoveHandler(zipHandler);
  cppDEL(zipHandler);
  cppDEL(fileSystem);
}
void mafExtractZIP(const mafString& filename, const mafString& entry_name, void *& buffer, size_t& size)
{
  wxFileInputStream in(filename.toWx());
  wxZipInputStream zip(in);
  if (!in || !zip)
    return;
  wxZipEntry *entry = NULL;
  // convert the local name we are looking for into the internal format
  wxString name = wxZipEntry::GetInternalName(entry_name.toWx());

  // call GetNextEntry() until the required internal name is found
  // to be re-factored for efficiency reasons.
  do 
  {
    if (entry)
    {
      delete entry;
      entry = NULL;
    }
    entry = zip.GetNextEntry();
  } while(entry != NULL && entry->GetInternalName() != name);

  if (entry != NULL) 
  {
    size = entry->GetSize();
    buffer = new char[size];
    zip.Read(buffer, size);
    delete entry;
    entry = NULL;
  }
}

//----------------------------------------------------------------------------
void mafExtractZIP(const mafString& filename, const mafString& temp_directory, const mafString& entry_name)
//----------------------------------------------------------------------------
{
  wxFileInputStream in(filename.toWx());
  wxZipInputStream zip(in);
  if (!in || !zip)
    return;
  wxZipEntry *entry = NULL;
  // convert the local name we are looking for into the internal format
  wxString name = wxZipEntry::GetInternalName(entry_name.toWx());

  // call GetNextEntry() until the required internal name is found
  // to be re-factored for efficiency reasons.
  do 
  {
    if (entry)
    {
      delete entry;
      entry = NULL;
    }
    entry = zip.GetNextEntry();
  } while(entry != NULL && entry->GetInternalName() != name);

  if (entry != NULL) 
  {
    // read the entry's data...
    mafString out_file;
    char *buf;
    int s_size;
    std::ofstream out_file_stream;
    out_file = temp_directory + _R("/") + entry_name;
    out_file_stream.open(out_file.GetCStr(), std::ios_base::binary); // the file to extract is a binary
    s_size = entry->GetSize();
    buf = new char[s_size];
    zip.Read(buf, s_size);
    out_file_stream.write(buf, s_size);
    out_file_stream.close();
    delete[] buf;
    delete entry;
    entry = NULL;
  }
}
//----------------------------------------------------------------------------
bool mafMakeZip(const mafString &zipname, const std::vector<mafString>& files)
//----------------------------------------------------------------------------
{
  mafString name, path, short_name, ext;
  wxFileOutputStream out(zipname.toWx());
  wxZipOutputStream zip(out);

  if (!out || !zip)
    return false;

  for (size_t i = 0; i < files.size(); i++)
  {
    name = files[i];
    mafSplitPath(name, &path, &short_name, &ext);
    short_name += _R(".");
    short_name += ext;

    if (mafDirExists(name)) 
    {
      if (!zip.PutNextDirEntry(name.toWx())) // put the file inside the archive
        return false;
    }
    else 
    {
      wxFFileInputStream in(name.toWx());

      if (in.Ok()) 
      {
        wxDateTime dt(wxFileModificationTime(name.toWx())); // get the file modification time

        if (!zip.PutNextEntry(short_name.toWx(), dt, in.GetLength()) || !zip.Write(in) || !in.Eof()) // put the file inside the archive
          return false;
      }
    }
  }

  return zip.Close() && out.Close();
}

//----------------------------------------------------------------------------
void mafZIPSave(const mafString& filename, const mafString& dir)
//----------------------------------------------------------------------------
{
  if(filename.IsEmpty())
    return;

  wxArrayString files;
  wxDir::GetAllFiles(dir.toWx(), &files);

  std::vector<mafString> directory;
  for (size_t i = 0; i < files.GetCount(); i++)
    directory.push_back(mafWxToString(files.Item(i)));

  if(!mafMakeZip(filename, directory))
    mafErrorMessage(_M(mafString(_L("Failed to create compressed archive!"))));
}
