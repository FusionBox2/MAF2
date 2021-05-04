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

#ifndef __mafOpCreateEllipsoid_H__
#define __mafOpCreateEllipsoid_H__

#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVMESurfaceParametric;
class mafGUI;
class mafEvent;
class mafVMEEllipsoid;
//----------------------------------------------------------------------------
// mafOpCreateSurfaceParametric :
//----------------------------------------------------------------------------
/** */
class MAF_EXPORT mafOpCreateEllipsoid: public mafOp
{
public:
  mafOpCreateEllipsoid(const mafString& label = _R("Create Ellipsoid"));
  ~mafOpCreateEllipsoid(); 

  mafTypeMacro(mafOpCreateEllipsoid, mafOp);

  mafOp* Copy();

  bool Accept(mafNode *node);
  void OpRun();
  void OpDo();

protected: 
  mafVMEEllipsoid *m_Ellipsoid;
};
#endif
