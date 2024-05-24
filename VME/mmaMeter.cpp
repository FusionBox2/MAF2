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
int mmaMeter::InternalStore(mafStorageElementBuilder& parent)
//-----------------------------------------------------------------------
{  
  if (Superclass::InternalStore(parent)==MAF_OK)
  {
    parent[_R("MeterMode")].StoreInteger( m_MeterMode);
    parent[_R("ColorMode")].StoreInteger( m_ColorMode);
    parent[_R("Representation")].StoreInteger( m_Representation);
    parent[_R("Capping")].StoreInteger( m_Capping);
    parent[_R("MeasureType")].StoreInteger( m_MeasureType);
    parent[_R("GenerateEvent")].StoreInteger( m_GenerateEvent);
    parent[_R("DeltaPercent")].StoreDouble( m_DeltaPercent);
    parent[_R("ThresholdEvent")].StoreInteger( m_ThresholdEvent);
    parent[_R("LabelVisibility")].StoreInteger( m_LabelVisibility);
    parent[_R("InitMeasure")].StoreDouble(m_InitMeasure);
    parent[_R("TubeRadius")].StoreDouble(m_TubeRadius);
    parent[_R("DistanceRange")].StoreVectorN(m_DistanceRange,2);
    return MAF_OK;
  }
  return MAF_ERROR;
}
//-----------------------------------------------------------------------
int mmaMeter::InternalRestore(const mafStorageElement& node)
//-----------------------------------------------------------------------
{
  if (Superclass::InternalRestore(node)==MAF_OK)
  {
    //mafMatrix matrix;  //modified by Marco. 29-9-2005 It seems this field is not stored...
    ///if (node->RestoreMatrix("Transform",&matrix)==MAF_OK)
    //{
      node[_R("MeterMode")].RestoreInteger( m_MeterMode);
      node[_R("ColorMode")].RestoreInteger( m_ColorMode);
      node[_R("Representation")].RestoreInteger( m_Representation);
      node[_R("Capping")].RestoreInteger( m_Capping);
      node[_R("MeasureType")].RestoreInteger( m_MeasureType);
      node[_R("GenerateEvent")].RestoreInteger( m_GenerateEvent);
      node[_R("DeltaPercent")].RestoreDouble( m_DeltaPercent);
      node[_R("ThresholdEvent")].RestoreInteger( m_ThresholdEvent);
      node[_R("LabelVisibility")].RestoreInteger( m_LabelVisibility);
      node[_R("InitMeasure")].RestoreDouble(m_InitMeasure);
      node[_R("TubeRadius")].RestoreDouble(m_TubeRadius);
      node[_R("DistanceRange")].RestoreVectorN(m_DistanceRange, 2);
      return MAF_OK;
    //}
  }
  return MAF_ERROR;
}
//-----------------------------------------------------------------------
void mmaMeter::Print(std::ostream& os, const int tabs) const
//-----------------------------------------------------------------------
{
  Superclass::Print(os,tabs);
  mafIndent indent(tabs);
}
