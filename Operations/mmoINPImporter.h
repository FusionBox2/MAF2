/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoINPImporter.h,v $
  Language:  C++
  Date:      $Date: 2007-08-22 14:01:40 $
  Version:   $Revision: 1.1 $
  Authors:   Fedor Moiseev / Vladik Aranov
==========================================================================
  Copyright (c) 2001/2007 
  ULB - Universite Libre de Bruxelles (www.ulb.ac.be)
=========================================================================*/

#ifndef __mmoINPImporter_H__
#define __mmoINPImporter_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafNode;
class mafVMESurface;

//----------------------------------------------------------------------------
//mmoINPImporter :
//----------------------------------------------------------------------------
/**
  VRML files are commonly called worlds and have the .wrl extension. 
  Although VRML worlds use a text format they may often be compressed using gzip so 
  that they transfer over the internet more quickly. 
  This modality in not supported by maf vrml importer, it can only import uncompressed wrl.

*/
class mmoINPImporter: public mafOp
{
public:
           mmoINPImporter(const wxString &label = "INP Importer");
  virtual ~mmoINPImporter();
  
  mafTypeMacro(mmoINPImporter, mafOp);

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
  mafString m_File;
  mafString m_FileDir;
  
  mafVMESurface *m_Surface;
};
#endif
