/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: mafMtrReader.h,v $
  Language:  C++
  Date:      $Date: 2006-07-24 14:14:09 $
  Version:   $Revision: 1.1 $

  Copyright (c) 2005 Vladik Aranov, all right belongs to ULB and CINECA 
  ULB - Free University of Brussels

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME mafMTRReader - read ASCII or binary AMIRA INP files
// .SECTION Description
// mafMTRReader is a source object that reads ASCII or binary stereo 
// lithography files (.stl files). The m_FileName must be specified to
// mafMTRReader. The object automatically detects whether the file is
// ASCII or binary.

#ifndef __mafMTRReader_h
#define __mafMTRReader_h

#include "vtkPolyDataAlgorithm.h"

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

class MAF_EXPORT mafMTRReader: public vtkPolyDataAlgorithm 
{
public:
  static const int SetNotDefined = -1;
  vtkTypeMacro(mafMTRReader, vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  // Description:
  // Construct object with merging set to true.
  static mafMTRReader *New();

  // Description:
  // Specify file name of stereo lithography file.
  mafSetStringMacro(FileName);
  mafGetStringMacro(FileName);

  void SetSet(int set) {m_Set = set;}
  int  GetPointsRead() {return m_PointsRead;}
  void SetMode(bool lineset){m_linesetMode = lineset;if(!lineset)m_tendonMode = false;}
  void SetTendonMode(bool tendon){m_tendonMode = tendon;}

protected:
  mafMTRReader();
  ~mafMTRReader() override;

  char   *m_FileName;
  int    m_Set;
  int    m_PointsRead;
  double m_PointShift;
  bool   m_linesetMode;
  bool   m_tendonMode;

  int RequestData(vtkInformation*, vtkInformationVector**, vtkInformationVector*) override;

  int ReadASCIIMTR(FILE *fp, vtkPoints *vpPoints, vtkCellArray* tpTriangles);
private:
  mafMTRReader(const mafMTRReader&);  // Not implemented.
  void operator=(const mafMTRReader&);  // Not implemented.
};

#endif


