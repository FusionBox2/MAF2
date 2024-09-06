/*=========================================================================

 Program: MAF2Medical
 Module: medLogicWithManagers
 Authors: Matteo Giacomoni, Gianluigi Crimi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __medLogicWithManagers_H__
#define __medLogicWithManagers_H__

//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------

#include "mafLogicWithManagers.h"

//----------------------------------------------------------------------------
// forward reference
//----------------------------------------------------------------------------
class medWizardManager;
class medWizard;

/**
  Class Name: medLogicWithManagers.
  Class for handle the high level logic of a medical application.
*/
class MED_COMMON_EXPORT medLogicWithManagers : public mafLogicWithManagers
{
public:
  /** constructor. */
	medLogicWithManagers();
  /** destructor. */
  ~medLogicWithManagers() override; 

  /** Process events coming from other objects */
  void OnEvent(mafEventBase *maf_event) override;

  /**  Plug a new view */
  void Plug(mafView* view, bool visibleInMenu = true) override;

  /**  Plug a new operation and its undo flag: if the operation does not support
  undo the undo flag has no effect */
  void Plug(mafOp *op, const mafString& menuPath = _R(""), bool canUndo = true, mafGUISettings *setting = NULL) override;
	
  /**  Plug a new wizard */
	virtual void Plug(medWizard *wizard, const mafString& menuPath = _R(""));
	
  /** Must be called before Configure */
  void PlugWizardManager(bool b){m_UseWizardManager=b;};

  /** Configure the application.
  At this point are plugged all the managers, the side-bar docking panel. 
  Are plugged also all the setting to the dialogs interface. */
  void Configure() override;

  /** Fill the View and operation menu's and set the application stamp to the VMEManager.*/
  void Show() override;

  /** Manage application exception and allow to save at least the tree. */
  void HandleException() override;

  /** Program Initialization */
  void Init(int argc, char **argv) override;
  
protected:
  /** Show contextual menu for views when right mouse click arrives.*/
  void ViewContextualMenu(bool vme_menu) override;

  /** Called on Quit event. */
  void OnQuit() override;

  /** Redefined to add Print buttons */
  virtual void CreateWizardToolbar();
  
  /** Respond to a VME_SELECTED evt. Update the selection on the tree and view representation. */
  virtual void VmeSelected(mafNode *vme);

  /** Redefined to add View,Op,Import,Export, Wizard menu*/
  void CreateMenu() override;

  /** Called when an wizard starts. Disable all menu and lock the Selection */ 
  virtual void WizardRunStarting();

  /** Called when an wizard stops. Re-enable all menu and unlock the Selection */ 
  virtual void WizardRunTerminated();
  
  /** Called after FileOpen or Save operation */
  void UpdateFrameTitle() override;

  /** FILE OPEN evt. handler. 
  By default (file_to_open = NULL) it ask the user to choose a file to open,
  otherwise it open the given one.*/
  bool OnFileOpen(const mafString& file_to_open = _R("")) override;
 
  /** FILE SAVE evt. handler */
  bool OnFileSave() override;
  
  /** FILE SAVEAS evt. handler */
  bool OnFileSaveAs() override;

  virtual void ConfigureWizardManager();

  medWizardManager *m_WizardManager;
  bool m_UseWizardManager;
  bool m_WizardRunning;
  wxGauge *m_WizardGauge;
  wxStaticText* m_WizardLabel;
  bool m_CancelledBeforeOpStarting;
  wxMenu *m_WizardMenu;

};
#endif
