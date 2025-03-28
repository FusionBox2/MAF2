#pragma once

#include "mafGUISettings.h"

//----------------------------------------------------------------------------
// forward reference
//----------------------------------------------------------------------------
class mafViewManager;
class mmaApplicationLayout;
class wxFrame;
class mafStorage;
class mafParser;
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
	~mafGUIApplicationLayoutSettings() override; 

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
  void OnEvent(mafEventBase *maf_event) override;

  /** Assign ViewManager pointer to member variable. */
  void SetViewManager(mafViewManager *view_manager) {m_ViewManager = view_manager;};

  /** Set a reference to the interface elements to store their visibility.*/
  void SetApplicationFrame(wxFrame *win) {m_Win = win;};

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
  void CreateGui() override;

  /** Initialize Application layout used into the application.*/
  void InitializeSettings() override;

  /** Remove Selected Layout from the list*/
  void RemoveLayout();

  /** Apply selected layout */
  void ApplyLayout();

  /** Set layout as default*/
   void SetLayoutAsDefault();

  wxFrame               *m_Win = nullptr;
  mafString              m_DefaultLayoutName;
  mafString              m_DefaultLayoutFile;
  mafString              m_ActiveLayoutName;
  mafString              m_LayoutType;
  
  mafString              m_LayoutFileSave;
  mafViewManager        *m_ViewManager = nullptr;
  std::shared_ptr<mafVMERoot> m_XMLRoot;
  int                    m_VisibilityVme = 0;
  wxListBox				      *m_List = nullptr;
	int                    m_SelectedItem = -1;
  int                    m_DefaultFlag = 0;
  bool                   m_ModifiedLayouts = false;
};
