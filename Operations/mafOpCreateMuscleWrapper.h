/*=========================================================================

Program: MAF2
Module: mafOpCreateMuscleWrapper
Authors: Taha Jerbi

Copyright (c) B3C
All rights reserved. See Copyright.txt or
http://www.scsitaly.com/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafOpCreateMuscleWrapperAQ_H__
#define __mafOpCreateMuscleWrapperAQ_H__

#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------

class mafGUI;
class mafEvent;

class mafVMEMuscleWrapperAQ;
//----------------------------------------------------------------------------
// mafOpCreateSurfaceParametric :
//----------------------------------------------------------------------------
/** */
class MAF_EXPORT mafOpCreateMuscleWrapperAQ : public mafOp
{
public:
	mafOpCreateMuscleWrapperAQ(const mafString& label = "Muscle WrapperAQ");
	~mafOpCreateMuscleWrapperAQ();

	mafTypeMacro(mafOpCreateMuscleWrapperAQ, mafOp);

	mafOp* Copy();

	bool Accept(mafNode *node);
	void OpRun();
	void OpDo();

protected:
	mafVMEMuscleWrapperAQ *m_MuscleWrapper;
};
#endif
