/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafFilesDirs.h,v $
  Language:  C++
  Date:      $Date: 2007-11-06 12:48:05 $
  Version:   $Revision: 1.7 $
  Authors:   Based on itkDirectory (www.itk.org), adapted by Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafFilesDirs_h
#define __mafFilesDirs_h

#include "mafString.h"

MAF_EXPORT bool mafDirMake(const mafString& directory);
MAF_EXPORT bool mafDirRemove(const mafString& directory);
MAF_EXPORT bool mafDirExists(const mafString& directory);
MAF_EXPORT bool mafFileRemove(const mafString& file);
MAF_EXPORT bool mafFileRename(const mafString& files, const mafString& filet);
MAF_EXPORT bool mafFileCopy(const mafString& files, const mafString& filet, bool overwrite = true);
MAF_EXPORT bool mafFileExists(const mafString& file);
MAF_EXPORT mafString mafFindFirstFile(const mafString& spec, bool dir = false);
MAF_EXPORT mafString mafFindNextFile();
MAF_EXPORT mafString mafCreateTempFileName(const mafString& base);
MAF_EXPORT void mafSplitPath(const mafString& fullname, mafString* path, mafString* name, mafString* ext);
MAF_EXPORT void mafSplitPath(const mafString& fullname, mafString* path, mafString* nameext);
MAF_EXPORT mafString mafPathOnly(const mafString& fullname);

MAF_EXPORT void mafRemoveDirectory(const mafString& directory);
MAF_EXPORT mafString mafOpenZIP(const mafString& filename, const mafString& stor_tmp, mafString& tmpDir);
MAF_EXPORT void mafZIPSave(const mafString& filename, const mafString& dir);

#endif
