/*=========================================================================

 Program: MAF2
 Module: mafGUIListCtrlBitmap
 Authors: Paolo Quadrani
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafGUIListCtrlBitmap_H__
#define __mafGUIListCtrlBitmap_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include <wx/laywin.h>
#include <wx/image.h>
#include <wx/imaglist.h>
#include <wx/listctrl.h>
#include <wx/hash.h>
#include "mafEvent.h"
#include "mafGUINamedPanel.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mafObserver;

//----------------------------------------------------------------------------
// Constants :
//----------------------------------------------------------------------------
/// possible values for icon
enum ITEM_ICONS
{
   ITEM_ERROR  = -1, //returned by GetItemIcon(id) if 'id' doesn't exist
   ITEM_YELLOW = 0,
   ITEM_GRAY,
   ITEM_RED,
   ITEM_BLUE
};
//----------------------------------------------------------------------------
// mafGUIListCtrlBitmap :
//----------------------------------------------------------------------------
/** mafGUIListCtrlBitmap allows a simplified use of a wxWindows ListCtrl widget. */
class mafGUIListCtrlBitmap: public mafGUINamedPanel
{
public:
                 mafGUIListCtrlBitmap (wxWindow* parent, wxWindowID id=-1, bool CloseButton = false, bool HideTitle = false); 
  virtual       ~mafGUIListCtrlBitmap();
  
	void SetListener(mafObserver *listener)   {m_Listener = listener;}; 

  /** Clear the control list. */
  void Reset();

  /** Add one item to the list. */
  bool AddItem(long item_id, wxString label, wxBitmap *bmp = NULL);

  /** Remove the item from the list. */
  bool DeleteItem(long item_id);

  /** Set the item's label. */
  bool SetItemLabel(long item_id, wxString label);

  /** Get the item's label. */
  wxString GetItemLabel  (long item_id);

  /** Set the item's icon, return true on success. */
  bool SetItemIcon(long item_id, wxBitmap *bmp);

  /** Get the item's icon. */
  ITEM_ICONS GetItemIcon(long item_id);

  /** Select the item 'id'. */
  bool SelectItem(long item_id);

  /** Set the label for the list's column. */
  void SetColumnLabel(int col, wxString label);

protected:
  /** Notify the Listener of item selection and deselection. */
  void OnSelectionChanged(wxListEvent& event);

  bool         m_PreventNotify;
  wxListCtrl  *m_List;         
  wxImageList *m_Images;       
  mafObserver *m_Listener;     

DECLARE_EVENT_TABLE()
}; // end of mafGUIListCtrlBitmap
#endif
