/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafTimeMapUX.txx,v $
  Language:  C++
  Date:      $Date: 2006-10-06 14:31:56 $
  Version:   $Revision: 1.1 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafTimeMap_txx
#define __mafTimeMap_txx

#include "mafTimeMap.h"
#include "mafIndent.h"
#include <math.h>
#include <assert.h>

//-----------------------------------------------------------------------
template <class T, template<typename> typename Ptr>
void mafTimeMap<T, Ptr>::AppendAndSetItem(T *m)
//-----------------------------------------------------------------------
{
  assert(m);
  if (this->GetNumberOfItems()>0)
  {
    // Get last item
	  assert(m_TimeMap.rbegin()->second.get());
    // append adding 1 to the last time 
    m->SetTimeStamp(m_TimeMap.rbegin()->first+1);
  }
  else
	{
    m->SetTimeStamp(0);
  }
  AppendItem(m);
}

//-----------------------------------------------------------------------
template <class T, template<typename> typename Ptr>
void mafTimeMap<T, Ptr>::AppendItem(T *m)
//-----------------------------------------------------------------------
{
	assert(m);
  if (!m_ItemTypeName.empty())
  {
    assert(m->IsA(m_ItemTypeName.GetCStr()));
    if (!m->IsA(m_ItemTypeName.GetCStr()))
    {
      mafErrorMacro("Unsupported Item type \""<<m->GetTypeName()<<"\", allowed type is \""<<m_ItemTypeName.GetCStr()<<"\": cannot Append item!");
      return;
    }
  }
	m_TimeMap.insert(m_TimeMap.end(),mmuTimePair(m->GetTimeStamp(),m));
	Modified();
}

//-----------------------------------------------------------------------
template <class T, template<typename> typename Ptr>
void mafTimeMap<T, Ptr>::PrependItem(T *m)
//-----------------------------------------------------------------------
{
  assert(m);
  if (!m_ItemTypeName.empty())
  {
    assert(m->IsA(m_ItemTypeName.GetCStr()));
    if (!m->IsA(m_ItemTypeName.GetCStr()))
    {
      mafErrorMacro("Unsupported Item type \""<<m->GetTypeName()<<"\", allowed type is \""<<m_ItemTypeName.GetCStr()<<"\": cannot Prepend item!");
      return;
    }
  }
  m_TimeMap.insert(m_TimeMap.begin(),mmuTimePair(m->GetTimeStamp(),m));
  Modified();
}
//-------------------------------------------------------------------------
template <class T, template<typename> typename Ptr>
void mafTimeMap<T, Ptr>::InsertItem(T *m)
//-------------------------------------------------------------------------
{
  assert(m);
  if (!m_ItemTypeName.empty())
  {
    assert(m->IsA(m_ItemTypeName.GetCStr()));
    if (!m->IsA(m_ItemTypeName.GetCStr()))
    {
      mafErrorMacro("Unsupported Item type \""<<m->GetTypeName()<<"\", allowed type is \""<<m_ItemTypeName.GetCStr()<<"\": cannot Insert item!");
      return;
    }
  }

  m_TimeMap[m->GetTimeStamp()]=m;	
  Modified();
}

//-------------------------------------------------------------------------
template <class T, template<typename> typename Ptr>
void mafTimeMap<T, Ptr>::GetTimeBounds(mafTimeStamp tbounds[2])
//-------------------------------------------------------------------------
{
  // this is a sorted array
  if (m_TimeMap.size()>0)
  {
	  tbounds[0]=m_TimeMap.begin()->first;
	  tbounds[1]=m_TimeMap.rbegin()->first;
  }
  else
  {
	  tbounds[0]=-1;
	  tbounds[1]=-1;
  }
}

//-------------------------------------------------------------------------
template <class T, template<typename> typename Ptr>
void mafTimeMap<T, Ptr>::GetTimeStamps(mmuTimeVector &kframes) const
//-------------------------------------------------------------------------
{
  kframes.clear();

  for (typename mafTimeMap<T, Ptr>::TimeMap::const_iterator it=m_TimeMap.begin();it!=m_TimeMap.end();it++)
  {
    kframes.push_back(it->first);
  }
}

//-------------------------------------------------------------------------
template <class T, template<typename> typename Ptr>
void mafTimeMap<T, Ptr>::DeepCopy(mafTimeMap *o)
//-------------------------------------------------------------------------
{
  RemoveAllItems();
  //m_TimeMap=o->m_TimeMap;
  for (auto& elem : *o)
  {
    T *m=elem.second.get();
    T *new_item=m->NewInstance();
    new_item->DeepCopy(m);
    AppendItem(new_item);
  }
  Modified();
}

//-------------------------------------------------------------------------
template <class T, template<typename> typename Ptr>
bool mafTimeMap<T, Ptr>::Equals(mafTimeMap *o)
//-------------------------------------------------------------------------
{
  if (o==NULL)
    return false;

  if (GetNumberOfItems()!=o->GetNumberOfItems())
    return false;

  typename mafTimeMap<T, Ptr>::TimeMap::iterator it;
  typename mafTimeMap<T, Ptr>::TimeMap::iterator it2;
  for (it=m_TimeMap.begin(),it2=o->m_TimeMap.begin();it!=m_TimeMap.end();it++,it2++)
  {
    T *m=it->second.get();
    T *m2=it2->second.get();

    if (!m->Equals(m2))
      return false;
  }

  return true;
}


//-------------------------------------------------------------------------
template <class T, template<typename> typename Ptr>
void mafTimeMap<T, Ptr>::RemoveAllItems()
//-------------------------------------------------------------------------
{
  m_TimeMap.clear();
  Modified();
}

//-------------------------------------------------------------------------
template <class T, template<typename> typename Ptr>
typename mafTimeMap<T, Ptr>::TimeMap::iterator mafTimeMap<T, Ptr>::FindNearestItem(mafTimeStamp t)
//-------------------------------------------------------------------------
{
  auto range=m_TimeMap.equal_range(t);
  if (range.first!=m_TimeMap.end())
  {
    if (range.second!=m_TimeMap.end())
    {
      if (fabs(range.first->first-t)>fabs(range.second->first-t))
         return range.second;
    }    
    return range.first;
  }
  else if (range.second!=m_TimeMap.end())
  {
    return range.second;
  }

  return --range.second;
}


//-------------------------------------------------------------------------
template <class T, template<typename> typename Ptr>
typename mafTimeMap<T, Ptr>::TimeMap::iterator mafTimeMap<T, Ptr>::FindItemBefore(mafTimeStamp t)
//-------------------------------------------------------------------------
{
  if (m_TimeMap.size()>0)
  {
    auto it=m_TimeMap.lower_bound(t); // find first item >= t
    if (it==m_TimeMap.end()||it!=m_TimeMap.begin()&&it->first>t) // if > t get the previous 
      --it;

    return it;
  }
  else
  {
    return m_TimeMap.end();
  }
  
}

//----------------------------------------------------------------------------
template <class T, template<typename> typename Ptr>
void mafTimeMap<T, Ptr>::Print(std::ostream& os, const int tabs) const
//----------------------------------------------------------------------------
{
  mafIndent indent(tabs);
  os << indent << "Number of Items:"<<GetNumberOfItems()<<"\n";
  mmuTimeVector tvector;
  os << indent << "Time Stamps: {";
  GetTimeStamps(tvector);
  for (unsigned int i=0;i<tvector.size();i++)
  {
    if (i!=0)
    os << ", ";
    os << tvector[i];
  }

  os << "}\n";
}

//----------------------------------------------------------------------------
template <class T, template<typename> typename Ptr>
T *mafTimeMap<T, Ptr>::GetItem(mafTimeStamp t)
//----------------------------------------------------------------------------
{
  auto it=FindItem(t);
  return (it!=m_TimeMap.end())?it->second.get():NULL;
}
//----------------------------------------------------------------------------
template <class T, template<typename> typename Ptr>
T *mafTimeMap<T, Ptr>::GetNearestItem(mafTimeStamp t)
//----------------------------------------------------------------------------
{
  auto it=FindNearestItem(t);
  return (it!=m_TimeMap.end())?it->second.get():NULL;
}
//----------------------------------------------------------------------------
template <class T, template<typename> typename Ptr>
T *mafTimeMap<T, Ptr>::GetItemBefore(mafTimeStamp t)
//----------------------------------------------------------------------------
{
  auto it=FindItemBefore(t);
  return (it!=m_TimeMap.end())?it->second.get():NULL;
}

#endif
