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
template <class T, template<typename> typename Ptr, typename ArgPtr>
void mafTimeMap<T, Ptr, ArgPtr>::AppendAndSetItem(ArgPtr m)
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
template <class T, template<typename> typename Ptr, typename ArgPtr>
void mafTimeMap<T, Ptr, ArgPtr>::AppendItem(ArgPtr m)
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
template <class T, template<typename> typename Ptr, typename ArgPtr>
void mafTimeMap<T, Ptr, ArgPtr>::PrependItem(ArgPtr m)
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
template <class T, template<typename> typename Ptr, typename ArgPtr>
void mafTimeMap<T, Ptr, ArgPtr>::InsertItem(ArgPtr m)
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
template <class T, template<typename> typename Ptr, typename ArgPtr>
void mafTimeMap<T, Ptr, ArgPtr>::GetTimeBounds(mafTimeStamp tbounds[2])
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
template <class T, template<typename> typename Ptr, typename ArgPtr>
void mafTimeMap<T, Ptr, ArgPtr>::GetTimeStamps(mmuTimeVector &kframes) const
//-------------------------------------------------------------------------
{
  kframes.clear();

  for (typename mafTimeMap<T, Ptr, ArgPtr>::TimeMap::const_iterator it=m_TimeMap.begin();it!=m_TimeMap.end();it++)
  {
    kframes.push_back(it->first);
  }
}

template<class T>
auto CloneEntry(std::shared_ptr<T> v)
{
  return v->MakeClone();
}

template<class T>
auto CloneEntry(mafAutoPointer<T>& v)
{
  T *new_item=v->NewInstance();
  new_item->DeepCopy(v.get());
  return new_item;
}

//-------------------------------------------------------------------------
template <class T, template<typename> typename Ptr, typename ArgPtr>
void mafTimeMap<T, Ptr, ArgPtr>::DeepCopy(mafTimeMap *o)
//-------------------------------------------------------------------------
{
  RemoveAllItems();
  //m_TimeMap=o->m_TimeMap;
  for (auto& elem : *o)
  {
    AppendItem(CloneEntry(elem.second));
  }
  Modified();
}

template <class T>
bool Compare(std::shared_ptr<T> v1, std::shared_ptr<T> v2)
{
  return *v1 == *v2;
}

template <class T>
bool Compare(mafAutoPointer<T>& v1, mafAutoPointer<T>& v2)
{
  return v1->Equals(v2.get());
}

//-------------------------------------------------------------------------
template <class T, template<typename> typename Ptr, typename ArgPtr>
bool mafTimeMap<T, Ptr, ArgPtr>::Equals(mafTimeMap *o)
//-------------------------------------------------------------------------
{
  if (o==NULL)
    return false;

  if (GetNumberOfItems()!=o->GetNumberOfItems())
    return false;

  for (auto it=m_TimeMap.begin(),it2=o->m_TimeMap.begin();it!=m_TimeMap.end();it++,it2++)
  {
    if (!Compare(it->second, it2->second))
      return false;
  }

  return true;
}


//-------------------------------------------------------------------------
template <class T, template<typename> typename Ptr, typename ArgPtr>
void mafTimeMap<T, Ptr, ArgPtr>::RemoveAllItems()
//-------------------------------------------------------------------------
{
  m_TimeMap.clear();
  Modified();
}

//-------------------------------------------------------------------------
template <class T, template<typename> typename Ptr, typename ArgPtr>
typename mafTimeMap<T, Ptr, ArgPtr>::TimeMap::iterator mafTimeMap<T, Ptr, ArgPtr>::FindNearestItem(mafTimeStamp t)
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
template <class T, template<typename> typename Ptr, typename ArgPtr>
typename mafTimeMap<T, Ptr, ArgPtr>::TimeMap::iterator mafTimeMap<T, Ptr, ArgPtr>::FindItemBefore(mafTimeStamp t)
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
template <class T, template<typename> typename Ptr, typename ArgPtr>
void mafTimeMap<T, Ptr, ArgPtr>::Print(std::ostream& os, const int tabs) const
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

template<class T>
auto RetVal(std::shared_ptr<T> v)
{
  return v;
}

template<class T>
auto RetVal(mafAutoPointer<T>& v)
{
  return v.get();
}

//----------------------------------------------------------------------------
template <class T, template<typename> typename Ptr, typename ArgPtr>
ArgPtr mafTimeMap<T, Ptr, ArgPtr>::GetItem(mafTimeStamp t)
//----------------------------------------------------------------------------
{
  auto it=FindItem(t);
  return (it!=m_TimeMap.end())?RetVal(it->second):NULL;
}
//----------------------------------------------------------------------------
template <class T, template<typename> typename Ptr, typename ArgPtr>
ArgPtr mafTimeMap<T, Ptr, ArgPtr>::GetNearestItem(mafTimeStamp t)
//----------------------------------------------------------------------------
{
  auto it=FindNearestItem(t);
  return (it!=m_TimeMap.end())?RetVal(it->second):NULL;
}
//----------------------------------------------------------------------------
template <class T, template<typename> typename Ptr, typename ArgPtr>
ArgPtr mafTimeMap<T, Ptr, ArgPtr>::GetItemBefore(mafTimeStamp t)
//----------------------------------------------------------------------------
{
  auto it=FindItemBefore(t);
  return (it!=m_TimeMap.end())?RetVal(it->second):NULL;
}

#endif
