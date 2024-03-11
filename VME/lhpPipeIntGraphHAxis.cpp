/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpPipeIntGraphHAxis.cpp,v $
  Language:  C++
  Date:      $Date: 2008-04-14 12:03:18 $
  Version:   $Revision: 1.6 $
  Authors:   Fedor Moiseev / Vladik Aranov
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

#include "lhpPipeIntGraphHAxis.h"
#include "mafDecl.h"
#include "mafJointAnalysis.h"

#include "mafMatrix3x3.h"
#include "mafVME.h"
#include "mafVMEHelAxis.h"

//----------------------------------------------------------------------------
// constants
//----------------------------------------------------------------------------
namespace
{
  const mafString saVarDesc[]  = {_R("Axis direction X"), _R("Axis direction Y"), _R("Axis direction Z"), _R("Rot center X"), _R("Rot center Y"), _R("Rot center Z"), _R("Axis translation"), _R("Axis rotation angle")};

  const mafString saVarUnits[] = {_R(""), _R(""), _R(""), _R("mm"), _R("mm"), _R("mm"), _R("mm"),_R("deg")};

  const mafString emptyDesc = _R("");

  double saCoefs[] = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,mafMatrix3x3::DegreesToRadians()};
}


mafCxxTypeMacro(lhpPipeIntGraphHAxis);


const mafString& lhpPipeIntGraphHAxis::GetVarTitle(int i) const
{
  if(i >= GDT_LAST)
  {
    wxASSERT(false);
    return emptyDesc;
  }
  if(i < Superclass::GDT_LAST)
    return Superclass::GetVarTitle(i);
  return saVarDesc[i - Superclass::GDT_LAST];
}

const mafString& lhpPipeIntGraphHAxis::GetVarUnit(int i)const
{
  if(i >= GDT_LAST)
  {
    wxASSERT(false);
    return emptyDesc;
  }
  if(i < Superclass::GDT_LAST)
    return Superclass::GetVarUnit(i);
  return saVarUnits[i - Superclass::GDT_LAST];
}

double lhpPipeIntGraphHAxis::GetVarDerivativeCoef(int i)const
{
  if(i >= GDT_LAST)
  {
    wxASSERT(false);
    return 1.0;
  }
  if(i < Superclass::GDT_LAST)
    return Superclass::GetVarDerivativeCoef(i);
  return saCoefs[i - Superclass::GDT_LAST];
}


//----------------------------------------------------------------------------
lhpPipeIntGraphHAxis::lhpPipeIntGraphHAxis()
//----------------------------------------------------------------------------
{
  m_vars.resize(GDT_LAST);
  InvalidateAllVars();
}
//----------------------------------------------------------------------------
lhpPipeIntGraphHAxis::~lhpPipeIntGraphHAxis()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
bool lhpPipeIntGraphHAxis::StoreValueByIdx(int nVarID, mafTimeStamp ts, mafTimeStamp prevts)
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

  if(GDT_HA_DIRX <= nVarID && nVarID <= GDT_HA_DIRZ)
  {
    V3d<double> dir = mafVMEHelAxis::SafeDownCast(m_Node)->GetDirection();
    SetValue(GDT_HA_DIRX, dir.x);
    SetValue(GDT_HA_DIRY, dir.y);
    SetValue(GDT_HA_DIRZ, dir.z);
  }
  else if(GDT_HA_PNTX <= nVarID && nVarID <= GDT_HA_PNTZ)
  {
    V3d<double> pnt= mafVMEHelAxis::SafeDownCast(m_Node)->GetStartPoint();
    SetValue(GDT_HA_PNTX, pnt.x);
    SetValue(GDT_HA_PNTY, pnt.y);
    SetValue(GDT_HA_PNTZ, pnt.z);
  }
  else if(GDT_HA_ANG== nVarID)
  {
    float oldVal= 0.f;
    float newVal= 0.f;

    if(m_Graph->GetUsedMemSpace() != 0 && prevts < ts)
    {
      oldVal= GetValue(GDT_HA_ANG);
    }
    newVal= mafVMEHelAxis::SafeDownCast(m_Node)->GetAngle();
    newVal= Fix180Difference(oldVal, newVal);
    SetValue(GDT_HA_ANG, newVal);
  }
  else if(GDT_HA_TRL== nVarID)
  {
    SetValue(GDT_HA_TRL, mafVMEHelAxis::SafeDownCast(m_Node)->GetTranslation());
  }
  return true;
}

