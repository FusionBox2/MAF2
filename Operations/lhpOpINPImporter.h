/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpOpINPImporter.h,v $
  Language:  C++
  Date:      $Date: 2007-10-12 10:24:00 $
  Version:   $Revision: 1.2 $
  Authors:   Fedor Moiseev / Vladik Aranov
==========================================================================
  Copyright (c) 2001/2007 
  ULB - Universite Libre de Bruxelles (www.ulb.ac.be)
=========================================================================*/

#ifndef __lhpOpINPImporter_H__
#define __lhpOpINPImporter_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafOp.h"
#include <vector>

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafNode;
class mafVMESurface;

//----------------------------------------------------------------------------
//lhpOpINPImporter :
//----------------------------------------------------------------------------
/**
  VRML files are commonly called worlds and have the .wrl extension. 
  Although VRML worlds use a text format they may often be compressed using gzip so 
  that they transfer over the internet more quickly. 
  This modality in not supported by maf vrml importer, it can only import uncompressed wrl.

*/
class lhpOpINPImporter: public mafOp
{
public:
           lhpOpINPImporter(const mafString& label = "INP Importer");
  virtual ~lhpOpINPImporter();
  
  mafTypeMacro(lhpOpINPImporter, mafOp);

  mafOp* Copy();

  /** Return true for the acceptable vme type. */
  bool Accept(mafNode* node) {return true;}

  /** Builds operation's interface. */
  void OpRun();

  /** Makes the undo for the operation. */
  void OpUndo();

  /** Execute the operation. */
  void OpDo();

  /** Import data. */
  void ImportData();

protected:
  std::vector<mafString>      m_Files;
  mafString                   m_FileDir;
  std::vector<mafVMESurface*> m_Surfaces;
};
#endif
