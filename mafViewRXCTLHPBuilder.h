/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafViewRXCTLHPBuilder.h,v $
  Language:  C++
  Date:      $Date: 2006-06-27 14:24:00 $
  Version:   $Revision: 1.1 $
  Authors:   Paolo Quadrani
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mafViewRXCTLHPBuilder_H__
#define __mafViewRXCTLHPBuilder_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafViewCompound.h"
#include "mafViewRXCT.h"
#include "mafVMESurface.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafViewRX;
class mafVMEVolume;
class vtkLookupTable;
class mafGizmoSlice;
class mafMyViewCompound;
class mafVMESurface;

//----------------------------------------------------------------------------
// mafViewRXCT :
//----------------------------------------------------------------------------
/** */
class mafViewRXCTLHPBuilder: public mafViewCompound
{
public:
  mafViewRXCTLHPBuilder(wxString label = "View RXCT");
  virtual ~mafViewRXCTLHPBuilder(); 
  
  virtual mafView *Copy(mafObserver *Listener);

  virtual void OnEvent(mafEventBase *maf_event);

  mafTypeMacro(mafViewRXCTLHPBuilder, mafViewCompound);

  /** Redefined to intercept the VME selection, to manage widget to enlarge sections.*/
  void VmeSelect(mafNode *node, bool select);

  virtual void PackageView();
  
  virtual void OnSize(wxSizeEvent &event);

  /** Delete the gizmo. */
  void GizmoDelete();
  
  /**Set and create the gizmo*/
  void GizmoCreate(mafNode *node);
  
  /** 
  Show/Hide VMEs into plugged sub-views*/
  virtual void VmeShow(mafNode *node, bool show);

  /** Remove VME into plugged sub-views*/
  virtual void VmeRemove(mafNode *node);

  enum VIEW_RXCT_WIDGET_ID
  {
    ID_LUT_CHOOSER = Superclass::ID_LAST,
    ID_MOVE_ALL_SLICES,
	  ID_RIGHT_OR_LEFT,
	  ID_SNAP,
	  ID_ADJUST_SLICES,
	  ID_BORDER_CHANGE,
	  ID_ALL_SURFACE,
    ID_LAST
  };

protected:
  virtual mmgGui  *CreateGui();

  /**Enable or disable the GUI of the setting of the thickness border in the slices*/
  void SetGuiForBorder(mafNode *node,bool select);

  /** 
  Redefine to arrange views to generate RXCT visualization.*/
  virtual void LayoutSubViewCustom(int width, int height);

  /**Go throw the tree*/
  void NavigateTree(mafNode *root);

  /** 
  Enable/disable view widgets.*/
  void EnableWidgets(bool enable = true);

	/**
	Sort the slice */
	void SortSlices();

  int m_RightOrLeft;
  int m_Snap;
  int m_AllSurface;
  wxSize m_Size;
  double m_Border;
  std::vector<mafSceneNode*> m_CurrentSurface;

	int			m_Sort[6];
	double	m_Pos[6];
  double	m_BorderColor[6][3];

  mafGizmoSlice   *m_Gizmo[6];
  mafVMEVolume    *m_CurrentVolume; ///< Current visualized volume
  mmgLutSwatch    *m_LutSwatch;
  vtkLookupTable  *m_ColorLUT;
  mafViewRX       *m_ViewsRX[2];
  mafViewCompound *m_ViewCT;

	int m_NumSlice;

  int m_MoveAllSlices;

};
#endif
