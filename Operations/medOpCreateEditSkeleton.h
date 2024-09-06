/*=========================================================================

 Program: MAF2Medical
 Module: medOpCreateEditSkeleton
 Authors: Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __medOpCreateEditSkeleton_H__
#define __medOpCreateEditSkeleton_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------

#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVME;
class medVMEPolylineGraph;
class medGeometryEditorPolylineGraph;
class vtkPolyData;

/**
class name: medOpCreateEditSkeleton
Class for creating and editing the skeleton polyline.
*/
class MED_OPERATION_EXPORT medOpCreateEditSkeleton: public mafOp
{
public:
  /** constructor */
	medOpCreateEditSkeleton(const mafString& label = _R("Create/Edit Skeleton"));
  /** destructor */
	~medOpCreateEditSkeleton() override; 

  /** RTTI macro */
	mafTypeMacro(medOpCreateEditSkeleton, mafOp);

  /** Return a copy of itself, this needs to put the operation into the undo stack. */
	mafOp* Copy() override;

  /** Precess events coming from other objects */
  void OnEvent(mafEventBase *maf_event) override;

	/** Return true for the acceptable vme type. */
  bool Accept(mafNode* vme) override;

	/** Builds operation's interface by calling CreateOpDialog() method. */
  void OpRun() override;

	/** Execute the operation. */
  void OpDo() override;

	/** Makes the undo for the operation. */
  void OpUndo() override;

protected:
	/** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
  void OpStop(int result) override;

  /** Internally used to create a new instance of the GUI.*/
	void CreateGui();

	medVMEPolylineGraph	*m_Skeleton;
	vtkPolyData					*m_ResultPolydata;

	medGeometryEditorPolylineGraph *m_Editor;
};
#endif
