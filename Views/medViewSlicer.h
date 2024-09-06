/*=========================================================================

 Program: MAF2Medical
 Module: medViewSlicer
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __medViewSlicer_H__
#define __medViewSlicer_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------

#include "medViewCompoundWindowing.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------

class mafViewVTK;
class mafGizmoTranslate;
class mafGizmoRotate;
class mafVMEVolumeGray;
class mafVMESlicer;
class mafVMEImage;
class mafMatrix;
class mafAttachCamera;
class mafGUI;
class mafGUILutSlider;
class mafGUILutSwatch;

//----------------------------------------------------------------------------
// mafViewOrthoSlice :
//----------------------------------------------------------------------------
/** 
  This compound view is made of four child views used to analyze different orthogonal slices of the volume*/
class MED_VIEWS_EXPORT medViewSlicer: public medViewCompoundWindowing
{
public:
  medViewSlicer(const mafString& label = _R("View Arbitrary Slice"), bool show_ruler = false);
  ~medViewSlicer() override; 

  mafTypeMacro(medViewSlicer, medViewCompoundWindowing);

	enum ID_GUI
	{
		ID_COMBO_GIZMOS = Superclass::ID_LAST,
		ID_RESET,
		ID_HELP,
		ID_LAST,
	};

  /** 
  Set the visualization status for the node (visible, not visible, mutex, ...) 
  \sa mafSceneGraph mafView*/
  int GetNodeStatus(mafNode *vme) override;

	/** Create visual pipe and initialize them to build an OrthoSlice visualization */
  void PackageView() override;

	/** Show/Hide VMEs into plugged sub-views*/
  void VmeShow(mafNode *node, bool show) override;

	/** Remove VME into plugged sub-views*/
  void VmeRemove(mafNode *node) override;

	/** Create the GUI on the bottom of the compounded view. */
  void CreateGuiView() override;

  /** Function that handles events sent from other objects. */
  void OnEvent(mafEventBase *maf_event) override;

   /** Function that clones instance of the object. */
  mafView* Copy(mafBaseEventHandler *Listener, bool lightCopyEnabled = false) override;

  /** Force the updating of the camera. */
  void CameraUpdate() override;

  //virtual void VmeSelect(mafNode *node, bool select);


protected:

	/** Internally used to create a new instance of the GUI. 
  This function should be overridden by subclasses to create specialized GUIs. 
  Each subclass should append its own widgets and define the enum of IDs for the widgets as 
  an extension of the superclass enum. The last id value must be defined as "LAST_ID" to allow the 
  subclass to continue the ID enumeration from it. For appending the widgets in the
  same panel GUI, each CreateGUI() function should first call the superclass' one.*/
  mafGUI* CreateGui() override;

	/** Enable/disable view widgets.*/
  void EnableWidgets(bool enable = true) override;

  /** Handling events sent from other objects. Called by public method OnEvent().*/
	void OnEventThis(mafEventBase *maf_event);  

  /**Protected method returning true if windowing can be enabled.
  It overrides superclass method.*/
  bool ActivateWindowing(mafNode *node) override;

  /**Update lutslider with correct values in case of bool variable is true, otherwise disable the widget. */
  void UpdateWindowing(bool enable, mafNode *node) override;

  /** Update windowing on slicer visualization. */
  void SlicerWindowing(mafVMESlicer *slicer);

	
	mafViewVTK *m_ViewSlice;
	mafViewVTK *m_ViewArbitrary;

	
	mafVME	      *m_CurrentVolume;
  mafVMEImage   *m_CurrentImage;
	mafVMESlicer  *m_CurrentSlicer;

	mafAttachCamera		*m_AttachCamera;

	double	m_SliceCenterSurface[3];
	double	m_SliceCenterSurfaceReset[3];
	double	m_SliceAngleReset[3];
	
};
#endif
