/*=========================================================================

Program: MAF2
Module: mafOpCreateQuadricSurfaceFitting
Authors: Taha Jerbi

Copyright (c) B3C
All rights reserved. See Copyright.txt or
http://www.scsitaly.com/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafOpCreateQuadricSurfaceFitting_H__
#define __mafOpCreateQuadricSurfaceFitting_H__

#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVMEQuadricSurfaceFitting;
class mafGUI;
class mafEvent;
//----------------------------------------------------------------------------
// mafOpCreateSurfaceParametric :
//----------------------------------------------------------------------------
/** */
class MAF_EXPORT mafOpCreateQuadricSurfaceFitting : public mafOp
{
public:
	mafOpCreateQuadricSurfaceFitting(const mafString& label = _R("Create Quadric Surface Fitting"));
	~mafOpCreateQuadricSurfaceFitting();

	mafTypeMacro(mafOpCreateQuadricSurfaceFitting, mafOp);

	mafOp* Copy();

	bool Accept(mafNode *node);
	void OpRun();
	void OpDo();

protected:
	mafVMEQuadricSurfaceFitting *m_QuadricSurfaceFitting;
};
#endif
