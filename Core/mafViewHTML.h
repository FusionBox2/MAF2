/*=========================================================================

 Program: MAF2
 Module: mafViewHTML
 Authors: Paolo Quadrani    Silvano Imboden
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafViewHTML_H__
#define __mafViewHTML_H__


#include "mafView.h"

//----------------------------------------------------------------------------
// forward references
//----------------------------------------------------------------------------
class mafRWI;
class mafSceneGraph;
class mafSceneGraph;
class mafGUICheckListBox;
class wxHtmlWindow;
//----------------------------------------------------------------------------
// mafViewHTML :
//----------------------------------------------------------------------------
/**   */
class MAF_EXPORT mafViewHTML: public mafView
{
public:
	          mafViewHTML(const mafString& label = _R("vtkViewHTML"), int camera_position = CAMERA_PERSPECTIVE, bool show_axes = true, bool show_grid = false, int stereo = 0);
	          ~mafViewHTML() override;

  mafTypeMacro(mafViewHTML, mafView);

	mafView				*Copy(mafBaseEventHandler *Listener = NULL, bool lightCopyEnabled = false) override;
  void           Create() override;
	mafSceneGraph *GetSceneGraph() override;
  mafRWIBase    *GetDefaultRWI();

  void VmeAdd   (mafNode *vme) override;
	void VmeRemove(mafNode *vme) override;
	void VmeSelect(mafNode *vme, bool select) override;
	void VmeShow  (mafNode *vme, bool show) override;
	void VmeUpdateProperty(mafNode*n, bool fromTag = false) override {};
  void VmeCreatePipe(mafNode *vme) override;
  void VmeDeletePipe(mafNode *vme) override {};

	void OnEvent(mafEventBase *maf_event) override;

    /** IDs for the GUI */
  enum VIEW_HTML_WIDGET_ID
  {
	  ID_LOAD = MINID,
	  ID_FORWARD,
	  ID_BACK,
    ID_URL,
  };

protected:
  mafRWI	      *m_Rwi;
  mafSceneGraph *m_Sg;
  mafNode        *m_ActiveNote;

  void  OnLoad();
  void  OnForward();
  void  OnBack();
  
	mafGUI *CreateGui() override;

  wxHtmlWindow *m_Html;
  mafString m_Url;
};
#endif
