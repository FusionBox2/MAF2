#pragma once

#include "ftkConfigure.h"

#include "mafGUIPanel.h"
#include "mafDecl.h"
#include "mafEvent.h"
#include "mafGUIVMEChooser.h"
#include "ftk/Core/NodeManager.h"
#include "mafStorage.h"
#include "wx/filehistory.h"
#include <memory>

#include "mafBaseEventHandler.h"
#include "mafSideBar.h"
#include <wx/notebook.h>
#include <wx/icon.h>
#include "ftk/Gui/MainFrame.h"


class mafView;
class mafGUINamedPanel;
class mafGUITimeBar;
class mafWXLog;
class mafVTKLog;
class mafGUILocaleSettings;
class mafGUIMeasureUnitSettings;
class mafGUIApplicationSettings;
class mafGUISettingsStorage;
class mafGUISettingsTimeBar;

class mafViewManager;
class mafOpManager;
class mafSideBar;
class mafGUIMaterialChooser;
class mafVME;
class mafInteractionManager;
class mafPrintSupport;
class mafGUISettingsDialog;
class mafGUIApplicationLayoutSettings;
class mafGUISettings;
class mafGUISettingsHelp;

class MAF_EXPORT mafStorageData
{
public:
  mafStorageData(const mafString& extention, bool makeBakFile, const mafString& msfDir)
    : m_Extension(extention)
    , m_MakeBakFile(makeBakFile)
    , m_MSFDir(msfDir)
  {}
  mafString m_Extension;
  bool      m_MakeBakFile;
  mafString m_MSFDir;
  mafString m_MSFFile;
  mafString m_ZipFile;
};

struct mafMenuElems
{
  mafMenuElems(bool op, int id, mafID command) :m_op(op), m_id(id), m_command(command) {}
  bool m_op;
  int  m_id;
  mafID m_command;
};

class InnerLogic
{
public:
  InnerLogic(mafBaseEventHandler* listener);
  ~InnerLogic();
  mafGUIMDIFrame* m_frame = nullptr;

  std::unique_ptr<mafGUILocaleSettings> m_LocaleSettings;
  std::unique_ptr<mafGUIMeasureUnitSettings> m_MeasureUnitSettings;
  std::unique_ptr<mafGUIApplicationSettings> m_ApplicationSettings;
  std::unique_ptr<mafGUISettingsStorage>     m_StorageSettings;
  std::unique_ptr<mafGUISettingsTimeBar>     m_TimeBarSettings;

  std::unique_ptr<mafPrintSupport> m_PrintSupport;

  std::unique_ptr<mafGUISettingsDialog> m_SettingsDialog;


  wxToolBar* m_ToolBar = nullptr;
  wxMenuBar* m_MenuBar = nullptr;

  wxConfigBase* m_Config = nullptr;           ///< Application configuration for file history management

  bool m_LogToFile = false;
  bool m_LogAllEvents = false;

  bool m_PlugMenu = true;
  bool m_PlugToolbar = true;
  bool m_PlugSidebar = true;
  long m_SidebarStyle = mafSideBar::DOUBLE_NOTEBOOK;
  bool m_PlugTimebar = true;
  bool m_PlugLogbar = true;

  mafWXLog* m_Logger = nullptr;
  mafVTKLog* m_VtkLog = nullptr;


  wxString m_LastSelectedPanel;
  mafGUITimeBar* m_TimePanel = nullptr;
  std::vector<wxAcceleratorEntry> m_AccelTable;

  std::unique_ptr<mafSideBar>             m_SideBar;
  std::unique_ptr<mafNodeManager>         m_NodeManager;
  std::unique_ptr<mafViewManager>         m_ViewManager;
  std::unique_ptr<mafOpManager>           m_OpManager;
  std::unique_ptr<mafInteractionManager>  m_InteractionManager;
  std::vector<mafMenuElems> m_MenuElems;

  long m_UserCommandIndex = 0;
  std::unique_ptr<mafGUIMaterialChooser>  m_MaterialChooser;
  wxMenu* m_ImportMenu = nullptr;
  wxMenu* m_ExportMenu = nullptr;
  wxMenu* m_RecentFileMenu = nullptr;
  wxMenu* m_OpMenu = nullptr;
  wxMenu* m_ViewMenu = nullptr;
  wxFileHistory	m_FileHistory;      ///< Used to hold recently opened files
  wxMenu* m_EditMenu = nullptr;
  wxMenu* m_ViewListMenu = nullptr;



  bool m_CameraLinkingObserverFlag = false;
  bool m_ExternalViewFlag = false;

  std::unique_ptr<mafGUIApplicationLayoutSettings> m_ApplicationLayoutSettings;
  std::unique_ptr<mafGUISettingsHelp> m_HelpSettings;

  std::unique_ptr<mafStorage> m_Storage;
  std::unique_ptr<mafStorageData> m_StorageData;


  std::vector<mafString> m_AppStamp;
  int                     m_FileHistoryIdx = -1;
};


class MAF_EXPORT mafLogicWithManagers: public mafBaseEventHandler
{
public:
  mafLogicWithManagers();
  ~mafLogicWithManagers() override; 

	virtual bool Configure();

	virtual mafGUIMDIFrame* GetTopWin();


  void OnEvent(mafEventBase *maf_event) override;

	virtual void Plug(mafView* view, bool visibleInMenu = true);
	virtual void Plug(mafOp *op, const mafString& menuPath = _R(""), bool canUndo = true, mafGUISettings *setting = NULL);

  virtual void Init(int argc, char **argv);

  virtual void Show();

  virtual void ShowSplashScreen();
  virtual void ShowSplashScreen(wxBitmap& splashImage);

  void SetApplicationStamp(const mafString &app_stamp);
  void SetApplicationStamp(const std::vector<mafString>& app_stamp);

  void SetExternalViewFlag(bool external = false);

  bool GetExternalViewFlag();

  virtual void HandleException();

  void FindVME();

  void MAFExpertModeOn(){(*GetMAFExpertMode()) = TRUE;};
  void MAFExpertModeOff(){(*GetMAFExpertMode())= FALSE;};
  void SetMAFExpertMode(int value){(*GetMAFExpertMode()) = value;};

  void SetFileExtension(mafString& extension);

protected:
  void AddToMenu(const mafString& name, long id, wxMenu* path_menu, const mafString& menuPath = _R(""));
  void SetAccelerator(const mafString& name, long id);

	bool AskConfirmAndSave();
  void EnableOperations(bool enable = true);
   virtual void UpdateTimeBounds();

  virtual void TimeSet(double t);
  
	virtual void OnFileNew();
	virtual bool OnFileOpen(const mafString& file_to_open = _R(""));
	virtual void OnFileHistory(int fileId);
  virtual void Save();
  virtual bool OnFileSave();
  virtual bool OnFileSaveAs();
  virtual bool OnFileClose(bool force = false);
  virtual void OnQuit();

  virtual void ImportExternalFile(mafString &filename);

	virtual void VmeSelect(mafEvent &e);
	virtual void VmeSelected(std::shared_ptr<mafNode> vme, bool remote = true);
  virtual void VmeDoubleClicked(mafEvent &e);
	virtual void VmeShow(mafNode *vme, bool visibility);
	virtual void VmeModified(mafNode *vme);


  virtual void VmeExpand(mafNode *vme);
  virtual void VmeCollapse(mafNode *vme);
  virtual void VmeExpandSubTree(mafNode *vme);
  virtual void VmeCollapseSubTree(mafNode *vme);
  virtual void VmeExpandVisible(mafNode *vme);

	virtual void VmeAdded(mafNode *vme);
	virtual void VmeRemove(mafNode *vme);
	virtual void VmeRemoving(mafNode *vme);
	virtual std::vector<mafNode* > VmeChoose(intptr_t vme_accept_function = 0, long style = REPRESENTATION_AS_TREE, mafString title = _R("Choose Node"), bool multiSelect = false);

  virtual void VmeChooseMaterial(mafVME *vme, bool updateProperty);
  virtual void VmeUpdateProperties(mafVME *vme, bool updatePropertyFromTag);

	virtual void OpRunStarting();
	virtual void OpRunTerminated();
	virtual void OpShowGui(bool push_gui, mafGUIPanel *panel);
	virtual void OpHideGui(bool view_closed);
  
  virtual void TreeContextualMenu(mafEvent &e);

  virtual void ViewContextualMenu(bool vme_menu);

	virtual void ViewCreate(int viewId);
	virtual void ViewCreated(mafView *v);
	virtual void ViewSelect();

  virtual void RestoreLayout();

  void UpdateMeasureUnit();

  void EnableMenuAndToolbar(bool enable);

  virtual void UpdateFrameTitle();


  void SetDirName(const mafString& dirname);

  mafID GetNewMenuId();
  void AddCreationDate(mafNode *vme);
  bool SetAppTag(mafNode *vme);
  bool CheckAppTag(mafNode *vme);

  void EnableItem(int item, bool enable);
  std::unique_ptr<InnerLogic> m_logic;
};
