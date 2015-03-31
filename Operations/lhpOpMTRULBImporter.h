/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpOpMTRULBImporter.h,v $
  Language:  C++
  Date:      $Date: 2007-07-05 14:19:17 $
  Version:   $Revision: 1.0 $
  Authors:   Fedor Moiseev / Vladik Aranov
==========================================================================
  Copyright (c) 2001/2007 
  ULB - Universite Libre de Bruxelles (www.ulb.ac.be)
=========================================================================*/

#ifndef __lhpOpMTRULBImporter_H__
#define __lhpOpMTRULBImporter_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafNode;
class mafVMEGroup;
class vtkMatrix4x4;

//----------------------------------------------------------------------------
//lhpOpMTRULBImporter :
//----------------------------------------------------------------------------
class lhpOpMTRULBImporter: public mafOp
{
public:
           lhpOpMTRULBImporter(const mafString& label = "MTRImporter");
  virtual ~lhpOpMTRULBImporter();
  
  mafTypeMacro(lhpOpMTRULBImporter, mafOp);

  mafOp* Copy();

  /** Return true for the acceptable vme type. */
  bool Accept(mafNode* node) {return true;};

  /** Builds operation's interface. */
  void OpRun();

  /** Makes the undo for the operation. */
  void OpUndo();

  /** Execute the operation. */
  void OpDo();

  /** Import data. */
  void ImportData();
  void ProcessSingleFile(const wxString &fileName);

protected:
  void ReadMatrix(wxChar const *fstring, vtkMatrix4x4 *mat);

  std::vector<mafString>     m_Files;
  mafString m_FileDir;
  
  std::vector<mafVMEGroup*>  m_Groups;
};
#endif
