/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpPipeIntGraphGlobal.cpp,v $
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

#include "lhpPipeIntGraphGlobal.h"
#include "mafDecl.h"
#include "mafJointAnalysis.h"

#include "mafMatrix3x3.h"
#include "mafVME.h"

//----------------------------------------------------------------------------
// constants
//----------------------------------------------------------------------------
namespace
{
  const char *saVarDesc[]  = {"Pos X", "Pos Y", "Pos Z", "Ori X", "Ori Y", "Ori Z",};

  const char *saVarUnits[] = {"mm" ,"mm" ,"mm", "deg","deg","deg",};

  double saCoefs[] = {1.0, 1.0, 1.0, mafMatrix3x3::DegreesToRadians(), mafMatrix3x3::DegreesToRadians(), mafMatrix3x3::DegreesToRadians(),};
}


mafCxxTypeMacro(lhpPipeIntGraphGlobal);


const char *lhpPipeIntGraphGlobal::GetVarTitle(int i) const
{
  if(i < Superclass::GDT_LAST)
    return Superclass::GetVarTitle(i);
  return saVarDesc[i - Superclass::GDT_LAST];
}

const char *lhpPipeIntGraphGlobal::GetVarUnit(int i)const
{
  if(i < Superclass::GDT_LAST)
    return Superclass::GetVarUnit(i);
  return saVarUnits[i - Superclass::GDT_LAST];
}

double lhpPipeIntGraphGlobal::GetVarDerivativeCoef(int i)const
{
  if(i < Superclass::GDT_LAST)
    return Superclass::GetVarDerivativeCoef(i);
  return saCoefs[i - Superclass::GDT_LAST];
}


//----------------------------------------------------------------------------
lhpPipeIntGraphGlobal::lhpPipeIntGraphGlobal()
//----------------------------------------------------------------------------
{
  m_vars.resize(GDT_LAST);
  InvalidateAllVars();
}
//----------------------------------------------------------------------------
lhpPipeIntGraphGlobal::~lhpPipeIntGraphGlobal()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
bool lhpPipeIntGraphGlobal::StoreValueByIdx(int nVarID, mafTimeStamp ts, mafTimeStamp prevts)
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

  //GTM
  if(GDT_GTM_POSX <= nVarID && nVarID <= GDT_GTM_ROTZ)
  {
    mafMatrix mat;
    V4d<double> vPos, vRot;
    //get full trio in proper convention and axises
    GetGlobalMatrix(m_Vme, ts, mat);
    mafTransfInverseTransformUpright(&mat, &vPos, &vRot);

    SetValue(GDT_GTM_POSX, vPos.x);
    SetValue(GDT_GTM_POSY, vPos.y);
    SetValue(GDT_GTM_POSZ, vPos.z);
    SetValue(GDT_GTM_ROTX, vRot.x * mafMatrix3x3::RadiansToDegrees());
    SetValue(GDT_GTM_ROTY, vRot.y * mafMatrix3x3::RadiansToDegrees());
    SetValue(GDT_GTM_ROTZ, vRot.z * mafMatrix3x3::RadiansToDegrees());
  }
  return true;
}

