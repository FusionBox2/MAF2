/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafINPWriter.h,v $
  Language:  C++
  Date:      $Date: 2007-08-22 14:01:40 $
  Version:   $Revision: 1.1 $
  Authors:   Fedor Moiseev / Vladik Aranov
==========================================================================
  Copyright (c) 2001/2007 
  ULB - Universite Libre de Bruxelles (www.ulb.ac.be)
=========================================================================*/

#ifndef __mafINPWriter_h
#define __mafINPWriter_h

//----------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------
#include "vtkPolyDataWriter.h"

//----------------------------------------------------------------------------
// class mafINPWriter
//----------------------------------------------------------------------------
class MAF_EXPORT mafINPWriter : public vtkPolyDataWriter
{
public:
  static mafINPWriter *New();
  vtkTypeRevisionMacro(mafINPWriter,vtkPolyDataWriter);
  virtual void PrintSelf(ostream& os, vtkIndent indent);

protected:
  mafINPWriter();
  ~mafINPWriter() {};

  void WriteData();

  void WriteBinaryINP(vtkPoints *pts, vtkCellArray *polys);
  void WriteAsciiINP(vtkPoints *pts, vtkCellArray *polys);
private:
  mafINPWriter(const mafINPWriter&);  // Not implemented.
  void operator=(const mafINPWriter&);  // Not implemented.
};
#endif
