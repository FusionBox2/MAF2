/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpPipeIntGraphPolyline.h,v $
  Language:  C++
  Date:      $Date: 2008-03-06 22:11:37 $
  Version:   $Revision: 1.2 $
  Authors:   Fedor Moiseev
==========================================================================
  Copyright (c) 2001/2007 
  ULB - Universite Libre de Bruxelles (www.ulb.ac.be)
=========================================================================*/

#ifndef _lhpPipeIntGraphPolyline_H_
#define _lhpPipeIntGraphPolyline_H_

#include "lhpDefines.h"

//----------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------
#include "lhpPipeIntGraphTime.h"

//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// lhpPipeIntGraphPolyline :
//----------------------------------------------------------------------------
class lhpPipeIntGraphPolyline : public lhpPipeIntGraphTime
{
public:
  mafTypeMacro(lhpPipeIntGraphPolyline, lhpPipeIntGraphTime);
  
  lhpPipeIntGraphPolyline();
  virtual     ~lhpPipeIntGraphPolyline();

  enum GRAPH_IDS
  {
    //Meters' length
    GDT_LENGTH = Superclass::GDT_LAST,
    //control
    GDT_LAST
  };

  virtual const mafString& GetVarTitle(int i) const;
  virtual const mafString& GetVarUnit(int i) const;
  virtual double       GetVarDerivativeCoef(int i) const;

protected:
  virtual bool StoreValueByIdx(int nVarID, mafTimeStamp ts, mafTimeStamp prevts);
};  
#endif // _lhpPipeIntGraphPolyline_H_
