/*=========================================================================

 Program: MAF2Medical
 Module: medOpCropDeformableROI
 Authors: Matteo Giacomoni - Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __medOpCropDeformableROI_H__
#define __medOpCropDeformableROI_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------

#include "mafOp.h"
#include "mafVME.h"
#include "mafVMEOutput.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------

class vtkMaskPolyDataFilter;
class mafVMEVolumeGray;
class mafVMESurface;

//----------------------------------------------------------------------------
// medOpCropDeformableROI :
//----------------------------------------------------------------------------
/**
  class name: medOpCropDeformableROI
   Operation that use m_MaskPolydataFilter, cropping  a volume with a output surface of a vme.
*/
class MED_OPERATION_EXPORT medOpCropDeformableROI: public mafOp
{
public:
  /** construct */
	medOpCropDeformableROI(const mafString& label = _R("CropDeformableROI"));
  /** destructor */
	~medOpCropDeformableROI() override; 
  /** process events coming from other components */
  void OnEvent(mafEventBase *maf_event) override;
  /** RTTI Macro */
	mafTypeMacro(medOpCropDeformableROI, mafOp);

  /** acceptance static function */
	static bool OutputSurfaceAccept(mafNode *node) {return(node != NULL && ((mafVME*)node)->GetOutput()->IsA("mafVMEOutputSurface"));};

  /** Return a copy of the operation.*/
	mafOp* Copy() override;

	/** Return true for the acceptable vme type. */
	bool Accept(mafNode *node) override;

	/** Builds operation's interface. */
	void OpRun() override;

	/** Execute the operation. */
	void OpDo() override;

	/** Makes the undo for the operation. */
	void OpUndo() override;

protected:
	/** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
	void OpStop(int result) override;

  /** use  vtkMaskPolydataFilter to  achieve the output */
	void Algorithm(mafVME *vme);

  /** Select a surface as volume masking */
	void MaskSelection();

  /** Creates operation GUI*/
	void CreateGui();

	vtkMaskPolyDataFilter *m_MaskPolydataFilter;

	double m_Distance;
	double m_FillValue;
	int		m_InsideOut;
	mafNode *m_PNode;

	mafVMEVolumeGray *m_ResultVme;

};
#endif
