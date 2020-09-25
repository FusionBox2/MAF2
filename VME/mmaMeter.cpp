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
#include "mafVMEMuscleWrapper.h"

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
int mmaMeter::InternalStore(mafStorageElement *parent)
//-----------------------------------------------------------------------
{  
  if (Superclass::InternalStore(parent)==MAF_OK)
  {
    parent->StoreInteger(_R("MeterMode"), m_MeterMode);
    parent->StoreInteger(_R("ColorMode"), m_ColorMode);
    parent->StoreInteger(_R("Representation"), m_Representation);
    parent->StoreInteger(_R("Capping"), m_Capping);
    parent->StoreInteger(_R("MeasureType"), m_MeasureType);
    parent->StoreInteger(_R("GenerateEvent"), m_GenerateEvent);
    parent->StoreDouble(_R("DeltaPercent"), m_DeltaPercent);
    parent->StoreInteger(_R("ThresholdEvent"), m_ThresholdEvent);
    parent->StoreInteger(_R("LabelVisibility"), m_LabelVisibility);
    parent->StoreDouble(_R("InitMeasure"),m_InitMeasure);
    parent->StoreDouble(_R("TubeRadius"),m_TubeRadius);
    parent->StoreVectorN(_R("DistanceRange"),m_DistanceRange,2);
    return MAF_OK;
  }
  return MAF_ERROR;
}
//-----------------------------------------------------------------------
int mmaMeter::InternalRestore(mafStorageElement *node)
//-----------------------------------------------------------------------
{
  if (Superclass::InternalRestore(node)==MAF_OK)
  {
    //mafMatrix matrix;  //modified by Marco. 29-9-2005 It seems this field is not stored...
    ///if (node->RestoreMatrix("Transform",&matrix)==MAF_OK)
    //{
      node->RestoreInteger(_R("MeterMode"), m_MeterMode);
      node->RestoreInteger(_R("ColorMode"), m_ColorMode);
      node->RestoreInteger(_R("Representation"), m_Representation);
      node->RestoreInteger(_R("Capping"), m_Capping);
      node->RestoreInteger(_R("MeasureType"), m_MeasureType);
      node->RestoreInteger(_R("GenerateEvent"), m_GenerateEvent);
      node->RestoreDouble(_R("DeltaPercent"), m_DeltaPercent);
      node->RestoreInteger(_R("ThresholdEvent"), m_ThresholdEvent);
      node->RestoreInteger(_R("LabelVisibility"), m_LabelVisibility);
      node->RestoreDouble(_R("InitMeasure"),m_InitMeasure);
      node->RestoreDouble(_R("TubeRadius"),m_TubeRadius);
      node->RestoreVectorN(_R("DistanceRange"),m_DistanceRange,2);
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
