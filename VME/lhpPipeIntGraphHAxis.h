/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpPipeIntGraphHAxis.h,v $
  Language:  C++
  Date:      $Date: 2008-03-06 22:11:37 $
  Version:   $Revision: 1.2 $
  Authors:   Fedor Moiseev
==========================================================================
  Copyright (c) 2001/2007 
  ULB - Universite Libre de Bruxelles (www.ulb.ac.be)
=========================================================================*/

#ifndef _lhpPipeIntGraphHAxis_H_
#define _lhpPipeIntGraphHAxis_H_

#include "mafDefines.h"

//----------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------
#include "lhpPipeIntGraphTime.h"

//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// lhpPipeIntGraphHAxis :
//----------------------------------------------------------------------------
class lhpPipeIntGraphHAxis : public lhpPipeIntGraphTime
{
public:
  mafTypeMacro(lhpPipeIntGraphHAxis, lhpPipeIntGraphTime);
  
  lhpPipeIntGraphHAxis();
  ~lhpPipeIntGraphHAxis() override;

  enum GRAPH_IDS
  {
    GDT_HA_DIRX = Superclass::GDT_LAST,
    GDT_HA_DIRY,
    GDT_HA_DIRZ,

    GDT_HA_PNTX,
    GDT_HA_PNTY,
    GDT_HA_PNTZ,

    GDT_HA_TRL,
    GDT_HA_ANG,

    //control
    GDT_LAST
  };

  const mafString& GetVarTitle(int i) const override;
  const mafString& GetVarUnit(int i) const override;
  double       GetVarDerivativeCoef(int i) const override;

protected:
  bool StoreValueByIdx(int nVarID, mafTimeStamp ts, mafTimeStamp prevts) override;
};  
#endif // _lhpPipeIntGraphHAxis_H_
