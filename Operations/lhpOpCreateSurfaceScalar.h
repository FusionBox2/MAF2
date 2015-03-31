/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpOpCreateSurfaceScalar.h,v $
  Language:  C++
  Date:      $Date: 2008-02-08 12:34:08 $
  Version:   $Revision: 1.1 $
  Authors:   Paolo Quadrani
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __lhpOpCreateSurfaceScalar_H__
#define __lhpOpCreateSurfaceScalar_H__

#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class lhpVMESurfaceScalarVarying;

//----------------------------------------------------------------------------
// lhpOpCreateSurfaceScalar :
//----------------------------------------------------------------------------
/** Operation used to create a new lhpVMESurfaceScalarVarying.
@sa lhpVMESurfaceScalarVarying lhpVisualPipeSurfaceScalar*/
class lhpOpCreateSurfaceScalar: public mafOp
{
public:
  lhpOpCreateSurfaceScalar(const mafString& label = "Create surface scalar");
  ~lhpOpCreateSurfaceScalar(); 

  mafTypeMacro(lhpOpCreateSurfaceScalar, mafOp);

  mafOp* Copy();

  bool Accept(mafNode *node);
  void OpRun();

protected: 
  lhpVMESurfaceScalarVarying *m_SurfaceScalar;
};
#endif
