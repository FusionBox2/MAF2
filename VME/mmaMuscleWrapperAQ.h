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
#ifndef __mmaMuscleWrapperAQ_h
#define __mmaMuscleWrapperAQ_h
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafAttribute.h"

/** mmaMeter 
*/
class MAF_EXPORT mmaMuscleWrapperAQ : public mafAttribute
{
public:
	mmaMuscleWrapperAQ();
	virtual ~mmaMuscleWrapperAQ();

	mafTypeMacro(mmaMuscleWrapperAQ, mafAttribute);

  /** print a dump of this object */
  virtual void Print(std::ostream& os, const int tabs=0) const;

  /** Copy the contents of another Meter attribute into this one. */
  virtual void DeepCopy(const mafAttribute *a);

  /** Compare with another Meter attribute. */
  virtual bool Equals(const mafAttribute *a);

  int m_MeterMode;
  int m_ColorMode;
  int m_Representation;
  int m_Capping;
  int m_MeasureType;

  int m_GenerateEvent;
  int m_ThresholdEvent;

  double m_InitMeasure;
  double m_DeltaPercent;
  double m_TubeRadius;
  double m_DistanceRange[2];

  int    m_LabelVisibility;

protected:
  virtual int InternalStore(mafStorageElement *parent);
  virtual int InternalRestore(mafStorageElement *node);
};
#endif