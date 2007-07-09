/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoMTRImporter.h,v $
  Language:  C++
  Date:      $Date: 2007-07-09 15:47:44 $
  Version:   $Revision: 1.1 $
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

  /** Set the filename to import */
  void SetFileName(const char *file_name);

  /** Import data. */
  void ImportData();
protected:
  mafString    m_File;
  mafString    m_FileDir;
  mafVMEGroup  *m_Group;
};
#endif
