/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpPipeIntGraphGlobal.h,v $
  Language:  C++
  Date:      $Date: 2008-03-06 22:11:37 $
  Version:   $Revision: 1.2 $
  Authors:   Fedor Moiseev / Vladik Aranov
==========================================================================
  Copyright (c) 2001/2007 
  ULB - Universite Libre de Bruxelles (www.ulb.ac.be)
=========================================================================*/

#ifndef _lhpPipeIntGraphGlobal_H_
#define _lhpPipeIntGraphGlobal_H_

#include "mafDefines.h"

//----------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------
#include "lhpPipeIntGraphTime.h"

//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// lhpPipeIntGraphGlobal :
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// lhpPipeIntGraphGlobal :
//----------------------------------------------------------------------------
class lhpPipeIntGraphGlobal : public lhpPipeIntGraphTime
{
public:
  mafTypeMacro(lhpPipeIntGraphGlobal, lhpPipeIntGraphTime);

  lhpPipeIntGraphGlobal();
  ~lhpPipeIntGraphGlobal() override;

  enum GRAPH_IDS
  {
  //GTM
  GDT_GTM_POSX    = Superclass::GDT_LAST,
  GDT_GTM_POSY   ,
  GDT_GTM_POSZ   ,
  GDT_GTM_ROTX   ,
  GDT_GTM_ROTY   ,
  GDT_GTM_ROTZ   ,

  //control
  GDT_LAST
  };

  const mafString& GetVarTitle(int i) const override;
  const mafString& GetVarUnit(int i) const override;
  double       GetVarDerivativeCoef(int i) const override;

protected:
  bool StoreValueByIdx(int nVarID, mafTimeStamp ts, mafTimeStamp prevts) override;
};
#endif // _lhpPipeIntGraphGlobal_H_
