/*=========================================================================

 Program: MAF2
 Module: vtkMAFSmartPointer
 Authors: Marco Petrone
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __vtkMAFSmartPointer_h
#define __vtkMAFSmartPointer_h

#include "mafConfigure.h"
#include "vtkSmartPointer.h"

template <class T>
class vtkMAFSmartPointer: public vtkSmartPointer<T>
{
public:
  /**
  Initialize smart pointer to a new instanse of class T.*/
  vtkMAFSmartPointer() {this->Object=T::New();}

  /**
  Initialize smart pointer to given object pointer and reference the given object.*/
  vtkMAFSmartPointer(T* r): vtkSmartPointer<T>(r) {}
  
  /**
  Initialize smart pointer with a new reference to the same object
  referenced by given smart pointer.*/
  vtkMAFSmartPointer(const vtkSmartPointerBase& r): vtkSmartPointerBase(r) {}
};

#endif
