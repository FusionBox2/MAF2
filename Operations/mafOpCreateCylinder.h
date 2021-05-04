/*=========================================================================

 Program: MAF2
 Module: mafOpCreateQuadric
 Authors: Taha Jerbi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafOpCreateCylinder_H__
#define __mafOpCreateCylinder_H__

#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
//class mafVMESurfaceParametric;
class mafGUI;
class mafEvent;
class mafVMECylinder;
//----------------------------------------------------------------------------
// mafOpCreateSurfaceParametric :
//----------------------------------------------------------------------------
/** */
class MAF_EXPORT mafOpCreateCylinder : public mafOp
{
public:
	mafOpCreateCylinder(const mafString& label = _R("Create Cylinder"));
	~mafOpCreateCylinder();

	mafTypeMacro(mafOpCreateCylinder, mafOp);

  mafOp* Copy();

  bool Accept(mafNode *node);
  void OpRun();
  void OpDo();

protected: 
	mafVMECylinder *m_Cylinder;
};
#endif
