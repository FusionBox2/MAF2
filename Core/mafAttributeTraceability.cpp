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
int mafAttributeTraceability::InternalStore(mafStorageElementBuilder& parent)
//-------------------------------------------------------------------------
{
  if (Superclass::InternalStore(parent)==MAF_OK)
  {
    for (int i = 0; i < m_TraceabilityVector.size(); i++)
    {
      parent[_R("TrialEvent")].StoreText( m_TraceabilityVector[i].m_TrialEvent);
      parent[_R("Operation")].StoreText( m_TraceabilityVector[i].m_OperationName);
      parent[_R("Parameters")].StoreText( m_TraceabilityVector[i].m_Parameters);
      parent[_R("Date")].StoreText( m_TraceabilityVector[i].m_Date);
      parent[_R("Application")].StoreText( m_TraceabilityVector[i].m_AppStamp);
      parent[_R("OperatorID")].StoreText(  m_TraceabilityVector[i].m_OperatorID);
      if (m_TraceabilityVector[i].m_TrialEvent == _R("Create"))
      {
        parent[_R("IsNatural")].StoreText( m_TraceabilityVector[i].m_IsNatural);
      }
    }  
  }
  return MAF_OK;
}

//-------------------------------------------------------------------------
int mafAttributeTraceability::InternalRestore(const mafStorageElement& node)
//-------------------------------------------------------------------------
{
  if (Superclass::InternalRestore(node) == MAF_OK)
  {
    m_Traceability traceability;

    std::vector<mafStorageElement*> listTrialEventElems = node.GetElementsByName(_R("TrialEvent"));

	std::vector<mafString> listTrialEvent(listTrialEventElems.size());
    std::vector<mafString> listOperation(listTrialEventElems.size());
	std::vector<mafString> listParameters(listTrialEventElems.size());
	std::vector<mafString> listDate(listTrialEventElems.size());
	std::vector<mafString> listApplication(listTrialEventElems.size());
	std::vector<mafString> listOperatorID(listTrialEventElems.size());
	std::vector<mafString> listIsNatural(listTrialEventElems.size());
	node.RestoreVectorN(listTrialEvent, _R("TrialEvent"));
    node.RestoreVectorN(listOperation, _R("Operation"));
    node.RestoreVectorN(listParameters, _R("Parameters"));
    node.RestoreVectorN(listDate, _R("Date"));
    node.RestoreVectorN(listApplication, _R("Application"));
    node.RestoreVectorN(listOperatorID, _R("OperatorID"));
    node.RestoreVectorN(listIsNatural, _R("IsNatural"));

    try
    {
      int iCreateIdx = 0;   //BES: 27.11.2008 - BUG FIX
      for (int i  = 0; i < listTrialEvent.size(); i++)
      {
        m_Traceability traceability;

        traceability.m_TrialEvent = listTrialEvent[i];
        traceability.m_OperationName = listOperation[i];
        if (listParameters.size() > i)
            traceability.m_Parameters = listParameters[i];
        traceability.m_Date = listDate[i];
        traceability.m_AppStamp = listApplication[i];
        traceability.m_OperatorID = listOperatorID[i];
        if (traceability.m_TrialEvent == _R("Create"))
        {
          //BES: 27.11.2008 - BUG FIX - listIsNatural can contain less items than listTrialEvent
            traceability.m_IsNatural = listIsNatural[iCreateIdx++];
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
