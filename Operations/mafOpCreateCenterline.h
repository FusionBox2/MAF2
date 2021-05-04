/*=========================================================================

 Program: MAF2
 Module: mafOpCreateGravityLine
 Authors: Taha JErbi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafOpCreateCenterline_H__
#define __mafOpCreateCenterline_H__

#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVMECenterLine;
class mafGUI;
class mafEvent;
//----------------------------------------------------------------------------
// mafOpCreateMeter :
//----------------------------------------------------------------------------
/** */
class MAF_EXPORT mafOpCreateCenterLine : public mafOp
{
public:
	mafOpCreateCenterLine(const mafString& label = _R("CreateCenterLine"));
	~mafOpCreateCenterLine();

	mafTypeMacro(mafOpCreateCenterLine, mafOp);

  mafOp* Copy();

  bool Accept(mafNode *node);
  void OpRun();
  void OpDo();

protected: 
	mafVMECenterLine *m_Meter;
};
#endif
