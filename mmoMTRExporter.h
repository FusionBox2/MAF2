/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoMTRExporter.h,v $
  Language:  C++
  Date:      $Date: 2007-07-19 12:37:35 $
  Version:   $Revision: 1.2 $
  Authors:   Fedor Moiseev / Vladik Aranov
==========================================================================
  Copyright (c) 2001/2007 
  ULB - Universite Libre de Bruxelles (www.ulb.ac.be)
=========================================================================*/

#ifndef __mmoMTRExporter_H__
#define __mmoMTRExporter_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafOp.h"


//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafEvent;
class mafVMEGroup;
class mafVME;

//----------------------------------------------------------------------------
// mmoMTRExporter :
//----------------------------------------------------------------------------
/** Exporter for the landmark coordinates: the data are exported in ASCII format. 
Each raw represents a landmark and contains the (x,y,z) coordinate.*/
class mmoMTRExporter: public mafOp
{
public:
  mmoMTRExporter(const wxString& label = "MTRExporter");
  ~mmoMTRExporter(); 
  mafOp* Copy();

  /** Return true for the acceptable vme type. */
  bool Accept(mafNode *node);

  /** Build the interface of the operation, i.e the dialog that let choose the name of the output file. */
  void OpRun();

  void ExportLandmark();

  /** Set the filename for export */
  void SetFileName(const char *file_name) {m_File = file_name;};

protected:
  wxString m_File;
  wxString m_FileDir;
  mafVME   *m_Vme; 
  bool     *m_State;

};
#endif
