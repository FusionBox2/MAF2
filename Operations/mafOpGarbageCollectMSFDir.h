/*=========================================================================

 Program: MAF2
 Module: mafOpGarbageCollectMSFDir
 Authors: Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafOpGarbageCollectMSFDir_H__
#define __mafOpGarbageCollectMSFDir_H__

#include "mafOp.h"
#include <set>
#include <xstring>

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafNode;

//----------------------------------------------------------------------------
// mafOpGarbageCollectMSFDir :
//----------------------------------------------------------------------------
/** 
  Class Name: mafOpGarbageCollectMSFDir

  Remove garbage files in MSF directory. These are usually left by do/undo stuff
  not synchronized  with storage. It will also clear the undo stack.

  Sample usage:

  mafOpGarbageCollectMSFDir *op = new mafOpGarbageCollectMSFDir();
  op->SetInput(anyVMETreeNode);
  int returnValue = op->GarbageCollect();

  Removed files absolute paths are written to the log area.
  
*/

class MAF_EXPORT mafOpGarbageCollectMSFDir: public mafOp
{
public:
  /** constructor */
  mafOpGarbageCollectMSFDir(const mafString& label = _R("Garbage Collect MSF Tree"));
  
  /** destructor */
  ~mafOpGarbageCollectMSFDir(); 
  
  /**
  Remove useless garbage files in MSF dir and clear the Undo stack: return MAF_OK or MAF_ERROR*/
  int GarbageCollect();

  /** RTTI macro */
  mafTypeMacro(mafOpGarbageCollectMSFDir, mafOp);

  /** Return a copy of the operation.*/
  mafOp* Copy();

  /** This operation accept everything as input.*/
  bool Accept(mafNode *node) {return true;};

  /** Create the user interface and initialize variables.*/
  void OpRun();

protected: 
  
  int GetFilesToRemove(std::set<std::string> &filesToRemoveSet);

  /** 
  Return s1 - s2 */
  std::set<std::string> SetDifference(std::set<std::string> &s1, std::set<std::string> &s2 );
 
  std::set<std::string> GetMSFDirABSFileNamesSet();

  void PrintSet(std::set<std::string> inputSet);

  /** Log errors into the log area according to the error number reported.*/
  void ErrorLog(int error_num, const char *node_name, const char *description = NULL);
 
  mafString GetMSFDirAbsPath(mafNode *anyTreeNode);
 
  mafString GetMSFXMLFileAbsFileName(mafNode *anyTreeNode);
 
  /** files linked to vme in trees */
  std::set<std::string> m_MSFTreeBinaryFilesSet;
  std::set<std::string> m_MSFDirABSFileNamesSet;
};
#endif
