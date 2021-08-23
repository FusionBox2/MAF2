/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpPipeIntGraphEuler.cpp,v $
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

#include "lhpPipeIntGraphEuler.h"
#include "mafDecl.h"
#include "mafJointAnalysis.h"
#include "mafPlotMath.h"

#include "mafMatrix3x3.h"
#include "mafVME.h"

#define EulerAnglesDescr(Convention) _R("Euler "#Convention" Angle1"), _R("Euler "#Convention" Angle2"), _R("Euler "#Convention" Angle3")
//----------------------------------------------------------------------------
// constants
//----------------------------------------------------------------------------
namespace
{
  const mafString saVarDesc[] = 
  {
    EulerAnglesDescr(XYZs),
    EulerAnglesDescr(XYXs),
    EulerAnglesDescr(XZYs),
    EulerAnglesDescr(XZXs),
    EulerAnglesDescr(YZXs),
    EulerAnglesDescr(YZYs),
    EulerAnglesDescr(YXZs),
    EulerAnglesDescr(YXYs),
    EulerAnglesDescr(ZXYs),
    EulerAnglesDescr(ZXZs),
    EulerAnglesDescr(ZYXs),
    EulerAnglesDescr(ZYZs),
    EulerAnglesDescr(ZYXr),
    EulerAnglesDescr(XYXr),
    EulerAnglesDescr(YZXr),
    EulerAnglesDescr(XZXr),
    EulerAnglesDescr(XZYr),
    EulerAnglesDescr(YZYr),
    EulerAnglesDescr(ZXYr),
    EulerAnglesDescr(YXYr),
    EulerAnglesDescr(YXZr),
    EulerAnglesDescr(ZXZr),
    EulerAnglesDescr(XYZr),
    EulerAnglesDescr(ZYZr)
  };
  const int   _Conventions[24] = {EulOrdXYZs, EulOrdXYXs, EulOrdXZYs, EulOrdXZXs, EulOrdYZXs, EulOrdYZYs,
    EulOrdYXZs, EulOrdYXYs, EulOrdZXYs, EulOrdZXZs, EulOrdZYXs, EulOrdZYZs,
    EulOrdZYXr, EulOrdXYXr, EulOrdYZXr, EulOrdXZXr, EulOrdXZYr, EulOrdYZYr,
    EulOrdZXYr, EulOrdYXYr, EulOrdYXZr, EulOrdZXZr, EulOrdXYZr, EulOrdZYZr};

  const mafString sVarUnit = _R("deg");
}


mafCxxTypeMacro(lhpPipeIntGraphEuler);


const mafString& lhpPipeIntGraphEuler::GetVarTitle(int i) const
{
  if(i < Superclass::GDT_LAST)
    return Superclass::GetVarTitle(i);
  return saVarDesc[i - Superclass::GDT_LAST];
}

const mafString& lhpPipeIntGraphEuler::GetVarUnit(int i)const
{
  if(i < Superclass::GDT_LAST)
    return Superclass::GetVarUnit(i);
  return sVarUnit;
}

double lhpPipeIntGraphEuler::GetVarDerivativeCoef(int i)const
{
  if(i < Superclass::GDT_LAST)
    return Superclass::GetVarDerivativeCoef(i);
  return mafMatrix3x3::DegreesToRadians();
}


//----------------------------------------------------------------------------
lhpPipeIntGraphEuler::lhpPipeIntGraphEuler()
//----------------------------------------------------------------------------
{
  m_vars.resize(GDT_LAST);
  InvalidateAllVars();
}
//----------------------------------------------------------------------------
lhpPipeIntGraphEuler::~lhpPipeIntGraphEuler()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
bool lhpPipeIntGraphEuler::StoreValueByIdx(int nVarID, mafTimeStamp ts, mafTimeStamp prevts)
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

  //Euler
  if(GDT_EUL_ROTXXYZs <= nVarID && nVarID <= GDT_EUL_ROTZZYZr)
  {
#ifdef NEWEST_CODE
    mafMatrix mLTM;
    V4d<double>    vRot;
    float  oldValueX= 0.f;
    float  oldValueY= 0.f;
    float  oldValueZ= 0.f;

    //vRot.w= _Conventions[(nVarID - GDT_EUL_ROTXXYZs) / 3];

    GetLocalMatrix(mafVME::SafeDownCast(m_Node), ts, mLTM, m_Proximal);
    mafTransfMatrixToEuler(&mLTM, &vRot, _Conventions[(nVarID - GDT_EUL_ROTXXYZs) / 3]);

    vRot.x *= mafMatrix3x3::RadiansToDegrees();
    vRot.y *= mafMatrix3x3::RadiansToDegrees();
    vRot.z *= mafMatrix3x3::RadiansToDegrees();

    int xindex= nVarID - (nVarID - GDT_EUL_ROTXXYZs) % 3;
    if(m_Graph->GetUsedMemSpace() != 0 && prevts < ts)
    {
      oldValueX= GetValue(xindex + 0);
      oldValueY= GetValue(xindex + 1);
      oldValueZ= GetValue(xindex + 2);
    }
    vRot.x= Fix180Difference( oldValueX, vRot.x);
    vRot.y= Fix180Difference(-oldValueY, vRot.y);
    vRot.z= Fix180Difference( oldValueZ, vRot.z);
#else
    mafMatrix mLTM;
    //V4d<double>    vRot;
    V3d<double> vRot;
    V3d<double> oldValue;

    int xindex = nVarID - (nVarID - GDT_EUL_ROTXXYZs) % 3;
    if (m_Graph->GetUsedMemSpace() != 0 && prevts < ts)
    {
      oldValue[0] = GetValue(xindex + 0) / mafMatrix3x3::RadiansToDegrees();
      oldValue[1] = GetValue(xindex + 1) / mafMatrix3x3::RadiansToDegrees();
      oldValue[2] = GetValue(xindex + 2) / mafMatrix3x3::RadiansToDegrees();
    }

    //vRot.w= _Conventions[(nVarID - GDT_EUL_ROTXXYZs) / 3];

    GetLocalMatrix(mafVME::SafeDownCast(m_Node), ts, mLTM, m_Proximal);
    vRot = EulerAngles<double>(*mLTM.GetVTKMatrix(), _Conventions[(nVarID - GDT_EUL_ROTXXYZs) / 3], oldValue);
    //mafTransfMatrixToEuler(&mLTM, &vRot, _Conventions[(nVarID - GDT_EUL_ROTXXYZs) / 3]);

    vRot.x *= mafMatrix3x3::RadiansToDegrees();
    vRot.y *= mafMatrix3x3::RadiansToDegrees();
    vRot.z *= mafMatrix3x3::RadiansToDegrees();
#endif
    SetValue(xindex + 0, vRot.x);
    SetValue(xindex + 1, vRot.y);
    SetValue(xindex + 2, vRot.z);
  }
  return true;
}

