/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoMTRImporter.h,v $
  Language:  C++
  Date:      $Date: 2007-10-12 10:23:48 $
  Version:   $Revision: 1.2 $
  Authors:   Fedor Moiseev / Vladik Aranov
==========================================================================
  Copyright (c) 2001/2007 
  ULB - Universite Libre de Bruxelles (www.ulb.ac.be)
=========================================================================*/

#ifndef __mmoMTRImporter_H__
#define __mmoMTRImporter_H__

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
//mmoMTRImporter :
//----------------------------------------------------------------------------
class mmoMTRImporter: public mafOp
{
public:
           mmoMTRImporter(const wxString &label = "MTRImporter");
  virtual ~mmoMTRImporter();
  
  mafTypeMacro(mmoMTRImporter, mafOp);

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
protected:
  std::vector<mafString>    m_Files;
  mafString                 m_FileDir;
  std::vector<mafVMEGroup*> m_Groups;
};
#endif
