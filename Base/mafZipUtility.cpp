/*=========================================================================

 Program: MAF2
 Module: mafZipUtility
 Authors: Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include <wx/zipstrm.h>
#include <wx/wfstream.h>

#include "mafZipUtility.h"
#include "mafFilesDirs.h"
#include "mafString.h"

#include <fstream>

//----------------------------------------------------------------------------
std::vector<mafString> ZIPOpen(const mafString& file)
//----------------------------------------------------------------------------
{
  std::vector<mafString> filesCreated;

  mafString ZipFile,tmpDir,MSFFile;
  ZipFile = file;
  mafString zip_cache = mafPathOnly(file);
  if (zip_cache.IsEmpty())
  {
    return filesCreated;
  }

  if (!mafDirExists(zip_cache))
    mafDirMake(zip_cache);
  tmpDir = zip_cache;

  mafString path, name, ext, complete_name, zfile, out_file;
  mafSplitPath(ZipFile,&path,&name,&ext);
  complete_name = name + _R(".") + ext;

  mafString pkg = _R("#zip:");
  mafString header_name = complete_name + pkg;
  int length_header_name = header_name.Length();
  bool enable_mid = false;

  std::auto_ptr<wxZipEntry> entry;

  wxFFileInputStream in(file.toWx());
  wxZipInputStream zip(in);

  while (entry.reset(zip.GetNextEntry()), entry.get() != NULL)
  {
    mafString name = path;
    name+=_R("\\");
    name+= mafWxToString(entry->GetName());
    zip.OpenEntry(*(entry.get()));
    std::ofstream out_file_stream;
    out_file_stream.open(name.GetCStr(), std::ios_base::binary);
    int s_size = entry->GetSize();
    char *buf = new char[s_size];
    zip.Read(buf,s_size);
    out_file_stream.write(buf, s_size);

    filesCreated.push_back(name);

    delete []buf;
  }

  return filesCreated;
}