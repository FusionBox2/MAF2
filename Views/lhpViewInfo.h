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

#include "lhpDefines.h"

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
  virtual ~lhpViewInfo(); 

  mafTypeMacro(lhpViewInfo, mafView);

  virtual void      OnEvent(mafEventBase *maf_event);
  virtual mafView*  Copy(mafBaseEventHandler *Listener, bool lightCopyEnabled = false);
  virtual void      Create();
  /** Add the vme to the view's scene-graph*/
  virtual void VmeAdd(mafNode *vme);
  /** Remove the vme from the view's scene-graph*/
  virtual void VmeRemove(mafNode *vme);
  virtual void VmeSelect(mafNode *vme, bool select);
  /** Called to show/hide vme*/
  virtual void VmeShow(mafNode *vme, bool show);
  /** 
  Called to update visual pipe properties of the vme passed as argument. If the 'fromTag' flag is true,
  the update is done by reading the visual parameters from tags.*/
  virtual void VmeUpdateProperty(mafNode *vme, bool fromTag = false);
  /** 
  Create the visual pipe for the node passed as argument. 
  To create visual pipe first check in m_PipeMap if custom visual pipe is defined, 
  otherwise ask to vme which is its visual pipe. */
  virtual void VmeCreatePipe(mafNode *vme);
  /** Delete vme's visual pipe. It is called when vme is removed from visualization.*/
  virtual void VmeDeletePipe(mafNode *vme);

  virtual void CameraUpdate();
  virtual void CameraReset(mafNode *node = NULL);
  virtual mafSceneGraph *GetSceneGraph()    {return m_Sg;}; 
  /** Return a pointer to the image of the renderwindow.*/
  void GetImage(wxBitmap &bmp, int magnification = 1);
  /** Called to update all components that depends on Application Options.*/
  virtual void OptionsUpdate();
  /** 
  Set the visualization status for the node (visible, not visible, mutex, ...) \sa mafSceneGraph*/
  virtual int GetNodeStatus(mafNode *vme);
  /** 
  Return a pointer to the visual pipe of the node passed as argument. 
  It is used in mafSideBar to plug the visual pipe's GUI in the tabbed vme panel. \sa mafSideBar*/
  virtual mafPipe* GetNodePipe(mafNode *vme);
  /** Access function. See name. */
  wxHtmlWindow       *GetRenderWindow()    {return m_RenderWindow;}

  /** Show view settings into the tabbed sidebar. */
  //void	ShowSettings();
  /** Update all gui widgets*/
  void      UpdateGui();
  /** Set the vtk RenderWindow size. Used only for Linux (not necessary for Windows) */
  void SetWindowSize(int w, int h);

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
  virtual void Print(wxDC *dc, wxRect margins);
  /** print a dump of this object */
  virtual void Print(std::ostream& os, const int tabs=0);// const;

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

  virtual mafGUI *CreateGui();
  /** Return the visual pipe's name.*/
  void GetVisualPipeName(mafNode *node, mafString &pipe_name);
};

#endif
