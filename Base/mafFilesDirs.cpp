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
#include "mafDirectory.h"
#include "mafFilesDirs.h"

#include <wx/zipstrm.h>
#include <wx/zstream.h>
#include <wx/ffile.h>
#include <wx/wfstream.h>
#include <wx/fs_zip.h>
#include <wx/filename.h>
#include "wx/dir.h"

#include <list>
#include <fstream>

mafString mafStripMenuCodes(const mafString& com)
{
  return mafWxToString(wxStripMenuCodes(mafStringToWx(com)));
}


bool mafDirMake(const mafString& directory)
{
  return wxMkdir(mafStringToWx(directory));
}
bool mafDirRemove(const mafString& directory)
{
  return ::wxRmdir(mafStringToWx(directory));
}
bool mafDirExists(const mafString& directory)
{
  return ::wxDirExists(mafStringToWx(directory));
}
bool mafFileRemove(const mafString& file)
{
  return ::wxRemoveFile(mafStringToWx(file));
}
bool mafFileRename(const mafString& files, const mafString& filet)
{
  return wxRenameFile(mafStringToWx(files), mafStringToWx(filet));
}
bool mafFileCopy(const mafString& files, const mafString& filet, bool overwrite)
{
  return wxCopyFile(mafStringToWx(files), mafStringToWx(filet), overwrite);
}
bool mafFileExists(const mafString& file)
{
  return ::wxFileExists(mafStringToWx(file));
}
mafString mafFindFirstFile(const mafString& spec, bool dir)
{
  return mafWxToString(wxFindFirstFile(mafStringToWx(spec), dir ? wxDIR : wxFILE));
}
mafString mafFindNextFile()
{
  return mafWxToString(wxFindNextFile());
}
mafString mafPathOnly(const mafString& fullname)
{
  return mafWxToString(wxPathOnly(mafStringToWx(fullname)));
}
mafString mafFileNameFromPath(const mafString& fullname)
{
  return mafWxToString(wxFileNameFromPath(mafStringToWx(fullname)));
}


mafString mafCreateTempFileName(const mafString& base)
{
  mafString name;
  name = mafWxToString(wxFileName::CreateTempFileName(mafStringToWx(base))); // used to get a valid temporary name for cache directory
  mafFileRemove(name);
  ParsePathName(name);
  return name;
}
void mafSplitPath(const mafString& fullname, mafString* path, mafString* name, mafString* ext)
{
  wxString wpath, wname, wext;
  wxFileName::SplitPath(mafStringToWx(fullname),&wpath,&wname,&wext);
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
  if(directory.empty())
    return;
  wxSetWorkingDirectory(mafStringToWx(mafGetApplicationDirectory()));
  if(!mafDirExists(directory))
    return;
  mafString file_match = directory + _R("/*.*");
  mafString f = mafFindFirstFile(file_match);
  while (!f.empty())
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
  mafString           MSFFile;

  mafString path, name, ext;

  mafString zip_cache = mafPathOnly(filename); // get the directory
  if (zip_cache.empty())
    zip_cache = stor_tmp;

  zip_cache += _R("/");
  zip_cache = mafCreateTempFileName(zip_cache); // used to get a valid temporary name for cache directory
  mafFileRemove(zip_cache);
  mafSplitPath(zip_cache,&path,&name,&ext);
  zip_cache = path + _R("/") + name + ext;
  ParsePathName(zip_cache);

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
  int length_header_name = header_name.length();
  bool enable_mid = false;

  auto fileSystem = std::make_unique<wxFileSystem>();///< File system manager
  auto zipHandler = std::make_unique<wxZipFSHandler>();///< Handler for zip archive (used to open zmsf files)
  fileSystem->AddHandler(zipHandler.get());
  fileSystem->ChangePathTo(mafStringToWx(filename));
  // extract filename from the zip archive
  zfile = mafWxToString(fileSystem->FindFirst(mafStringToWx(complete_name+pkg+name+_R("\\*.*"))));
  if (zfile.empty())
  {
    enable_mid = true;
    // no files found: try to search inside the archive without filename
    zfile = mafWxToString(fileSystem->FindFirst(mafStringToWx(complete_name+pkg+_R("\\*.*"))));
  }
  if (zfile.empty())
  {
    fileSystem->RemoveHandler(zipHandler.get());
    mafRemoveDirectory(tmpDir); // remove the temporary directory
    return mafString();
  }


  for (;!zfile.empty(); zfile = mafWxToString(fileSystem->FindNext()))
  {
    zfileStream = fileSystem->OpenFile(mafStringToWx(zfile));
    if (zfileStream == NULL) // unable to open the file
    {
      fileSystem->RemoveHandler(zipHandler.get());
      mafRemoveDirectory(tmpDir); // remove the temporary directory
      return mafString();
    }
    zip_is = (wxZlibInputStream *)zfileStream->GetStream();
    mafSplitPath(zfile,&path,&name,&ext);
    complete_name = name + _R(".") + ext;
    if (enable_mid)
      complete_name = mafWxToString(mafStringToWx(complete_name).Mid(length_header_name));
    out_file = tmpDir + _R("\\") + complete_name;
    if(ext == _R("msf"))
    {
      MSFFile = out_file; // The file to extract is an msf
    }
    std::ofstream out_file_stream(out_file.GetCStr(), std::ios_base::binary); // The file to extract is a binary
    std::vector<char> buf(zip_is->GetSize());
    zip_is->Read(buf.data(), buf.size());
    out_file_stream.write(buf.data(), buf.size());
    out_file_stream.close();
    zfileStream->UnRef();
    delete zfileStream;
  }

  fileSystem->ChangePathTo(mafStringToWx(tmpDir), TRUE);

  if (MSFFile.empty()) // msf file not extracted
  {
    mafErrorMessage(_M(mafString(_L("compressed archive is not a valid msf file!"))));
    return mafString();
  }
  fileSystem->RemoveHandler(zipHandler.get());

  // return the extracted msf filename
  return MSFFile;
}
bool mafExtractZIP(const mafString& filename, const mafString& entry_name, void *& buffer, size_t& size)
{
  wxFileInputStream in(mafStringToWx(filename));
  wxZipInputStream zip(in);
  if (!in || !zip)
    return false;
  wxZipEntry *entry = NULL;
  // convert the local name we are looking for into the internal format
  wxString name = wxZipEntry::GetInternalName(mafStringToWx(entry_name));

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
  return true;
}

//----------------------------------------------------------------------------
void mafExtractZIP(const mafString& filename, const mafString& temp_directory, const mafString& entry_name)
//----------------------------------------------------------------------------
{
  wxFileInputStream in(mafStringToWx(filename));
  wxZipInputStream zip(in);
  if (!in || !zip)
    return;
  wxZipEntry *entry = NULL;
  // convert the local name we are looking for into the internal format
  wxString name = wxZipEntry::GetInternalName(mafStringToWx(entry_name));

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
    mafString out_file = temp_directory + _R("/") + entry_name;
    std::ofstream out_file_stream(out_file.GetCStr(), std::ios_base::binary); // the file to extract is a binary
    std::vector<char> buf(entry->GetSize());
    zip.Read(buf.data(), buf.size());
    out_file_stream.write(buf.data(), buf.size());
    out_file_stream.close();
    delete entry;
    entry = NULL;
  }
}
//----------------------------------------------------------------------------
bool mafMakeZip(const mafString &zipname, const std::vector<mafString>& files)
//----------------------------------------------------------------------------
{
  mafString name, path, short_name, ext;
  wxFileOutputStream out(mafStringToWx(zipname));
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
      if (!zip.PutNextDirEntry(mafStringToWx(name))) // put the file inside the archive
        return false;
    }
    else 
    {
      wxFFileInputStream in(mafStringToWx(name));

      if (in.Ok()) 
      {
        wxDateTime dt(wxFileModificationTime(mafStringToWx(name))); // get the file modification time

        if (!zip.PutNextEntry(mafStringToWx(short_name), dt, in.GetLength()) || !zip.Write(in) || !in.Eof()) // put the file inside the archive
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
  if(filename.empty())
    return;

  wxArrayString files;
  wxDir::GetAllFiles(mafStringToWx(dir), &files);

  std::vector<mafString> directory;
  for (size_t i = 0; i < files.GetCount(); i++)
    directory.push_back(mafWxToString(files.Item(i)));

  if(!mafMakeZip(filename, directory))
    mafErrorMessage(_M(mafString(_L("Failed to create compressed archive!"))));
}


//----------------------------------------------------------------------------
mafString BaseName(const mafString& str)
//----------------------------------------------------------------------------
{
    mafString res(str);
    MakeBaseName(res);
    return res;
}

//----------------------------------------------------------------------------
void MakeBaseName(mafString& str)
//----------------------------------------------------------------------------
{
#ifdef _WIN32
    auto pos = str.find_last_of(_R("/\\"));
#else
    auto pos = str.find_last_of(_R("/"));
#endif
    if (pos != mafString::npos)
    {
        str.erase(0, pos + 1);
    }
}

//----------------------------------------------------------------------------
void ExtractPathName(mafString& str)
//----------------------------------------------------------------------------
{
    //wxString path, s;
    str = mafWxToString(wxPathOnly(mafStringToWx(str)));
    //Set(path.c_str());

  /*  int idx=FindLastChr('/');

    if (idx>=0)
    {
      Erase(idx+1,-1);
    }
    else
    {
      Set("");
    }*/
}
//----------------------------------------------------------------------------
void ParsePathName(mafString& str)
//----------------------------------------------------------------------------
{
	if (str.empty())
		return;
	// for Windows platforms parse the string to substitute "/" and "\\" with the right one.
#ifdef _WIN32
	size_t len = str.length();
	size_t start = 0;
	if (len >= 2)
	{
		if (str[0] == '\\' && str[1] == '\\')
			start = 2;
	}
	for (size_t i = start; i < len; i++)
	{
		if (str[i] == '\\')
			str[i] = '/';
	}
#endif
}

//----------------------------------------------------------------------------
std::vector<mafString> mafZIPOpen(const mafString& file)
//----------------------------------------------------------------------------
{
  std::vector<mafString> filesCreated;

  mafString tmpDir, MSFFile;
  mafString zip_cache = mafPathOnly(file);
  if (zip_cache.empty())
  {
    return filesCreated;
  }

  if (!mafDirExists(zip_cache))
    mafDirMake(zip_cache);
  tmpDir = zip_cache;

  mafString path, name, ext, complete_name, zfile, out_file;
  mafSplitPath(file, &path, &name, &ext);
  complete_name = name + _R(".") + ext;

  mafString pkg = _R("#zip:");
  mafString header_name = complete_name + pkg;
  int length_header_name = header_name.length();
  bool enable_mid = false;

  std::unique_ptr<wxZipEntry> entry;

  wxFFileInputStream in(mafStringToWx(file));
  wxZipInputStream zip(in);

  while (entry.reset(zip.GetNextEntry()), entry.get() != NULL)
  {
    mafString name = path + _R("\\") + mafWxToString(entry->GetName());
    zip.OpenEntry(*(entry.get()));
    std::ofstream out_file_stream;
    out_file_stream.open(name.GetCStr(), std::ios_base::binary);
    std::vector<char> buf(entry->GetSize());
    zip.Read(buf.data(), buf.size());
    out_file_stream.write(buf.data(), buf.size());

    filesCreated.push_back(name);
  }

  return filesCreated;
}