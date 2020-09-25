/*=========================================================================

 Program: MAF2
 Module: mafOpCreateSurfaceParametric
 Authors: Taha Jerbi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafOpCreateHyperboloid2S_H__
#define __mafOpCreateHyperboloid2S_H__

#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVMESurfaceParametric;
class mafGUI;
class mafEvent;
class mafVMEHyperboloid2S;
//----------------------------------------------------------------------------
// mafOpCreateSurfaceParametric :
//----------------------------------------------------------------------------
/** */
class MAF_EXPORT mafOpCreateHyperboloid2S : public mafOp
{
public:
	mafOpCreateHyperboloid2S(const mafString& label = "Create Hyperboloid");
	~mafOpCreateHyperboloid2S();

	mafTypeMacro(mafOpCreateHyperboloid2S, mafOp);

  mafOp* Copy();

  bool Accept(mafNode *node);
  void OpRun();
  void OpDo();

protected: 
	mafVMEHyperboloid2S *m_Hyperboloid;
};
#endif
