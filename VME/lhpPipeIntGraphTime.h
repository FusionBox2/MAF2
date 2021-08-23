/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpPipeIntGraphTime.h,v $
  Language:  C++
  Date:      $Date: 2008-03-06 22:11:37 $
  Version:   $Revision: 1.2 $
  Authors:   Fedor Moiseev
==========================================================================
  Copyright (c) 2001/2007 
  ULB - Universite Libre de Bruxelles (www.ulb.ac.be)
=========================================================================*/

#ifndef _lhpPipeIntGraphTime_H_
#define _lhpPipeIntGraphTime_H_

#include "lhpDefines.h"

//----------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------
#include "lhpPipeIntGraphFixed.h"

//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------
class mafGUICheckListBox;

//----------------------------------------------------------------------------
// lhpPipeIntGraphTime :
//----------------------------------------------------------------------------
class lhpPipeIntGraphTime : public lhpPipeIntGraphFixed
{
public:
  mafTypeMacro(lhpPipeIntGraphTime, lhpPipeIntGraphFixed);
  
  lhpPipeIntGraphTime();
  virtual     ~lhpPipeIntGraphTime();

  enum GRAPH_IDS
  {
    GDT_NA     = -1,
    GDT_FRAME  = Superclass::GDT_LAST,
    //control
    GDT_LAST
  };


  virtual const mafString& GetVarTitle(int i) const;
  virtual const mafString& GetVarUnit(int i) const;
  virtual double       GetVarDerivativeCoef(int i) const;

protected:
  virtual bool StoreValueByIdx(int nVarID, mafTimeStamp ts, mafTimeStamp prevts);
};  
#endif // _lhpPipeIntGraphTime_H_
