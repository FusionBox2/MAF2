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
void mafGraphDataImpl::GetIDDesc(unsigned index, unsigned int deriv, char *sDescript,unsigned int nLength) const
{
  const char *val = "";
  const char *der = "Derivative ";
  if(GetID(index) == 0)
  {
    _snprintf(sDescript, nLength, "%s%s, %s", (deriv == 0) ? val : der, m_Pipe->GetVarTitle(0), m_Pipe->GetVarUnit(0));
    return;
  }
  _snprintf(sDescript,nLength,"%s%s %s, %s", (deriv == 0) ? val : der, m_Pipe->m_Node->GetName().GetCStr(), m_Pipe->GetVarTitle(GetID(index)), m_Pipe->GetVarUnit(GetID(index)));
  return;
}

//----------------------------------------------------------------------------
double mafGraphDataImpl::GetDerivCoef(unsigned index) const
{
  return m_Pipe->GetVarDerivativeCoef(GetID(index));
}
