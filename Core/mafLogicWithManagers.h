#pragma once

#include "ftkConfigure.h"

#include "mafBaseEventHandler.h"

#include <memory>

class mafView;
namespace gui::wx
{
	class NamedPanel;
	using Panel = wxPanel;
}
using mafGUINamedPanel = gui::wx::NamedPanel;
using mafGUIPanel = gui::wx::Panel;

namespace model::data
{
	class Node;
	class NodeManager;
}
using mafNode = model::data::Node;
class mafEvent;
class mafVME;
class mafOp;
class mafGUISettings;


class InnerLogic;


class MAF_EXPORT mafLogicWithManagers : public mafBaseEventHandler
{
public:
	mafLogicWithManagers();
	~mafLogicWithManagers() override;

	virtual bool Configure();

	virtual wxFrame* GetTopWin();

	void OnEvent(mafEventBase* maf_event) override;

	virtual void Plug(std::unique_ptr<mafView> view, bool visibleInMenu = true);
	virtual void Plug(mafOp* op, const mafString& menuPath = _R(""), bool canUndo = true, mafGUISettings* setting = nullptr);

	virtual void Init(int argc, char** argv);

	virtual void Show();

	virtual void ShowSplashScreen();
	virtual void ShowSplashScreen(wxBitmap& splashImage);

	void SetApplicationStamp(const mafString& app_stamp);
	void SetApplicationStamp(const std::vector<mafString>& app_stamp);

	void SetExternalViewFlag(bool external = false);

	bool GetExternalViewFlag();

	virtual void HandleException();

	void FindVME();

	void MAFExpertModeOn() { *GetMAFExpertMode() = TRUE; }
	void MAFExpertModeOff() { *GetMAFExpertMode() = FALSE; }
	void SetMAFExpertMode(int value) { *GetMAFExpertMode() = value; }

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

	virtual void ImportExternalFile(mafString& filename);

	virtual void VmeSelect(mafEvent& e);
	virtual void VmeSelected(std::shared_ptr<mafNode> vme, bool remote = true);
	virtual void VmeDoubleClicked(mafEvent& e);
	virtual void VmeShow(mafNode* vme, bool visibility);
	virtual void VmeModified(mafNode* vme);


	virtual void VmeExpand(mafNode* vme);
	virtual void VmeCollapse(mafNode* vme);
	virtual void VmeExpandSubTree(mafNode* vme);
	virtual void VmeCollapseSubTree(mafNode* vme);
	virtual void VmeExpandVisible(mafNode* vme);

	virtual void VmeAdded(mafNode* vme);
	virtual void VmeRemove(mafNode* vme);
	virtual void VmeRemoving(mafNode* vme);
	virtual std::vector<mafNode* > VmeChoose(intptr_t vme_accept_function, long style, mafString title = _R("Choose Node"), bool multiSelect = false);

	virtual void VmeChooseMaterial(mafVME* vme, bool updateProperty);
	virtual void VmeUpdateProperties(mafVME* vme, bool updatePropertyFromTag);

	virtual void OpRunStarting();
	virtual void OpRunTerminated();
	virtual void OpShowGui(bool push_gui, mafGUIPanel* panel);
	virtual void OpHideGui(bool view_closed);

	virtual void TreeContextualMenu(mafEvent& e);

	virtual void ViewContextualMenu(bool vme_menu);

	virtual void ViewCreate(int viewId);
	virtual void ViewCreated(mafView* v);
	virtual void ViewSelect();

	virtual void RestoreLayout();

	void UpdateMeasureUnit();

	void EnableMenuAndToolbar(bool enable);

	virtual void UpdateFrameTitle();


	void SetDirName(const mafString& dirname);

	mafID GetNewMenuId();
	void AddCreationDate(mafNode* vme);
	bool SetAppTag(mafNode* vme);
	bool CheckAppTag(mafNode* vme);

	void EnableItem(int item, bool enable);
	std::unique_ptr<InnerLogic> m_logic;
};
