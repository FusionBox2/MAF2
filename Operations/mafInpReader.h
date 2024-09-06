/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafInpReader.h,v $
  Language:  C++
  Date:      $Date: 2007-08-22 14:01:40 $
  Version:   $Revision: 1.1 $
  Authors:   Fedor Moiseev / Vladik Aranov
==========================================================================
  Copyright (c) 2001/2007 
  ULB - Universite Libre de Bruxelles (www.ulb.ac.be)
=========================================================================*/

#ifndef __mafINPReader_h
#define __mafINPReader_h

//----------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------
#include "vtkPolyDataAlgorithm.h"
#include <vector>

//----------------------------------------------------------------------------
// Class forwarding
//----------------------------------------------------------------------------
class vtkCellArray;
class vtkFloatArray;
class vtkPointLocator;
class vtkPoints;
class wxTextFile;

//
#define mafSetStringMacro(name) \
virtual void Set##name (const char* _arg) \
  { \
  vtkDebugMacro(<< this->GetClassName() << " (" << this << "): setting " << #name " to " << _arg ); \
  if ( this->m_##name == NULL && _arg == NULL) { return;} \
  if ( this->m_##name && _arg && (!strcmp(this->m_##name,_arg))) { return;} \
  if (this->m_##name) { delete [] this->m_##name; } \
  if (_arg) \
    { \
    this->m_##name = new char[strlen(_arg)+1]; \
    strcpy(this->m_##name,_arg); \
    } \
   else \
    { \
    this->m_##name = NULL; \
    } \
  this->Modified(); \
  } 

#define mafGetStringMacro(name) \
virtual char* Get##name () { \
  vtkDebugMacro(<< this->GetClassName() << " (" << this << "): returning " << #name " of " << this->m_##name); \
  return this->m_##name; \
  } 

//----------------------------------------------------------------------------
// class mafINPReader
//----------------------------------------------------------------------------
class MAF_EXPORT mafINPReader: public vtkPolyDataAlgorithm
{
public:
  vtkTypeMacro(mafINPReader, vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  // Description:
  // Construct object with merging set to true.
  static mafINPReader *New();

  // Description:
  // Specify file name of stereo lithography file.
  mafSetStringMacro(FileName);
  mafGetStringMacro(FileName);

protected:
  mafINPReader();
  ~mafINPReader() override;

  char *m_FileName;

  int RequestData(vtkInformation*, vtkInformationVector**, vtkInformationVector*) override;

  int ReadASCIIINP(FILE *fp, std::vector<double> &pointsRead, std::vector<int> &pointsIndRead, std::vector<vtkIdType> &indRead);
private:
  mafINPReader(const mafINPReader&);  // Not implemented.
  void operator=(const mafINPReader&);  // Not implemented.
};
#endif
