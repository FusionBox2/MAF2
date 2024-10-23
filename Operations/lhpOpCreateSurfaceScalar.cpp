/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpOpCreateSurfaceScalar.cpp,v $
  Language:  C++
  Date:      $Date: 2008-02-08 12:34:08 $
  Version:   $Revision: 1.1 $
  Authors:   Paolo Quadrani
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------


#include "lhpOpCreateSurfaceScalar.h"
#include "mafDecl.h"

#include "lhpVMESurfaceScalarVarying.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(lhpOpCreateSurfaceScalar);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
lhpOpCreateSurfaceScalar::lhpOpCreateSurfaceScalar(const mafString& label) : Superclass(label)
//----------------------------------------------------------------------------
{
  m_OpType	= OPTYPE_OP;
  m_Canundo = true;
  m_SurfaceScalar   = NULL;
}
//----------------------------------------------------------------------------
lhpOpCreateSurfaceScalar::~lhpOpCreateSurfaceScalar()
//----------------------------------------------------------------------------
{
  mafDEL(m_SurfaceScalar);
}
//----------------------------------------------------------------------------
mafOp* lhpOpCreateSurfaceScalar::Copy()   
//----------------------------------------------------------------------------
{
	return new lhpOpCreateSurfaceScalar(GetLabel());
}
//----------------------------------------------------------------------------
bool lhpOpCreateSurfaceScalar::Accept(mafNode *node)
//----------------------------------------------------------------------------
{
  return (node && node->IsMAFType(mafVME));
}
//----------------------------------------------------------------------------
void lhpOpCreateSurfaceScalar::OpRun()
//----------------------------------------------------------------------------
{
  mafNEW(m_SurfaceScalar);
  m_SurfaceScalar->SetName(_R("Surface Scalar"));
  m_Output = m_SurfaceScalar;
  {mafEvent evUnq(this,OP_RUN_OK); InvokeEvent(evUnq);}
}
