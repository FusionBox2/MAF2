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

#include <fstream>


#include <wx/zipstrm.h>
#include <wx/zstream.h>
#include <wx/ffile.h>
#include <wx/wfstream.h>
#include <wx/fs_zip.h>
#include <wx/filename.h>
#include "wx/dir.h"
#include "mafDirectory.h"

bool mafDirMake(const mafString& directory)
{
  return wxMkdir(directory.GetCStr());
}
bool mafDirRemove(const mafString& directory)
{
  return ::wxRmdir(directory.GetCStr());
}
bool mafDirExists(const mafString& directory)
{
  return ::wxDirExists(directory.GetCStr());
}
bool mafFileRemove(const mafString& file)
{
  return ::wxRemoveFile(file.GetCStr());
}
bool mafFileRename(const mafString& files, const mafString& filet)
{
  return wxRenameFile(files.GetCStr(), filet.GetCStr());
}
bool mafFileCopy(const mafString& files, const mafString& filet, bool overwrite)
{
  return wxCopyFile(files.GetCStr(), filet.GetCStr(), overwrite);
}
bool mafFileExists(const mafString& file)
{
  return ::wxFileExists(file.GetCStr());
}
mafString mafFindFirstFile(const mafString& spec, bool dir)
{
  return wxFindFirstFile(spec.GetCStr(), dir ? wxDIR : wxFILE).c_str();
}
mafString mafFindNextFile()
{
  return wxFindNextFile().c_str();
}
mafString mafPathOnly(const mafString& fullname)
{
  return wxPathOnly(fullname.GetCStr()).c_str();
}
mafString mafFileNameFromPath(const mafString& fullname)
{
  return wxFileNameFromPath(fullname.GetCStr()).c_str();
}


mafString mafCreateTempFileName(const mafString& base)
{
  mafString name;
  name = wxFileName::CreateTempFileName(base.GetCStr()); // used to get a valid temporary name for cache directory
  mafFileRemove(name);
  name.ParsePathName();
  return name;
}
void mafSplitPath(const mafString& fullname, mafString* path, mafString* name, mafString* ext)
{
  wxString wpath, wname, wext;
  wxSplitPath(fullname.GetCStr(),&wpath,&wname,&wext);
  if(path)
    *path = wpath;
  if(name)
    *name = wname;
  if(ext)
    *ext = wext;
}
void mafSplitPath(const mafString& fullname, mafString* path, mafString* nameext)
{
  mafString name, ext;
  mafSplitPath(fullname, path, &name, &ext);
  if(nameext)
    *nameext = name + "." + ext;
}


//----------------------------------------------------------------------------
void mafRemoveDirectory(const mafString& directory)
//----------------------------------------------------------------------------
{
  if(directory.IsEmpty())
    return;
  wxSetWorkingDirectory(mafGetApplicationDirectory().GetCStr());
  if(!mafDirExists(directory))
    return;
  mafString file_match = directory + "/*.*";
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

  wxString path, name, ext;

  mafString zip_cache = wxPathOnly(filename.GetCStr()); // get the directory
  if (zip_cache.IsEmpty())
    zip_cache = stor_tmp;

  zip_cache += "/";
  zip_cache = wxFileName::CreateTempFileName(zip_cache.GetCStr()); // used to get a valid temporary name for cache directory
  wxRemoveFile(zip_cache.GetCStr());
  wxSplitPath(zip_cache.GetCStr(),&path,&name,&ext);
  zip_cache = path + "/" + name + ext;
  zip_cache.ParsePathName();

  if (!wxDirExists(zip_cache.GetCStr()))
    wxMkdir(zip_cache.GetCStr()); // create a temporary directory in which extract the archive
  tmpDir = zip_cache;

  wxString complete_name, zfile, out_file;
  wxSplitPath(filename.GetCStr(),&path,&name,&ext);
  complete_name = name + "." + ext;

  wxFSFile *zfileStream;
  wxZlibInputStream *zip_is;
  wxString pkg = "#zip:";
  wxString header_name = complete_name + pkg;
  int length_header_name = header_name.Length();
  bool enable_mid = false;

  fileSystem->ChangePathTo(filename.GetCStr());
  // extract filename from the zip archive
  zfile = fileSystem->FindFirst(complete_name+pkg+name+"\\*.*");
  if (zfile == "")
  {
    enable_mid = true;
    // no files found: try to search inside the archive without filename
    zfile = fileSystem->FindFirst(complete_name+pkg+"\\*.*");
  }
  if (zfile == "")
  {
    cppDEL(fileSystem);
    mafRemoveDirectory(tmpDir); // remove the temporary directory
    tmpDir = "";
    return "";
  }

  char *buf;
  int s_size;
  std::ofstream out_file_stream;

  for (;zfile != ""; zfile = fileSystem->FindNext())
  {
    zfileStream = fileSystem->OpenFile(zfile);
    if (zfileStream == NULL) // unable to open the file
    {
      cppDEL(fileSystem);
      mafRemoveDirectory(tmpDir); // remove the temporary directory
      tmpDir = "";
      return "";
    }
    zip_is = (wxZlibInputStream *)zfileStream->GetStream();
    wxSplitPath(zfile,&path,&name,&ext);
    complete_name = name + "." + ext;
    if (enable_mid)
      complete_name = complete_name.Mid(length_header_name);
    out_file = tmpDir + "\\" + complete_name;
    if(ext == "msf")
    {
      MSFFile = out_file; // The file to extract is an msf
      out_file_stream.open(out_file, std::ios_base::out);
    }
    else
      out_file_stream.open(out_file, std::ios_base::binary); // The file to extract is a binary
    s_size = zip_is->GetSize();
    buf = new char[s_size];
    zip_is->Read(buf,s_size);
    out_file_stream.write(buf, s_size);
    out_file_stream.close();
    delete[] buf;
    zfileStream->UnRef();
    delete zfileStream;
  }

  fileSystem->ChangePathTo(tmpDir.GetCStr(), TRUE);

  if (MSFFile.IsEmpty()) // msf file not extracted
  {
    mafMessage(_("compressed archive is not a valid msf file!"), _("Error"));
    return "";
  }
  fileSystem->CleanUpHandlers(); // Handlers are shared trough file systems.
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

  wxString path, name, ext;

  mafString zipFile = filename;
  std::vector<mafString> extractedFiles;

  wxString complete_name, zfile, out_file;
  wxSplitPath(zipFile.GetCStr(),&path,&name,&ext);
  complete_name = name + "." + ext;

  wxFSFile *zfileStream;
  wxZlibInputStream *zip_is;
  wxString pkg = "#zip:";
  wxString header_name = complete_name + pkg;
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

  fileSystem->ChangePathTo(zipFile.GetCStr());
  // extract filename from the zip archive
  zfile = fileSystem->FindFirst(complete_name+pkg+name+"\\*.*");
  if (zfile == "")
  {
    enable_mid = true;
    // no files found: try to search inside the archive without filename
    zfile = fileSystem->FindFirst(complete_name+pkg+"\\*.*");
  }
  /*if (zfile == "")
    return;*/
  for(; zfile != ""; zfile = fileSystem->FindNext())
  {
    zfileStream = fileSystem->OpenFile(zfile);
    if (zfileStream == NULL) // unable to open the file
    {
      for(std::vector<mafString>::iterator it = extractedFiles.begin(); it != extractedFiles.end(); ++it)
      {
        wxRemoveFile((*it).GetCStr());
      }
      return;
    }
    wxSplitPath(zfile,&path,&name,&ext);
    complete_name = name + "." + ext;
    if (enable_mid)
      complete_name = complete_name.Mid(length_header_name);
    zip_is = (wxZlibInputStream *)zfileStream->GetStream();
    out_file = temp_directory + "\\" + complete_name;
    char *buf;
    int s_size;
    std::ofstream out_file_stream;
    out_file_stream.open(out_file, std::ios_base::binary); // the file to extract is a binary
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
  fileSystem->ChangePathTo(temp_directory.GetCStr(), TRUE);
  fileSystem->CleanUpHandlers(); // Handlers are shared trough file systems.
  cppDEL(fileSystem);
}
void mafExtractZIP(const mafString& filename, const mafString& entry_name, void *& buffer, size_t& size)
{
  wxFileInputStream in(filename.GetCStr());
  wxZipInputStream zip(in);
  if (!in || !zip)
    return;
  wxZipEntry *entry = NULL;
  // convert the local name we are looking for into the internal format
  wxString name = wxZipEntry::GetInternalName(entry_name.GetCStr());

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
  wxFileInputStream in(filename.GetCStr());
  wxZipInputStream zip(in);
  if (!in || !zip)
    return;
  wxZipEntry *entry = NULL;
  // convert the local name we are looking for into the internal format
  wxString name = wxZipEntry::GetInternalName(entry_name.GetCStr());

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
    wxString out_file;
    char *buf;
    int s_size;
    std::ofstream out_file_stream;
    out_file = temp_directory + "/" + entry_name;
    out_file_stream.open(out_file, std::ios_base::binary); // the file to extract is a binary
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
  wxString name, path, short_name, ext;
  wxFileOutputStream out(zipname.GetCStr());
  wxZipOutputStream zip(out);

  if (!out || !zip)
    return false;

  for (size_t i = 0; i < files.size(); i++)
  {
    name = files[i].GetCStr();
    wxSplitPath(name, &path, &short_name, &ext);
    short_name += ".";
    short_name += ext;

    if (wxDirExists(name)) 
    {
      if (!zip.PutNextDirEntry(name)) // put the file inside the archive
        return false;
    }
    else 
    {
      wxFFileInputStream in(name);

      if (in.Ok()) 
      {
        wxDateTime dt(wxFileModificationTime(name)); // get the file modification time

        if (!zip.PutNextEntry(short_name, dt, in.GetLength()) || !zip.Write(in) || !in.Eof()) // put the file inside the archive
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
  wxDir::GetAllFiles(dir.GetCStr(), &files);

  std::vector<mafString> directory;
  for (size_t i = 0; i < files.GetCount(); i++)
    directory.push_back(files.Item(i).c_str());

  if(!mafMakeZip(filename, directory))
    mafMessage(_("Failed to create compressed archive!"),_("Error"));
}
