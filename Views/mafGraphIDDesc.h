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
  virtual double   GetValue(unsigned int point,unsigned int coord, unsigned int deriv = 0) const {return (*this)(point, coord, deriv);}
  virtual bool     GetValueByParam(double& res, double param,unsigned int coord, unsigned int deriv = 0) const {return mafMemoryGraph::GetValueByParam(res, param, coord, deriv);}
  virtual unsigned GetVarNum() const{return GetDim();}
  virtual unsigned GetXVarNum() const {return GetXDim();}
  virtual unsigned GetYVarNum() const {return GetYDim();}
  virtual unsigned GetXDer(unsigned index) const {return GetXDeriv(index);}
  virtual unsigned GetYDer(unsigned index) const {return GetYDeriv(index);}
  virtual unsigned GetIndexX(unsigned index) const {return GetXIndex(index);}
  virtual unsigned GetIndexY(unsigned index) const {return GetYIndex(index);}
  virtual unsigned GetSize() const {return GetUsedMemSpace();}
  virtual unsigned GetBreakBegin() const {return BreakBegin();}
  virtual unsigned GetBreakEnd()  const {return BreakEnd();}
  virtual double   GetDerivCoef(unsigned index) const;
  virtual void     GetIDDesc(unsigned index, unsigned int deriv, char *sDescript,unsigned int nLength) const;
  virtual double   GetTime() const;
private:
  lhpPipeIntGraphAbstract *m_Pipe;
};

#endif // __mafGraphIDDesc_H___