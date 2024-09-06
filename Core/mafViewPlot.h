/*=========================================================================

 Program: MAF2
 Module: mafViewPlot
 Authors: Silvano Imboden - Paolo Quadrani
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __mafViewPlot_H__
#define __mafViewPlot_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafView.h"
#include "mafSceneGraph.h"
#include "mafSceneNode.h" //used in subclasses
#include <map>

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVME;
class mafDeviceButtonsPadMouse;

/** Struct containing information regarding visual pipe plugged into the view. */
struct mafVisualPipeInfo
{
  mafString m_PipeName;
  long      m_Visibility;
};

#ifdef MAF_EXPORTS
#include "mafDllMacros.h"
EXPORT_STL_MAP(MAF_EXPORT,mafString,mafVisualPipeInfo);
#endif

//----------------------------------------------------------------------------
// mafViewPlot :
//----------------------------------------------------------------------------
/** 
mafViewPlot is a View that got a mafXYPlot and plot curves on a plane.
*/
class MAF_EXPORT mafViewPlot: public mafView
{
public:
  mafViewPlot(const mafString& label = _R("plot View"));
  ~mafViewPlot() override;

  mafTypeMacro(mafViewPlot, mafView);

  mafView*  Copy(mafBaseEventHandler *Listener, bool lightCopyEnabled = false) override;
  void      Create() override;
  void			OnEvent(mafEventBase *maf_event) override;

  /** IDs for the GUI */
/*  enum VIEW_VTK_WIDGET_ID
  {
    ID_SHOW_GRID = Superclass::ID_LAST,
    ID_GRID_NORMAL,
    ID_LAST
  };*/

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
  Set the visualization status for the node (visible, not visible, mutex, ...) \sa mafSceneGraph*/
  int GetNodeStatus(mafNode *vme) override;

  /** 
  Return a pointer to the visual pipe of the node passed as argument. 
  It is used in mafSideBar to plug the visual pipe's GUI in the tabbed vme panel. \sa mafSideBar*/
  mafPipe*  GetNodePipe(mafNode *vme) override;
  
  /** 
  Create the visual pipe for the node passed as argument. 
  To create visual pipe first check in m_PipeMap if custom visual pipe is defined, 
  otherwise ask to vme which is its visual pipe. */
  void VmeCreatePipe(mafNode *vme) override;

  /** 
  Delete vme's visual pipe. It is called when vme is removed from visualization.*/
  void VmeDeletePipe(mafNode *vme) override;

  mafSceneGraph *GetSceneGraph() override {return m_Sg;}; 

  /** Return a pointer to the image of the plot.*/
  void GetImage(wxBitmap &bmp, int magnification = 1) override;

  typedef std::map<mafString, mafVisualPipeInfo> mafPipeMap;

  /** Plug a visual pipe for a particular vme. It is used also to plug custom pipe.*/
  void PlugVisualPipe(mafString vme_type, mafString pipe_type, long visibility = VISIBLE);

  mafPipeMap m_PipeMap; ///< Map used to store visual pipeline associated with vme types

  /** Print this view.*/
  void Print(wxDC *dc, wxRect margins) override;

protected:
  mafSceneGraph *m_Sg;

  mafGUI *CreateGui() override;

  /** Return the visual pipe's name.*/
  void GetVisualPipeName(mafNode *node, mafString &pipe_name);
};
#endif
