/*=========================================================================

Program: MAF2
Module: mafOpCreateMuscleWrapper2
Authors: Taha Jerbi

Copyright (c) B3C
All rights reserved. See Copyright.txt or
http://www.scsitaly.com/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafOpCreateMuscleWrapping2_H__
#define __mafOpCreateMuscleWrapping2_H__

#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVMEMuscleWrapping;
class mafGUI;
class mafEvent;
//----------------------------------------------------------------------------
// mafOpCreateSurfaceParametric :
//----------------------------------------------------------------------------
/** */
class MAF_EXPORT mafOpCreateMuscleWrapping2 : public mafOp
{
public:
	mafOpCreateMuscleWrapping2(const mafString& label = "Create Quadric Surface Fitting");
	~mafOpCreateMuscleWrapping2();

	mafTypeMacro(mafOpCreateMuscleWrapping2, mafOp);

	mafOp* Copy();

	bool Accept(mafNode *node);
	void OpRun();
	void OpDo();

protected:
	mafVMEMuscleWrapping *m_QuadricSurfaceFitting;
};
#endif
