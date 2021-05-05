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

#include "mmaMuscleWrapperAQ.h"
//#include "mafVMEMeter.h"
#include "mafVMEMuscleWrapper.h"
#include "mafStorageElement.h"
#include "mafIndent.h"

#include <assert.h>

//-------------------------------------------------------------------------
mafCxxTypeMacro(mmaMuscleWrapperAQ)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
mmaMuscleWrapperAQ::mmaMuscleWrapperAQ()
//-------------------------------------------------------------------------
{
  m_Name           = "MuscleWrapperAQAttributes";
  m_LabelVisibility= 1;
  m_ColorMode = mafVMEMuscleWrapperAQ::ONE_COLOR;
  m_Representation = mafVMEMuscleWrapperAQ::LINE_REPRESENTATION;
  m_Capping        = 0;
  m_MeterMode = mafVMEMuscleWrapperAQ::POINT_DISTANCE;
  m_TubeRadius     = 1.0;
  m_InitMeasure    = 0.0;
  m_GenerateEvent  = 0;
  m_ThresholdEvent = 0;
  m_DeltaPercent   = 10.0;
  m_MeasureType = mafVMEMuscleWrapperAQ::ABSOLUTE_MEASURE;

  m_DistanceRange[0] = 0.0;
  m_DistanceRange[1] = 1.0;
}
//-------------------------------------------------------------------------
mmaMuscleWrapperAQ::~mmaMuscleWrapperAQ()
//-------------------------------------------------------------------------
{
}
//-------------------------------------------------------------------------
void mmaMuscleWrapperAQ::DeepCopy(const mafAttribute *a)
//-------------------------------------------------------------------------
{ 
  Superclass::DeepCopy(a);
  m_MeterMode = ((mmaMuscleWrapperAQ *)a)->m_MeterMode;
  m_ColorMode = ((mmaMuscleWrapperAQ *)a)->m_ColorMode;
  m_Representation = ((mmaMuscleWrapperAQ *)a)->m_Representation;
  m_Capping = ((mmaMuscleWrapperAQ *)a)->m_Capping;
  m_MeasureType = ((mmaMuscleWrapperAQ *)a)->m_MeasureType;
  m_GenerateEvent = ((mmaMuscleWrapperAQ *)a)->m_GenerateEvent;
  m_DeltaPercent = ((mmaMuscleWrapperAQ *)a)->m_DeltaPercent;
  m_ThresholdEvent = ((mmaMuscleWrapperAQ *)a)->m_ThresholdEvent;
  m_InitMeasure = ((mmaMuscleWrapperAQ *)a)->m_InitMeasure;
  m_TubeRadius = ((mmaMuscleWrapperAQ *)a)->m_TubeRadius;
  m_DistanceRange[0] = ((mmaMuscleWrapperAQ *)a)->m_DistanceRange[0];
  m_DistanceRange[1] = ((mmaMuscleWrapperAQ *)a)->m_DistanceRange[1];
  m_LabelVisibility = ((mmaMuscleWrapperAQ *)a)->m_LabelVisibility;
}
//-------------------------------------------------------------------------
bool mmaMuscleWrapperAQ::Equals(const mafAttribute *a)
//-------------------------------------------------------------------------
{
  if (Superclass::Equals(a))
  {
	  return (m_MeterMode == ((mmaMuscleWrapperAQ *)a)->m_MeterMode       &&
		  m_ColorMode == ((mmaMuscleWrapperAQ *)a)->m_ColorMode       &&
		  m_Representation == ((mmaMuscleWrapperAQ *)a)->m_Representation  &&
		  m_Capping == ((mmaMuscleWrapperAQ *)a)->m_Capping         &&
			m_MeasureType == ((mmaMuscleWrapperAQ *)a)->m_MeasureType     &&
			m_GenerateEvent == ((mmaMuscleWrapperAQ *)a)->m_GenerateEvent   &&
			m_DeltaPercent == ((mmaMuscleWrapperAQ *)a)->m_DeltaPercent    &&
			m_ThresholdEvent == ((mmaMuscleWrapperAQ *)a)->m_ThresholdEvent  &&
			m_InitMeasure == ((mmaMuscleWrapperAQ *)a)->m_InitMeasure     &&
			m_TubeRadius == ((mmaMuscleWrapperAQ *)a)->m_TubeRadius      &&
			m_DistanceRange[0] == ((mmaMuscleWrapperAQ *)a)->m_DistanceRange[0] &&
			m_DistanceRange[1] == ((mmaMuscleWrapperAQ *)a)->m_DistanceRange[1] &&
			m_LabelVisibility == ((mmaMuscleWrapperAQ *)a)->m_LabelVisibility);
  }
  return false;
}
//-----------------------------------------------------------------------
int mmaMuscleWrapperAQ::InternalStore(mafStorageElement *parent)
//-----------------------------------------------------------------------
{  
  if (Superclass::InternalStore(parent)==MAF_OK)
  {
    parent->StoreInteger("MeterMode", m_MeterMode);
    parent->StoreInteger("ColorMode", m_ColorMode);
    parent->StoreInteger("Representation", m_Representation);
    parent->StoreInteger("Capping", m_Capping);
    parent->StoreInteger("MeasureType", m_MeasureType);
    parent->StoreInteger("GenerateEvent", m_GenerateEvent);
    parent->StoreDouble("DeltaPercent", m_DeltaPercent);
    parent->StoreInteger("ThresholdEvent", m_ThresholdEvent);
    parent->StoreInteger("LabelVisibility", m_LabelVisibility);
    parent->StoreDouble("InitMeasure",m_InitMeasure);
    parent->StoreDouble("TubeRadius",m_TubeRadius);
    parent->StoreVectorN("DistanceRange",m_DistanceRange,2);
    return MAF_OK;
  }
  return MAF_ERROR;
}
//-----------------------------------------------------------------------
int mmaMuscleWrapperAQ::InternalRestore(mafStorageElement *node)
//-----------------------------------------------------------------------
{
  if (Superclass::InternalRestore(node)==MAF_OK)
  {
    //mafMatrix matrix;  //modified by Marco. 29-9-2005 It seems this field is not stored...
    ///if (node->RestoreMatrix("Transform",&matrix)==MAF_OK)
    //{
      node->RestoreInteger("MeterMode", m_MeterMode);
      node->RestoreInteger("ColorMode", m_ColorMode);
      node->RestoreInteger("Representation", m_Representation);
      node->RestoreInteger("Capping", m_Capping);
      node->RestoreInteger("MeasureType", m_MeasureType);
      node->RestoreInteger("GenerateEvent", m_GenerateEvent);
      node->RestoreDouble("DeltaPercent", m_DeltaPercent);
      node->RestoreInteger("ThresholdEvent", m_ThresholdEvent);
      node->RestoreInteger("LabelVisibility", m_LabelVisibility);
      node->RestoreDouble("InitMeasure",m_InitMeasure);
      node->RestoreDouble("TubeRadius",m_TubeRadius);
      node->RestoreVectorN("DistanceRange",m_DistanceRange,2);
      return MAF_OK;
    //}
  }
  return MAF_ERROR;
}
//-----------------------------------------------------------------------
void mmaMuscleWrapperAQ::Print(std::ostream& os, const int tabs) const
//-----------------------------------------------------------------------
{
  Superclass::Print(os,tabs);
  mafIndent indent(tabs);
}
