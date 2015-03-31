/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpPipeIntGraphLocal.cpp,v $
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

#include "lhpPipeIntGraphLocal.h"
#include "mafDecl.h"
#include "mafJointAnalysis.h"

#include "mafMatrix3x3.h"
#include "mafVME.h"

//----------------------------------------------------------------------------
// constants
//----------------------------------------------------------------------------
namespace
{
  const char *saVarDesc[]  = {"Local Pos X", "Local Pos Y", "Local Pos Z", "Local Ori X", "Local Ori Y", "Local Ori Z"};

  const char *saVarUnits[] = {"mm" ,"mm" ,"mm", "deg","deg","deg"};

  double saCoefs[] = {1.0, 1.0, 1.0, mafMatrix3x3::DegreesToRadians(), mafMatrix3x3::DegreesToRadians(), mafMatrix3x3::DegreesToRadians()};
}


mafCxxTypeMacro(lhpPipeIntGraphLocal);


const char *lhpPipeIntGraphLocal::GetVarTitle(int i) const
{
  if(i < Superclass::GDT_LAST)
    return Superclass::GetVarTitle(i);
  return saVarDesc[i - Superclass::GDT_LAST];
}

const char *lhpPipeIntGraphLocal::GetVarUnit(int i)const
{
  if(i < Superclass::GDT_LAST)
    return Superclass::GetVarUnit(i);
  return saVarUnits[i - Superclass::GDT_LAST];
}

double lhpPipeIntGraphLocal::GetVarDerivativeCoef(int i)const
{
  if(i < Superclass::GDT_LAST)
    return Superclass::GetVarDerivativeCoef(i);
  return saCoefs[i - Superclass::GDT_LAST];
}


//----------------------------------------------------------------------------
lhpPipeIntGraphLocal::lhpPipeIntGraphLocal()
//----------------------------------------------------------------------------
{
  m_vars.resize(GDT_LAST);
  InvalidateAllVars();
}
//----------------------------------------------------------------------------
lhpPipeIntGraphLocal::~lhpPipeIntGraphLocal()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
bool lhpPipeIntGraphLocal::StoreValueByIdx(int nVarID, mafTimeStamp ts, mafTimeStamp prevts)
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

  //LTM
  if(GDT_LTM_POSX <= nVarID && nVarID <= GDT_LTM_ROTZ)
  {
    mafMatrix mLTM;
    V4d<double>    vPos, vRot;
    GetLocalMatrix(m_Vme, ts, mLTM);
    mafTransfInverseTransformUpright(&mLTM, &vPos, &vRot);

    SetValue(GDT_LTM_POSX, vPos.x);
    SetValue(GDT_LTM_POSY, vPos.y);
    SetValue(GDT_LTM_POSZ, vPos.z);
    SetValue(GDT_LTM_ROTX, vRot.x * mafMatrix3x3::RadiansToDegrees());
    SetValue(GDT_LTM_ROTY, vRot.y * mafMatrix3x3::RadiansToDegrees());
    SetValue(GDT_LTM_ROTZ, vRot.z * mafMatrix3x3::RadiansToDegrees());
  }
  return true;
}

