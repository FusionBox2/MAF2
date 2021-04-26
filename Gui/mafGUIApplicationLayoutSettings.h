/*=========================================================================

 Program: MAF2
 Module: mafGUIApplicationLayoutSettings
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafGUIApplicationLayoutSettings_H__
#define __mafGUIApplicationLayoutSettings_H__

#include "mafGUISettings.h"

//----------------------------------------------------------------------------
// forward reference
//----------------------------------------------------------------------------
class mafViewManager;
class mmaApplicationLayout;
class mafGUIMDIFrame;
class mafVMEStorage;
class mafXMLParser;
class mafVMERoot;

//----------------------------------------------------------------------------
// mafGUIApplicationLayoutSettings :
//----------------------------------------------------------------------------
/**
  Class Name: mafGUIApplicationLayoutSettings.
  Class that handles the option and operation like load or save regardings  layout like:
  - views layout
  - vmes visibility 
  - camera position
  - gui layout (toolbar, logbar, sidebar)
*/
class MAF_EXPORT mafGUIApplicationLayoutSettings : public mafGUISettings
{
public:
  /** constructor .*/
	mafGUIApplicationLayoutSettings(mafBaseEventHandler *listener, const mafString &label = _L("Application Layout"));
  /** destructor .*/
	~mafGUIApplicationLayoutSettings(); 

  /** GUI IDs*/
  enum APPLICATION_LAYOUT_WIDGET_ID
  {
    SAVE_TREE_LAYOUT_ID = MINID,
    APPLY_TREE_LAYOUT_ID,
    LAYOUT_NAME_ID,
    LAYOUT_VISIBILITY_VME,
    ID_LIST_LAYOUT,
    OPEN_LAYOUT_ID,
    APPLY_LAYOUT_ID,
    ADD_LAYOUT_ID,
    REMOVE_LAYOUT_ID,
    DEFAULT_LAYOUT_ID,
    SAVE_APPLICATION_LAYOUT_ID,
  };

  /** Answer to the messages coming from interface. */
  void OnEvent(mafEventBase *maf_event);

  /** Assign ViewManager pointer to member variable. */
  void SetViewManager(mafViewManager *view_manager) {m_ViewManager = view_manager;};

  /** Set a reference to the interface elements to store their visibility.*/
  void SetApplicationFrame(mafGUIMDIFrame *win) {m_Win = win;};

   /** Set a reference to the interface elements to store their visibility.*/
  void SetVisibilityVME(bool value) {m_VisibilityVme = value;};

  /** Return Layout Modification Flag*/
  bool GetModifiedLayouts(){return m_ModifiedLayouts;};
 
  /** Store in file Layout List*/
  void SaveApplicationLayout();

   /** Save current application layout into the attribute mmaApplicationLayout, 
  inside current MSF data*/
  void SaveTreeLayout();

  /** Add current layout to the list*/
  void AddLayout();

  /** Add current layout to the list*/
  void ApplyTreeLayout();
  
  /** Load layouts stored in a file*/
  void LoadLayout(bool fileDefault = false);

protected:
  /** Create the GUI for the setting panel.*/
  void CreateGui();

  /** Initialize Application layout used into the application.*/
  void InitializeSettings();

  /** Remove Selected Layout from the list*/
  void RemoveLayout();

  /** Apply selected layout */
  void ApplyLayout();

  /** Set layout as default*/
   void SetLayoutAsDefault();

  mafGUIMDIFrame           *m_Win;
  mafString              m_DefaultLayoutName;
  mafString              m_DefaultLayoutFile;
  mafString              m_ActiveLayoutName;
  mafString              m_LayoutType;
  
  mafString              m_LayoutFileSave;
  mafViewManager        *m_ViewManager;
  mmaApplicationLayout  *m_Layout;
  //mafVMEStorage         *m_Storage;
	mafXMLParser          *m_XMLStorage;
	mafVMERoot            *m_XMLRoot;
  int                    m_VisibilityVme;
  wxListBox				      *m_List;
	int                    m_SelectedItem;
  int                    m_DefaultFlag;
  bool                   m_ModifiedLayouts;
};
#endif
