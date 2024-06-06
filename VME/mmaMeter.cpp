/*=========================================================================

 Program: MAF2
 Module: mmaMeter
 Authors: Marco Petrone, Paolo Quadrani
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mmaMeter.h"
#include "mafVMEMeter.h"
#include "mafStorageElement.h"
#include "mafIndent.h"

#include <assert.h>

//-------------------------------------------------------------------------
mafCxxTypeMacro(mmaMeter)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
mmaMeter::mmaMeter()
//-------------------------------------------------------------------------
{
  m_Name           = _R("MeterAttributes");
  m_LabelVisibility= 1;
  m_ColorMode      = mafVMEMeter::ONE_COLOR;
  m_Representation = mafVMEMeter::LINE_REPRESENTATION;
  m_Capping        = 0;
  m_MeterMode      = mafVMEMeter::POINT_DISTANCE;
  m_TubeRadius     = 1.0;
  m_InitMeasure    = 0.0;
  m_GenerateEvent  = 0;
  m_ThresholdEvent = 0;
  m_DeltaPercent   = 10.0;
  m_MeasureType    = mafVMEMeter::ABSOLUTE_MEASURE;

  m_DistanceRange[0] = 0.0;
  m_DistanceRange[1] = 1.0;
}
//-------------------------------------------------------------------------
mmaMeter::~mmaMeter()
//-------------------------------------------------------------------------
{
}
//-------------------------------------------------------------------------
void mmaMeter::DeepCopy(const mafAttribute *a)
//-------------------------------------------------------------------------
{ 
  Superclass::DeepCopy(a);
  m_MeterMode       = ((mmaMeter *)a)->m_MeterMode;
  m_ColorMode       = ((mmaMeter *)a)->m_ColorMode;
  m_Representation  = ((mmaMeter *)a)->m_Representation;
  m_Capping         = ((mmaMeter *)a)->m_Capping;
  m_MeasureType     = ((mmaMeter *)a)->m_MeasureType;
  m_GenerateEvent   = ((mmaMeter *)a)->m_GenerateEvent;
  m_DeltaPercent    = ((mmaMeter *)a)->m_DeltaPercent;
  m_ThresholdEvent  = ((mmaMeter *)a)->m_ThresholdEvent;
  m_InitMeasure     = ((mmaMeter *)a)->m_InitMeasure;
  m_TubeRadius      = ((mmaMeter *)a)->m_TubeRadius;
  m_DistanceRange[0]= ((mmaMeter *)a)->m_DistanceRange[0];
  m_DistanceRange[1]= ((mmaMeter *)a)->m_DistanceRange[1];
  m_LabelVisibility = ((mmaMeter *)a)->m_LabelVisibility;
}
//-------------------------------------------------------------------------
bool mmaMeter::Equals(const mafAttribute *a)
//-------------------------------------------------------------------------
{
  if (Superclass::Equals(a))
  {
    return (m_MeterMode       == ((mmaMeter *)a)->m_MeterMode       &&
            m_ColorMode       == ((mmaMeter *)a)->m_ColorMode       &&
            m_Representation  == ((mmaMeter *)a)->m_Representation  &&
            m_Capping         == ((mmaMeter *)a)->m_Capping         &&
            m_MeasureType     == ((mmaMeter *)a)->m_MeasureType     &&
            m_GenerateEvent   == ((mmaMeter *)a)->m_GenerateEvent   &&
            m_DeltaPercent    == ((mmaMeter *)a)->m_DeltaPercent    &&
            m_ThresholdEvent  == ((mmaMeter *)a)->m_ThresholdEvent  &&
            m_InitMeasure     == ((mmaMeter *)a)->m_InitMeasure     &&
            m_TubeRadius      == ((mmaMeter *)a)->m_TubeRadius      &&
            m_DistanceRange[0]== ((mmaMeter *)a)->m_DistanceRange[0]&&
            m_DistanceRange[1]== ((mmaMeter *)a)->m_DistanceRange[1]&&
            m_LabelVisibility == ((mmaMeter *)a)->m_LabelVisibility);
  }
  return false;
}
//-----------------------------------------------------------------------
void mmaMeter::InternalStore(mafStorageElementBuilder& parent)
//-----------------------------------------------------------------------
{  
  Superclass::InternalStore(parent);
  parent[_R("MeterMode")].SetValue(m_MeterMode);
  parent[_R("ColorMode")].SetValue(m_ColorMode);
  parent[_R("Representation")].SetValue(m_Representation);
  parent[_R("Capping")].SetValue(m_Capping);
  parent[_R("MeasureType")].SetValue(m_MeasureType);
  parent[_R("GenerateEvent")].SetValue(m_GenerateEvent);
  parent[_R("DeltaPercent")].SetValue(m_DeltaPercent);
  parent[_R("ThresholdEvent")].SetValue(m_ThresholdEvent);
  parent[_R("LabelVisibility")].SetValue(m_LabelVisibility);
  parent[_R("InitMeasure")].SetValue(m_InitMeasure);
  parent[_R("TubeRadius")].SetValue(m_TubeRadius);
  parent[_R("DistanceRange")].SetValue(mafToString(m_DistanceRange, 2));
}
//-----------------------------------------------------------------------
void mmaMeter::InternalRestore(const mafStorageElement& node)
//-----------------------------------------------------------------------
{
  Superclass::InternalRestore(node);
  m_MeterMode = node[_R("MeterMode")].As<int>();
  m_ColorMode = node[_R("ColorMode")].As<int>();
  m_Representation = node[_R("Representation")].As<int>();
  m_Capping = node[_R("Capping")].As<int>();
  m_MeasureType = node[_R("MeasureType")].As<int>();
  m_GenerateEvent = node[_R("GenerateEvent")].As<int>();
  m_DeltaPercent = node[_R("DeltaPercent")].As<double>();
  m_ThresholdEvent = node[_R("ThresholdEvent")].As<int>();
  m_LabelVisibility = node[_R("LabelVisibility")].As<int>();
  m_InitMeasure = node[_R("InitMeasure")].As<double>();
  m_TubeRadius = node[_R("TubeRadius")].As<double>();
  mafParseVector(node[_R("DistanceRange")].As<mafString>(), m_DistanceRange, 2);
}
//-----------------------------------------------------------------------
void mmaMeter::Print(std::ostream& os, const int tabs) const
//-----------------------------------------------------------------------
{
  Superclass::Print(os,tabs);
  mafIndent indent(tabs);
}
