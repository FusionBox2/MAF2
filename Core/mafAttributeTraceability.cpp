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


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mafAttributeTraceability.h"
#include "mafStorageElement.h"
#include "mafIndent.h"

#include <ostream>

mafCxxTypeMacro(mafAttributeTraceability);

//----------------------------------------------------------------------------
mafAttributeTraceability::mafAttributeTraceability()
//----------------------------------------------------------------------------
{  
  m_TraceabilityVector.clear();
}
//----------------------------------------------------------------------------
mafAttributeTraceability::~mafAttributeTraceability()
//----------------------------------------------------------------------------
{
  m_TraceabilityVector.clear();
}

//----------------------------------------------------------------------------
void mafAttributeTraceability::AddTraceabilityEvent(const mafString& trialEvent, const mafString& operationName, const mafString& parmaters, const mafString& date, const mafString& appStamp, const mafString& operatorID, const mafString& isNatural)
//----------------------------------------------------------------------------
{  
  m_Traceability traceability;
  traceability.m_TrialEvent = trialEvent;
  traceability.m_OperationName = operationName;
  traceability.m_Parameters = parmaters;
  traceability.m_Date = date;
  traceability.m_AppStamp = appStamp;
  traceability.m_OperatorID = operatorID;
  traceability.m_IsNatural = isNatural;
  m_TraceabilityVector.push_back(traceability);
}

//----------------------------------------------------------------------------
void mafAttributeTraceability::RemoveTraceabilityEvent()
//----------------------------------------------------------------------------
{  
  if (m_TraceabilityVector.size() != 0)
  {
    m_TraceabilityVector.pop_back();
  }
}

//-------------------------------------------------------------------------
void mafAttributeTraceability::DeepCopy(const mafAttribute *a)
//-------------------------------------------------------------------------
{
  Superclass::DeepCopy(a);
  int vecSize = ((mafAttributeTraceability *)a)->m_TraceabilityVector.size();
  for (int i = 0; i < vecSize; i++)
  {
    m_Traceability traceability;
    traceability.m_TrialEvent = ((mafAttributeTraceability *)a)->m_TraceabilityVector[i].m_TrialEvent;
    traceability.m_OperationName = ((mafAttributeTraceability *)a)->m_TraceabilityVector[i].m_OperationName;
    traceability.m_Parameters = ((mafAttributeTraceability *)a)->m_TraceabilityVector[i].m_Parameters;
    traceability.m_Date = ((mafAttributeTraceability *)a)->m_TraceabilityVector[i].m_Date;
    traceability.m_AppStamp = ((mafAttributeTraceability *)a)->m_TraceabilityVector[i].m_AppStamp;
    traceability.m_OperatorID = ((mafAttributeTraceability *)a)->m_TraceabilityVector[i].m_OperatorID;
    if (((mafAttributeTraceability *)a)->m_TraceabilityVector[i].m_TrialEvent == _R("Create"))
    {
      traceability.m_IsNatural = ((mafAttributeTraceability *)a)->m_TraceabilityVector[i].m_IsNatural;
    }
    m_TraceabilityVector.push_back(traceability);
  }
}

//-------------------------------------------------------------------------
bool mafAttributeTraceability::Equals(const mafAttribute *a) const
//-------------------------------------------------------------------------
{
  if (Superclass::Equals(a))
  {
    int vecSize = ((mafAttributeTraceability *)a)->m_TraceabilityVector.size();
    for (int i = 0; i < vecSize; i++)
    {
      if(m_TraceabilityVector[i].m_TrialEvent != ((mafAttributeTraceability *)a)->m_TraceabilityVector[i].m_TrialEvent ||
        m_TraceabilityVector[i].m_OperationName != ((mafAttributeTraceability *)a)->m_TraceabilityVector[i].m_OperationName ||
        m_TraceabilityVector[i].m_Parameters != ((mafAttributeTraceability *)a)->m_TraceabilityVector[i].m_Parameters ||
        m_TraceabilityVector[i].m_Date != ((mafAttributeTraceability *)a)->m_TraceabilityVector[i].m_Date ||
        m_TraceabilityVector[i].m_AppStamp != ((mafAttributeTraceability *)a)->m_TraceabilityVector[i].m_AppStamp ||
        m_TraceabilityVector[i].m_OperatorID != ((mafAttributeTraceability *)a)->m_TraceabilityVector[i].m_OperatorID)
        return false;

      if (((mafAttributeTraceability *)a)->m_TraceabilityVector[i].m_TrialEvent == _R("Create"))
      {
        if (m_TraceabilityVector[i].m_IsNatural != ((mafAttributeTraceability *)a)->m_TraceabilityVector[i].m_IsNatural)
          return false;
      }
    }
  }
  return true;
}

//-------------------------------------------------------------------------
int mafAttributeTraceability::InternalStore(mafStorageElement *parent)
//-------------------------------------------------------------------------
{
  if (Superclass::InternalStore(parent)==MAF_OK)
  {
    for (int i = 0; i < m_TraceabilityVector.size(); i++)
    {
      parent->StoreText(_R("TrialEvent"), m_TraceabilityVector[i].m_TrialEvent);
      parent->StoreText(_R("Operation"), m_TraceabilityVector[i].m_OperationName);
      parent->StoreText(_R("Parameters"), m_TraceabilityVector[i].m_Parameters);
      parent->StoreText(_R("Date"), m_TraceabilityVector[i].m_Date);
      parent->StoreText(_R("Application"), m_TraceabilityVector[i].m_AppStamp);
      parent->StoreText(_R("OperatorID"),  m_TraceabilityVector[i].m_OperatorID);
      if (m_TraceabilityVector[i].m_TrialEvent == _R("Create"))
      {
        parent->StoreText(_R("IsNatural"), m_TraceabilityVector[i].m_IsNatural);
      }
    }  
  }
  return MAF_OK;
}

//-------------------------------------------------------------------------
int mafAttributeTraceability::InternalRestore(mafStorageElement *node)
//-------------------------------------------------------------------------
{
  if (Superclass::InternalRestore(node) == MAF_OK)
  {
    m_Traceability traceability;

    std::vector<mafStorageElement*> listTrialEvent;
    node->GetNestedElementsByName(_R("TrialEvent"), listTrialEvent);

    std::vector<mafStorageElement*> listOperation;
    node->GetNestedElementsByName(_R("Operation"), listOperation);

    std::vector<mafStorageElement*> listParameters;
    node->GetNestedElementsByName(_R("Parameters"), listParameters);

    std::vector<mafStorageElement*> listDate;
    node->GetNestedElementsByName(_R("Date"), listDate);

    std::vector<mafStorageElement*> listApplication;
    node->GetNestedElementsByName(_R("Application"), listApplication);

    std::vector<mafStorageElement*> listOperatorID;
    node->GetNestedElementsByName(_R("OperatorID"), listOperatorID);

    std::vector<mafStorageElement*> listIsNatural;
    node->GetNestedElementsByName(_R("IsNatural"), listIsNatural);

    try
    {
      int iCreateIdx = 0;   //BES: 27.11.2008 - BUG FIX
      for (int i  = 0; i < listTrialEvent.size(); i++)
      {
        m_Traceability traceability;

        listTrialEvent[i]->RestoreText(traceability.m_TrialEvent);
        listOperation[i]->RestoreText(traceability.m_OperationName);
        if (listParameters.size() > i)
          listParameters[i]->RestoreText(traceability.m_Parameters);
        listDate[i]->RestoreText(traceability.m_Date);
        listApplication[i]->RestoreText(traceability.m_AppStamp);
        listOperatorID[i]->RestoreText(traceability.m_OperatorID);
        if (traceability.m_TrialEvent == _R("Create"))
        {
          //BES: 27.11.2008 - BUG FIX - listIsNatural can contain less items than listTrialEvent
          listIsNatural[iCreateIdx++]->RestoreText(traceability.m_IsNatural);  
        }
        m_TraceabilityVector.push_back(traceability);
      }
    }
    catch (...)
    {
      mafLogMessage(_M("Problems restoring audit trials attribute"));
    }
    
   
    return MAF_OK;
  }
  return MAF_ERROR;
}

//-------------------------------------------------------------------------
mafString mafAttributeTraceability::GetLastTrialEvent()
//-------------------------------------------------------------------------
{
  mafString trialType;
  if (m_TraceabilityVector.size() != 0)
  {
    trialType = m_TraceabilityVector[m_TraceabilityVector.size()-1].m_TrialEvent;
  }
  return trialType;
}
