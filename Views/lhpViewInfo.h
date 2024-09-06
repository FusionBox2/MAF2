/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpViewInfo.h,v $
  Language:  C++
  Date:      $Date: 2008-07-25 12:13:22 $
  Version:   $Revision: 1.3 $
  Authors:   Fedor Moiseev / Vladik Aranov
==========================================================================
  Copyright (c) 2001/2007 
  ULB - Universite Libre de Bruxelles (www.ulb.ac.be)
=========================================================================*/

#ifndef __lhpViewInfo_H__
#define __lhpViewInfo_H__

#include "mafDefines.h"

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "mafView.h"
#include "mafDecl.h"
#include "lhpViewInfoWnd.h"
#include <map>

//----------------------------------------------------------------------------
// defines
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafView.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVME;
class mafSceneGraph;

//----------------------------------------------------------------------------
// class lhpViewInfo
//----------------------------------------------------------------------------
class lhpViewInfo: public mafView
{
public:
  lhpViewInfo(const mafString& label = _R("Info view"));
  ~lhpViewInfo() override; 

  mafTypeMacro(lhpViewInfo, mafView);

  void      OnEvent(mafEventBase *maf_event) override;
  mafView*  Copy(mafBaseEventHandler *Listener, bool lightCopyEnabled = false) override;
  void      Create() override;
  /** Add the vme to the view's scene-graph*/
  void VmeAdd(mafNode *vme) override;
  /** Remove the vme from the view's scene-graph*/
  void VmeRemove(mafNode *vme) override;
  void VmeSelect(mafNode *vme, bool select) override;
  /** Called to show/hide vme*/
  void VmeShow(mafNode *vme, bool show) override;
  /** 
  Called to update visual pipe properties of the vme passed as argument. If the 'fromTag' flag is true,
  the update is done by reading the visual parameters from tags.*/
  void VmeUpdateProperty(mafNode *vme, bool fromTag = false) override;
  /** 
  Create the visual pipe for the node passed as argument. 
  To create visual pipe first check in m_PipeMap if custom visual pipe is defined, 
  otherwise ask to vme which is its visual pipe. */
  void VmeCreatePipe(mafNode *vme) override;
  /** Delete vme's visual pipe. It is called when vme is removed from visualization.*/
  void VmeDeletePipe(mafNode *vme) override;

  void CameraUpdate() override;
  void CameraReset(mafNode *node = NULL) override;
  mafSceneGraph *GetSceneGraph() override {return m_Sg;}; 
  /** Return a pointer to the image of the renderwindow.*/
  void GetImage(wxBitmap &bmp, int magnification = 1) override;
  /** Called to update all components that depends on Application Options.*/
  void OptionsUpdate() override;
  /** 
  Set the visualization status for the node (visible, not visible, mutex, ...) \sa mafSceneGraph*/
  int GetNodeStatus(mafNode *vme) override;
  /** 
  Return a pointer to the visual pipe of the node passed as argument. 
  It is used in mafSideBar to plug the visual pipe's GUI in the tabbed vme panel. \sa mafSideBar*/
  mafPipe* GetNodePipe(mafNode *vme) override;
  /** Access function. See name. */
  wxHtmlWindow       *GetRenderWindow()    {return m_RenderWindow;}

  /** Show view settings into the tabbed sidebar. */
  //void	ShowSettings();
  /** Update all gui widgets*/
  void      UpdateGui();
  /** Set the vtk RenderWindow size. Used only for Linux (not necessary for Windows) */
  void SetWindowSize(int w, int h) override;

  /** Struct containing information regarding visual pipe plugged into the view. */
  struct mafVisualPipeInfo
  {
    mafString m_PipeName;
    long      m_Visibility;
  };
  typedef std::map<mafString, mafVisualPipeInfo> mafPipeMap;

  /** Plug a visual pipe for a particular vme. It is used also to plug custom pipe.*/
  void PlugVisualPipe(mafString vme_type, mafString pipe_type, long visibility = VISIBLE);

  mafPipeMap m_PipeMap; ///< Map used to store visual pipeline associated with vme types

  /** Print this view.*/
  void Print(wxDC *dc, wxRect margins) override;
  /** print a dump of this object */
  void Print(std::ostream& os, const int tabs=0) override;// const;

  enum VIEWINTGRAPH_WIDGET_ID
  {
    ID_DEFAULT = Superclass::ID_LAST,
    ID_ROLLOUT_RENDER,
    ID_LAST
  };

  void UpdatePage();
protected:
  //graph rendering window
  wxHtmlWindow            *m_RenderWindow;
  mafSceneGraph           *m_Sg;
  std::vector<mafPipe *>   m_VNodes;

  mafGUI *CreateGui() override;
  /** Return the visual pipe's name.*/
  void GetVisualPipeName(mafNode *node, mafString &pipe_name);
};

#endif
