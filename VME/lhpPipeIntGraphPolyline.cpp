/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpPipeIntGraphPolyline.cpp,v $
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

#include "lhpPipeIntGraphPolyline.h"
#include "mafDecl.h"

#include "mafVME.h"
#include "mafVMEOutputPolyline.h"
#include "medVMEWrappedMeter.h"
#include "mafVMEMeter.h"
#include "medVMEComputeWrapping.h"
#include "lhpVMELeverArm.h"
#include "mafMatrix3x3.h"
#include "mafVMEMuscleWrapping.h"
//----------------------------------------------------------------------------
// constants
//----------------------------------------------------------------------------
namespace
{
    const mafString saVarDesc[] = { _R("Angle"), _R("Length") };

    const mafString saVarUnits[] = { _R("deg"), _R("mm") };

    const mafString emptyDesc = _R("");
}
mafCxxTypeMacro(lhpPipeIntGraphPolyline);


const mafString& lhpPipeIntGraphPolyline::GetVarTitle(int i) const
{
  if(i >= GDT_LAST)
  {
    wxASSERT(false);
    return emptyDesc;
  }
  if(i < Superclass::GDT_LAST)
    return Superclass::GetVarTitle(i);
  if(mafVMEMeter *m = mafVMEMeter::SafeDownCast(m_Node))
  {
    if(m->GetMeterMode() == mafVMEMeter::LINE_ANGLE)
      return saVarDesc[0];
  }
  return saVarDesc[1];
}

const mafString& lhpPipeIntGraphPolyline::GetVarUnit(int i)const
{
  if(i >= GDT_LAST)
  {
    wxASSERT(false);
    return emptyDesc;
  }
  if(i < Superclass::GDT_LAST)
    return Superclass::GetVarUnit(i);
  if(mafVMEMeter *m = mafVMEMeter::SafeDownCast(m_Node))
  {
    if(m->GetMeterMode() == mafVMEMeter::LINE_ANGLE)
      return saVarUnits[0];
  }

  return saVarUnits[1];
}

double lhpPipeIntGraphPolyline::GetVarDerivativeCoef(int i)const
{
  if(i >= GDT_LAST)
  {
    wxASSERT(false);
    return 1.0;
  }
  if(i < Superclass::GDT_LAST)
    return Superclass::GetVarDerivativeCoef(i);
  if(mafVMEMeter *m = mafVMEMeter::SafeDownCast(m_Node))
  {
    if(m->GetMeterMode() == mafVMEMeter::LINE_ANGLE)
      return mafMatrix3x3::DegreesToRadians();
  }

  return 1.0;
}


//----------------------------------------------------------------------------
lhpPipeIntGraphPolyline::lhpPipeIntGraphPolyline()
//----------------------------------------------------------------------------
{
  m_vars.resize(GDT_LAST);
  InvalidateAllVars();
}
//----------------------------------------------------------------------------
lhpPipeIntGraphPolyline::~lhpPipeIntGraphPolyline()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
bool lhpPipeIntGraphPolyline::StoreValueByIdx(int nVarID, mafTimeStamp ts, mafTimeStamp prevts)
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

  if(GDT_LENGTH == nVarID)
  {
    mafVMEOutputPolyline *output = mafVMEOutputPolyline::SafeDownCast(mafVME::SafeDownCast(m_Node)->GetOutput());
    wxASSERT(output != NULL);
    if(output != NULL)
    {
      medVMEComputeWrapping *cwrap   = medVMEComputeWrapping::SafeDownCast(m_Node);
      medVMEWrappedMeter    *wrapped = medVMEWrappedMeter::SafeDownCast(m_Node);
      mafVMEMeter           *meter   = mafVMEMeter::SafeDownCast(m_Node);
      lhpVMELeverArm        *lever   = lhpVMELeverArm::SafeDownCast(m_Node);
	  mafVMEMuscleWrapping *mwrapping = mafVMEMuscleWrapping::SafeDownCast(m_Node);

      if(wrapped != NULL)
        SetValue(GDT_LENGTH, wrapped->GetDistance());
      else if (meter != NULL)
      {
        if(meter->GetMeterMode() != mafVMEMeter::LINE_ANGLE)
          SetValue(GDT_LENGTH, meter->GetDistance());
        else
          SetValue(GDT_LENGTH, meter->GetAngle());
      }
      else if (cwrap != NULL)
        SetValue(GDT_LENGTH, cwrap->GetDistance());
      else if (lever != NULL)
        SetValue(GDT_LENGTH, lever->GetDistance());
	  else if (mwrapping!=NULL)
		  SetValue(GDT_LENGTH, mwrapping->GetDistance());
      else
        SetValue(GDT_LENGTH, output->CalculateLength());
    }
  }
  return true;
}

