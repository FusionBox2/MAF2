/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpOpFindCentroid.h,v $
  Language:  C++
  Date:      $Date: 2008-07-25 07:03:51 $
  Version:   $Revision: 1.2 $
  Authors:   Paolo Quadrani
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __lhpOpFindCentroid_H__
#define __lhpOpFindCentroid_H__

#include "mafOp.h"

//----------------------------------------------------------------------------
// lhpOpFindCentroid :
//----------------------------------------------------------------------------
/** */
class lhpOpFindCentroid: public mafOp
{
public:
  lhpOpFindCentroid(const mafString& label = _R("Find centroid"));
  ~lhpOpFindCentroid() override; 

  mafTypeMacro(lhpOpFindCentroid, mafOp);

  mafOp* Copy() override;

  bool Accept(mafNode *node) override;
  void OpRun() override;
};
#endif
