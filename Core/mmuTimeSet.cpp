/*=========================================================================

 Program: MAF2
 Module: mmuTimeSet
 Authors: Marco Petrone
 
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


#include "mmuTimeSet.h"
#include <math.h>
//------------------------------------------------------------------------------
//mafCxxTypeMacro(mmuTimeSet)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
mmuTimeSet::mmuTimeSet()
//------------------------------------------------------------------------------
{
}

//------------------------------------------------------------------------------
mmuTimeSet::~mmuTimeSet()
//------------------------------------------------------------------------------
{
}

//------------------------------------------------------------------------------
mmuTimeSet::mmuTimeSet(const mmuTimeSet& c)
//------------------------------------------------------------------------------
{ 
  m_TSet=c.m_TSet;
}

//------------------------------------------------------------------------------
mmuTimeSet &mmuTimeSet::operator=(const mmuTimeSet &o)
//------------------------------------------------------------------------------
{
  m_TSet=o.m_TSet;
  return *this;
}

//------------------------------------------------------------------------------
bool mmuTimeSet::operator==(const mmuTimeSet &o)
//------------------------------------------------------------------------------
{
  if (GetNumberOfTimeStamps()!=o.GetNumberOfTimeStamps())
    return false;

  Iterator it;
  TSet::const_iterator it2;
  for (it=m_TSet.begin(),it2=o.m_TSet.begin();it!=m_TSet.end();it++,it2++)
  {
    if (*it!=*it2)
      return false;
  }

  return true;
}
//------------------------------------------------------------------------------
void mmuTimeSet::Insert(mafTimeStamp t)
//------------------------------------------------------------------------------
{
  m_TSet.insert(t);
}
//------------------------------------------------------------------------------
void mmuTimeSet::Append(mafTimeStamp t)
//------------------------------------------------------------------------------
{
  m_TSet.insert(m_TSet.end(),t);
}
//------------------------------------------------------------------------------
void mmuTimeSet::Prepend(mafTimeStamp t)
//------------------------------------------------------------------------------
{
  m_TSet.insert(m_TSet.begin(),t);
}
//------------------------------------------------------------------------------
void mmuTimeSet::Remove(Iterator ts)
//------------------------------------------------------------------------------
{
  m_TSet.erase(ts);
}

//------------------------------------------------------------------------------
mafID mmuTimeSet::FindTimeStampIndex(mafTimeStamp t)
//------------------------------------------------------------------------------
{
  Iterator it=FindTimeStamp(t);
  return it!=m_TSet.end()?std::distance(m_TSet.begin(),it):-1;
}
//------------------------------------------------------------------------------
mafTimeStamp mmuTimeSet::GetByIndex(mafID idx)
//------------------------------------------------------------------------------
{
  Iterator it=m_TSet.begin();
  std::advance(it,idx);
  return (it!=m_TSet.end()?*it:-1);
}
//------------------------------------------------------------------------------
mafTimeStamp mmuTimeSet::GetNearestTimeStamp(mafTimeStamp t)
//------------------------------------------------------------------------------
{
  Iterator it=FindNearestTimeStamp(t);
  return it!=m_TSet.end()?*it:-1;
}
//------------------------------------------------------------------------------
mafTimeStamp mmuTimeSet::GetTimeStampBefore(mafTimeStamp t)
//------------------------------------------------------------------------------
{
  Iterator it=FindTimeStampBefore(t);
  return it!=m_TSet.end()?*it:-1;
}
//------------------------------------------------------------------------------
mmuTimeSet::Iterator mmuTimeSet::FindTimeStamp(mafTimeStamp t)
//------------------------------------------------------------------------------
{
  return m_TSet.find(t);
}
//------------------------------------------------------------------------------
mmuTimeSet::Iterator mmuTimeSet::FindNearestTimeStamp(mafTimeStamp t)
//------------------------------------------------------------------------------
{
  std::pair<Iterator,Iterator> range=m_TSet.equal_range(t);
  if (range.first!=m_TSet.end())
  {
    if (range.second!=m_TSet.end())
    {
      return fabs(*(range.first)-t)>fabs(*(range.second)-t)?range.second:range.first;
    }
    else
    {
      return range.first;
    }
  }
  else if (range.second!=m_TSet.end())
  {
    return range.second;
  }

  return --range.second;
}
//------------------------------------------------------------------------------
mmuTimeSet::Iterator mmuTimeSet::FindTimeStampBefore(mafTimeStamp t)
//------------------------------------------------------------------------------
{
  Iterator it=m_TSet.lower_bound(t); // find item < t
 
  if (it==m_TSet.end()||it!=m_TSet.begin()&&*it>t)
    --it;
  return it;
}

//-------------------------------------------------------------------------
void mmuTimeSet::Merge(const mmuTimeSet &v)
//-------------------------------------------------------------------------
{
  m_TSet.insert(v.m_TSet.begin(), v.m_TSet.end());
}

//-------------------------------------------------------------------------
int mmuTimeSet::GetNumberOfTimeStamps() const
//-------------------------------------------------------------------------
{
  return m_TSet.size();
}

//-------------------------------------------------------------------------
std::vector<mafTimeStamp> mmuTimeSet::Merge(const std::vector<mafTimeStamp> &v1,const std::vector<mafTimeStamp> &v2)
//-------------------------------------------------------------------------
{
  std::vector<mafTimeStamp> outv;
  std::set_union(v1.begin(), v1.end(), v2.begin(), v2.end(), std::back_inserter(outv));
  return outv;
}

//-------------------------------------------------------------------------
void mmuTimeSet::Merge(const std::vector<mafTimeStamp> &v)
//-------------------------------------------------------------------------
{
  m_TSet.insert(v.begin(), v.end());
}