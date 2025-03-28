#pragma once

#include "ftkConfigure.h"

#include "mafLogicWithManagers.h"

class medWizardManager;
class medWizard;

class MED_COMMON_EXPORT medLogicWithManagers : public mafLogicWithManagers
{
public:
	medLogicWithManagers();
  ~medLogicWithManagers() override; 

  void OnEvent(mafEventBase *maf_event) override;

  void Plug(mafView* view, bool visibleInMenu = true) override;

  void Plug(mafOp *op, const mafString& menuPath = _R(""), bool canUndo = true, mafGUISettings *setting = NULL) override;
	
	virtual void Plug(medWizard *wizard, const mafString& menuPath = _R(""));
	
  void PlugWizardManager(bool b){m_UseWizardManager=b;};

  bool Configure() override;

  void Show() override;

  void HandleException() override;

  void Init(int argc, char **argv) override;
  
protected:
  void ViewContextualMenu(bool vme_menu) override;

  void OnQuit() override;

  virtual void CreateWizardToolbar();
  
  virtual void VmeSelected(std::shared_ptr<mafNode> vme);

  virtual void WizardRunStarting();

  virtual void WizardRunTerminated();
  
  void UpdateFrameTitle() override;

  bool OnFileOpen(const mafString& file_to_open = _R("")) override;
 
  bool OnFileSave() override;
  
  bool OnFileSaveAs() override;

  virtual void ConfigureWizardManager();

  std::unique_ptr<medWizardManager> m_WizardManager;
  bool m_UseWizardManager = false;
  bool m_WizardRunning = false;
  wxGauge *m_WizardGauge;
  wxStaticText* m_WizardLabel;
  bool m_CancelledBeforeOpStarting;
  wxMenu *m_WizardMenu;

};
