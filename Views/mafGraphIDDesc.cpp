/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGraphIDDesc.cpp,v $
  Language:  C++
  Date:      $Date: 2008-03-06 22:09:32 $
  Version:   $Revision: 1.2 $
  Authors:   Fedor Moiseev/Vladik Aranov
==========================================================================
  Copyright (c) 2001/2005 
  ULB - Universite Libre de Bruxelles
=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mafVME.h"
#include "mafGraphIDDesc.h"
#include "lhpPipeIntGraphAbstract.h"

//----------------------------------------------------------------------------
// Functions
//----------------------------------------------------------------------------

mafGraphDataImpl::mafGraphDataImpl(lhpPipeIntGraphAbstract *pipe, double garbage,unsigned int size):mafMemoryGraph(garbage, size),m_Pipe(pipe)
{
}

//----------------------------------------------------------------------------
mafString mafGraphDataImpl::GetIDDesc(unsigned index, unsigned int deriv) const
{
  mafString retVal;
  static const mafString val = _R("");
  static const mafString der = _R("Derivative ");
  if(GetID(index) == 0)
  {
      retVal = (deriv == 0) ? val : der;
      retVal += m_Pipe->GetVarTitle(0);
  }
  else
  {
      retVal = (deriv == 0) ? val : der;
      retVal += m_Pipe->m_Node->GetName();
      retVal += _R("");
      retVal += m_Pipe->GetVarTitle(GetID(index));
  }
  return retVal;
}

//----------------------------------------------------------------------------
double mafGraphDataImpl::GetDerivCoef(unsigned index) const
{
  return m_Pipe->GetVarDerivativeCoef(GetID(index));
}

double mafGraphDataImpl::GetTime() const 
{
  return ((mafVME*)(m_Pipe->m_Node))->GetTimeStamp();
}
