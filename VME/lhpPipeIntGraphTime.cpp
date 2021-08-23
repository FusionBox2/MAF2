/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpPipeIntGraphTime.cpp,v $
  Language:  C++
  Date:      $Date: 2008-04-14 12:03:18 $
  Version:   $Revision: 1.6 $
  Authors:   Fedor Moiseev
==========================================================================
  Copyright (c) 2001/2007 
  ULB - Universite Libre de Bruxelles (www.ulb.ac.be)
=========================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "lhpPipeIntGraphTime.h"
#include "mafDecl.h"
#include "mafEventSender.h"
#include "mafViewIntGraph.h"
#include "mafGUICheckListBox.h"

#include "mafVME.h"
#include "mafGraphIDDesc.h"

//----------------------------------------------------------------------------
// constants
//----------------------------------------------------------------------------
namespace
{
    const mafString sVarDesc = _R("Time");

    const mafString sVarUnit = _R("s");

    const mafString emptyDesc = _R("");
}

mafCxxTypeMacro(lhpPipeIntGraphTime);

const mafString& lhpPipeIntGraphTime::GetVarTitle(int i) const
{
  if(i != GDT_FRAME)
  {
      wxASSERT(false);
      return emptyDesc;
  }
  return sVarDesc;
}

const mafString& lhpPipeIntGraphTime::GetVarUnit(int i)const
{
    if (i != GDT_FRAME)
    {
        wxASSERT(false);
        return emptyDesc;
    }
    return sVarUnit;
}

double lhpPipeIntGraphTime::GetVarDerivativeCoef(int i)const
{
  if(i != GDT_FRAME)
  {
    wxASSERT(false);
    return 1.0;
  }
  return 1.0;
}


//----------------------------------------------------------------------------
lhpPipeIntGraphTime::lhpPipeIntGraphTime()
//----------------------------------------------------------------------------
{
  m_vars.resize(GDT_LAST);
  m_vars[GDT_FRAME].first  = -1;
  InvalidateAllVars();
}
//----------------------------------------------------------------------------
lhpPipeIntGraphTime::~lhpPipeIntGraphTime()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
bool lhpPipeIntGraphTime::StoreValueByIdx(int nVarID, mafTimeStamp ts, mafTimeStamp prevts)
//----------------------------------------------------------------------------
{
  if(nVarID >= GDT_LAST)
  {
    wxASSERT(false);
    return false;
  }

  if(nVarID < Superclass::GDT_LAST)
  {
    return Superclass::StoreValueByIdx(nVarID, ts, prevts);
  }

  if(nVarID != GDT_FRAME)
  {
    wxASSERT(false);
    return false;
  }
  SetValue(GDT_FRAME, ts);
  //variable should be calculated and validated here 
  return true;
}
