/*=========================================================================

 Program: MAF2
 Module: mafStorage
 Authors: Marco Petrone
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __mafStorage_h__
#define __mafStorage_h__

#include "mafBaseEventHandler.h"
#include "mafEventSender.h"
#include <set>
//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mafNodeManager;

class MAF_EXPORT mafStorage : public mafBaseEventHandler, public mafEventSender
{
public:

  enum STORAGE_IO_ERRORS { IO_OK = 0, IO_GENERIC_ERROR, IO_WRONG_OBJECT_TYPE, IO_RESTORE_ERROR, IO_WRONG_FILE_TYPE, IO_WRONG_FILE_VERSION, IO_WRONG_URL, IO_LAST_ERROR };

  mafStorage();
  virtual ~mafStorage();
  //mafStorage(const mafString& filetype, const mafString& version);
  //virtual ~mafStorage();

  void SetManager(mafNodeManager* manager);

  /** Set the URL of the document to be read or written */
  virtual void SetURL(const mafString& name);

  /** Force Copy storage URL to parser URL (used by the MAF1.x importer) */
  void ForceParserURL();

  /** Return the URL of the document to be read or written */
  const mafString& GetURL();

  virtual void OnEvent(mafEventBase* e) override;

  /**
    Return the name of the last read file, this is internally used when writing to
    a new file to read and copy information between the old and new file */
  const mafString& GetParserURL();

  /** perform storing. the argument is the tag of the document node */
  int Store();

  /** perform restoring. the argument is the tag of the document node  */
  int Restore();


  /** resolve an URL and provide local filename to be used as input */
  virtual int ResolveInputURL(const mafString& url, mafString& filename, mafBaseEventHandler* observer = NULL);

  /** resolve an URL and provide a local filename to be used as output */
  //virtual bool ResolveOutputURL(const mafCString url, mafString &filename)=0;

  /**
    store a file to an URL. This API transfer a tmp local file to URL.
    If no URL is specified use the storage URL as a base URL. */
  virtual int StoreToURL(const mafString& filename, const mafString& url);

  /**
    Remove a file from the specified URL. Used when a file in the storage
    is no more necessary. If the specified URL is a local file name try
    to prepend the storage URL as a base URL name.*/
  virtual int ReleaseURL(const mafString& url);

  /**
    Return a name of file to be used as tmp file during store/restore.
    This is usually located insed the MSF file directory */
  virtual void GetTmpFile(mafString& filename);

  /** remove the tmp file */
  void ReleaseTmpFile(const mafString& filename);

  /**
    Check if a file is present in the storage directory. The directory list is
    open when Store() is called and directory data is updated at that time. */
  bool IsFileInDirectory(const mafString& filename);

  /**
    Set the folder where to store tmp files. If not specified the storage
    will automatically use a default folder, like the current directory
    or the storage file folder. */
  virtual void SetTmpFolder(const mafString& folder) { m_TmpFolder = folder; }

  /** return the folder where tmp files are stored */
  virtual const mafString& GetTmpFolder();

  void SetErrorCode(int err) { m_ErrorCode = err; }
  int GetErrorCode() { return m_ErrorCode; }

  /** remove the file from URL */
  virtual int DeleteURL(const mafString& url);

  /** empty the garbage collector list deleting old files */
  virtual void EmptyGarbageCollector();

  mafString m_TmpDir;           ///< Temporary directory for zmsf extraction

protected:
  /** This is called by Store() and must be reimplemented by subclasses */
  virtual int InternalStore(const mafString& filename);

  /** This is called by Restore() and must be reimplemented by subclasses */
  virtual int InternalRestore(const mafString& filename);

  /** populate the list of files in the storage folder */
  virtual int OpenDirectory(const mafString& dir_name);

  std::set<mafString> m_GarbageCollector; ///< collect URL to be released
  mafString  m_DefaultTmpFolder; ///< used to store the current default tmp folder

  mafString           m_FileType;  ///< The type of file to be opened
  mafString           m_Version;   ///< Current MSF version
  mafString           m_URL;          ///< name of the file being accessed
  mafString           m_ParserURL;    ///< name of the last parsed file (used for SaveAs)
  mafID               m_TmpFileId;    ///< counter for unique tmp file naming
  mafString           m_TmpFolder;    ///< folder where to store tmp files

  std::set<mafString> m_TmpFileNames; ///< name of tmp files in the MSF dir
  std::set<mafString> m_FilesDictionary; ///< list of files in the storage folder: to be populated by OpenDirectory()
  int                 m_ErrorCode;    ///< the error code 0==OK
  mafNodeManager* m_Document;        ///< document object to be stored, or being restored
};
#endif // _mafStorage_h_
