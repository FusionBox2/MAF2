/*=========================================================================

 Program: MAF2
 Module: mafVMEOutputNULL
 Authors: Marco Petrone
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __mafVMEOutputNULL_h
#define __mafVMEOutputNULL_h
//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "mafVMEOutput.h"
//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mafVMERoot;

/** NULL output for VME node without output data (only pose).
  mafVMEOutputNULL is the output produced by a VME node which does not produce any
  output data, only the pose matrix.
*/
class MAF_EXPORT mafVMEOutputNULL : public mafVMEOutput
{
public:
  mafTypeMacro(mafVMEOutputNULL,mafVMEOutput)

#ifdef MAF_USE_VTK
  /**
    Return a VTK dataset corresponding to the current time. This is
    the output of the DataPipe currently attached to the VME.
    Usually the output is a  "smart copy" of one of the dataset in 
    the DataArray. In some cases it can be NULL, e.g. in case the number
    of stored Items is 0. Also special VME could not support VTK dataset output.
    An event is rised when the output data changes to allow attached classes to 
    update their input.*/
  virtual vtkAlgorithmOutput *GetVTKData() {return NULL;}
#endif

  /**
    Update all the output data structures (data, bounds, matrix and abs matrix).*/
  virtual void Update() {}
    
protected:
  mafVMEOutputNULL(); // to be allocated with New()
  virtual ~mafVMEOutputNULL(); // to be deleted with Delete()

private:
  mafVMEOutputNULL(const mafVMEOutputNULL&); // Not implemented
  void operator=(const mafVMEOutputNULL&); // Not implemented
};

#endif
