/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafLHPBuilderViewCompound.h,v $
  Language:  C++
  Date:      $Date: 2006-06-27 14:24:00 $
  Version:   $Revision: 1.1 $
  Authors:   Paolo Quadrani
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mafMyViewCompound_H__
#define __mafMyViewCompound_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafView.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mmdMouse;
class mafSceneGraph;
class mmgGui;
class mafRWIBase;
class mmgLutSlider;
class mafVMEVolume;

//----------------------------------------------------------------------------
// mafViewCompound :
//----------------------------------------------------------------------------
/** 
mafViewCompound is the base class for Compound Views in MAF.

The member function GetNodeStatus(vme) will be called by the mmgCheckTree 
in order to update it's icons. (Note that mmgCheckTree doesn't know about
the SceneGraph anymore)

mafViewCompound doesn't have a Scenegraph, nor knowledge about VTK /sa mafViewVTK.
*/
class mafLHPBuilderViewCompound: public mafViewCompound
{
public:
  mafLHPBuilderViewCompound(wxString label = "View Compound", int num_row = 1, int num_col = 2);
  virtual ~mafLHPBuilderViewCompound(); 

  mafTypeMacro(mafLHPBuilderViewCompound, mafViewCompound);

  enum LAYOUT_CONFIGURATION_ID
  {
    GRID_LAYOUT = 0,
    LAYOUT_1,
    LAYOUT_2,
    LAYOUT_3,
    LAYOUT_CUSTOM
  };

  virtual mafView *Copy(mafObserver *Listener);
  
  /** 
  Create the GUI on the bottom of the compounded view. */
  virtual void     CreateGuiView();

  virtual void VmeShow(mafNode *node, bool show);

  virtual void     OnEvent(mafEventBase *maf_event);

  virtual void OnSize(wxSizeEvent &event);

  /** IDs for the GUI */
  enum VIEWCOMPOUND_WIDGET_ID
  {
    ID_DEFAULT_CHILD_VIEW = Superclass::ID_LAST,
    ID_LAYOUT_CHOOSER,
    ID_LINK_SUBVIEW,
    ID_LAST
  };

  /** 
  Set the size of the windowing double slider according to the size of the view. */
  void OnLayoutLHPBuilder();

  /**set the lut range*/
  void SetLut(int low,int hight);

	void EnableWidgets(bool enable = true);

protected:

  mmgLutSlider *m_Luts;
  mafVMEVolume *m_CurrentVolume;
  vtkLookupTable  *m_ColorLUT;
  
};
#endif
