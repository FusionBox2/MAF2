/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpOpMTRExporter.h,v $
  Language:  C++
  Date:      $Date: 2008-04-02 09:37:27 $
  Version:   $Revision: 1.3 $
  Authors:   Fedor Moiseev / Vladik Aranov
==========================================================================
  Copyright (c) 2001/2007 
  ULB - Universite Libre de Bruxelles (www.ulb.ac.be)
=========================================================================*/

#ifndef __lhpOpMTRExporter_H__
#define __lhpOpMTRExporter_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafOp.h"
#include <iostream>

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafEvent;
class mafVMEGroup;
class mafVME;
class mafVMELandmarkCloud;

//----------------------------------------------------------------------------
// lhpOpMTRExporter :
//----------------------------------------------------------------------------
/** Exporter for the landmark coordinates: the data are exported in ASCII format. 
Each raw represents a landmark and contains the (x,y,z) coordinate.*/
class lhpOpMTRExporter: public mafOp
{
public:
  mafTypeMacro(lhpOpMTRExporter, mafOp)
  lhpOpMTRExporter(const mafString& label = _R("MTRExporter"));
  ~lhpOpMTRExporter() override; 
  mafOp* Copy() override;
  void OnEvent(mafEventBase *maf_event) override;

  /** Return true for the acceptable vme type. */
  bool Accept(mafNode *node) override;

  /** Build the interface of the operation, i.e the dialog that let choose the name of the output file. */
  void OpRun() override;

  void ExportLandmark();

  /** Set the filename for export */
  void SetFileName(const char *file_name) {m_File = _R(file_name);};

protected:
  void ExportingTraverse(std::ostream &out, const char *dirName, mafNode* node);
  void ExportOneCloud(std::ostream &out, mafVMELandmarkCloud* cloud);
  mafString m_File;
  mafString m_FileDir;
  mafVME   *m_Vme; 
  int      m_ABSPos;
};
#endif
