/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpPipeIntGraph.h,v $
  Language:  C++
  Date:      $Date: 2008-03-06 22:11:37 $
  Version:   $Revision: 1.2 $
  Authors:   Fedor Moiseev
==========================================================================
  Copyright (c) 2001/2007 
  ULB - Universite Libre de Bruxelles (www.ulb.ac.be)
=========================================================================*/

#ifndef _lhpPipeIntGraph_H_
#define _lhpPipeIntGraph_H_

#include "mafDefines.h"

//----------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------
#include "lhpPipeIntGraphEuler.h"

//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// lhpPipeIntGraph :
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// lhpPipeIntGraph :
//----------------------------------------------------------------------------
class lhpPipeIntGraph : public lhpPipeIntGraphEuler
{
public:
  mafTypeMacro(lhpPipeIntGraph, lhpPipeIntGraphEuler);

  lhpPipeIntGraph();
  virtual     ~lhpPipeIntGraph();
protected:
};
#endif // _lhpPipeIntGraph_H_
