/*=========================================================================

 Program: MAF2
 Module: mafLogicWithManagers
 Authors: Silvano Imboden, Paolo Quadrani
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __mafLogicWithManagers_H__
#define __mafLogicWithManagers_H__


//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "mafGUIPanel.h"
#include "mafDecl.h"
#include "mafEvent.h"
#include "mafLogicWithGUI.h"
#include "mafGUIVMEChooser.h"
#include "mafNodeManager.h"
#include "mafStorage.h"
#include "wx/filehistory.h"
#include <memory>


//----------------------------------------------------------------------------
// forward reference
//----------------------------------------------------------------------------
class mafViewManager;
class mafOpManager;
class mafSideBar;
class mafGUIMaterialChooser;
class mafVME;
class mafInteractionManager;
class mafPrintSupport;
#ifdef MAF_USE_CURL
class mafRemoteLogic;
#endif
class mafGUISettingsDialog;
class mafGUIApplicationLayoutSettings;
class mafGUISettings;
class mafGUISettingsHelp;

//----------------------------------------------------------------------------
// mafLogicWithManagers :
//----------------------------------------------------------------------------
/** 
mafLogicWithManagers provide:
- the VMEManager
- the OperationManager, and the support for plugging operations
- the ViewManager, and the support for plugging views
and manage all the Events related to Nodes,Operations and Views.
Each Evt. will call a corresponding evt. handler.

mafLogicWithManagers also provide the following standard operation:
- Select,Cut,Copy,Paste,Delete,Undo,Redo

mafLogicWithManagers also provide to enable/disable the user interface elements following
the requirement of the application status.

Derived classes can replace any of these manager with a customized version.

It is Recommended that Derived Classes call mafLogicWithManagers::OnEvent to 
for every un-handled event.

USAGE - member function MUST be called in this order:
  -- create a mafLogicWithManagers object (-> create the mainwindow)
  -- call PlugXXX:          to customize the GUI and the Managers
  -- call Configure:        to create the GUI and the managers
  -- call Plug              to plug-in operations and views
  -- call Show              it is before Init, to eventually show the Progress-bar during a Load
  -- call Init(argc,argv)   will call MSFNew or MSFLoad

*/

class MAF_EXPORT mafStorageData
{
public:
  mafStorageData(const mafString& extention, bool makeBakFile, const mafString& msfDir)
    : m_Extension(extention)
    , m_MakeBakFile(makeBakFile)
    , m_MSFDir(msfDir)
  {}
  mafString m_Extension;
  bool      m_MakeBakFile;      ///< Flag used to create or not the backup file of the saved msf.
  mafString m_MSFDir;           ///< Directory name in which is present the msf file.
  mafString m_MSFFile;          ///< File name of the data associated to the tree.
  mafString m_ZipFile;          ///< File name of compressed archive in which save the data associated to the tree.
};
class MAF_EXPORT mafLogicWithManagers: public mafLogicWithGUI
{
public:
               mafLogicWithManagers();
               ~mafLogicWithManagers() override; 

  enum UPLOAD_FLAGS
  {
    UPLOAD_SELECTED_VME = 0,
    UPLOAD_SUBTREE,
    UPLOAD_TREE,
    UPLOAD_COMPRESSED_VME,
    UPLOAD_COMPRESSED_SUBTREE,
    UPLOAD_COMPRESSED_TREE,
  };

  /** */
               void OnEvent(mafEventBase *maf_event) override;

  /**  Plug a new view */
	virtual void Plug(mafView* view, bool visibleInMenu = true);


  /**  Plug a new operation and its undo flag: if the operation does not support
  undo the undo flag has no effect */
	virtual void Plug(mafOp *op, const mafString& menuPath = _R(""), bool canUndo = true, mafGUISettings *setting = NULL);
	
  /** Configure the application.
  At this point are plugged all the managers, the side-bar docking panel. 
  Are plugged also all the setting to the dialogs interface. */
               void Configure() override;

  /** Program Initialization */
               void Init(int argc, char **argv) override;

  /** Fill the View and operation menu's and set the application stamp to the VMEManager.*/
               void Show() override;

  /** Set the application stamp for the application, 
  if set to OPEN_ALL_DATA let's the application to open all msf file. 
  As default the application stamp is the name of the application and it is set into the Show() method. */
  void SetApplicationStamp(const mafString &app_stamp);
  void SetApplicationStamp(const std::vector<mafString>& app_stamp);

  /** Allow to set the flag for views to by External to the main frame or to by child of parent frame. */
  void SetExternalViewFlag(bool external = false);

  /** Retrieve the value for the external view flag.*/
  bool GetExternalViewFlag();

  /** Manage application exception and allow to save at least the tree. */
  virtual void HandleException();

  /** Open a Find VME dialog.*/
  void FindVME();

  /* Set MAF Expert Mode*/
  void MAFExpertModeOn(){(*GetMAFExpertMode()) = TRUE;};
  void MAFExpertModeOff(){(*GetMAFExpertMode())= FALSE;};
  void SetMAFExpertMode(int value){(*GetMAFExpertMode()) = value;};

  /* Set the file extension */
  void SetFileExtension(mafString &extension) {m_StorageData->m_Extension = extension;};

protected:
  bool AskConfirmAndSave();
  void EnableOperations(bool enable = true);
  //---------------------------------------------------------
  // Description:
  // Method to update the time bounds when 
  // the tree representation change by adding or deleting a vme.
               void UpdateTimeBounds() override;

  // Description:
  // Set the time to update the time bar.
  virtual void TimeSet(double t);
//---------------------------------------------------------
  
  /** Redefined to add View,Op,Import,Export menu */
               void CreateMenu() override;

  /** Redefined to add Print buttons */
               void CreateToolbar() override;

  // EVENT HANDLERS

  /** FILE NEW evt. handler */
	virtual void OnFileNew();
  /** FILE OPEN evt. handler. 
  By default (file_to_open = NULL) it ask the user to choose a file to open,
  otherwise it open the given one.*/
	virtual bool OnFileOpen(const mafString& file_to_open = _R(""));
  /** FILE UPLOAD evt. handler 
  By default (remote_file = NULL) AND the entire msf is uploaded and only the remote directory is asked to the user, 
  otherwise given parameters are managed to upload the file correctly. 'upload_flag' can be:
  UPLOAD_SELECTED_VME, UPLOAD_SUBTREE or UPLOAD_TREE (default) and the corresponding compressed:
  UPLOAD_COMPRESSED_VME, UPLOAD_COMPRESSED_SUBTREE, UPLOAD_COMPRESSED_TREE.*/
  virtual void OnFileUpload(const char *remote_file, unsigned int upload_flag = UPLOAD_TREE);
  /** FILE HISTORY evt. handler */
	virtual void OnFileHistory(int fileId);
  /** FILE SAVE evt. handler */
  virtual void Save();
  /** FILE SAVE evt. handler */
  virtual bool OnFileSave();
  /** FILE SAVEAS evt. handler */
  virtual bool OnFileSaveAs();
  /** FILE CLOSE evt. handler */
  virtual bool OnFileClose(bool force = false);
  /** Called on Quit event. */
               void OnQuit() override;

  /** Called when a user drag a non msf or zmsf file over the application.
  Example: dragging a VTK file over the application cause the application to inport it through the importer.*/
  virtual void ImportExternalFile(mafString &filename);

  /** Respond to a VME_SELECT evt. Instantiate the 'Select' operation. */
	virtual void VmeSelect(mafEvent &e);
  /** Respond to a VME_SELECTED evt. Update the selection on the tree and view representation. */
	virtual void VmeSelected(mafNode *vme, bool remote = true);
  /** Respond to a VME_DCLICKED evt. Manage the 'Double click' on Selected VME. */
  virtual void VmeDoubleClicked(mafEvent &e);
	/** Respond to a VME_SHOW evt. Show/Hide the vme. */
	virtual void VmeShow(mafNode *vme, bool visibility);
  /** Respond to a VME_Modified evt.*/
	virtual void VmeModified(mafNode *vme);
  /** Respond to a VME_ADD evt. Add a new vme to the tree. */
	virtual void VmeAdd(mafNode *vme);


  virtual void VmeExpand(mafNode *vme);
  virtual void VmeCollapse(mafNode *vme);
  virtual void VmeExpandSubTree(mafNode *vme);
  virtual void VmeCollapseSubTree(mafNode *vme);
  virtual void VmeExpandVisible(mafNode *vme);




  /** Respond to a VME_ADDED evt. propagate evt. to SideBar,ViewManager,ecc.. */
	virtual void VmeAdded(mafNode *vme);
  /** Respond to a VME_REMOVE evt. Remove a vme from the tree. */
	virtual void VmeRemove(mafNode *vme);
  /** Respond to a VME_REMOVING evt. propagate evt. to SideBar,ViewManager,ecc.. */
	virtual void VmeRemoving(mafNode *vme);
	/** Respond to a VME_CHOOSE evt. Build a dialog containing the vme tree and return the vme choosed from the user. */
	virtual std::vector<mafNode*> VmeChoose(intptr_t vme_accept_function = 0, long style = REPRESENTATION_AS_TREE, mafString title = _R("Choose Node"), bool multiSelect = false);

  /** Build a dialog to show all available materials. */	
  virtual void VmeChooseMaterial(mafVME *vme, bool updateProperty);

  //Update Vme Visual Properties
  virtual void VmeUpdateProperties(mafVME *vme, bool updatePropertyFromTag);

  /** Called when an operation starts. Disable all menu and lock the Selection */ 
	virtual void OpRunStarting();
  /** Called when an operation stops. Re-enable all menu and unlock the Selection */ 
	virtual void OpRunTerminated();
	/** Show the gui of the running operation or the settings of the selected view. */
	virtual void OpShowGui(bool push_gui, mafGUIPanel *panel);
	/** Hide the setting's gui when a view is destroyed. */
	virtual void OpHideGui(bool view_closed);
  
  /** Show contextual menu for tree when right mouse click arrives.*/
  virtual void TreeContextualMenu(mafEvent &e);

  /** Show contextual menu for views when right mouse click arrives.*/
  virtual void ViewContextualMenu(bool vme_menu);

  /** Create a new view. */
	virtual void ViewCreate(int viewId);
	/** A new View has Been Created - Logic must create the View Frame */
	virtual void ViewCreated(mafView *v);
	/** Select a view and update the display list for the tree. */
	virtual void ViewSelect();

  virtual void RestoreLayout();

  /** Called when user change the measure unit from menù Options.*/
  void UpdateMeasureUnit();

  /** Enable/Disable menu items when an operation start/end running. */
  void EnableMenuAndToolbar(bool enable);

  /** Called after FileOpen or Save operation */
  virtual void UpdateFrameTitle();

  std::unique_ptr<mafSideBar>             m_SideBar;
  std::unique_ptr<mafNodeManager>         m_NodeManager;
  std::unique_ptr<mafViewManager>         m_ViewManager;
  std::unique_ptr<mafOpManager>           m_OpManager;
  std::unique_ptr<mafInteractionManager>  m_InteractionManager;
#ifdef MAF_USE_CURL
  std::unique_ptr<mafRemoteLogic>         m_RemoteLogic;
#endif
  
  std::unique_ptr<mafGUIMaterialChooser>  m_MaterialChooser;
  std::unique_ptr<mafPrintSupport> m_PrintSupport;

  wxMenu *m_ImportMenu; 
  wxMenu *m_ExportMenu; 
  wxMenu *m_RecentFileMenu;
  wxMenu *m_OpMenu;
  wxMenu *m_ViewMenu; 
  wxFileHistory	m_FileHistory;      ///< Used to hold recently opened files
  wxConfigBase* m_Config;           ///< Application configuration for file history management
  wxMenu *m_EditMenu; 
  wxMenu *m_ViewListMenu; 

  bool m_CameraLinkingObserverFlag;
  bool m_ExternalViewFlag;

  std::unique_ptr<mafGUISettingsDialog> m_SettingsDialog;
  std::unique_ptr<mafGUIApplicationLayoutSettings> m_ApplicationLayoutSettings;
  std::unique_ptr<mafGUISettingsHelp> m_HelpSettings;

	std::unique_ptr<mafStorage> m_Storage;          ///< Associated storage
  std::unique_ptr<mafStorageData> m_StorageData;

  std::vector<mafString> m_AppStamp;      ///< Application stamps for our application.
  int                     m_FileHistoryIdx;   ///< Identifier of the file to open

  /** Set the filename for the current tree. */
  void SetDirName (const mafString& dirname) { m_StorageData->m_MSFDir = dirname;};

  struct mafMenuElems
  {
    mafMenuElems(bool op, int id, mafID command):m_op(op), m_id(id), m_command(command){}
    bool m_op;
    int  m_id;
    mafID m_command;
  };
  std::vector<mafMenuElems> m_MenuElems;

  long m_UserCommandIndex;
  mafID GetNewMenuId();
  /** Set tag with creation date for the node passed as argument.*/
  void AddCreationDate(mafNode *vme);
  /** Set tag with application stamp.*/
  bool SetAppTag(mafNode *vme);
  /** Check tag with application stamp to assure compatibility.*/
  bool CheckAppTag(mafNode *vme);
};
#endif
