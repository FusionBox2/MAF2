#pragma once

#include "ftkConfigure.h"

#include "ftk/Base/String.h"

BEGIN_FTK_NAMESPACE

FTK_BASE_EXPORT mafString mafStripMenuCodes(const mafString& com);

FTK_BASE_EXPORT bool mafDirMake(const mafString& directory);
FTK_BASE_EXPORT bool mafDirRemove(const mafString& directory);
FTK_BASE_EXPORT bool mafDirExists(const mafString& directory);
FTK_BASE_EXPORT bool mafFileRemove(const mafString& file);
FTK_BASE_EXPORT bool mafFileRename(const mafString& files, const mafString& filet);
FTK_BASE_EXPORT bool mafFileCopy(const mafString& files, const mafString& filet, bool overwrite = true);
FTK_BASE_EXPORT bool mafFileExists(const mafString& file);
FTK_BASE_EXPORT mafString mafFindFirstFile(const mafString& spec, bool dir = false);
FTK_BASE_EXPORT mafString mafFindNextFile();
FTK_BASE_EXPORT mafString mafCreateTempFileName(const mafString& base);
FTK_BASE_EXPORT void mafSplitPath(const mafString& fullname, mafString* path, mafString* name, mafString* ext);
FTK_BASE_EXPORT void mafSplitPath(const mafString& fullname, mafString* path, mafString* nameext);
FTK_BASE_EXPORT mafString mafPathOnly(const mafString& fullname);
FTK_BASE_EXPORT mafString mafFileNameFromPath(const mafString& fullname);

FTK_BASE_EXPORT void mafRemoveDirectory(const mafString& directory);
FTK_BASE_EXPORT mafString mafOpenZIP(const mafString& filename, const mafString& stor_tmp, mafString& tmpDir);
FTK_BASE_EXPORT void mafExtractZIP(const mafString& filename, const mafString& temp_directory, const mafString& entry_name);
FTK_BASE_EXPORT bool mafExtractZIP(const mafString& filename, const mafString& entry_name, void *& buffer, size_t& size);
FTK_BASE_EXPORT void mafZIPSave(const mafString& filename, const mafString& dir);

FTK_BASE_EXPORT std::vector<mafString> mafZIPOpen(const mafString& zipFile);

FTK_BASE_EXPORT mafString BaseName(const mafString& str);
FTK_BASE_EXPORT void MakeBaseName(mafString& str);
FTK_BASE_EXPORT void ExtractPathName(mafString& str);
FTK_BASE_EXPORT void ParsePathName(mafString& str);

END_FTK_NAMESPACE