/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafViewIntGraph.h,v $
  Language:  C++
  Date:      $Date: 2008-07-25 12:13:22 $
  Version:   $Revision: 1.3 $
  Authors:   Fedor Moiseev / Vladik Aranov
==========================================================================
  Copyright (c) 2001/2007 
  ULB - Universite Libre de Bruxelles (www.ulb.ac.be)
=========================================================================*/

#ifndef __mafViewIntGraph_H__
#define __mafViewIntGraph_H__

#include "lhpDefines.h"

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "mafView.h"
#include "mafDecl.h"
#include "mafViewIntGraphWindow.h"
#include <map>

//----------------------------------------------------------------------------
// defines
//----------------------------------------------------------------------------
#define mafINTG_SAVEINFO_TAG  "LHP_PLOT_INFORMATION_TAG"
#define mafINTGG_SAVEINFO_TAG "LHP_GENERAL_PLOT_INFORMATION_TAG"

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafView.h"
#include "mafGraphIDDesc.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVME;
class mafSceneGraph;

//----------------------------------------------------------------------------
// class mafViewIntGraph
//----------------------------------------------------------------------------
class mafViewIntGraph: public mafView
{
public:
  mafViewIntGraph(const mafString& label = _R("Biomechanical graph"));
  virtual ~mafViewIntGraph(); 

  mafTypeMacro(mafViewIntGraph, mafView);

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
  mafViewIntGraphWindow *GetRenderWindow()    {return m_RenderWindow;}
  /** save information about current plot into VME*/
  void                  savePlot(void);
  /** restore information about current plot from VME tree*/
  void                  loadPlot(void);
  /** save information about current plot into VME: general settings*/
  void                  savePlotGen(void);
  /** restore information about current plot from VME tree: general settings*/
  void                  loadPlotGen(void);

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
    ID_SMOOTHING,
    ID_FREEZE_GRAPH,
    ID_REFERENCE_FRAME,
    ID_SAVE_PLOT,
    ID_LOAD_PLOT,
    ID_ROLLOUT_RENDER,
    ID_LAST
  };



protected:
  //graph rendering window
  mafViewIntGraphWindow   *m_RenderWindow;
  int                     m_IsFrozen;
  //mafTimeStamp            m_ReferenceFrame;
  double                  m_Smoothing;
  mafSceneGraph           *m_Sg;

  virtual mafGUI *CreateGui();
  /** Return the visual pipe's name.*/
  void GetVisualPipeName(mafNode *node, mafString &pipe_name);
};

#endif
