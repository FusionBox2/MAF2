#pragma once

#include "ftk/Base/Object.h"
#include "mafTimeStamped.h"
#include "ftk/Base/RegisteringPointer.h"
#include "ftk/Base/MTime.h"
#include "ftk/Base/String.h"
#include <map>
#include <vector>

typedef std::vector<mafTimeStamp> mmuTimeVector;

template <class T, template<typename> typename Ptr = mafAutoPointer, typename ArgPtr = T*>
class mafTimeMap : public mafObject, public mafTimeStamped
{
public:
  using TimeMap = std::map<mafTimeStamp, Ptr<T> >;
  using mmuTimePair = std::pair<mafTimeStamp, Ptr<T> >;

  mafTimeMap() = default;

	mafTimeMap(const mafTimeMap<T>&) = delete;
  mafTimeMap& operator=(const mafTimeMap<T>&) = delete;

	mafTimeMap(mafTimeMap<T>&&) = delete;
  mafTimeMap& operator=(mafTimeMap<T>&&) = delete;

	~mafTimeMap() override = default;

  //mafAbstractTypeMacro(mafTimeMap<T>,mafObject);

  /** set the TypeName of the kind of item accepted by this container */
  void SetItemTypeName(const char *tname) {m_ItemTypeName=_R(tname);}
  const char *GetItemTypeName() {return m_ItemTypeName.GetCStr();}
  
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
  auto FindItem(ArgPtr m){assert(m); return m_TimeMap.find(m->GetTimeStamp());}

  /**
    Set the item for a specified time. If no item with the same time exist
    the item is inserted in the vector. If an item with the same time exist, it's simply
    substituted with the new one. The item is always references and not copied.
    This function also automatically call the UpdateData() member function.*/
  virtual void InsertItem(ArgPtr m);
  
  /** Remove an item given its iterator */
  virtual void RemoveItem(typename TimeMap::iterator it) {/*m_TimeMap.erase(it); Modified();*/}


  /** Remove all the items*/
  virtual void RemoveAllItems();

  /** Return the list of timestamp of the key matrixes in the given vector*/
  void GetTimeStamps(mmuTimeVector &kframes) const;

  /** Return the number of ITEMS stored in this object*/
  int GetNumberOfItems() const {return m_TimeMap.size();};
  
  /** Return the time bounds for this vector, i.e. minimum and maximum time stamps */
  void GetTimeBounds(mafTimeStamp tbounds[2]);

  /** Copy data from another array*/
  void DeepCopy(mafTimeMap *vitem);

  /**
    Compare two different arrays for equality. They are considered
    equivalent if their key matrixes are equivalent.*/
  bool Equals(mafTimeMap *vmat);

  /** Find the item with the timestamp nearest to t*/
  typename TimeMap::iterator FindNearestItem(mafTimeStamp t);

  /** Find the item with timestamp <=t*/
  typename TimeMap::iterator FindItemBefore(mafTimeStamp t);

  /**
    Find the item with the timestamp==t. Returns the item index, and 
    set "item" to its pointer. Return -1 and NULL if not found.
    not*/
  auto FindItem(mafTimeStamp t){ return m_TimeMap.find(t); }

  /** find and return item corresponding to timestamp t. return NULL if not found. */
  ArgPtr GetItem(mafTimeStamp t);

  /**
    Return the pointer to the item with timestamp nearest the given one. NULL 
    is returned if not found.*/
  ArgPtr GetNearestItem(mafTimeStamp t);

  /**
    Return the pointer to the item with timestamp <= the given one. NULL 
    is returned if not found.*/
  ArgPtr GetItemBefore(mafTimeStamp t);

  void Print(std::ostream& os, const int tabs=0) const override;

  auto begin() {return m_TimeMap.begin();}
  auto end() {return m_TimeMap.end();}
  auto begin() const { return m_TimeMap.begin(); }
  auto end() const { return m_TimeMap.end(); }
  auto cbegin() const { return m_TimeMap.cbegin(); }
  auto cend() const { return m_TimeMap.cend(); }

protected:
  TimeMap         m_TimeMap;        ///< the set storing the datasets
  mafString       m_ItemTypeName;   ///< the name of the item type accepted by this container
};
