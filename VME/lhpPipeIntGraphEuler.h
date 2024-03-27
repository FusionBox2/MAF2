/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpPipeIntGraphEuler.h,v $
  Language:  C++
  Date:      $Date: 2008-03-06 22:11:37 $
  Version:   $Revision: 1.2 $
  Authors:   Fedor Moiseev / Vladik Aranov
==========================================================================
  Copyright (c) 2001/2007 
  ULB - Universite Libre de Bruxelles (www.ulb.ac.be)
=========================================================================*/

#ifndef _lhpPipeIntGraphEuler_H_
#define _lhpPipeIntGraphEuler_H_

#include "mafDefines.h"

//----------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------
#include "lhpPipeIntGraphOVPGES.h"

//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// lhpPipeIntGraphEuler :
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// lhpPipeIntGraphEuler :
//----------------------------------------------------------------------------
class lhpPipeIntGraphEuler : public lhpPipeIntGraphOVPGES
{
public:
  mafTypeMacro(lhpPipeIntGraphEuler, lhpPipeIntGraphOVPGES);

  lhpPipeIntGraphEuler();
  virtual     ~lhpPipeIntGraphEuler();

  enum GRAPH_IDS
  {
  //Euler
  GDT_EUL_ROTXXYZs  = Superclass::GDT_LAST,
  GDT_EUL_ROTYXYZs ,
  GDT_EUL_ROTZXYZs ,

  GDT_EUL_ROTXXYXs ,
  GDT_EUL_ROTYXYXs ,
  GDT_EUL_ROTZXYXs ,

  GDT_EUL_ROTXXZYs ,
  GDT_EUL_ROTYXZYs ,
  GDT_EUL_ROTZXZYs ,

  GDT_EUL_ROTXXZXs ,
  GDT_EUL_ROTYXZXs ,
  GDT_EUL_ROTZXZXs ,

  GDT_EUL_ROTXYZXs ,
  GDT_EUL_ROTYYZXs ,
  GDT_EUL_ROTZYZXs ,

  GDT_EUL_ROTXYZYs ,
  GDT_EUL_ROTYYZYs ,
  GDT_EUL_ROTZYZYs ,

  GDT_EUL_ROTXYXZs ,
  GDT_EUL_ROTYYXZs ,
  GDT_EUL_ROTZYXZs ,

  GDT_EUL_ROTXYXYs ,
  GDT_EUL_ROTYYXYs ,
  GDT_EUL_ROTZYXYs ,

  GDT_EUL_ROTXZXYs ,
  GDT_EUL_ROTYZXYs ,
  GDT_EUL_ROTZZXYs ,

  GDT_EUL_ROTXZXZs ,
  GDT_EUL_ROTYZXZs ,
  GDT_EUL_ROTZZXZs ,

  GDT_EUL_ROTXZYXs ,
  GDT_EUL_ROTYZYXs ,
  GDT_EUL_ROTZZYXs ,

  GDT_EUL_ROTXZYZs ,
  GDT_EUL_ROTYZYZs ,
  GDT_EUL_ROTZZYZs ,


  GDT_EUL_ROTXZYXr ,
  GDT_EUL_ROTYZYXr ,
  GDT_EUL_ROTZZYXr ,

  GDT_EUL_ROTXXYXr ,
  GDT_EUL_ROTYXYXr ,
  GDT_EUL_ROTZXYXr ,

  GDT_EUL_ROTXYZXr ,
  GDT_EUL_ROTYYZXr ,
  GDT_EUL_ROTZYZXr ,

  GDT_EUL_ROTXXZXr ,
  GDT_EUL_ROTYXZXr ,
  GDT_EUL_ROTZXZXr ,

  GDT_EUL_ROTXXZYr ,
  GDT_EUL_ROTYXZYr ,
  GDT_EUL_ROTZXZYr ,

  GDT_EUL_ROTXYZYr ,
  GDT_EUL_ROTYYZYr ,
  GDT_EUL_ROTZYZYr ,

  GDT_EUL_ROTXZXYr ,
  GDT_EUL_ROTYZXYr ,
  GDT_EUL_ROTZZXYr ,

  GDT_EUL_ROTXYXYr ,
  GDT_EUL_ROTYYXYr ,
  GDT_EUL_ROTZYXYr ,

  GDT_EUL_ROTXYXZr ,
  GDT_EUL_ROTYYXZr ,
  GDT_EUL_ROTZYXZr ,

  GDT_EUL_ROTXZXZr ,
  GDT_EUL_ROTYZXZr ,
  GDT_EUL_ROTZZXZr ,

  GDT_EUL_ROTXXYZr ,
  GDT_EUL_ROTYXYZr ,
  GDT_EUL_ROTZXYZr ,

  GDT_EUL_ROTXZYZr ,
  GDT_EUL_ROTYZYZr ,
  GDT_EUL_ROTZZYZr ,

  //control
  GDT_LAST
  };

  virtual const mafString& GetVarTitle(int i) const;
  virtual const mafString& GetVarUnit(int i) const;
  virtual double       GetVarDerivativeCoef(int i) const;

protected:
  virtual bool StoreValueByIdx(int nVarID, mafTimeStamp ts, mafTimeStamp prevts);
};
#endif // _lhpPipeIntGraphEuler_H_
