#pragma once

#include "mafEventSender.h"
//----------------------------------------------------------------------------
// forward references;
//----------------------------------------------------------------------------
class mafView;
class mafEvent;
class mafGUICheckTree;
class mafNode;
class mafSceneGraph;
class mafVME;

//----------------------------------------------------------------------------
// mafGUITreeContextualMenu :
//----------------------------------------------------------------------------
/** Used to create and manage contextual menu' for tree widget used in side bar
@sa mafGUIContextualMenu
*/
class MAF_EXPORT mafGUITreeContextualMenu : public wxMenu, public mafEventSender
{
public:
  mafGUITreeContextualMenu();
  ~mafGUITreeContextualMenu() override;

  /** Create a contextual menu*/
  virtual void CreateContextualMenu(mafGUICheckTree *tree, mafView *view, std::shared_ptr<mafNode> vme, bool vme_menu);

  /** Visualize contextual men for the MDI child and selected view. */
  void ShowContextualMenu();

protected:
  mafView       *m_ViewActive;
  std::shared_ptr<mafVME> m_VmeActive;
  std::shared_ptr<mafNode> m_NodeActive;
  mafGUICheckTree  *m_NodeTree;

  mafSceneGraph *m_SceneGraph;
  
  bool m_Autosort;
  bool m_CryptoCheck;

  /** Enable/disable crypto for a subtree. */
  void CryptSubTree(bool crypt);
  
	/** Answer contextual men's selection. */
	virtual void OnContextualMenu(wxCommandEvent& event);
  DECLARE_EVENT_TABLE()
};
