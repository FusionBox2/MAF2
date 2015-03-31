/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafStorage.h,v $
  Language:  C++
  Date:      $Date: 2007-12-11 11:25:08 $
  Version:   $Revision: 1.16 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mafParser_h__
#define __mafParser_h__

#include "mafObject.h"
#include "mafString.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mafStorageElement;
class mafStorable;


class MAF_EXPORT mafParser: public mafObject
{
public:
  mafAbstractTypeMacro(mafParser,mafObject);

  enum PARSER_IO_ERRORS {IO_OK=0,IO_GENERIC_ERROR,IO_WRONG_OBJECT_TYPE,IO_RESTORE_ERROR,IO_WRONG_FILE_TYPE,IO_WRONG_FILE_VERSION,IO_WRONG_URL,IO_LAST_ERROR};

  mafParser();
  virtual ~mafParser() {}

  /** The TAG identifying the type (i.e. format) of file. (e.g. "MSF") */
  void SetFileType(const mafString& filetype){m_FileType = filetype;}
  /** The TAG identifying the type (i.e. format) of file. (e.g. "MSF") */
  const mafString& GetFileType(){return m_FileType;}

  /** The version of the file format used type of file. (default "1.1") */
  void SetVersion(const mafString& version){m_Version = version;}

  /** The version of the file format used type of file. (default "1.1") */
  const mafString& GetVersion(){return m_Version;}

  /** Set the URL of the document to be read or written */
  virtual void SetURL(const mafString& url){m_URL = url;}


  /** Return the URL of the document to be read or written */
  const mafString& GetURL(){return m_URL;}

  /** perform storing. the argument is the tag of the document node */
  int Store();

  /** perform restoring. the argument is the tag of the document node  */
  int Restore();

  /** set the document element to be stored */
  void SetDocument (mafStorable *doc){m_Document = doc;}

  /** return the document object restored */
  mafStorable *GetDocument(){return m_Document;}

  void SetErrorCode(int err) {m_ErrorCode=err;}
  int  GetErrorCode() {return m_ErrorCode;}
  bool NeedsUpgrade() {return m_NeedsUpgrade;}

protected:
  /** This is called by Store() and must be reimplemented by subclasses */
  virtual int InternalStore()=0;

  /** This is called by Restore() and must be reimplemented by subclasses */
  virtual int InternalRestore()=0;

  mafString           m_FileType;  ///< The type of file to be opened
  mafString           m_Version;   ///< Current MSF version
  mafStorable         *m_Document;        ///< document object to be stored, or being restored
  mafString           m_URL;          ///< name of the file being accessed
  int                 m_ErrorCode;    ///< the error code 0==OK
  bool                m_NeedsUpgrade; ///< Flag used to enable or not the upgrade mechanism.
};

#endif // _mafParser_h_
