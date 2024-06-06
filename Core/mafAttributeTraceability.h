/*=========================================================================

 Program: MAF2
 Module: mafAttributeTraceability
 Authors: Roberto Mucci
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __mafAttributeTraceability_h
#define __mafAttributeTraceability_h
//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "mafAttribute.h"
#include <vector>

//----------------------------------------------------------------------------
// mafAttributeTraceability:
//----------------------------------------------------------------------------
/** Set trials information about operations which created or modified VME */  
class MAF_EXPORT mafAttributeTraceability : public mafAttribute
{
public:
           mafAttributeTraceability();
  virtual ~mafAttributeTraceability();
  
  mafTypeMacro(mafAttributeTraceability,mafAttribute);

  /** add a create event */
  void AddTraceabilityEvent(const mafString& m_TrialEvent, const mafString& operationName, const mafString& parmaters, const mafString& date, const mafString& appStamp, const mafString& operatorID, const mafString& isNatural = _R(""));
 
  /** remove a traceability event */
  void RemoveTraceabilityEvent(); 

  /** copy the content of the given attribute. Attributes must be type compatible */
  virtual void DeepCopy(const mafAttribute *a);

  /** create a copy of this attribute */
  mafAttributeTraceability *MakeCopy();

  /** return true if this attribute equals the given one */
  bool Equals(const mafAttribute *a) const;

  /** return trial type form vetor of traceability event*/
  mafString GetLastTrialEvent();

  struct Traceability
  {
    mafString m_TrialEvent;
    mafString m_OperationName;
    mafString m_Parameters;
    mafString m_Date;
    mafString m_AppStamp;
    mafString m_OperatorID;
    mafString m_IsNatural;
  };
  std::vector<Traceability> m_TraceabilityVector;

protected:
  void InternalStore(mafStorageElementBuilder& parent) override;
  void InternalRestore(const mafStorageElement& node) override;
};

#endif 

