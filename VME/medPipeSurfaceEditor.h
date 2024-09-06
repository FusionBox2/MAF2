/*=========================================================================

 Program: MAF2Medical
 Module: medPipeSurfaceEditor
 Authors: Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __medPipeSurfaceEditor_H__
#define __medPipeSurfaceEditor_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------

#include "mafPipeVTK.h"

//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------
class vtkSphereSource;
class vtkGlyph3D;
class vtkTubeFilter;
class vtkPolyDataMapper;
class vtkAppendPolyData;
class vtkActor;
class vtkLookupTable;
class vtkMAFFixedCutter;
class vtkPlane;

/**
  Class Name: medPipeSurfaceEditor.
  Default pipe of the medVMESurfaceEditor, that is a vme that permit the editing of topology.
*/
class MED_VME_EXPORT medPipeSurfaceEditor : public mafPipeVTK
{
public:
  /** RTTI Macro */
	mafTypeMacro(medPipeSurfaceEditor,mafPipeVTK);

  /** constructor. */
	medPipeSurfaceEditor();
  /** destructor. */
  ~medPipeSurfaceEditor() override;

	/** process events coming from gui */
  void OnEvent(mafEventBase *maf_event) override;

  /** function that create the pipeline instancing vtk graphic pipe. */
  void Create(mafNode *node, mafView *view) override;
  /** Show can be used for hide or show elements (actually empty) */
	void Show(bool show); 
  /** During selection of vme, it can visualize graphic element (actually empty)*/
  void Select(bool select) override;


protected:
  /** Creation of the gui that will be attached to visual prop panel.*/
  mafGUI  *CreateGui() override;

	vtkPolyDataMapper	*m_Mapper;
	vtkActor					*m_Actor;

	vtkLookupTable		*m_LUT;
};
#endif // __medPipeSurfaceEditor_H__
