/*=========================================================================

 Program: MAF2
 Module: mafGUIDictionaryWidget
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafGUIDictionaryWidget_H__
#define __mafGUIDictionaryWidget_H__

// mafGUIDictionaryWidget :
/**
- mafGUIDictionaryWidget is the class that treats dictionaries.
*/
#include "mafBaseEventHandler.h"
#include "mafEventSender.h"
#include "mafDefines.h"
#include "mafTo.h"
//----------------------------------------------------------------------------
// Forward Refs:
//----------------------------------------------------------------------------
class mafVME;
class mafGUIListCtrl;
class mafEvent;
class mafStorageElement;

/**
  class name : mafGUIDictionaryWidget
  Widget that handle a dictionary object. 
*/
class MAF_EXPORT mafGUIDictionaryWidget : public mafBaseEventHandler, public mafEventSender
{

public:
  /** constructor*/
            mafGUIDictionaryWidget(wxWindow *parent, int id);
  /** destructor */
           ~mafGUIDictionaryWidget();
  /** Answer to the messages coming from interface. */
  void      OnEvent(mafEventBase *event);

  /** Set the reference cloud. */
  void SetCloud(mafVME *vme);
  
	/** Load dictionary opening file dialog. */
  void LoadDictionary();
  
	/** Load dictionary from file. */
  void LoadDictionary(const mafString& file);
  
	/** Return dictionary GUI. */
  wxWindow *GetWidget() {return (wxWindow *)m_List;}

  /** Return file name dictionary */
  const mafString& GetDictionaryFileName(){return m_File;}

protected:
	/** Set the icon of the item to red or gray according to valid. */
  void ValidateItem(const mafString& item, bool valid = true);
  
	/** Set the icon of all the items present into the cloud to red or gray according to valid. */
  void ValidateAllItem(bool valid = true);

	mafString    m_File;
  int          m_NumItem;
	mafString    **m_Items;
  mafGUIListCtrl *m_List;
  mafVME      *m_Vme;
};

/**
 class name: mafStorableDictionary
  Utility object that supply the  InternalStore  and InternalRestore method for serialization.
*/
class MAF_EXPORT mafStorableDictionary: public mafObject
{
public:
  /** RTTI macro*/
  mafTypeMacro(mafStorableDictionary,mafObject);
  /** constructor */
  mafStorableDictionary();
  /** destructor */
  ~mafStorableDictionary();
  void Restore(const mafStorageElement& element) { InternalRestore(element); }
  virtual void InternalRestore(const mafStorageElement& node);
  std::vector<mafString> m_StrVector;
};
#endif
