/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpOpINPExporter.h,v $
  Language:  C++
  Date:      $Date: 2007-10-12 10:23:30 $
  Version:   $Revision: 1.2 $
  Authors:   Fedor Moiseev / Vladik Aranov
==========================================================================
  Copyright (c) 2001/2007 
  ULB - Universite Libre de Bruxelles (www.ulb.ac.be)
=========================================================================*/

#ifndef __lhpOpINPExporter_H__
#define __lhpOpINPExporter_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafEvent.h"
#include "mafOp.h"
#include <iostream>


//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafNode;
class mafVMEOutputSurface;

//----------------------------------------------------------------------------
// lhpOpINPExporter :
//----------------------------------------------------------------------------
/** */
class lhpOpINPExporter: public mafOp
{
public:
  lhpOpINPExporter(const mafString& label = _R("INPExporter"));
  ~lhpOpINPExporter(); 

  mafTypeMacro(lhpOpINPExporter, mafOp);

  mafOp* Copy();
  void OnEvent(mafEventBase *maf_event);

  /** Return true for the acceptable vme type. */
  bool Accept(mafNode *node);

  /** Builds operation's interface. */
  void OpRun();

  /** Set the filename for the .stl to export */
  void SetFileName(const char *file_name) {m_File = _R(file_name);}

  void ApplyABSMatrixOn() {m_ABSMatrixFlag = 1;};
  void ApplyABSMatrixOff() {m_ABSMatrixFlag = 0;};
  void SetApplyABSMatrix(int apply_matrix) {m_ABSMatrixFlag = apply_matrix;};

  void ExportAsBynaryOn() {m_Binary = 1;};
  void ExportAsBynaryOff() {m_Binary = 0;};
  void SetExportAsBynary(int binary_file) {m_Binary = binary_file;};

  /** Export the surface. */
  void ExportSurface();

protected:
  /** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
  //void OpStop(int result);
  void ExportingTraverse(const char *dirName, mafNode* node);
  void ExportOneSurface(const char *filename, mafVMEOutputSurface* surf);

  mafString  m_File;
  mafString  m_FileDir;
  int        m_Binary;
  int        m_ABSMatrixFlag;
};
#endif
