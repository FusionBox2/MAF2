#pragma once

#include "ftkConfigure.h"

#include "ftk/Base/Object.h"
#include "mafTimeStamped.h"
#include "mafIndent.h"
#include "ftk/Base/MTime.h"
#include "ftk/Base/mfString.h"

#include <map>
#include <vector>

#include <math.h>
#include <assert.h>

BEGIN_FTK_NAMESPACE

using mmuTimeVector = std::vector<mafTimeStamp>;

template <class T, template<typename> typename Ptr = std::shared_ptr, typename ArgPtr = std::shared_ptr<T> >
class mafTimeMap : public mafTimeStamped
{
public:
	using TimeMap = std::map<mafTimeStamp, Ptr<T> >;
	using mmuTimePair =  std::pair<mafTimeStamp, Ptr<T> >;

	mafTimeMap() = default;

	mafTimeMap(const mafTimeMap<T>&) = delete;
	mafTimeMap& operator=(const mafTimeMap<T>&) = delete;

	virtual ~mafTimeMap() = default;

	/** set the TypeName of the kind of item accepted by this container */
	void SetItemTypeName(const char* tname) { m_ItemTypeName = _R(tname); }

	const char* GetItemTypeName() const { return m_ItemTypeName.GetCStr(); }

	/**
	  Insert an item to the vector trying to append it, anyway the array
	  is kept sorted. */
	virtual void AppendItem(ArgPtr m);

	/** append item setting its timestamp to the highest one + 1 */
	virtual void AppendAndSetItem(ArgPtr m);

	/**
	 Insert an item to the vector trying to prepend it, anyway the array is kept sorted.
	 Item's timestamp must be >=0 */
	virtual void PrependItem(ArgPtr m);

	/** Find an item index given its pointer*/
	auto FindItem(ArgPtr m) { assert(m); return m_TimeMap.find(m->GetTimeStamp()); }

	/**
	  Set the item for a specified time. If no item with the same time exist
	  the item is inserted in the vector. If an item with the same time exist, it's simply
	  substituted with the new one. The item is always references and not copied.
	  This function also automatically call the UpdateData() member function.*/
	virtual void InsertItem(ArgPtr m);

	/** Remove an item given its iterator */
	virtual void RemoveItem(typename TimeMap::iterator it) {/*m_TimeMap.erase(it); Modified();*/ }


	/** Remove all the items*/
	virtual void RemoveAllItems();

	/** Return the list of timestamp of the key matrixes in the given vector*/
	void GetTimeStamps(mmuTimeVector& kframes) const;

	/** Return the number of ITEMS stored in this object*/
	int GetNumberOfItems() const { return m_TimeMap.size(); };

	/** Set/Get the Current time for this object*/
	//mafTimeStamp GetTimeStamp() {return m_CurrentTime;}
	//void SetTimeStamp(mafTimeStamp t);

	/** Return the time bounds for this vector, i.e. minimum and maximum time stamps */
	void GetTimeBounds(mafTimeStamp tbounds[2]);

	/** Copy data from another array*/
	void DeepCopy(mafTimeMap* vitem);

	/**
	  Compare two different arrays for equality. They are considered
	  equivalent if their key matrixes are equivalent.*/
	bool Equals(mafTimeMap* vmat);

	/** Find the item with the timestamp nearest to t*/
	typename TimeMap::iterator FindNearestItem(mafTimeStamp t);

	/** Find the item with timestamp <=t*/
	typename TimeMap::iterator FindItemBefore(mafTimeStamp t);

	/**
	  Find the item with the timestamp==t. Returns the item index, and
	  set "item" to its pointer. Return -1 and NULL if not found.
	  not*/
	auto FindItem(mafTimeStamp t) { return m_TimeMap.find(t); }

	/** find and return item corresponding to timestamp t. return NULL if not found. */
	ArgPtr GetItem(mafTimeStamp t);

	/**
	  Return the pointer to the item with timestamp nearest the given one. NULL
	  is returned if not found.*/
	ArgPtr GetNearestItem(mafTimeStamp t);

	/**
	  Return the pointer to the item with timestamp nearest the given one. NULL
	  is returned if not found.*/
	ArgPtr GetItemBefore(mafTimeStamp t);

	void Print(std::ostream& os, const int tabs = 0) const;

	auto begin() { return m_TimeMap.begin(); }
	auto end() { return m_TimeMap.end(); }
	auto begin() const { return m_TimeMap.begin(); }
	auto end() const { return m_TimeMap.end(); }
	auto cbegin() const { return m_TimeMap.cbegin(); }
	auto cend() const { return m_TimeMap.cend(); }

protected:
	TimeMap         m_TimeMap;        ///< the set storing the datasets
	mafString       m_ItemTypeName;   ///< the name of the item type accepted by this container
};

//-----------------------------------------------------------------------
template <class T, template<typename> typename Ptr, typename ArgPtr>
void mafTimeMap<T, Ptr, ArgPtr>::AppendAndSetItem(ArgPtr m)
//-----------------------------------------------------------------------
{
	assert(m);
	if (this->GetNumberOfItems() > 0)
	{
		// Get last item
		assert(m_TimeMap.rbegin()->second.get());
		// append adding 1 to the last time 
		m->SetTimeStamp(m_TimeMap.rbegin()->first + 1);
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
			mafErrorMacro("Unsupported Item type \"" << m->GetTypeName() << "\", allowed type is \"" << m_ItemTypeName.GetCStr() << "\": cannot Append item!");
			return;
		}
	}
	m_TimeMap.insert(m_TimeMap.end(), mmuTimePair(m->GetTimeStamp(), m));
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
			mafErrorMacro("Unsupported Item type \"" << m->GetTypeName() << "\", allowed type is \"" << m_ItemTypeName.GetCStr() << "\": cannot Prepend item!");
			return;
		}
	}
	m_TimeMap.insert(m_TimeMap.begin(), mmuTimePair(m->GetTimeStamp(), m));
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
			mafErrorMacro("Unsupported Item type \"" << m->GetTypeName() << "\", allowed type is \"" << m_ItemTypeName.GetCStr() << "\": cannot Insert item!");
			return;
		}
	}

	m_TimeMap[m->GetTimeStamp()] = m;
	Modified();
}

//-------------------------------------------------------------------------
template <class T, template<typename> typename Ptr, typename ArgPtr>
void mafTimeMap<T, Ptr, ArgPtr>::GetTimeBounds(mafTimeStamp tbounds[2])
//-------------------------------------------------------------------------
{
	// this is a sorted array
	if (m_TimeMap.size() > 0)
	{
		tbounds[0] = m_TimeMap.begin()->first;
		tbounds[1] = m_TimeMap.rbegin()->first;
	}
	else
	{
		tbounds[0] = -1;
		tbounds[1] = -1;
	}
}

//-------------------------------------------------------------------------
template <class T, template<typename> typename Ptr, typename ArgPtr>
void mafTimeMap<T, Ptr, ArgPtr>::GetTimeStamps(mmuTimeVector& kframes) const
//-------------------------------------------------------------------------
{
	kframes.clear();

	for (typename mafTimeMap<T, Ptr, ArgPtr>::TimeMap::const_iterator it = m_TimeMap.begin(); it != m_TimeMap.end(); it++)
	{
		kframes.push_back(it->first);
	}
}

template<class T>
auto CloneEntry(std::shared_ptr<T> v)
{
	return v->MakeClone();
}

//-------------------------------------------------------------------------
template <class T, template<typename> typename Ptr, typename ArgPtr>
void mafTimeMap<T, Ptr, ArgPtr>::DeepCopy(mafTimeMap* o)
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

//-------------------------------------------------------------------------
template <class T, template<typename> typename Ptr, typename ArgPtr>
bool mafTimeMap<T, Ptr, ArgPtr>::Equals(mafTimeMap* o)
//-------------------------------------------------------------------------
{
	if (o == NULL)
		return false;

	if (GetNumberOfItems() != o->GetNumberOfItems())
		return false;

	for (auto it = m_TimeMap.begin(), it2 = o->m_TimeMap.begin(); it != m_TimeMap.end(); it++, it2++)
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
	auto range = m_TimeMap.equal_range(t);
	if (range.first != m_TimeMap.end())
	{
		if (range.second != m_TimeMap.end())
		{
			if (fabs(range.first->first - t) > fabs(range.second->first - t))
				return range.second;
		}
		return range.first;
	}
	else if (range.second != m_TimeMap.end())
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
	if (m_TimeMap.size() > 0)
	{
		auto it = m_TimeMap.lower_bound(t); // find first item >= t
		if (it == m_TimeMap.end() || it != m_TimeMap.begin() && it->first > t) // if > t get the previous 
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
	os << indent << "Number of Items:" << GetNumberOfItems() << "\n";
	mmuTimeVector tvector;
	os << indent << "Time Stamps: {";
	GetTimeStamps(tvector);
	for (unsigned int i = 0; i < tvector.size(); i++)
	{
		if (i != 0)
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

//----------------------------------------------------------------------------
template <class T, template<typename> typename Ptr, typename ArgPtr>
ArgPtr mafTimeMap<T, Ptr, ArgPtr>::GetItem(mafTimeStamp t)
//----------------------------------------------------------------------------
{
	auto it = FindItem(t);
	return (it != m_TimeMap.end()) ? RetVal(it->second) : nullptr;
}
//----------------------------------------------------------------------------
template <class T, template<typename> typename Ptr, typename ArgPtr>
ArgPtr mafTimeMap<T, Ptr, ArgPtr>::GetNearestItem(mafTimeStamp t)
//----------------------------------------------------------------------------
{
	auto it = FindNearestItem(t);
	return (it != m_TimeMap.end()) ? RetVal(it->second) : nullptr;
}
//----------------------------------------------------------------------------
template <class T, template<typename> typename Ptr, typename ArgPtr>
ArgPtr mafTimeMap<T, Ptr, ArgPtr>::GetItemBefore(mafTimeStamp t)
//----------------------------------------------------------------------------
{
	auto it = FindItemBefore(t);
	return (it != m_TimeMap.end()) ? RetVal(it->second) : nullptr;
}

END_FTK_NAMESPACE
