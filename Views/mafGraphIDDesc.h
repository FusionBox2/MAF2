/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGraphIDDesc.h,v $
  Language:  C++
  Date:      $Date: 2008-03-06 22:09:32 $
  Version:   $Revision: 1.2 $
  Authors:   Vladik Aranov    
  Purpose:   class for variables descriptions header 
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/
#ifndef __mafGraphIDDesc_H___
#define __mafGraphIDDesc_H___
    
#include "mafMemGraph.h"
#include "mafViewIntGraphWindow.h"

//----------------------------------------------------------------------------
// Constant
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
// Forward definitions
//----------------------------------------------------------------------------

class lhpPipeIntGraphAbstract;
//----------------------------------------------------------------------------
// Classes
//----------------------------------------------------------------------------


class mafGraphDataImpl: public mafGraphData, public mafMemoryGraph
{
public:
  mafGraphDataImpl(lhpPipeIntGraphAbstract *pipe, double garbage,unsigned int size = 50000);
  double   GetValue(unsigned int point,unsigned int coord, unsigned int deriv = 0) const override {return (*this)(point, coord, deriv);}
  bool     GetValueByParam(double& res, double param,unsigned int coord, unsigned int deriv = 0) const override {return mafMemoryGraph::GetValueByParam(res, param, coord, deriv);}
  unsigned GetVarNum() const override {return GetDim();}
  unsigned GetXVarNum() const override {return GetXDim();}
  unsigned GetYVarNum() const override {return GetYDim();}
  unsigned GetXDer(unsigned index) const override {return GetXDeriv(index);}
  unsigned GetYDer(unsigned index) const override {return GetYDeriv(index);}
  unsigned GetIndexX(unsigned index) const override {return GetXIndex(index);}
  unsigned GetIndexY(unsigned index) const override {return GetYIndex(index);}
  unsigned GetSize() const override {return GetUsedMemSpace();}
  unsigned GetBreakBegin() const override {return BreakBegin();}
  unsigned GetBreakEnd()  const override {return BreakEnd();}
  double   GetDerivCoef(unsigned index) const override;
  mafString GetIDDesc(unsigned index, unsigned int deriv) const override;
  double   GetTime() const override;
private:
  lhpPipeIntGraphAbstract *m_Pipe;
};

#endif // __mafGraphIDDesc_H___