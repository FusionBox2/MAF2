/*=========================================================================

 Program: MAF2Medical
 Module: medOpCreateSurface
 Authors: Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __medOpCreateSurface_H__
#define __medOpCreateSurface_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------

#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVMESurface;

//----------------------------------------------------------------------------
// medOpCreateSurface :
//----------------------------------------------------------------------------
/**Convert a Parametric Surface into a Normal Surface*/
class MED_OPERATION_EXPORT medOpCreateSurface: public mafOp
{
public:

	medOpCreateSurface(const mafString& label = _R("Create Surface"));
	~medOpCreateSurface() override; 

	mafTypeMacro(medOpCreateSurface, mafOp);

	mafOp* Copy() override;

	/** Return true for the acceptable vme type. */
	bool Accept(mafNode* vme) override;

	/** Builds operation's interface by calling CreateOpDialog() method. */
	void OpRun() override;

	/** Execute the operation. */
	void OpDo() override;

protected:

	/** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
	void OpStop(int result) override;

	mafVMESurface *m_Surface;

};
#endif
