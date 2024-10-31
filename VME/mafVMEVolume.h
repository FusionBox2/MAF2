/*=========================================================================

 Program: MAF2
 Module: mafVMEVolume
 Authors: Marco Petrone
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __mafVMEVolume_h
#define __mafVMEVolume_h
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafVMEGeneric.h"
#include "mafVMEOutputVolume.h"
//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class vtkDataSet;
class vtkVolumeData;
class mmaVolumeMaterial;

/** mafVMEVolume - a VME featuring an internal array for matrices and VTK datasets.
  mafVMEVolume is a specialized VME inheriting the VME-Generic features to internally
  store data, and specialized for VTK data. This specialization consists in creating
  a specialized data pipe and to redefining some APIs for casting to concrete classes.
  @sa mafVME mafMatrixVector mafDataVector  */
class MAF_EXPORT mafVMEVolume : public mafVMEGeneric
{
public:
  mafAbstractTypeMacro(mafVMEVolume,mafVMEGeneric);

  /** return the right type of output */  
  mafVMEOutputVolume *GetVolumeOutput() {return (mafVMEOutputVolume *)GetOutput();}

  /** return the right type of output */
  mafVMEOutput *GetOutput() override;

  /** private to avoid calling by external classes */
  int SetData(vtkDataSet *data, mafTimeStamp t, int mode=MAF_VME_COPY_DATA) override;

  /** return icon */
  static const char** GetIcon();

  /** Return pointer to material attribute. */
  std::shared_ptr<mmaVolumeMaterial> GetMaterial();

  /** Return the suggested pipe-typename for the visualization of this vme */
  mafString GetVisualPipe() override {return mafString(_R("mafPipeBox"));}

protected:
  mafVMEVolume();
  ~mafVMEVolume() override;

  /** used to initialize and create the material attribute if not yet present */
  int InternalInitialize() override;

private:
  mafVMEVolume(const mafVMEVolume&); // Not implemented
  void operator=(const mafVMEVolume&); // Not implemented
};

#endif
